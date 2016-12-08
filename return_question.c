char *prompt_1 = "Do you like ";
char *prompt_2 = "? (y/n)\n";


char* return_question(Node *root, int question_number){
    char question[MAX_LINE];
    // the question we want to return
    char *interests;


    Node *current;
    current = root;

    while(question_number > 1){
        current = current->next;
        question_number--;
    }

    interests = current->str;

    char *question = malloc(sizeof(char)*(strlen(prompt_1) + strlen(prompt_2)+
        strlen(interests) +1));

    strcat(question, prompt_1);
    strcat(question, interests);
    strcat(question, prompt_2);
    question[strlen(question)] = '\0';

    printf("%s\n", question);

    return question;
}