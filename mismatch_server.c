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
#include "questions.h"
#include "categorizer.h"
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
char *goodbye = "Goodbye!!\n";
char *errormsg = "The command is not valid at this stage.\n";
char *emptymsg = "No command is recieved\n";
//char *collect = "Collecting your interests\n";
char *havetest = "You have already done your test\n";
char *neg_result = "Sorry, no users with similar interests joined yet\n\n";
char *pos_result1 = "friend recommendation for user %s:\n";
char *pos_result2 = "You have total %d potential friend(s)!!!\n\n";
void addclient(int fd, struct in_addr add);
void removeclient(int fd);


int main(int argc, char **argv)
{
	Client *curr;
	int cmd_argc;
	char *cmd_argv;
	int cmdresult;
	extern void newconnection(int serv_socket_fd);

	// argument check
	if (argc < 2) {
        printf ("Usage: mismatch_server text_file\n");
        return 1;
    }
    // get interest
    interests = get_list_from_file(argv[1]);

    // build question tree
    root = add_next_level (root,  interests);

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
				if (curr->state == -1) {
					// get user name
					char username[MAX_NAME];
					// return how many bytes have been read in
					// int len = read(curr->fd, username, sizeof username);
					int len = read_from_client(username, curr);
					if (len < 0){
				    	perror("read");
				    } else if (len == 0) {
				    	// innet_ntoa: turn an address into a string 
						removeclient(curr->fd);
				    } else{
					    if (len > 128)
					    	username[127] = '\0';
					    strcpy(curr->username, username);
						curr->state = 0;
						write(curr->fd, greeting, strlen(greeting));
					}
				} else if (curr->state <= NUM_QUESTION){
					// user is answering questions
					//int len = read(curr->fd, userinput, sizeof userinput);
					int len = read_from_client(userinput, curr);
					if (len < 0){
				    	perror("read");
				    } else if (len == 0) {
				    	// innet_ntoa: turn an address into a string 
						removeclient(curr->fd);
				    } else{
						cmd_argc = tokenize(userinput, &cmd_argv);
			//			printf("%d\n", cmd_argc);
						printf("%s\n", userinput);
			//			printf("%s\n", curr->username);
			//			printf("%d\n", curr->fd);
			//			printf("%s\n", cmd_argv[0]);
			//			printf("%d ---\n", curr->state);

						cmdresult = process_args(cmd_argc, userinput, &cmd_argv, &root, interests, curr, head);
						switch (cmdresult){
							case -1:
								write(curr->fd, goodbye, strlen(goodbye));
								removeclient(curr->fd);
								close(curr->fd);
								break;
							case -2:
								write(curr->fd, errormsg, strlen(errormsg));
								break;
							case -3:
								write(curr->fd, havetest, strlen(havetest));
								break;
							case -4:
								write(curr->fd, emptymsg, strlen(emptymsg));
								break;
							case 1:
								//printf("");
								break;
								//write(curr->fd, collect, strlen(collect));
							default:
								break;
						}
					}
				} else {
					// user already finished tests
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
	p->state = -1;
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
			fflush(stdout);
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

void wrap_up(){
    //end of main loop - the user typed "q"
    print_qtree (root, 0);
    
    free_list (interests);
    free_qtree(root);
    
    exit(0);
}

// print list of potential friends for user
void print_friends(Node *list, char *name){
    int friends = 0;

    // iterate over user list and count the number of friends
    while (list) {
	// ignore this user
        if (strcmp(list->str, name)) {
            friends++;
             
	    // if this is the first friend found, print successful message    
            if (friends == 1)
                printf(pos_result1, name);
            
	    // if friend was found, print his/her name
            printf("%s, ", list->str);
        }
            
        list = list->next;
    }
    
    // if friends were found, print the number of friends    
    if (friends){
        printf("\n");
        printf(pos_result2, friends);
        
    } else {
        printf("%s", neg_result);    
    }
}


int find_network_newline (char *buf, int inbuf) {
	int i;
	for (i = 0; i < inbuf - 1; i++)
		if ((buf[i] == '\r') && (buf[i + 1] == '\n'))
			return (i);
	return -1;
}

int read_from_client(char* userinput, Client curr){
	//userinput = curr.buf + curr.inbuf;
	int room = BUFFER_SIZE - curr.inbuf;
	int nbytes;
	//read next message into remaining room in buffer
	if ((nbytes = read(curr.fd, userinput, room)) > 0) {
		curr.inbuf += nbytes;
		int where = find_network_newline (curr.buf, curr.inbuf); //find new line
		if (where >= 0) {
		curr.buf[where] = '\0'; curr.buf[where+1] = '\0';
		//do_command (buf); //process buffer up to a new line
		where+=2;
		curr.inbuf -= where;
		memmove (curr.buf, curr.buf + where, curr.inbuf);
		}
	}
	return nbytes;
}


