#include "question.h"

// Checks if the last token is a question mark.
int is_question(char **tokens, int token_count) {
    if (strcmp(tokens[token_count - 1], "?") == 0) {
        return 1;
    }
    return 0;
}

// Decides the type of question based on the tokens.
int get_question_type(char **tokens, int token_count) {
    if (question_total(tokens, token_count) != 0) {
        return 1;
    } else if (question_where(tokens, token_count) != 0) {
        return 2;
    } else if (question_who_at(tokens, token_count) != 0) {
        return 3;
    } else if (question_inventory(tokens, token_count) != 0) {
        return 4;
    }
    return 0;
}

/**
 * The functions below (question_...) evaluate if the given tokens represent a question of a certain type.
 * If the tokens represent a question of that type, the function returns 1. Otherwise, it returns 0.
 * Then, the "execute_question" function is called with the appropriate arguments.
 */

// <Subject(s)> total Item ?
int question_total(char **tokens, int token_count) {
    if (token_count > 3 && token_count % 2 == 0 && strcmp(tokens[token_count - 3], "total") == 0 && is_valid_item(tokens[token_count - 2])) {
        for (int i = 0; i < token_count - 3; i++) {
            if (i % 2 == 0 && is_valid_subject(tokens[i]) == 0) {
                return 0;
            } else if (i % 2 == 1 && strcmp(tokens[i], "and") != 0) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

// <Subject> where ?
int question_where(char **tokens, int token_count) {
    if (token_count == 3 && strcmp(tokens[1], "where") == 0 && is_valid_subject(tokens[0])) {
        return 1;
    }
    return 0;
}

// who at <Location> ?
int question_who_at(char **tokens, int token_count) {
    if (token_count == 4 && strcmp(tokens[0], "who") == 0 && strcmp(tokens[1], "at") == 0 && is_valid_location(tokens[2])) {
        return 1;
    }
    return 0;
}

// <Subject> total ?
int question_inventory(char **tokens, int token_count) {
    if (token_count == 3 && strcmp(tokens[1], "total") == 0 && is_valid_subject(tokens[0])) {
        return 1;
    }
    return 0;
}

// Executes the appropriate function based on the question type.
void execute_question(char **tokens, Subject **all_subjects, int *all_subjects_count, int token_count) {
    switch (get_question_type(tokens, token_count)) {
    // <Subject(s)> total Item ?
    case 1: {
        int subject_count = token_count - 3;
        char *item_name = tokens[token_count - 2];
        int total_count = 0;
        for (int i = 0; i < subject_count; i += 2) {
            char *name = tokens[i];
            if (!does_exist(name, all_subjects, (*all_subjects_count))) {
                create_subject(name, all_subjects, all_subjects_count);
            }
            int one_subjects_count = get_item_count_for_subject(all_subjects[get_index_of_subject(name, all_subjects, (*all_subjects_count))], item_name);
            total_count += one_subjects_count;
        }
        printf("%d\n", total_count);
        break;
    }

    // <Subject> where ?
    case 2: {
        char *name = tokens[0];
        if (does_exist(name, all_subjects, (*all_subjects_count))) {
            printf("%s\n", get_subject_location(all_subjects[get_index_of_subject(name, all_subjects, (*all_subjects_count))]));
        } else {
            create_subject(name, all_subjects, all_subjects_count);
            printf("NOWHERE\n");
        }
        break;
    }

    // who at <Location> ?
    case 3: {
        char *location = tokens[token_count - 2];
        Subject **subjects = malloc((*all_subjects_count) * sizeof(Subject *));
        int count = get_subjects_from_location(all_subjects, (*all_subjects_count), location, subjects);
        if (count == 0) {
            printf("NOBODY\n");
        } else {
            for (int i = 0; i < count; i++) {
                if (i != count - 1) {
                    printf("%s and ", subjects[i]->name);
                } else {
                    printf("%s\n", subjects[i]->name);
                }
            }
        }
        free(subjects);
        break;
    }

    // <Subject> total ?
    case 4: {
        char *name = tokens[0];
        if (does_exist(name, all_subjects, (*all_subjects_count))) {
            print_inventory(all_subjects[get_index_of_subject(name, all_subjects, (*all_subjects_count))]);
        } else {
            create_subject(name, all_subjects, all_subjects_count);
            printf("NOTHING\n");
            break;
        }
        break;
    }
    default: {
        printf("INVALID\n");
        break;
    }
    }
}
