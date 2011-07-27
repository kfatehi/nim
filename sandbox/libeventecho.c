/*
 * libevent echo server example using buffered events.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>

/* Required by event.h. */
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <queue.h>

/* Libevent. */
#include <event.h>

#include "tcl.h"

/* Port to listen on. */
#define SERVER_PORT 5555
int debug = 0;

/**
 * A struct for client specific data, also includes pointer to create
 * a list of clients.
 */
struct client {
	/* The clients socket. */
	int fd;

	/* The bufferedevent for this client. */
	struct bufferevent *buf_ev;
};

struct reader_entry {
	struct client *cli;
	char *pattern;
	TAILQ_ENTRY(reader_entry) entries;
};
TAILQ_HEAD(, reader_entry) readers;

struct tuple_entry {
	char *tuple_string;
	TAILQ_ENTRY(tuple_entry) entries;
};
TAILQ_HEAD(, tuple_entry) tuples;


void
signal_handler(int sig) {
	switch(sig) {
		case SIGTERM:
		case SIGHUP:
		case SIGINT:
			event_loopbreak();
			break;
        default:
            syslog(LOG_WARNING, "Unhandled signal (%d) %s", strsignal(sig));
            break;
    }
}

/**
 * Set a socket to non-blocking mode.
 */
int
setnonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

int
find_readers(char *tuple, struct evbuffer *evb)
{
	struct reader_entry *entry, *tmp_entry;
	char buf[16384];

	*buf = '\0';
	for (entry = TAILQ_FIRST(&readers); entry != NULL; entry = tmp_entry) {
		tmp_entry = TAILQ_NEXT(entry, entries);
		if (Tcl_StringMatch(tuple, entry->pattern)) {
			evbuffer_add_printf(evb, "ok %s\n", tuple);
			TAILQ_REMOVE(&readers, entry, entries);
			return 1;
		}
	}
	return 0;
}

/**
 * Called by libevent when there is data to read.
 */
void
buffered_on_read(struct bufferevent *bev, void *arg)
{
	/* Write back the read buffer. It is important to note that
	 * bufferevent_write_buffer will drain the incoming data so it
	 * is effectively gone after we call it. */
	struct client *cli = (struct client *)arg;
	struct tuple_entry *entry, *tmp_entry;
	struct reader_entry *reader;
	struct evbuffer *evb;
	char *cmd, *pattern, *data;
	char buf[16384];
	int i;
	
	*buf = '\0';
	cmd = evbuffer_readline(bev->input);
	if (cmd == NULL) {
		return;
	}
	
	evb = evbuffer_new();
	if (strncmp(cmd, "write", 5) == 0) {
		if (!find_readers(cmd+6, evb)) {
			entry = malloc(sizeof(*entry));
			entry->tuple_string = malloc(strlen(cmd+6)+1);
			strcpy(entry->tuple_string, cmd+6);
			TAILQ_INSERT_TAIL(&tuples, entry, entries);
		}
		evbuffer_add_printf(evb, "ok write\n");
	} else if (strncmp(cmd, "read", 4) == 0) {
		pattern = cmd+5;
		for (entry = TAILQ_FIRST(&tuples); entry != NULL; entry = tmp_entry) {
			tmp_entry = TAILQ_NEXT(entry, entries);
			if (Tcl_StringMatch(entry->tuple_string, pattern)) {
				evbuffer_add_printf(evb, "ok %s\n", entry->tuple_string);
				TAILQ_REMOVE(&tuples, entry, entries);
				free(entry->tuple_string);
				free(entry);
				goto out;
			}
		}
		reader = malloc(sizeof(*reader));
		reader->cli = cli;
		reader->pattern = malloc(strlen(pattern)+1);
		strcpy(reader->pattern, pattern);
		TAILQ_INSERT_TAIL(&readers, reader, entries);
	} else if (strncmp(cmd, "dump", 4) == 0) {
		TAILQ_FOREACH(entry, &tuples, entries) {
			evbuffer_add_printf(evb, "ok %s\n", entry->tuple_string);
        }
	} else if (strncmp(cmd, "exit", 4) == 0
			   || strncmp(cmd, "quit", 4) == 0) {
		evbuffer_add_printf(evb, "ok bye\n");
		shutdown(cli->fd, SHUT_RDWR);
	} else {
		evbuffer_add_printf(evb, "error unknown command\n");
	}
	
out:

	bufferevent_write_buffer(bev, evb);
	evbuffer_free(evb);
	free(cmd);
}

