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