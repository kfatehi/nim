#!/usr/bin/env node
// CLIENT

var tty = require('tty');
    tty.setRawMode(true);  
var net = require('net');
var fs = require('fs');
var url = require("url"); // for parsing nimbus_url
var argv = process.ARGV;

// --------- 
// Nim class
// ---------
var Nim = function () {
  this.socket = null;
  this.nimbus_id = null;
  this.filepath = null;
  this.host = null;
  this.port = null;
  this.nimbus_url = null;
  this.joining = function (_nimbus_url) {
    nimbus_url = _nimbus_url;
    console.log('Trying to reach active nimbus at: '+nimbus_url);
    var nu = url.parse(nimbus_url, true);
    nimbus_id = nu.pathname.slice(1, nu.pathname.length);
    host = nu.hostname;
    port = nu.port;
    socket = connect(host, port);
    socket.write('join_nimbus:'+nimbus_id);
  }
  this.creating = function (_filepath, _host, _port) {
    filepath = _filepath;
    host = _host;
    port = _port;
    console.log('Starting up a new nimbus from '+filepath);
    socket = connect(host, port);
    socket.write('create_new_nimbus');
  }
  // -----

  var doKeypress = function (chunk, key) {
    if (key && key.name) {
      switch (key.name) {
        case 'c':
          if (key.ctrl) process.exit();
        case 'backspace': 
          // caret.delprev
        case 'delete':
          // caret.delnext
        case 'left':
          // caret.goleft
        default:
          // fall through to text inputting
          // for now, output debug data
          process.stdout.write('[chunk: '+chunk+'] ');
          console.log(key);
      }
    } else {
      return chunk;
    }
  }

  var editorLoop = function () {
    console.log('Launching editor...');
    // we'll have to learn to draw a real editor soon.
    console.log('window size: '+tty.getWindowSize(0));
    process.openStdin();
    process.stdin.on('keypress', doKeypress);
  }

  // -----
  var connect = function () {
    var client = net.createConnection(port, host);
    client.setEncoding("UTF8");
    client.addListener("connect", function () {
      console.log('Connected to nimbus network!');
    });
    client.addListener("data", function (data) {
      if (data[0] == '@') {
        // reserved for real time data exchange
      } else {
        var parts = data.split(':');
        var message = parts[0];
        var params = parts.slice(1, parts.length);
        switch (message) {
          case 'new_nimbus':{
            nimbus_id = params[0].slice(0,-1);
            nimbus_url = 'nim:'+host+':'+port+'/'+nimbus_id;
            console.log('Initialized a new nimbus: '+nimbus_url);
            // get file contents and send
            socket.write('end_buffer:'+nimbus_id);
            console.log('Sent buffer for nimbus: '+nimbus_id);
            break;
          }
          case 'end_buffer':{
            editorLoop()
            break;
          }
          default:{
            console.log('Unknown message: '+message);
            console.log('Included params: '+params);
            socket.end();
          }
        }
      }
    });
    client.addListener("end", function () {
      console.log('Connection closed.');
      process.exit();
    })
    return client;
  };
}

// --------
// startup
// --------
switch (argv.length) {
  case 3:{
    var nim = new Nim().joining(argv[2]);
    break;
  }
  case 5:{
    var nim = new Nim().creating(argv[2], argv[3], argv[4]);
    break;
  }
  default:{
    console.log('Usage: \n\
      Create new nimbus from file:   nim <file> <host> <port>\n\
      Join an existing nimbus:       nim <nimbus_url>');
    process.exit();    
  }
}