#include "nimbus.h"
int sendToNimbus(char *msg) {
	writeSocket(sockfd, msg);
}
int getFromNimbus(char *msg) {
	readSocket(sockfd, msg, BIGBUF);
}