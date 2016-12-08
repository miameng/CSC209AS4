#include "categorizer.h"

char* return_question(Node *root, int question_number){
    
    char *interests;
    char *prompt_1 = "Do you like ";
    char *prompt_2 = "? (y/n)\n";

    Node *current;
    current = root;

    while(question_number != 0){
        current = current->next;
        question_number--;
    }

    interests = current->str;

    strcat(prompt_1, interests);
    strcat(prompt_1, prompt_2);
    return prompt_1;
}

//the index!!
void store_user(QNode* root, char* name, int* answer){
    QNode* current = root;
    Node* user_list;
    int total_num_question = NUM_QUESTION;
    int num = 0;
    while(total_num_question != 0){
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
