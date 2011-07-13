#!/usr/bin/env node
// SERVER

// need to install redis....

var net = require('net');
var exec = require('child_process').exec;

var server = net.createServer(function (socket) {
  socket.setEncoding("UTF8");
  socket.addListener("data", function (data) {
    if (data[0] == '@') {
      // reserved for real time data exchange
    } else {
      var parts = data.split(':');
      var message = parts[0];
      var params = parts.slice(1, parts.length);
      switch (message) {
        case 'join_nimbus':{
          var nimbus_id = params[0];
          console.log('Client is joining nimbus: '+nimbus_id);
          // get the buffer from redis and send 
          socket.write('end_buffer:'+nimbus_id);
          console.log('Sent buffer for nimbus: '+nimbus_id);
          break;
        }
        case 'create_new_nimbus':{
          exec('openssl rand -base64 6', function (err, stdout, stderr) {
            var nimbus_id = stdout;
            // create an empty buffer in redis with the nimbus_id
            socket.write('new_nimbus:'+nimbus_id);
          });
          break;
        }
        default:{
          console.log('Unknown message: '+data);
          socket.end();
        }
      }
    }
  });
}).listen(8000, "127.0.0.1", function () {
  console.log('Listening on port 8000');
});
