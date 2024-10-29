#ifndef SENTENCE
#define SENTENCE

#include "state.h"
#include "struct.h"
#include "utils.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_UNDERLINE "\x1b[4m"

Sentence **parse_sentences(char **tokens, int token_count, Subject **all_subjects, int *all_subject_count);

void execute_sentences(char **tokens, Subject **all_subjects, int *all_subject_count, int token_count);
void execute_sentence(Sentence *sentence, Subject **all_subjects, int *all_subject_count);
void execute_action(Action *action);
void execute_actions(Action **actions, int action_count);

int check_condition(Condition *condition, Subject **all_subjects, int *all_subject_count);
int check_conditions(Condition **conditions, int condition_count, Subject **all_subjects, int *all_subject_count);

void print_sentence(Sentence *sentence);
void print_action(Action *action);
void print_condition(Condition *condition);

const char *get_action_type_name(ActionType type);
const char *get_condition_type_name(ConditionType type);

#endif