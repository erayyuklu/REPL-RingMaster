#ifndef QUESTION
#define QUESTION

#include "struct.h"
#include "utils.c"

int is_question(char **tokens, int token_count);
int get_question_type(char **tokens, int token_count);
int question_total(char **tokens, int token_count);
int question_where(char **tokens, int token_count);
int question_who_at(char **tokens, int token_count);
int question_inventory(char **tokens, int token_count);
void execute_question(char **tokens, Subject **all_subjects, int *all_subjects_count, int token_count);

#endif