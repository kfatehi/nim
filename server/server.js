#!/usr/bin/env node
// SERVER
Array.prototype.remove = function(e) {
  for (var i = 0; i < this.length; i++) {
    if (e == this[i]) { return this.splice(i, 1); }
  }
};

DEBUG = (process.ARGV[2] == '--debug' ? true : false);
var crypto = require('crypto');
var shasum = crypto.createHash('sha1');
var net = require('net');
var http = require('http');
var url = require('url');
var redis = require("redis").createClient();

process.on('uncaughtException', function (err) {
  console.error(err.stack);
});
redis.on("error", function (err) {
    console.log("Redis client error: " + err);
});

var doWebBufferRequest = function (id, socket)
{
  console.log('Web request for: '+id);
  redis.exists('nimbus:'+id, function (err, reply) {
    if (reply)
      redis.get('nimbus:'+id, function (err, buffer) {
        if (err) {
          socket.end('error: Redis error while retrieving buffer.');
          console.error('  Redis-related error. Web display failed: '+id);
          console.error(err);
        } else {
          socket.end(buffer);
          console.log('Web server sent buffer for nimbus: '+id);
        }
      });
    else socket.end('Nothing found with id: '+id);
  });
}
var doCreateNewNimbus = function (socket)
{
  var id = generateId();
  redis.exists('nimbus:'+id, function (err, reply) {
    if (reply == 1)
      doCreateNewNimbus(socket); // try a new id...
    else {
      console.log('Created new nimbus: '+id);
      socket.write('new_nimbus:'+id);
    }
  });
}
var doSeedBuffer = function (data, id, socket)
{
  var buffer = data.slice(18, data.length);
  console.log('Seeding buffer: '+id);
  redis.set('nimbus:'+id, buffer, function (err, reply) {
    if (reply == 'OK') {
      socket.write('buffer_seed_ok');
      console.log('  Buffer seeded successfully: '+id);
    } else {
      socket.write('error: Failed to insert buffer.');
      console.error('  Buffer seeding failed: '+id);
      console.error('  Printing failed input for: '+id+'>\n'+buffer);
      console.error(err);
    }
  });
}
var generateId = function ()
{
  var shasum = crypto.createHash('sha1');
  shasum.update(new Date().getTime().toString());
  return shasum.digest('hex').slice(0,6);
}
var clients = [];
var server = net.createServer(function (socket) {
  socket.setEncoding("UTF8");
  socket.addListener("data", function (data) {
    if (data[0] == '@') {
      // reserved for real time data exchange
    } else {
      var parts = data.split(':');
      var message = parts[0];
      var params = parts.slice(1, parts.length);
      if (DEBUG) console.log(params);
      if (params[0] && params[0].length >= 6)
        var id = params[0].slice(0, 5);
      switch (message) {
        case 'join_nimbus':{
          var id = params[0];
          console.log('Client is joining nimbus: '+id);
          redis.get('nimbus:'+id, function (err, buffer) {
            if (err) {
              socket.write('error: Redis error while retrieving buffer.');
              console.error('  Redis-related error. Buffer seeding failed: '+id);
              console.error(err);
            } else {
              socket.write('seed_buffer:'+id+'>'+buffer);
              console.log('Sent buffer for nimbus: '+id);
            }
          });
          break;
        }
        case 'create_new_nimbus':{ doCreateNewNimbus(socket); break; }
        case 'seed_buffer':{ doSeedBuffer(data, id, socket); break; }
        case 'c':{ // this is for debugging
          if (DEBUG) {
            var c_data = data.slice(2, data.length);
            console.log("Broadcasting: "+c_data);
            clients.forEach(function(s) {
        			try { s.write(c_data); }
        			catch (e) { clients.remove(s); }
        		});
            break;
          }  
        }
        default: {
          if (/HTTP/i.test(message)) {
            var id = message.match(/GET \/(.*) HTTP/)[1];
            if (id) doWebBufferRequest(id, socket);
            else socket.end('Welcome to the Nimbus web server.');
          }
          else {
            console.log('Unknown message: '+data);
            socket.end('What? '+data);
          }
        }
      }
    }
  });
}).on("connection", function (socket) {
  clients.push(socket);
}).on("end", function (socket) {
  clients.remove(socket);
}).listen(8000, "0.0.0.0", function () {
  if (DEBUG) 
    console.log('DEBUG ENABLED\nListening on port 8000');
  else
    console.log('Listening on port 8000');
});
