#include "sentence.h"

#define DEBUG 0

/*
 * Parses the given tokens into sentences, the backbone of the program.
 * Used a pseudo-state machine to parse the tokens into sentences.
 * The state of the parsing is kept in the SentenceState, ActionState, and ConditionState enums.
 * The parsing is done in a while loop, where the state is updated based on the current state and the current token.
 * Unlike an LR1 parser, the parsing is done using lookahead of multiple tokens.
 * Returns an array of sentences.
 */
Sentence **parse_sentences(char **tokens, int token_count, Subject **all_subjects, int *all_subject_count) {

    // Minimum 4 tokens per sentence => `<subject> go to <location>` or `<subject> buy <number> <itemname>`
    Sentence **sentences = (Sentence **)malloc(sizeof(Sentence *) * token_count / 4);
    int sentence_count = 0;

    // Used to return in case of an invalid sentence, allowing the caller to continue.
    Sentence **noSentences = (Sentence **)malloc(sizeof(Sentence *) * token_count / 4);

    Sentence *cur_sentence;
    Action *cur_action;
    Condition *cur_condition;

    // Initialize states
    SentenceState s = START;
    ActionState a = A_START;
    ConditionState c = C_START;

    // Index of the last occurance of a head in a condition (A Subject). Used to fall back from condition to action parsing, ending a sentence thereby.
    int last_head = 0;

    int i = 0;
    while (i < token_count + 1) {
        switch (s) {

        // Initialize sentences
        case START:

            // In case this sentence is not the first sentence, reset Action and Condition states
            a = A_START;
            c = C_START;

            // Initialize a new sentence
            cur_sentence = (Sentence *)malloc(sizeof(Sentence));

            cur_sentence->actions = (Action **)malloc(sizeof(Action *) * token_count / 4);
            cur_sentence->conditions = (Condition **)malloc(sizeof(Condition *) * token_count / 4);
            cur_sentence->action_count = 0;
            cur_sentence->condition_count = 0;

            last_head = 0;

            s = ACTION;
            break;

        // Parse Actions
        case ACTION: {
            switch (a) {
            case A_START: {
                cur_action = (Action *)malloc(sizeof(Action));
                cur_action->subjects = (Subject **)malloc(sizeof(Subject *) * 256);
                cur_action->subject_count = 0;
                cur_action->item_count = 0;
                a = A_SUBJECT;
                break;
            }
            case A_SUBJECT: {

                if (token_count == i + 1 || token_count == i) {
                    printf("INVALID\n");
                    return noSentences;
                }

                if (is_valid_subject(tokens[i]) == 0) {
                    printf("INVALID\n");
                    return noSentences;
                }

                if (does_exist(tokens[i], all_subjects, *all_subject_count) == 0) {
                    create_subject(tokens[i], all_subjects, all_subject_count);
                }

                if (does_exist_subject_in_action(tokens[i], cur_action) == 0) {
                    cur_action->subjects[cur_action->subject_count] = all_subjects[get_index_of_subject(tokens[i], all_subjects, *all_subject_count)];
                    cur_action->subject_count++;
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }

                if (strcmp(tokens[i + 1], "and") == 0) {
                    if (token_count == i + 2) {
                        printf("INVALID\n");
                        return noSentences;
                    }
                    i += 2;
                } else {
                    a = A_VERB;
                    i++;
                }
                break;
            }
            case A_VERB: {
                if (token_count == i) {
                    printf("INVALID\n");
                }
                if (strcmp(tokens[i], "go") == 0) {
                    if (strcmp(tokens[i + 1], "to") == 0) {
                        cur_action->type = GO_TO;
                        a = A_LOCATION;
                        i += 2;
                    } else {
                        printf("INVALID\n");
                        return noSentences;
                    }
                } else if (strcmp(tokens[i], "buy") == 0) {
                    cur_action->type = BUY;
                    a = A_ITEM;
                    i++;

                } else if (strcmp(tokens[i], "sell") == 0) {
                    cur_action->type = SELL;
                    a = A_ITEM;
                    i++;
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }

                break;
            }
            case A_ITEM: {
                int item_num = get_nonnegative_number(tokens[i]);
                if (item_num == -1) {
                    printf("INVALID\n");
                    return noSentences;
                } else {
                    if (tokens[i] == NULL) {
                        printf("INVALID\n");
                        return noSentences;
                    }
                    if (tokens[i + 1] == NULL) {
                        printf("INVALID\n");
                        return noSentences;
                    } else if (is_valid_item(tokens[i + 1]) == 0) {
                        printf("INVALID\n");
                        return noSentences;
                    } else if (does_exist_item_in_action(tokens[i + 1], cur_action) == 0) {
                        create_item_in_action(tokens[i + 1], item_num, cur_action);

                        if (tokens[i + 2] == NULL || strcmp(tokens[i + 2], "if") == 0) {
                            i += 2;
                            a = A_END;
                        } else if (strcmp(tokens[i + 2], "and") == 0 && get_nonnegative_number(tokens[i + 3]) != -1) {
                            i += 3;
                        } else if (strcmp(tokens[i + 2], "and") == 0 && get_nonnegative_number(tokens[i + 3]) == -1) {
                            i += 2;
                            a = A_END;
                        } else if (strcmp(tokens[i + 2], "to") == 0 && cur_action->type == SELL) {
                            cur_action->type = SELL_TO;
                            a = A_O_SUBJECT;
                            i += 3;
                        } else if (strcmp(tokens[i + 2], "from") == 0 && cur_action->type == BUY) {
                            cur_action->type = BUY_FROM;
                            a = A_O_SUBJECT;
                            i += 3;
                        } else {
                            printf("INVALID\n");
                            return noSentences;
                        }
                    } else {
                        printf("INVALID\n");
                        return noSentences;
                    }
                }

                break;
            }
            case A_LOCATION: {
                if (token_count == i) {
                    printf("INVALID\n");
                    return noSentences;
                }
                if (is_valid_location(tokens[i]) == 0) {
                    printf("INVALID\n");
                    return noSentences;
                }
                char *p = malloc(strlen(tokens[i]) + 1);
                strcpy(p, tokens[i]);
                cur_action->location = p;
                a = A_END;
                i++;
                break;
            }
            case A_O_SUBJECT: {
                if (token_count == i) {
                    printf("INVALID\n");
                    return noSentences;
                }
                if (is_valid_subject(tokens[i]) == 0) {
                    printf("INVALID\n");
                    return noSentences;
                }
                if (does_exist(tokens[i], all_subjects, *all_subject_count) == 0) {
                    create_subject(tokens[i], all_subjects, all_subject_count);
                }

                if (cur_action->type == SELL_TO && does_exist_subject_in_action(tokens[i], cur_action) == 0) {
                    cur_action->to = all_subjects[get_index_of_subject(tokens[i], all_subjects, *all_subject_count)];
                } else if (cur_action->type == BUY_FROM && does_exist_subject_in_action(tokens[i], cur_action) == 0) {
                    cur_action->from = all_subjects[get_index_of_subject(tokens[i], all_subjects, *all_subject_count)];
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }

                a = A_END;
                i++;

                break;
            }
            case A_END: {
                cur_sentence->actions[cur_sentence->action_count] = cur_action;
                cur_sentence->action_count++;

                if (tokens[i] == NULL) {
                    s = END;
                    sentences[sentence_count] = cur_sentence;
                    sentence_count++;
                    return sentences;
                } else if (strcmp(tokens[i], "and") == 0) {
                    i++;
                    a = A_START;
                } else if (strcmp(tokens[i], "if") == 0) {
                    i++;
                    s = CONDITION;
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }
                break;
            }
            }
            break;
        }

        // Parse Conditions
        case CONDITION: {
            switch (c) {
            case C_START: {
                cur_condition = (Condition *)malloc(sizeof(Condition));
                cur_condition->subjects = (Subject **)malloc(sizeof(Subject *) * 256);
                cur_condition->subject_count = 0;
                cur_condition->item_count = 0;
                last_head = i; // Save the index of the last condition head.
                c = C_SUBJECT;
                break;
            }
            case C_SUBJECT: {
                if (token_count == i + 1 || token_count == i) {
                    printf("INVALID\n");
                    return noSentences;
                }

                if (is_valid_subject(tokens[i]) == 0) {
                    printf("INVALID\n");
                    return noSentences;
                }
                if (does_exist(tokens[i], all_subjects, *all_subject_count) == 0) {
                    create_subject(tokens[i], all_subjects, all_subject_count);
                }

                if (does_exist_subject_in_condition(tokens[i], cur_condition) == 0) {
                    cur_condition->subjects[cur_condition->subject_count] = all_subjects[get_index_of_subject(tokens[i], all_subjects, *all_subject_count)];
                    cur_condition->subject_count++;
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }
                if (strcmp(tokens[i + 1], "and") == 0) {
                    if (token_count == i + 2) {
                        printf("INVALID\n");
                        return noSentences;
                    }
                    i += 2;
                } else {
                    c = C_VERB;
                    i++;
                }

                break;
            }
            case C_VERB: {
                if (strcmp(tokens[i], "at") == 0) {
                    c = C_LOCATION;
                    i++;
                } else if (strcmp(tokens[i], "has") == 0) {
                    if (tokens[i + 1] == NULL || tokens[i + 2] == NULL) {
                        printf("INVALID\n");
                        return noSentences;
                    }
                    if (strcmp(tokens[i + 1], "more") == 0 && strcmp(tokens[i + 2], "than") == 0) {
                        cur_condition->type = HAS_MORE;
                        c = C_ITEM;
                        i += 3;
                    } else if (strcmp(tokens[i + 1], "less") == 0 && strcmp(tokens[i + 2], "than") == 0) {
                        cur_condition->type = HAS_LESS;
                        c = C_ITEM;
                        i += 3;
                    } else if (get_nonnegative_number(tokens[i + 1]) != -1) {
                        cur_condition->type = HAS;
                        c = C_ITEM;
                        i++;
                    } else {
                        printf("INVALID\n");
                        return noSentences;
                    }
                } else if (cur_sentence->condition_count > 0 && (strcmp(tokens[i], "go") == 0 || strcmp(tokens[i], "buy") == 0 || strcmp(tokens[i], "sell") == 0)) {
                    s = END;
                } else if (cur_sentence->condition_count == 0 && (strcmp(tokens[i], "go") == 0 || strcmp(tokens[i], "buy") == 0 || strcmp(tokens[i], "sell") == 0)) {
                    printf("INVALID\n");
                    return noSentences;
                }

                else {
                    printf("INVALID\n");
                    return noSentences;
                }
                break;
            }
            case C_ITEM: {
                int item_num = get_nonnegative_number(tokens[i]);
                if (item_num == -1) {
                    printf("INVALID\n");
                    return noSentences;
                } else {
                    if (tokens[i] == NULL) {
                        printf("INVALID\n");
                        return noSentences;
                    }
                    if (tokens[i + 1] == NULL) {
                        printf("INVALID\n");
                        return noSentences;
                    } else if (is_valid_item(tokens[i + 1]) == 0) {
                        printf("INVALID\n");
                        return noSentences;
                    } else if (does_exist_item_in_condition(tokens[i + 1], cur_condition) == 0) {
                        create_item_in_condition(tokens[i + 1], item_num, cur_condition);

                        if (tokens[i + 2] == NULL) {
                            i += 2;
                            c = C_END;

                        } else if (strcmp(tokens[i + 2], "and") == 0 && get_nonnegative_number(tokens[i + 3]) != -1) {
                            i += 3;
                        } else if (strcmp(tokens[i + 2], "and") == 0 && get_nonnegative_number(tokens[i + 3]) == -1) {
                            i += 2;
                            c = C_END;
                        } else {
                            printf("INVALID\n");
                            return noSentences;
                        }
                    } else {
                        printf("INVALID\n");
                        return noSentences;
                    }
                }

                break;
            }
            case C_LOCATION: {
                if (is_valid_location(tokens[i]) == 0) {
                    printf("INVALID\n");
                    return noSentences;
                }
                alter_subject_location_for_condition(cur_condition, tokens[i]);

                c = C_END;
                i++;

                break;
            }
            case C_END: {
                cur_sentence->conditions[cur_sentence->condition_count] = cur_condition;
                cur_sentence->condition_count++;
                if (tokens[i] == NULL) {
                    s = END;
                    sentences[sentence_count] = cur_sentence;
                    sentence_count++;
                    return sentences;
                } else if (strcmp(tokens[i], "and") == 0) {
                    i++;
                    c = C_START;
                } else {
                    printf("INVALID\n");
                    return noSentences;
                }
            }
            }
            break;
        }

        case END: {
            sentences[sentence_count] = cur_sentence;
            sentence_count++;
            if (tokens[i] == NULL) {
                return sentences;
            } else if ((strcmp(tokens[i], "go") == 0) || (strcmp(tokens[i], "buy") == 0) || (strcmp(tokens[i], "sell") == 0)) {
                i = last_head; // Fall back to the last head of the condition, and start a new sentence.
                s = START;
            } else {
                printf("INVALID\n");
                return noSentences;
            }
            break;
        }
        }
    }
    return sentences;
}

