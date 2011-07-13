#!/usr/bin/env node
// CLIENT
var tty = require('tty');
    tty.setRawMode(true);  
var net = require('net');
var fs = require('fs');
var url = require("url"); // for parsing nimbus_url
var util = require('util');
// var util = require('util'),
//     exec = require('child_process').exec,
//     child;
// 
// child = exec('clear',  function (e, o, se) { console.log(o) });


var argv = process.ARGV;
process.on('uncaughtException', function (err) {
  if (err.code == 'EAFNOSUPPORT')
    console.log('Invalid nimbus url: '+nimbus_url);
  else if (err.code == 'ENOTFOUND')
    console.log('Invalid hostname: '+host);
  else if (err.code == 'ECONNREFUSED')
    console.log('Connection refused');
  else { process.stdout.write(err.stack); process.exit(); }
});

var TextFormatter = function () {
  this.color = {
    end:'\033[0m',
    pink:function(t){return'\033[95m'+t+this.end},
    blue:function(t){return'\033[94m'+t+this.end},
    green:function(t){return'\033[92m'+t+this.end},
    yellow:function(t){return'\033[93m'+t+this.end},
    red:function(t){return'\033[91m'+t+this.end}
  }
  this.center = function () {
    
  }
}

// --------- 
// Window class
// ---------
var Window = function (nim) {
  var tf = new TextFormatter();
  var so = process.stdout;
  var windowSize = null; //height[0] width[0]
  var statusBar = null;
  var windowBuffer = ''; 
  var logBuf = '';
  var print = function (c, str) {
    if(c) logBuf += tf.color.green(str+'\n');
    else  logBuf += str+'\n';
  }
  var inspect = function (obj) {
    print(false, util.inspect(obj, true, null));
  }
  var onKeypress = function (chunk, key) {
    print('YELLOW', '[chunk: '+chunk+'] ');
    inspect(key);
    console.log(key);
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
      }
    }
  }
  var clear = function () {
    for(var line=0; line < windowSize[0]; line++) 
      for(var col=0; col < windowSize[1]; col++) so.write(' ')
        so.write('\n');
  }
  var draw = function () {
    clear();
    for(var line=0; line < windowSize[0]; line++) { // bottom up
      for(var col=0; col < windowSize[1]; col++) {
        if (line==0 && col < statusBar.length) { //statusbar
          process.stdout.write(statusBar[col]);
        }
      }
    }
  }
  var update = function () {
    x = tty.getWindowSize(0);
    statusBar = tf.color.green('Nim v0.1');
  }
  var loop = function () {
    update();
    draw();
  }
  this.init = function (fps) {
    process.openStdin();
    process.stdin.on('keypress', onKeypress);
    setInterval(loop, 1000/fps);    
  }
};

// --------- 
// Nim class
// ---------
var Nim = function (argv, argc, readyCallback) {
  var socket = null;
  var nimbus_id = null;
  var filepath = null;
  var host = null;
  var port = null;
  var nimbus_url = null;
  var buffer = ''; // used for editing
  this.join = function (_nimbus_url) {
    nimbus_url = _nimbus_url;
    console.log('Trying to reach a nimbus at: '+nimbus_url);
    var nu = url.parse(nimbus_url, true);
    nimbus_id = nu.pathname.slice(1, nu.pathname.length);
    host = nu.hostname;
    port = nu.port;
    socket = connect(host, port);
    socket.write('join_nimbus:'+nimbus_id);
  }
  this.create = function (_host, _port, _filepath) {
    filepath = _filepath;
    host = _host;
    port = _port;
    console.log('Starting up a new nimbus from '+filepath);
    socket = connect(host, port);
    socket.write('create_new_nimbus');
  }
  var connect = function () {
    var client = net.createConnection(port, host);
    client.setEncoding("UTF8");
    client.addListener("connect", function () {
      console.log('Connected to nimbus network!');
    });
    client.addListener("data", onData);
    client.addListener("end", function () {
      console.log('Connection closed.');
      process.exit();
    })
    return client;
  };
  var onData = function (data) {
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
          fs.readFile(filepath, "binary", function (err, _buffer) {
            buffer = (_buffer ? _buffer : '');
            socket.write('seed_buffer:'+nimbus_id+'>'+buffer);
            console.log('Sent buffer for nimbus: '+nimbus_id);
        	});
          break;
        }
        case 'seed_buffer':{
          console.log('Received buffer data for nimbus: '+nimbus_url)
          buffer = data.slice(20, data.length);
          break;
        }
        case 'buffer_seed_ok':{
          console.log('Server reported successful nimbus creation!');
          break;
        }
        case 'error':{
          gui.error('[server error] '+data);
          process.exit();
        }
        default:{
          console.log('Unknown message: '+message);
          console.log('Included params: '+params);
          process.exit();
        }
      }
    }
  }
  var launchGui = function () {
    process.stdout.write('Preflight buffer check... ');
    if (buffer == null) {
      console.log('\nBuffer not found -- requesting a buffer for: '+nimbus_id);
      socket.write('join_nimbus:'+nimbus_id);
    } else {
      console.log('OK!');
      readyCallback(this);
    }
  }
}

function main(argv, argc) {
  
  var window = new Window().init(10);
  
  
  // var nim = new Nim();
  // if (argc == 1)
  //   nim.join(argv[2]));
  // else if (argc == 3)
  //   new Gui(nim.create(argv[2], argv[3], argv[4]));
  // else 
  //   process.stdout.write('Usage: nim [<host> <port> <file> | <nimbus_url>]\n'); 
}

main(argv, argv.length-2);