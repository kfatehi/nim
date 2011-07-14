#!/usr/bin/env node
// CLIENT
var tty = require('tty');
    tty.setRawMode(true);  
var net = require('net');
var fs = require('fs');
var url = require("url"); // for parsing nimbus_url
var util = require('util');
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
  this.center = function ()
  {
    
  }
}

// --------- 
// Window class
// ---------
var Window = function () {
  var n = null;
  var tf = new TextFormatter();
  var so = process.stdout;
  var windowSize = null; // h,w
  var editorSize = null;
  var statusBar = null;
  var logBuf = '';
  var print = function (c, str)
  {
    if(c) logBuf += tf.color.green(str+'\n');
    else  logBuf += str+'\n';
  }
  var inspect = function (obj)
  {
    print(false, util.inspect(obj, true, null));
  }
  var onKeypress = function (chunk, key)
  {
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
  var update = function ()
  {
    statusBar = tf.color.green('Nim v0.1');
  }
  var clear = function () {
    for(var line=0; line < windowSize[0]; line++) 
      for(var col=0; col < windowSize[1]; col++) so.write(' ')
        so.write('\n');
  }  
  var draw = function ()
  {
    for(var line=0; line < windowSize[0]; line++) {
      for(var col=0; col < windowSize[1]; col++) {
        if (line==1 && col < statusBar.length)
          process.stdout.write(statusBar[col]);
        else {
          //everything else
        }
      }
      so.write('\n');
    }
  }
  var loop = function ()
  {
    windowSize = tty.getWindowSize();
    editorSize = [windowSize[0]-1, windowSize[1]]; 
    update();
    clear();
    draw();
  }
  this.init = function (fps)
  {
    process.openStdin();
    process.stdin.on('keypress', onKeypress);
    setInterval(loop, 1000/fps);    
  }
};

// --------- 
// Nim class
// ---------
var Nim = function (readyCallback)
{
  var socket = null;
  var nimbus_id = null;
  var filepath = null;
  var host = null;
  var port = null;
  var nimbus_url = null;
  var buffer = ''; // used for editing
  var chat = ''; // used for chat, log, system messages
  this.join = function (_nimbus_url)
  {
    nimbus_url = _nimbus_url;
    console.log('Trying to reach a nimbus at: '+nimbus_url);
    var nu = url.parse(nimbus_url, true);
    nimbus_id = nu.pathname.slice(1, nu.pathname.length);
    host = nu.hostname;
    port = nu.port;
    socket = connect(host, port);
    socket.write('join_nimbus:'+nimbus_id);
  }
  this.create = function (_host, _port, _filepath)
  {
    filepath = _filepath;
    host = _host;
    port = _port;
    console.log('Starting up a new nimbus from '+filepath);
    socket = connect(host, port);
    socket.write('create_new_nimbus');
  }
  var connect = function ()
  {
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
  var onData = function (data)
  {
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
          launchGui();
          break;
        }
        case 'buffer_seed_ok':{
          console.log('Server reported successful nimbus creation!');
          launchGui();
          break;
        }
        case 'error':{
          console.error('[server error] '+data);
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
  var launchGui = function ()
  {
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

function main(argv, argc)
{
  var nim = new Nim(function (n) {
    new Window(n).init(10);
  });
  if (argc == 1)
    nim.join(argv[2]);
  else if (argc == 3)
    nim.create(argv[2], argv[3], argv[4]);
  else 
    process.stdout.write('Usage: nim [<host> <port> <file> | <nimbus_url>]\n'); 
}

main(argv, argv.length-2);