/*
 * Executes the given tokens as sentences.
 * Parses the tokens into sentences and executes them.
 * Prints "OK" if the sentence is valid.
 * Does not print anything if the sentence is invalid as the error message "INVALID" is printed in the parsing function.
 */
void execute_sentences(char **tokens, Subject **all_subjects, int *all_subject_count, int token_count) {
    Sentence **sentences = parse_sentences(tokens, token_count, all_subjects, all_subject_count);
    for (int i = 0; sentences[i] != NULL; i++) {
        execute_sentence(sentences[i], all_subjects, all_subject_count);
    }
    if (sentences[0] != NULL) {
        printf("OK\n");
    }
    if (DEBUG) {
        for (int i = 0; sentences[i] != NULL; i++) {
            print_sentence(sentences[i]);
        }
    }
}

/*
 * Executes the given sentence by checking the conditions and executing the actions.
 */
void execute_sentence(Sentence *sentence, Subject **all_subjects, int *all_subject_count) {
    if (check_conditions(sentence->conditions, sentence->condition_count, all_subjects, all_subject_count)) {
        execute_actions(sentence->actions, sentence->action_count);
    }
}

/*
 * Executes the given actions.
 */
void execute_actions(Action **actions, int action_count) {
    for (int i = 0; actions[i] != NULL; i++) {
        assert(i < action_count);
        execute_action(actions[i]);
    }
    assert(actions[action_count] == NULL);
}

