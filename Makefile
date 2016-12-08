#Assignment 4
CC = gcc
PORT= 56288
CFLAGS+= -DPORT=\$(PORT) -Wall -std=c99

mismatch_server:
		$(CC) $(CFLAGS) mismatch_server.c utils.c questions.c qtree.c categorizer.c -o mismatch_server 
		 
clean:  
		rm mismatch_server 
