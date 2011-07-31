//
// Copied from javascript nim client
//
 
int onDataFromNimbus(const int sock) {
   // called from the select() when it detects activity on the sock fd
   // giant switch case goes here
}
int seedNimbus(const int sock, char *id, filepath) {
  char *msg = "seed_buffer:";
  strcat(prefix, id);
  strcat(prefix, ">");
  
  sendMessageToNimbus(sock, "seed_buffer:"+nimbus_id+">"+getFileContents(filepath)); 
}

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