/**
 * Called by libevent when the write buffer reaches 0.  We only
 * provide this because libevent expects it, but we don't use it.
 */
void
buffered_on_write(struct bufferevent *bev, void *arg)
{
}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void
buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
	struct client *client = (struct client *)arg;
	struct reader_entry *entry, *tmp_entry;

	if (what & EVBUFFER_EOF) {
		/* Client disconnected, remove the read event and the
		 * free the client structure. */
		printf("Client disconnected.\n");
	} else {
		warn("Client socket error, disconnecting.\n");
	}
	for (entry = TAILQ_FIRST(&readers); entry != NULL; entry = tmp_entry) {
		tmp_entry = TAILQ_NEXT(entry, entries);
		if ((void *)tmp_entry != NULL && client->fd == tmp_entry->cli->fd) {
			TAILQ_REMOVE(&readers, entry, entries);
			free(entry->pattern);
			free(entry);
		}
	}
	bufferevent_free(client->buf_ev);
	close(client->fd);
	free(client);
}

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void
on_accept(int fd, short ev, void *arg)
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		warn("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (setnonblock(client_fd) < 0)
		warn("failed to set client socket non-blocking");

	/* We've accepted a new client, create a client object. */
	client = calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	client->fd = client_fd;
	
	/* Create the buffered event.
	 *
	 * The first argument is the file descriptor that will trigger
	 * the events, in this case the clients socket.
	 *
	 * The second argument is the callback that will be called
	 * when data has been read from the socket and is available to
	 * the application.
	 *
	 * The third argument is a callback to a function that will be
	 * called when the write buffer has reached a low watermark.
	 * That usually means that when the write buffer is 0 length,
	 * this callback will be called.  It must be defined, but you
	 * don't actually have to do anything in this callback.
	 *
	 * The fourth argument is a callback that will be called when
	 * there is a socket error.  This is where you will detect
	 * that the client disconnected or other socket errors.
	 *
	 * The fifth and final argument is to store an argument in
	 * that will be passed to the callbacks.  We store the client
	 * object here.
	 */
	client->buf_ev = bufferevent_new(client_fd, buffered_on_read,
	    buffered_on_write, buffered_on_error, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ);
}

int
main(int argc, char **argv)
{
	int listen_fd, ch;
	int daemon = 0;
	int port = SERVER_PORT;
	struct sockaddr_in listen_addr;
	struct event ev_accept;
	int reuseaddr_on;
    pid_t   pid, sid;

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    while ((ch = getopt(argc, argv, "dvp:")) != -1) {
	    switch (ch) {
	    case 'd':
	        daemon = 1;
	        break;
	    case 'v':
	        debug = 1;
	        break;
	    case 'p':
	        port = atoi(optarg);
	        break;
	    }
    }

    if (daemon) {
	    pid = fork();
	    if (pid < 0) {
			exit(EXIT_FAILURE);
	    } else if (pid > 0) {
			exit(EXIT_SUCCESS);
	    }

	    umask(0);
	    sid = setsid();
	    if (sid < 0) {
	    	exit(EXIT_FAILURE);
	    }
	}


	TAILQ_INIT(&tuples);
	TAILQ_INIT(&readers);
	
	/* Initialize libevent. */
	event_init();

	/* Create our listening socket. */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		err(1, "listen failed");
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(port);
	if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		sizeof(listen_addr)) < 0)
		err(1, "bind failed");
	if (listen(listen_fd, 5) < 0)
		err(1, "listen failed");
	reuseaddr_on = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on, 
	    sizeof(reuseaddr_on));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	event_add(&ev_accept, NULL);

	/* Start the event loop. */
	event_dispatch();
	shutdown(listen_fd, SHUT_RDWR);
	close(listen_fd);
	printf("dying\n");
	return 0;
}