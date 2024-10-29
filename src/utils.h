#ifndef UTILS
#define UTILS

#include <ctype.h>
#include <string.h>
#include "struct.h"

char **tokenize(char *line, int *token_count);

int is_keyword(char *word);
int is_valid_subject(char *name);
int is_valid_item(char *name);
int is_valid_location(char *name);
int is_inventory_alterable(Subject *s, char *item_name, int count);

int get_nonnegative_number(const char *str);
int get_index_of_subject(char *subject_name, Subject **all_subjects, int all_subjects_count);
int get_item_count_for_subject(Subject *s, char *item_name);
int get_length(char **array);
int get_index_of_item(Subject *s, char *item_name);
int get_subjects_from_location(Subject **all_subjects, int all_subjects_count, char *location, Subject **subjects);

char *get_subject_location(Subject *s);

int does_exist(char *subject_name, Subject **all_subjects, int all_subjects_count);
int does_exist_item(char *item_name, Subject *s);
int does_exist_item_in_action(char *item_name, Action *a);
int does_exist_item_in_condition(char *item_name, Condition *c);
int does_exist_subject_in_action(char *subject_name, Action *a);
int does_exist_subject_in_condition(char *subject_name, Condition *c);

int create_subject(char *name, Subject **all_subjects, int *all_subjects_count);
void create_item_in_action(char *name, int item_num, Action *curr_action);
void create_item_in_condition(char *name, int item_num, Condition *curr_condition);

int alter_subject_location(Subject *s, char *location);
int alter_subject_location_for_condition(Condition *c, char *location);
int alter_subject_item_count(Subject *s, char *item_name, int count);
int get_subjects_from_location(Subject **all_subjects, int all_subjects_count, char *location, Subject **subjects);

void print_inventory(Subject *s);
void log_current_state(Subject **all_subjects, int all_subjects_count);

#endif