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

// --------- 
// GUI class
// ---------
var Gui = function (nim) {
  var color = {
    PINK    :'\033[95m',
    BLUE    :'\033[94m',
    GREEN   :'\033[92m',
    YELLOW  :'\033[93m',
    RED     :'\033[91m',
    END     :'\033[0m',
  };
  var chat = '';
  var print = function (c, str) {
    if (c)
      chat += color[c]+str+color['END']+'\n';
    else
      chat += str+'\n';
    // for now lets just print chat
    console.log(chat);
  }
  var inspect = function (obj) {
    print(false, util.inspect(obj, true, null));
  }
  var keypress = function (chunk, key) {
    print('YELLOW', '[chunk: '+chunk+'] ');
    inspect(key);
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
    } else {
      return chunk;
    }
  }
  this.init = function () {
    print('RED', 'LAUNCHING GUI');
    process.openStdin();
    process.stdin.on('keypress', keypress);
  }
  // console.log('window size: '+tty.getWindowSize(0));
  // console.log('Nimbus URL: '+nimbus_url);
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
  var joining = function (_nimbus_url) {
    nimbus_url = _nimbus_url;
    console.log('Trying to reach a nimbus at: '+nimbus_url);
    var nu = url.parse(nimbus_url, true);
    nimbus_id = nu.pathname.slice(1, nu.pathname.length);
    host = nu.hostname;
    port = nu.port;
    socket = connect(host, port);
    socket.write('join_nimbus:'+nimbus_id);
  }
  var creating = function (_host, _port, _filepath) {
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
          buffer = data.slice(20, data.length);
          init();
          break;
        }
        case 'buffer_seed_ok':{
          console.log('Server reported successful nimbus creation!');
          console.log('Switching into editor mode.');
          init();
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
  if (argc == 1) joining(argv[2]);
  else if (argc == 3) creating(argv[2], argv[3], argv[4]);
  else process.stdout.write('Usage: nim [<host> <port> <file> | <nimbus_url>]\n');
  var init = function () {
    console.log('Initializing editor...');
    if (buffer == null) {
      console.log('Buffer not found--requesting buffer for: '+nimbus_id);
      socket.write('join_nimbus:'+nimbus_id);
    } else {
      readyCallback(this);
    }
  }
}

new Nim(argv, argv.length-2, function (nim) { new Gui(nim).init(); });