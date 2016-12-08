

QNode *root = NULL;    
Node *interests = NULL;

char *user_prompt = "Please enter your name. Type 'q' to quit\n";
char *question_prompt = "Do you like %s? (y/n)\n";
char *neg_result = "Sorry, no users with similar interests joined yet\n\n";
char *pos_result1 = "friend recommendation for user %s:\n";
char *pos_result2 = "You have total %d potential friend(s)!!!\n\n";




char* return_question(Node *root, int question_number){
    
<<<<<<< HEAD
    char answer[MAX_LINE];    
    char name[MAX_LINE];
    
    if (argc < 2) {
        printf ("To run the program ./categorizer <name of input file>\n");
        return 1;
=======
    char *interests;
    char *prompt_1 = "Do you like ";
    char *prompt_2 = "? (y/n)\n";

    Node *current;
    current = root;

    while(question_number != 0){
        current = current->next;
        question_number--;
>>>>>>> 9bd00c9d64c506d52f687f114b0b68368e3613d8
    }

    interests = current->str;

    strcat(prompt_1, interests);
    strcat(prompt_1, prompt_2);
    return promtp_1;
}

//the index!!
void store_user(QNode* root, int* answer){
    QNode* current = root;
    Node* user_list;
    int total_num_question = NUM_QUESTION;
    int num = 0;
    while(answer_number != 0){
        current = find_branch(current, answer[num]);
        num++;
        total_num_question--;
    }

    user_list = current->children[answer[num]].fchild;
    current->children[answer[num]].fchild = add_user(user_list, name);
}




void print_greetings () {
    printf ("----------------------------------------------\n");
    printf ("Friend recommender system. Find people who are just like you!\n");
    printf ("CSC209 fall 2016 team. All rights reserved\n");
    printf ("----------------------------------------------\n");
}


void wrap_up(){
    //end of main loop - the user typed "q"
    print_qtree (root, 0);
    
    free_list (interests);
    free_qtree(root);
    
    exit(0);
}


int validate_user(char *name){
    int valid = 1;
    
    int len = strlen(name);
    if (len < 8 || len > 128)
        valid = 0;
    
    for (int i = 0; i < len; i++){
        if (!isalnum(name[i])){
            valid = 0;
            break;
        }
    }

    if (!valid)
        fprintf(stderr, "ERROR: User name must be at least 8 and at most "
            "128 characters, and can only contain alphanumeric "
            "characters.\n\n");

    return valid;
}


int validate_answer(char *answer){
    char *invalid_message = "ERROR: Answer must be one of 'y', 'n', 'q'.\n";
    
    if (strlen(answer) > 3){
        printf("%s", invalid_message);
        return 2;
    }
    
    if (answer[0] == 'q' || answer[0] == 'Q')
        wrap_up();
        
    if (answer[0] == 'n' || answer[0] == 'N')
        return 0;
        
    if (answer[0] == 'y' || answer[0] == 'Y')
        return 1;
        
    printf("%s", invalid_message);
    return 2;
}


//get the mismatch of the current client
//call the print_opposite_friends
char* get_opposite_friends(QNode *root, int* answer){
    Node* opp_user_list;
    QNode* current = root;
    int num = 0;
    int answer_number = NUM_QUESTION;
    while(answer_number != 0){
        current = find_branch(current, (1- answer[num]));
        num++;
        answer_number--;
    }

    opp_user_list = current->children[1-answer[num]].fchild;

    return get_opposite_friends_helper(opp_user_list);

}



//print the list of opposite friends for user
char* get_opposite_friends_helper(Node *list){
    char * prompt_1 = "No completing personalities found. Please try again later\n";
    char * prompt_2 = "Here are your mismatches:\n";

    if(list == NULL){
        return promtpt_1;
    }
    else{
        printf("Here are your best mismatches:\n");
        while(list){
            strcat(prompt_2, list->str);
            strcat(prompt_2, ", ");

	        list = list->next;
        }
        strcat(prompt_2, "\n");
        return prompt_2;
    }
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
