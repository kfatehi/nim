#!/usr/bin/env node
// CLIENT
var nc = require('ncurses');
var net = require('net');
var fs = require('fs');
var url = require("url"); // for parsing nimbus_url
var util = require('util');
var argv = process.ARGV;

var NimBuffer = function (seed){
  this.buffer = [''];
  this.clear = function(){this.buffer=['']};
  this.print = function(str){this.buffer[this.buffer.length-1]+=str};
  this.puts = function(str){this.buffer.push(str)};
}

var Nim = function (){
  // *********************************************************************
  // *************************** PERSISTENT DATA *************************
  // *********************************************************************
  var labels = [
    'Nim v0.1',
    'Status Bar'
  ];
  var editor = new NimBuffer;
  var chat = new NimBuffer;

  var socket = null;
  var nimbus_id = null;
  var filepath = null;
  var host = null;
  var port = null;
  var nimbus_url = null;
  
  var mode = null; // editor mode, chat mode, etc
  
  // *********************************************************************
  // ******************************* NCURSES *****************************
  // *********************************************************************
  var w = new nc.Window();
  var onInputChar = function (s,i){
    // if in chat mode or editor mode...
    switch(i){
      case nc.keys.ESC:{
        mode = null;
        break;
      }
      case nc.keys.BACKSPACE:
      case 127:{ // backspace is 127 on mac
        if (w.curx > 0) w.delch(w.cury, w.curx-1);
        break;
      }
      case nc.keys.DEL:{
        var prev_x = w.curx;
        w.delch(w.cury, w.curx);
        w.cursor(w.cury, prev_x);
        w.refresh();
        break;
      }
      case nc.keys.UP:{
        if (w.cury > 2) w.cursor(w.cury-1, w.curx);
        else w.scroll(-1);
        break;
      }
      case nc.keys.DOWN:{
        if (w.cury < w.height-3) w.cursor(w.cury+1, w.curx);
        else w.scroll(1);
        break;
      }
      case nc.keys.LEFT:{
        if (w.curx > 0) w.cursor(w.cury, w.curx-1);
        break;
      }
      case nc.keys.RIGHT:{
        if (w.curx < w.width) w.cursor(w.cury, w.curx+1);
        break;
      }
      default: w.print(s+' ('+i+')\n');
    }
    w.refresh();
  };
  var initBuffer = function(){
    var x = w.curx, y = w.cury,
      lines = buffer.split('\n'),
      numlines = lines.length;
    w.cursor(2,0);
    for (var i=0;i<lines.length;i++) {
      if(i >= w.height-3) {
        w.scroll(1);
        w.print(lines[i]);
      } else w.print(lines[i]);
    }
    w.scroll(1);
    // win.cursor(win.height-3, 0);
    // win.print("[" + time + "] ");
    // if (attrs)
    //   win.attron(attrs);
    // win.print(message);
    // if (attrs)
    //   win.attroff(attrs);
    // win.cursor(cury, curx);
    // win.refresh();
    //     
    //     ////////
    //     w.cursor(2,0);
    //     w.insdelln(200);
    //     w.insstr(2, 0, buffer);
    //     w.refresh();
  }
  var setupWindow = function (){
    w.clear();
    w.on('inputChar', onInputChar);
    w.label(labels[0], labels[1]);
    w.scrollok(true);
    w.setscrreg(2, w.height-3);
    w.hline(1, 0, w.width);
    w.hline(w.height-2, 0, w.width);
    w.cursor(2, 0);
    //initBuffer();
    w.refresh();
    w.inbuffer = "";
  }
  
  // *********************************************************************
  // ****************************** APP LOGIC ****************************
  // *********************************************************************
  this.join = function (_nimbus_url){
    nimbus_url = _nimbus_url;
    chat.puts('Trying to reach a nimbus at: '+nimbus_url);
    var nu = url.parse(nimbus_url, true);
    nimbus_id = nu.pathname.slice(1, nu.pathname.length);
    host = nu.hostname;
    port = nu.port;
    socket = connect(host, port);
    socket.write('join_nimbus:'+nimbus_id);
  }
  this.create = function (_host, _port, _filepath){
    filepath = _filepath;
    host = _host;
    port = _port;
    chat.puts('Starting up a new nimbus from '+filepath);
    socket = connect(host, port);
    socket.write('create_new_nimbus');
  }
  var connect = function (){
    var client = net.createConnection(port, host);
    client.setEncoding("UTF8");
    client.addListener("connect", function (){
      chat.puts('Connected to nimbus network!');
    });
    client.addListener("data", onData);
    client.addListener("end", function (){
      chat.puts('Connection closed.');
      process.exit();
    })
    return client;
  };
  var onData = function (data){
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
          chat.puts('Initialized a new nimbus: '+nimbus_url);
          fs.readFile(filepath, "binary", function (err, _buffer){
            buffer = (_buffer ? _buffer : '');
            socket.write('seed_buffer:'+nimbus_id+'>'+buffer);
            chat.puts('Sent buffer for nimbus: '+nimbus_id);
        	});
          break;
        }
        case 'seed_buffer':{
          chat.puts('Received buffer data for nimbus: '+nimbus_url)
          buffer = data.slice(20, data.length);
          preFlightCheck();
          break;
        }
        case 'buffer_seed_ok':{
          chat.puts('Server reported successful nimbus creation!');
          preFlightCheck();
          break;
        }
        case 'error':{
          console.error('[server error] '+data);
          process.exit();
        }
        default:{
          chat.puts('Unknown message: '+message);
          chat.puts('Included params: '+params);
          process.exit();
        }
      }
    }
  }
  var preFlightCheck = function (){
    chat.puts('Preflight buffer check... ');
    if (buffer == null) {
      chat.puts('\nBuffer not found -- requesting a buffer for: '+nimbus_id);
      socket.write('join_nimbus:'+nimbus_id);
    } else {
      chat.puts('OK!');
      setupWindow();
    }
  }
  process.on('uncaughtException', function (err) {
    if (err.code == 'EAFNOSUPPORT')
      chat.puts('Invalid nimbus url: '+nimbus_url);
    else if (err.code == 'ENOTFOUND')
      chat.puts('Invalid hostname: '+host);
    else if (err.code == 'ECONNREFUSED')
      chat.puts('Connection refused');
    else {
      w.close();
      console.log(err.stack);
    }
  });
}

function main(argv, argc){
  var nim = new Nim();
  if (argc == 1)
    nim.join(argv[2]);
  else if (argc == 3)
    nim.create(argv[2], argv[3], argv[4]);
  else 
    process.stdout.write('Usage: nim [<host> <port> <file> | <nimbus_url>]\n'); 
}
main(argv, argv.length-2);