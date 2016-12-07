#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>


// -------QQQQQ: without defining the port, I can still connect it-------QQQQQQ
// --- define the port that the server looking forward to listen to
#ifndef PORT
  #define PORT 56288
#endif

// defining a struct for client
//struct client *head = NULL;
struct client {
    int fd;
    struct in_addr ipaddr;
    char username[128];
    struct client *next;
} *head = NULL;

void addclient(int fd, struct in_addr add);
void removeclient(int fd);


int main(int argc, char **argv)
{
	// argument check


	// defining a socket
	int serv_socket_fd; // the file descriptor for server
	int port = 56288;
	if((serv_socket_fd = socket(AF_INET, SOCK_STREAM,0)) < 0){
		perror("socket");
		exit(1);
	}

	// asigning an address for the socket
	struct sockaddr_in server_addr; // declaring variable 'server_addr' of type socketaddr_un
	memset(&server_addr,'\0',sizeof(server_addr)); // clear all bytes to zero
	server_addr.sin_family = AF_INET; // set up address family NOTE: change to sin_family if this doesn't work
	server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

	// bind
	//struct sockaddr_un serv_addr; // ------------is it the same as line 32????
	if(bind(serv_socket_fd,(struct socketadd*)&server_addr,sizeof(server_addr))){
		perror("bind");
		return(1);
	}

	// listen
	if (listen(serv_socket_fd, 5)){
		perror("listen");
		return(1);
	} else {
		printf("listening on port %d", port);
	}

	// accept
	while(1){
		struct sockaddr_in client_addr;
		int len = sizeof (client_addr); 
		// store the fd returned by the accept and ask fot their name
		int client_fd; 
		if ((client_fd = accept(serv_socket_fd, (struct sockaddr *)&client_addr, &len)) < 0) {
			perror("accept");
			return(1);
		}
		// ############# get the name of the client ######
		printf("What is your username?");
		scanf("%s", client_addr.username);

		printf("connection from %s\n", inet_ntoa(client_addr.sin_addr));
		addclient(client_fd, client_addr.sin_addr);
		int tttt;
		while (1) {
		    fd_set fdlist;
		    struct timeval tv;
		    FD_ZERO(&fdlist);
		    FD_SET(client_fd, &fdlist);
		    tv.tv_sec = 15;
		    tv.tv_usec = 0;
		    switch (select(client_fd+1, &fdlist, NULL, NULL, &tv)){
				case 0:
					printf("abcd\n");
					removeclient(client_fd);
					break;
				case -1:
					perror("select\n");
					break;
				default:
					printf("hahaha\n");
		    }	    
		}
	}

	// ------ add this part before the program exit 
	// ------ so that the port will be released as soon as your server
	// ------ process terminates
	int on = 1;
	int status = setsockopt([sock_fd], SOL_SOCKET, SO_REUSEADDR,
		(const char *) &on, sizeof(on));
	if(status == -1) {
		perror("setsockopt -- REUSEADDR");
	}
	return(0);
}

void addclient(int fd, struct in_addr add){
	struct client *p = malloc(sizeof(struct client));
	if(!p){
		fprintf(stderr, "fail to allocate memory\n");
		exit(1);
	}
	p->fd = fd;
	p->ipaddr = add;

	p->next = head;
	head = p;

	printf("Adding client %s\n", inet_ntoa(add));
}


void removeclient(int fd){
	struct client *p = &head;
	// -----------QQQ try to find a way to reduce the running time
	while(p != NULL){
		p = p->next;
		if (p->fd == fd){ 
			struct client *temp;
			temp = p->next;
			free(p);
			p = temp;
		}
	}
	printf("Removing client #######");

}