/*
 * Checks the given conditions.
 * Returns 1 if all conditions are met, 0 otherwise.
 */
int check_conditions(Condition **conditions, int condition_count, Subject **all_subjects, int *all_subject_count) {
    for (int i = 0; conditions[i] != NULL; i++) {
        if (check_condition(conditions[i], all_subjects, all_subject_count) == 0) {
            return 0;
        }
    }
    assert(conditions[condition_count] == NULL);
    return 1;
}

/*
 * Checks the given condition.
 * Returns 1 if the condition is met, 0 otherwise.
 */
int check_condition(Condition *condition, Subject **all_subjects, int *all_subject_count) {
    switch (condition->type) {
    case AT: {
        for (int i = 0; i < condition->subject_count; i++) {

            int index_of_sub = get_index_of_subject(condition->subjects[i]->name, all_subjects, *all_subject_count);
            if (strcmp(all_subjects[index_of_sub]->location, condition->location) != 0) {
                return 0;
            }
        }
        return 1;
        break;
    } break;
    case HAS: {
        for (int i = 0; i < condition->subject_count; i++) {
            for (int j = 0; j < condition->item_count; j++) {
                int index_of_sub = get_index_of_subject(condition->subjects[i]->name, all_subjects, *all_subject_count);
                int index_of_item = get_index_of_item(all_subjects[index_of_sub], condition->item[j].name);
                if (condition->item[j].count == 0) {

                } else if (does_exist_item(condition->item[j].name, all_subjects[index_of_sub]) == 0) {
                    return 0;

                }

                else if (all_subjects[index_of_sub]->items[index_of_item].count != condition->item[j].count) {
                    return 0;
                }
            }
        }
        return 1;
        break;
    } break;
    case HAS_MORE: {
        for (int i = 0; i < condition->subject_count; i++) {
            for (int j = 0; j < condition->item_count; j++) {
                int index_of_sub = get_index_of_subject(condition->subjects[i]->name, all_subjects, *all_subject_count);
                int index_of_item = get_index_of_item(all_subjects[index_of_sub], condition->item[j].name);
                if (does_exist_item(condition->item[j].name, all_subjects[index_of_sub]) == 0) {
                    return 0;
                }

                else if (all_subjects[index_of_sub]->items[index_of_item].count <= condition->item[j].count) {
                    return 0;
                }
            }
        }
        return 1;
        break;
    } break;
    case HAS_LESS: {
        for (int i = 0; i < condition->subject_count; i++) {
            for (int j = 0; j < condition->item_count; j++) {
                int index_of_sub = get_index_of_subject(condition->subjects[i]->name, all_subjects, *all_subject_count);
                int index_of_item = get_index_of_item(all_subjects[index_of_sub], condition->item[j].name);
                if (does_exist_item(condition->item[j].name, all_subjects[index_of_sub]) == 0) {
                    continue;
                }

                else if (all_subjects[index_of_sub]->items[index_of_item].count >= condition->item[j].count) {
                    return 0;
                }
            }
        }
        return 1;
        break;
    } break;
    default:
        return 0;
        break;
    }
}

