#!/usr/bin/env node
// SERVER
process.on('uncaughtException', function (err) {
  // Just a failsafe--or in case we're getting fuzzed :P
  console.error(err.stack);
});

// need to install redis....
var crypto = require('crypto');
var shasum = crypto.createHash('sha1');
var net = require('net');
var redis = require("redis").createClient();

redis.on("error", function (err) {
    console.log("Redis client error: " + err);
});

var generateId = function () {
  var shasum = crypto.createHash('sha1');
  shasum.update(new Date().getTime().toString());
  return shasum.digest('hex').slice(0,6);
}

var doCreateNewNimbus = function (socket) {
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

var doSeedBuffer = function (data, id, socket) {
  var buffer = data.slice(20, data.length);
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

var server = net.createServer(function (socket) {
  socket.setEncoding("UTF8");
  socket.addListener("data", function (data) {
    if (data[0] == '@') {
      // reserved for real time data exchange
    } else {
      var parts = data.split(':');
      var message = parts[0];
      var params = parts.slice(1, parts.length);
      if (params[0] && params[0].length >= 6)
        var id = params[0].slice(0, 5);
      switch (message) {
        case 'join_nimbus':{
          var nimbus_id = params[0];
          console.log('Client is joining nimbus: '+nimbus_id);
          redis.get('nimbus:'+nimbus_id, function (err, buffer) {
            if (err) {
              socket.write('error: Redis error while retrieving buffer.');
              console.error('  Redis-related error. Buffer seeding failed: '+id);
              console.error(err);
            } else {
              socket.write('seed_buffer:'+nimbus_id+'>'+buffer);
              console.log('Sent buffer for nimbus: '+nimbus_id);
            }
          });
          break;
        }
        case 'create_new_nimbus':{ doCreateNewNimbus(socket); break; }
        case 'seed_buffer':{ doSeedBuffer(data, id, socket); break; }
        default: console.log('Unknown message: '+data);
      }
    }
  });
}).listen(8000, "127.0.0.1", function () {
  console.log('Listening on port 8000');
});
