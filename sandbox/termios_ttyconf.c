  /* * * * * * * * * * * * * * * * * * * * * * *\ 
   * Basic Configuration of a Serial Interface
   Copied from http://en.wikibooks.org/wiki/Serial_Programming/termios
    
   After a serial device has been opened, it is typical that its default configuration,
   like baud rate or line discipline need to be override with the desired parameters.
   This is done with a rather complex data structure, and the tcgetattr(3) and
   tcsetattr(3) functions. Before that is done, however, it is a good idea to check 
   if the opened device is indeed a serial device (aka tty). The following is an example
   of such a configuration. The details are explained later in this module.
  \* * * * * * * * * * * * * * * * * * * * * * */ 
#include <termios.h>
#include <unistd.h>
struct termios  config;
if(!isatty(fd)) { ... error handling ... }
if(tcgetattr(fd, &config) < 0) { ... error handling ... }
//
// Input flags - Turn off input processing
// convert break to null byte, no CR to NL translation,
// no NL to CR translation, don't mark parity errors or breaks
// no input parity check, don't strip high bit off,
// no XON/XOFF software flow control
//
config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                    INLCR | PARMRK | INPCK | ISTRIP | IXON);
//
// Output flags - Turn off output processing
// no CR to NL translation, no NL to CR-NL translation,
// no NL to CR translation, no column 0 CR suppression,
// no Ctrl-D suppression, no fill characters, no case mapping,
// no local output processing
//
// config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
//                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
config.c_oflag = 0;
//
// No line processing:
// echo off, echo newline off, canonical mode off, 
// extended input processing off, signal chars off
//
config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
//
// Turn off character processing
// clear current char size mask, no parity checking,
// no output processing, force 8 bit input
//
config.c_cflag &= ~(CSIZE | PARENB);
config.c_cflag |= CS8;
//
// One input byte is enough to return from read()
// Inter-character timer off
//
config.c_cc[VMIN]  = 1;
config.c_cc[VTIME] = 0;
//
// Communication speed (simple version, using the predefined
// constants)
//
if(cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0) {
    ... error handling ...
}
//
// Finally, apply the configuration
//
if(tcsetattr(fd, TCSAFLUSH, &config) < 0) { ... error handling ... }

  /* * * * * * * * * * * * * * * * * * * * * * *\ 
    From the man pages, or http://www.mkssoftware.com/docs/man3/tcsetattr.3.asp
   TCSANOW  
   The change occurs immediately.

   TCSADRAIN  
   The change occurs after all output written to the file descriptor has been transmitted. This action should be used when changing parameters that affect output.

   TCSAFLUSH  
   The change occurs after all output written to the file descriptor has been transmitted, and all input so far received but not read is discarded before the change is made.
  \* * * * * * * * * * * * * * * * * * * * * * */ 
