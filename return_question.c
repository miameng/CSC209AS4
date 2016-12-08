char *prompt_1 = "Do you like ";
char *prompt_2 = "? (y/n)\n";


char* return_question(Node *root, int question_number){
    char question[MAX_LINE];
    char *interests;


    Node *current;
    current = root;

    while(question_number > 1){
        current = current->next;
        question_number--;
    }

    interests = current->str;

    strcat(question, prompt_1);
    strcat(question, interests);
    strcat(question, prompt_2);
    printf("%s\n", question);

    return question;
}