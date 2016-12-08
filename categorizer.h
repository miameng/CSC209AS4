#include <ctype.h>
#include "qtree.h"

QNode *root = NULL;    
Node *interests = NULL;

char *user_prompt = "Please enter your name. Type 'q' to quit\n";
char *question_prompt = "Do you like %s? (y/n)\n";
char *neg_result = "Sorry, no users with similar interests joined yet\n\n";
char *pos_result1 = "friend recommendation for user %s:\n";
char *pos_result2 = "You have total %d potential friend(s)!!!\n\n";

void print_greetings();
int validate_user(char *name);
int validate_answer(char *answer);
void wrap_up();
void print_friends(Node *list, char *name);
char* get_opposite_friends(QNode *root, int* answer);
char* get_opposite_friends(Node *list);


char* return_question(Node *root, int question_number);
void store_user(Node* root, int* answer);
