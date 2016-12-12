#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "questions.h"

char *prompt_1 = "Do you like ";
char *prompt_2 = "? (y/n)\n";

//global variable for total number of question
int NUM_QUESTION = 0;

Node * get_list_from_file (char *input_file_name) {
    Node * head = NULL;
    char line[MAX_LINE];

    
    // open input file
    FILE *f = fopen(input_file_name, "r");
        if (!f){
        perror(input_file_name);
        exit(1);
    }
    
    // keep track of the last node in the linked list 
    // (since we insert at the end)
    Node *end = head;
    
    // read lines and add to the list
    while (fgets(line, MAX_LINE, f) != NULL){
        
        //update the total number of question
        NUM_QUESTION++;

        // remove end-of-line characters
        line[strcspn(line, "\r\n")] = '\0';
        
        // create new node
        Node *new = malloc(sizeof(Node));
        if (!new){
            perror("malloc");
            exit(1);
        }
        
        char *newstr = malloc((strlen(line)+1)*sizeof(char));
        if (!newstr){
            perror("malloc");
            exit(1);
        }
        strcpy(newstr, line);
        
        new->str = newstr;
        new->next = NULL;
        
        // add node to end of linked list
        if (!end){ // empty list
            head = new;
            end = head;
            
        } else {
            end->next = new;
            end = new;
        }
    }
    
    fclose(f);
    
    return head;
}

void print_list (Node *head) {
    // we use head as pointer to each successive node in the list
    for (; head; head = head->next)
        printf("%s\n", head->str);
}

char* return_question(Node *root, int question_number){

    char *interests;


    Node *current;
    current = root;

    while(question_number > 0){
        current = current->next;
        question_number--;
    }

    interests = current->str;

    char *question = malloc(sizeof(char)*(strlen(prompt_1) + strlen(prompt_2) + 
                        strlen(interests) + 1));


    strcpy(question, prompt_1);
    strcat(question, interests);
    strcat(question, prompt_2);
    question[strlen(question)] = '\0';

    return question;
}


void free_list (Node *head) {
    Node *next;

    // we use head as pointer to each successive node in the list    
    for (; head; head = next){
        // we need to keep track of the next pointer, since we cannot
        // access it through head once head has been freed
        next = head->next; 
        
        // free the node string
        free(head->str);
        // free the node itself
        free(head);
    }
}