/*
 * Executes the given action.
 */
void execute_action(Action *action) {
    switch (action->type) {
    case GO_TO: {
        for (int i = 0; i < action->subject_count; i++) {
            alter_subject_location(action->subjects[i], action->location);
        }
        break;
    }
    case BUY: {
        for (int i = 0; i < action->subject_count; i++) {
            for (int j = 0; j < action->item_count; j++) {
                alter_subject_item_count(action->subjects[i], action->item[j].name, action->item[j].count);
            }
        }
        break;
    }
    case SELL: {
        int is_doable = 1;
        for (int i = 0; i < action->subject_count; i++) {
            for (int j = 0; j < action->item_count; j++) {
                if (is_inventory_alterable(action->subjects[i], action->item[j].name, -action->item[j].count) == 0) {
                    is_doable = 0;
                }
            }
        }
        if (is_doable) {
            for (int i = 0; i < action->subject_count; i++) {
                for (int j = 0; j < action->item_count; j++) {
                    alter_subject_item_count(action->subjects[i], action->item[j].name, -action->item[j].count);
                }
            }
        }

        break;
    }
    case BUY_FROM: {
        int is_doable = 1;
        for (int j = 0; j < action->item_count; j++) {
            if (is_inventory_alterable(action->from, action->item[j].name, -action->subject_count * action->item[j].count) == 0) {
                is_doable = 0;
            }
        }

        if (is_doable) {
            for (int i = 0; i < action->subject_count; i++) {
                for (int j = 0; j < action->item_count; j++) {
                    alter_subject_item_count(action->from, action->item[j].name, -action->item[j].count);
                    alter_subject_item_count(action->subjects[i], action->item[j].name, action->item[j].count);
                }
            }
        }
        break;
    }
    case SELL_TO: {
        int is_doable = 1;
        for (int i = 0; i < action->subject_count; i++) {
            for (int j = 0; j < action->item_count; j++) {
                if (is_inventory_alterable(action->subjects[i], action->item[j].name, -action->item[j].count) == 0) {
                    is_doable = 0;
                }
            }
        }
        if (is_doable) {
            for (int i = 0; i < action->subject_count; i++) {
                for (int j = 0; j < action->item_count; j++) {
                    alter_subject_item_count(action->subjects[i], action->item[j].name, -action->item[j].count);
                    alter_subject_item_count(action->to, action->item[j].name, action->item[j].count);
                }
            }
        }
        break;
    }
    }
}

