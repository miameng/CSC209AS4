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
#include "utils.h"

// -------QQQQQ: without defining the port, I can still connect it-------QQQQQQ
// --- define the port that the server looking forward to listen to
#ifndef PORT
  #define PORT 56288
#endif

Client *head = NULL;
char userinput[BUFFER_SIZE];
QNode *root = NULL;
Node *interests = NULL;
char *askname = "What is your user name?\n";
char *greeting = "Welcome\nGo ahead and enter user commands>\n";
char *goodbye = "Goodbye\n";
void addclient(int fd, struct in_addr add);
void removeclient(int fd);


int main(int argc, char **argv)
{
	Client *curr;
	int cmd_argc;
	char **cmd_argv;
	int cmdresult;
	extern void newconnection(int serv_socket_fd);

	// argument check
	if (argc < 2) {
        printf ("Usage: mismatch_server text_file\n");
        return 1;
    }

	// defining a socket
	int serv_socket_fd; // the file descriptor for server
	if((serv_socket_fd = socket(AF_INET, SOCK_STREAM,0)) < 0){
		perror("socket");
		exit(1);
	}

	// asigning an address for the socket
	struct sockaddr_in server_addr; // declaring variable 'server_addr' of type socketaddr_un
	memset(&server_addr,'\0',sizeof(server_addr)); // clear all bytes to zero
	server_addr.sin_family = AF_INET; // set up address family NOTE: change to sin_family if this doesn't work
	server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

	// bind
	if(bind(serv_socket_fd, (struct socketaddr *)&server_addr, sizeof server_addr)){
		perror("bind");
		return(1);
	}

	// listen
	if (listen(serv_socket_fd, 5)){
		perror("listen");
		return(1);
	} else {
		printf("listening on port %d\n", PORT);
	}

	// ------ add this part before the program exit 
	// ------ so that the port will be released as soon as your server
	// ------ process terminates
	int on = 1;
	int status = setsockopt(serv_socket_fd, SOL_SOCKET, SO_REUSEADDR,
		(const char *) &on, sizeof(on));
	if(status == -1) {
		perror("setsockopt -- REUSEADDR");
	}

	// accept
	while(1){
		// 
		// ############# get the name of the client ######
		// the max of the file descriptor, if not connected to any client,
		// the max should be the fd of the server
	    int maxfd = serv_socket_fd;
	    fd_set fdlist;
		FD_ZERO(&fdlist);	// set fdlist to zero
		FD_SET(serv_socket_fd, &fdlist);
		// loop through client linked list to add fd
		for (curr = head; curr; curr = curr->next) {
		    FD_SET(curr->fd, &fdlist);
		    if (curr->fd > maxfd)
				maxfd = curr->fd;
		}
		// select fd
		// select will do some changes on the fdlist, and will only keep 
		// those who is speaking to the server
		if (select(maxfd+1, &fdlist, NULL, NULL, NULL) < 0){
			perror("select");
		} else {
			for (curr = head; curr; curr = curr->next)
				if (FD_ISSET(curr->fd, &fdlist))
					break;
			// if curr equals to null, this means we've loop through the linkedlist
			// and none of the client is talking to the server

			// curr exists, there is someone talking to the server	
			if (curr){
				// case with existing connection
				// already accepted 
				if (curr->state == 0) {
					// get user name
					char username[MAX_NAME];
					// return how many bytes have been read in
					int len = read(curr->fd, username, sizeof username);
					if (len < 0){
				    	perror("read");
				    } else if (len == 0) {
				    	// innet_ntoa: turn an address into a string 
						removeclient(curr->fd);
				    } else{
					    if (len > 128)
					    	username[127] = '\0';
					    strcpy(curr->username, username);
						curr->state = 1;
						write(curr->fd, greeting, strlen(greeting));
					}
				} else if (curr->state == 1){
					int len = read(curr->fd, userinput, sizeof userinput);
					if (len < 0){
				    	perror("read");
				    } else if (len == 0) {
				    	// innet_ntoa: turn an address into a string 
						removeclient(curr->fd);
				    } else{
						cmd_argc = tokenize(userinput, &cmd_argv);
						cmdresult = process_args(cmd_argc, &cmd_argv, &root, interests, curr, head);
						switch (cmdresult){
							case -1:
								write(curr->fd, goodbye, strlen(goodbye));
								removeclient(curr->fd);
								close(curr->fd);
							case 0:
								
						}
					}
				}
				else {
					
				}
			}
			// if the fd of the server is in the list, means that there are new
			// clients 
			if (FD_ISSET(serv_socket_fd, &fdlist)){
				// case add new connection
				newconnection(serv_socket_fd);
				write(head->fd, askname, strlen(askname));
			}
		}
		
	}
	return(0);
}


void addclient(int fd, struct in_addr add){
	Client *p = malloc(sizeof(Client));
	if(!p){
		fprintf(stderr, "fail to allocate memory\n");
		exit(1);
	}
	p->fd = fd;
	p->ipaddr = add;
	p->state = 0;
	p->next = head;
	head = p;

	printf("Adding client %s\n", inet_ntoa(add));
}


void removeclient(int fd){
	/*
	Client *p = head;
	// -----------QQQ try to find a way to reduce the running time
	while(p != NULL){
		p = p->next;
		if (p->fd == fd){ 
			Client *temp;
			temp = p->next;
			free(p);
			p = temp;
		}
	}
	printf("Removing client #######");*/
    Client **p;
    //------------- loop over the whole linked list, and set the pointer pointing to the node I wanna remove
    for (p = &head; *p && (*p)->fd != fd; p = &(*p)->next);
	//------------- for ( p = *top p && p->fd != fd; p = p->next)
    if (*p) {
		Client *t = (*p)->next;
			printf("Removing client %s\n", inet_ntoa((*p)->ipaddr));
			free(*p);
		*p = t; //------------because we have to free the memory of the node *p originally pointing to 
    } else {
		fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
    }

}

void newconnection(int serv_socket_fd){
	struct sockaddr_in client_addr;
	socklen_t socklen = sizeof client_addr;
	// store the fd returned by the accept and ask fot their name
	int client_fd; 
	if ((client_fd = accept(serv_socket_fd, (struct sockaddr *)&client_addr, &socklen)) < 0) {
		perror("accept");
	}
	printf("connection from %s\n", inet_ntoa(client_addr.sin_addr));
	addclient(client_fd, client_addr.sin_addr);
}
/*
void getname(struct ){
	printf("What is your username?");
	scanf("%s", client_addr.username);
}
*/
