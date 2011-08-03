#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NUMLETTER(c) ((c)>=(32)?(1):(0))

#define CTRL_R 18
#define CTRL_C 3
#define TAB 9
#define NEWLINE 10
#define RETURN 13
#define ESCAPE 27
#define BACKSPACE 127
#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

#define NB_ENABLE 0
#define NB_DISABLE 1

#define NIMBUS_ID_LEN 16
#define BIGBUF 256
#define SMALLBUF 32

#define HOSTNAME "127.0.0.1"
#define PORT "8000"

// Contexts
#define PREVIOUS -1
#define ROOT 0
#define CMND 1
#define EDIT 2
#define CHAT 3

// Socket Preconditions
#define NONE 0
#define WAITING_TO_SEED 1
//#define REMOTE_ERROR -1 // error:xxxxxxxxxx
//#define NIMBUS_CREATED 1 // new_nimbus:xxxxxx
//#define INCOMING_SEED 2 // seed_buffer:...
#define OVERFLOW_SEED 10 // expecting more data
//#define END_OF_SEED 3 // ...:end_seed
//#define SEED_CONFIRM 4 // buffer_seed_ok
#define EDITOR_BUF_SIZE 4092

// Methods of nimbus entries
#define BLANK 0
#define FROM_FILE 1
#define JOINED 2

#endif /* CONSTANTS_H */
