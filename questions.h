#ifndef QUESTIONS_H
#define QUESTIONS_H

#define MAX_LINE 256

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct str_node {
	char *str;
	struct str_node *next;
} Node;

int NUM_QUESTION;
Node * get_list_from_file (char *input_file_name);
char* return_question(Node *root, int question_number);
void print_list (Node *current);
void free_list (Node *head);

#endif