/*
 * Prints the given sentence by printing the actions and conditions associated with it.
 */
void print_sentence(Sentence *sentence) {
    for (int i = 0; i < sentence->action_count; i++) {
        printf("Action %d\n", i);
        print_action(sentence->actions[i]);
    }
    for (int i = 0; i < sentence->condition_count; i++) {
        printf("Condition %d\n", i);
        print_condition(sentence->conditions[i]);
    }
}

/*
 * Prints the given action.
 */
void print_action(Action *action) {
    printf(ANSI_COLOR_BLUE ANSI_BOLD "Action type: %s\n" ANSI_COLOR_RESET, get_action_type_name(action->type));
    for (int i = 0; i < action->subject_count; i++) {
        printf(ANSI_COLOR_MAGENTA "Subject %d: %s\n" ANSI_COLOR_RESET, i, action->subjects[i]->name);
    }
    for (int i = 0; i < action->item_count; i++) {
        printf(ANSI_COLOR_RED "Item %d: %s %d\n" ANSI_COLOR_RESET, i, action->item[i].name, action->item[i].count);
    }
    if (action->location != NULL) {
        printf(ANSI_UNDERLINE "Location: %s\n" ANSI_COLOR_RESET, action->location);
    }
    if (action->to != NULL) {
        printf("To: %s\n", action->to->name);
    }
    if (action->from != NULL) {
        printf("From: %s\n", action->from->name);
    }
}

