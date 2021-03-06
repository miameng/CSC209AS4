#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
char *collect = "Collecting your interests\n";
char *do_test_first = "Please finish the test first. Type in 'do_test'!\n";
char *test_complete = "Test Completed!\n";
char *syntaxerror = "Incorrect syntax\n";
char *testmsg = "Iamhere\n";
char *post_msg_err = "Did not find the client, please try again.\n";

/*
 * Print a formatted error message to stderr.
 */
void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

/*
 * Read and process commands
 */

int process_args(int cmd_argc, char **cmd_argv, QNode **root, Node *interests,
		 struct client *current_client, struct client *head) {
	QNode *qtree = *root;
	printf("command argument %d\n", cmd_argc);

	if (cmd_argc <= 0) {
		return -4;

	} else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
		/* Return an appropriate value to denote that the specified
		 * user is now need to be disconnected. */
		return -1;

	} else if (strcmp(cmd_argv[0], "do_test") == 0 && cmd_argc == 1) {
		/* The specified user is ready to start answering questions. You
		 * need to make sure that the user answers each question only
		 * once.
		 */
		 
		//if the test was done already
		if (current_client->state > NUM_QUESTION)
			return -3;
		//collect string to print

		write(current_client->fd, collect, strlen(collect));
		//write the first question
		
		char * first_question; 
		first_question = return_question(interests, current_client->state);
		write(current_client->fd, first_question, strlen(first_question));

		free(first_question);
		
		//int curr_state = current_client->state;

		current_client->state++;
	
		return 1;


	} else if (strcmp(cmd_argv[0], "get_all") == 0 && cmd_argc == 1) {
		/* Send the list of best mismatches related to the specified
		 * user. If the user has not taked the test yet, return the
		 * corresponding error value (different than 0 and -1).
		 */

		 if (current_client->state < NUM_QUESTION + 1) {
		 	// client need to finish tests first
		 	//#######
		 	write(current_client->fd,do_test_first, strlen(do_test_first));

		 } else {

		 	char * opp_friend_list;
		 	opp_friend_list = get_opposite_friends(qtree, current_client->answer);

		 	write(current_client->fd, opp_friend_list, strlen(opp_friend_list));
		 	if (!opp_friend_list)
		 		free(opp_friend_list);
		 }
		 return 1;

	} else if (strcmp(cmd_argv[0], "post") == 0 && cmd_argc == 3) {
		/* Send the specified message stored in cmd_argv[2] to the user
		 * stored in cmd_argv[1].
		 */
		printf("GET INTO THE POST\n");

		char* test = "abc";

		if(write(current_client->fd, test, strlen(test))<0){
			perror("write");
		}
	


		 if(current_client->state < NUM_QUESTION + 1){
			printf("wtf??didnt get in???\n");
			char* cantwrite = "not able to write??\n";

			write(current_client->fd,cantwrite, strlen(cantwrite));

		 	// client need to finish tests first
		 	write(current_client->fd, do_test_first, strlen(do_test_first));

			printf("problem with write?\n");
			return -2;
		 } else {
		 	Client *opp = head;
			printf("tst in post, %s\n", opp->username);
		 	while((opp) && strcmp(opp->username, cmd_argv[1])){
		 		opp = opp->next;
				printf("test if get into the loop in post\n");
		 	}

			if(opp==NULL){
				write(current_client->fd, post_msg_err, strlen(post_msg_err));
			}
			else{
		 		write(opp->fd, cmd_argv[2], strlen(cmd_argv[2]));

			}
		}
		printf("finished post\n");
		return 0;

	} else if (current_client->state <= NUM_QUESTION && validate_answer(cmd_argv[0]) != 2 && cmd_argc == 1) {

		//QNode *prev;
		int ans;
        	//prev = qtree;	//??
 	       ans = validate_answer(cmd_argv[0]);
		current_client->answer[current_client->state-1] = ans;

		//printf("%d\n", current_client->answer[current_client->state-1]);
	        //printf("%d\n", current_client->state);
	        //printf("%d\n", ans);
	        //questions following the first question
	        if(current_client->state == NUM_QUESTION){
        		// user have completed all tasks
        		write(current_client->fd, test_complete, strlen(test_complete));
        		current_client->state++;
			printf("%d, check state\n", current_client->state);
        		store_user(*root, current_client->username, current_client->answer);

        		// Print tree for testing purpose
        		//print_qtree(*root, 0);
        }
        else{
        	char* question;
        	question = return_question(interests, current_client->state);

        	//ask client the question
        	write(current_client->fd, question, strlen(question));
        	free(question);
        	current_client->state++;
        }

        return 1;
	}
	else {
		/* The input message is not properly formatted. */
		//error("Incorrect syntax");
		write(current_client->fd, syntaxerror, strlen(syntaxerror));
		return 0;
	}
	return 0;
}

/*
 * Tokenize the command stored in cmd.
 * Return the number of tokens, and store the tokens in cmd_argv.
 */

// we should invoke tokenize after handle the argument
// right after getting the user name
// pass the rest of the argument into the process_args()
int tokenize(char *cmd, char **cmd_argv) {
    int cmd_argc = 0;
    char *next_token = strtok(cmd, DELIM);

    while (next_token != NULL) {
        cmd_argv[cmd_argc] = next_token;
        ++cmd_argc;

	if(cmd_argc < (INPUT_ARG_MAX_NUM - 1))
	    next_token = strtok(NULL, DELIM);
	else
	    break;
    }

    if (cmd_argc == (INPUT_ARG_MAX_NUM - 1)) {
	cmd_argv[cmd_argc] = strtok(NULL, "");
	if(cmd_argv[cmd_argc] != NULL)
	    ++cmd_argc;
    }

    return cmd_argc;
}


int validate_answer(char *answer){
//    char *invalid_message = "ERROR: Answer must be one of 'y', 'n', 'q'.\n";
    
    if (strlen(answer) > 3){
       // printf("%s", invalid_message);
        return 2;
    }
 
    if (answer[0] == 'n' || answer[0] == 'N')
        return 0;
        
    if (answer[0] == 'y' || answer[0] == 'Y')
        return 1;
   // printf("%s", invalid_message);
    return 2;
}

void store_user(QNode* root, char* name, int* answer){
    QNode *prev, *current;
    prev = current = root;
    Node* user_list;
    int total_num_question = NUM_QUESTION;
    int num = 0;
    while(total_num_question > 0){
    	prev = current;
    	//printf("%d\n", answer[num]);
        current = find_branch(current, answer[num]);
        num++;
        total_num_question--;
        //printf("%d -- test for store user", num);
    }
    num--;
   // printf("%d\n", num);
    user_list = prev->children[answer[num]].fchild;
    prev->children[answer[num]].fchild = add_user(user_list, name);
}
