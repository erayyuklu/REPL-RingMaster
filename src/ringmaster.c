#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "question.c"
#include "sentence.c"
#include "struct.h"

#define DEBUG 0

int main() {

    char line[1025];
    Subject *all_subjects[MAX_SUBJECTS];
    int all_subject_count = 0;

    while (1) {
        // Print prompt, read input, and check for exit
        printf(">> ");
        fflush(stdout);
        if (fgets(line, 1025, stdin) == NULL) {
            break;
        }
        if (strcmp(line, "exit\n") == 0) {
            break;
        }

        // Log the current state of all subjects for debugging
        if (DEBUG && strcmp(line, "lcs\n") == 0) {
            log_current_state(all_subjects, all_subject_count);
            continue;
        }

        // Tokenize the input
        int token_count;
        char **tokens = tokenize(line, &token_count);
        if (token_count == 0 || strcmp(tokens[0], "\n") == 0) {
            continue;
        }

        // Log the tokens for debugging
        if (DEBUG) {
            for (int i = 0; i < token_count; i++) {
                printf("Token %d: %s\n", i, tokens[i]);
            }
        }

        // Invoke the appropriate function based on the input tokens
        if (is_question(tokens, token_count)) {
            // Parse and execute question
            execute_question(tokens, all_subjects, &all_subject_count, token_count);
        } else {
            // Parse and execute sentences
            execute_sentences(tokens, all_subjects, &all_subject_count, token_count);
        }
    }

    return 0;
}