/*
 * Prints the given condition.
 */
void print_condition(Condition *condition) {
    printf(ANSI_COLOR_GREEN ANSI_BOLD "Condition type: %s\n" ANSI_COLOR_RESET, get_condition_type_name(condition->type));
    for (int i = 0; i < condition->subject_count; i++) {
        printf(ANSI_COLOR_MAGENTA "Subject %d: %s\n" ANSI_COLOR_RESET, i, condition->subjects[i]->name);
    }
    for (int i = 0; i < condition->item_count; i++) {
        printf(ANSI_COLOR_RED "Item %d: %s %d\n" ANSI_COLOR_RESET, i, condition->item[i].name, condition->item[i].count);
    }
    if (condition->location != NULL) {
        printf(ANSI_UNDERLINE "Location: %s\n" ANSI_COLOR_RESET, condition->location);
    }
}

/*
 * Returns the name of the given ActionType as a constant char pointer.
 * Used within the print functions.
 */
const char *get_action_type_name(ActionType type) {
    switch (type) {
    case GO_TO:
        return "GO_TO";
    case SELL:
        return "SELL";
    case SELL_TO:
        return "SELL_TO";
    case BUY:
        return "BUY";
    case BUY_FROM:
        return "BUY_FROM";
    default:
        return "UNKNOWN ACTION";
    }
}

/*
 * Returns the name of the given ConditionType as a constant char pointer.
 * Used within the print functions.
 */
const char *get_condition_type_name(ConditionType type) {
    switch (type) {
    case AT:
        return "AT";
    case HAS:
        return "HAS";
    case HAS_MORE:
        return "HAS_MORE";
    case HAS_LESS:
        return "HAS_LESS";
    default:
        return "UNKNOWN CONDITION";
    }
}
