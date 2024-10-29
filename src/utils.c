#include "utils.h"

/**
 * Tokenizes the given line word by word and returns the tokens.
 * The count of tokens is stored in the token_count, which is passed by reference.
 */
char **tokenize(char *line, int *token_count) {
    char **tokens = malloc(512 * sizeof(char *));
    char *token = strtok(line, " \n");
    *token_count = 0;
    while (token != NULL) {
        tokens[*token_count] = token;
        (*token_count)++;
        token = strtok(NULL, " \n");
    }
    tokens[*token_count] = NULL;
    return tokens;
}

/**
 * Checks if the given word is a keyword.
 */
int is_keyword(char *word) {
    if (strcmp(word, "sell") == 0 || strcmp(word, "buy") == 0 || strcmp(word, "go") == 0 || strcmp(word, "to") == 0 ||
        strcmp(word, "from") == 0 || strcmp(word, "and") == 0 || strcmp(word, "at") == 0 || strcmp(word, "has") == 0 ||
        strcmp(word, "if") == 0 || strcmp(word, "less") == 0 || strcmp(word, "more") == 0 || strcmp(word, "than") == 0 ||
        strcmp(word, "exit") == 0 || strcmp(word, "where") == 0 || strcmp(word, "total") == 0 || strcmp(word, "who") == 0 ||
        strcmp(word, "NOBODY") == 0 || strcmp(word, "NOTHING") == 0 || strcmp(word, "NOWHERE") == 0) {
        return 1;
    }
    return 0;
}

/**
 * Checks if the given name is a valid subject name.
 * A valid subject name can only contain uppercase and lowercase letters and underscores.
 */
int is_valid_subject(char *name) {
    if (is_keyword(name) == 1) {
        return 0;
    }
    for (int i = 0; name[i] != '\0'; i++) {
        if (!(isupper(name[i]) || islower(name[i]) || name[i] == '_')) {
            return 0;
        }
    }
    return 1;
}

/**
 * Checks if the given name is a valid item name.
 * A valid item name can only contain uppercase and lowercase letters and underscores.
 */
int is_valid_item(char *name) {
    if (is_keyword(name) == 1) {
        return 0;
    }
    for (int i = 0; name[i] != '\0'; i++) {
        if (!(isupper(name[i]) || islower(name[i]) || name[i] == '_')) {
            return 0; // Invalid character found
        }
    }
    return 1;
}

/**
 * Checks if the given name is a valid location name.
 * A valid location name can only contain uppercase and lowercase letters and underscores.
 */
int is_valid_location(char *name) {
    if (is_keyword(name) == 1) {
        return 0;
    }
    for (int i = 0; name[i] != '\0'; i++) {
        if (!(isupper(name[i]) || islower(name[i]) || name[i] == '_')) {
            return 0;
        }
    }
    return 1;
}

/**
 * Checks if the inventory of the given subject can be altered by the given count.
 * Returns 1 if the inventory can be altered, 0 otherwise.
 * Utilized in the "buy", "buy to", "sell", and "sell to" commands.
 */
int is_inventory_alterable(Subject *s, char *item_name, int count) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(s->items[i].name, item_name) == 0) {
            if (s->items[i].count + count < 0) {
                return 0;
            }
            return 1;
        } else if (s->items[i].name[0] == '\0') {
            if (count < 0) {
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

/**
 * Converts a string to a non-negative number.
 * Returns -1 if the string is not a valid non-negative number.
 */
int get_nonnegative_number(const char *token) {
    char *endptr;
    long num = strtol(token, &endptr, 10);
    if (endptr == token || *endptr != '\0') {
        return -1;
    }
    if (num < 0) {
        return -1;
    }
    return num;
}

/**
 * Returns the index of the given subject in the array of subjects.
 * If the subject does not exist, returns -1.
 */
int get_index_of_subject(char *subject_name, Subject **all_subjects, int all_subjects_count) {
    for (int i = 0; i < all_subjects_count; i++) {
        if (strcmp(all_subjects[i]->name, subject_name) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Returns the count of the given item in the subject's inventory in case it exists.
 * Otherwise, returns 0.
 */
int get_item_count_for_subject(Subject *s, char *item_name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(s->items[i].name, item_name) == 0) {
            return s->items[i].count;
        } else if (s->items[i].name[0] == '\0') {
            break;
        }
    }
    return 0;
}

/**
 * Returns the length of the array of strings.
 */
int get_length(char **tokens) {
    int length = 0;
    while (*tokens != NULL) {
        length++;
        tokens++;
    }
    return length;
}

/**
 * Alters the count of the given item in the subject's inventory by the given count.
 * If the item does not exist in the inventory, adds the item to the inventory.
 * Returns 1 if the inventory is altered successfully, 0 otherwise.
 */
int alter_subject_item_count(Subject *s, char *item_name, int count) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(s->items[i].name, item_name) == 0) {
            if (s->items[i].count + count < 0) {
                return 0;
            }
            s->items[i].count += count;
            return 1;
        } else if (s->items[i].name[0] == '\0') {
            if (count < 0) {
                return 0;
            }
            strcpy(s->items[i].name, item_name);
            s->items[i].count = count;
            return 1;
        }
    }
    return 0;
}

/**
 * Alters the location of the given subject.
 * No checks are performed, the return value is always 1 for consistency.
 */
int alter_subject_location(Subject *s, char *location) {
    assert(s != NULL);
    s->location = location;
    return 1;
}

/**
 * Alters the location of the given condition.
 * No checks are performed, the return value is always 1 for consistency.
 */
int alter_subject_location_for_condition(Condition *c, char *location) {
    assert(c != NULL);
    c->location = location;
    return 1;
}

/**
 * Returns the location of the given subject.
 * If the subject has no location, returns "NOWHERE".
 */
char *get_subject_location(Subject *s) {
    assert(s != NULL);
    if (s->location == NULL) {
        return "NOWHERE";
    }
    return s->location;
}

/**
 * Checks if the given item exists in the inventory of the given subject.
 * Returns 1 if the item exists, 0 otherwise.
 */
int does_exist_item(char *item_name, Subject *s) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(s->items[i].name, item_name) == 0) {
            return 1;
        } else if (s->items[i].name[0] == '\0') {
            break;
        }
    }
    return 0;
}

/**
 * Checks if the given item exists in the item array of the given action.
 * Returns 1 if the item exists, 0 otherwise.
 */
int does_exist_item_in_action(char *item_name, Action *a) {
    for (int i = 0; i < a->item_count; i++) {
        if (strcmp(a->item[i].name, item_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if the given item exists in the item array of the given condition.
 * Returns 1 if the item exists, 0 otherwise.
 */
int does_exist_item_in_condition(char *item_name, Condition *c) {
    for (int i = 0; i < c->item_count; i++) {
        if (strcmp(c->item[i].name, item_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Returns the index of the given item in the item array of the given subject.
 * If the item does not exist, returns 0.
 */
int get_index_of_item(Subject *s, char *item_name) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(s->items[i].name, item_name) == 0) {
            return i;
        } else if (s->items[i].name[0] == '\0') {
            break;
        }
    }
    return 0;
}

/**
 * Checks if the given subject exists in the subject array.
 * Returns 1 if the subject exists, 0 otherwise.
 */
int does_exist(char *subject_name, Subject **all_subjects, int all_subjects_count) {
    for (int i = 0; i < all_subjects_count; i++) {
        if (strcmp(all_subjects[i]->name, subject_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if the given subject exists in the subject array of the given action.
 * Returns 1 if the subject exists, 0 otherwise.
 */
int does_exist_subject_in_action(char *subject_name, Action *a) {
    for (int i = 0; i < a->subject_count; i++) {
        if (strcmp(a->subjects[i]->name, subject_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Checks if the given subject exists in the subject array of the given condition.
 * Returns 1 if the subject exists, 0 otherwise.
 */
int does_exist_subject_in_condition(char *subject_name, Condition *c) {
    for (int i = 0; i < c->subject_count; i++) {
        if (strcmp(c->subjects[i]->name, subject_name) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Creates a new subject with the given name, and adds it to the array of subjects.
 * Returns 1 if the subject is created successfully, 0 otherwise.
 * As a side effect, the count of all subjects is incremented.
 */
int create_subject(char *name, Subject **all_subjects, int *all_subjects_count) {
    if ((*all_subjects_count) >= MAX_SUBJECTS) {
        return 0;
    }
    Subject *s = malloc(sizeof(Subject));
    strcpy(s->name, name);
    s->location = "NOWHERE";
    for (int i = 0; i < MAX_ITEMS; i++) {
        s->items[i].name[0] = '\0';
        s->items[i].count = 0;
    }
    all_subjects[(*all_subjects_count)] = s;
    (*all_subjects_count)++;
    return 1;
}

/**
 * Creates a new item with the given name and count, and adds it to the item array of the given action.
 * As a side effect, the count of items in the action is incremented.
 */
void create_item_in_action(char *item_name, int item_num, Action *curr_action) {
    Item *item = malloc(sizeof(Item));
    strcpy(item->name, item_name);
    item->count = item_num;
    if (curr_action->item == NULL) {
        curr_action->item = malloc(MAX_ITEMS * sizeof(Item));
    }
    curr_action->item[curr_action->item_count] = *item;
    curr_action->item_count++;
}

/**
 * Creates a new item with the given name and count, and adds it to the item array of the given condition.
 * As a side effect, the count of items in the condition is incremented.
 */
void create_item_in_condition(char *item_name, int item_num, Condition *curr_condition) {
    Item *item = malloc(sizeof(Item));
    strcpy(item->name, item_name);
    item->count = item_num;
    if (curr_condition->item == NULL) {
        curr_condition->item = malloc(MAX_ITEMS * sizeof(Item));
    }
    curr_condition->item[curr_condition->item_count] = *item;
    curr_condition->item_count++;
}

/**
 * Returns the subjects that are in the given location.
 * The subjects are stored in the subjects array.
 * Returns the count of subjects in the location.
 */
int get_subjects_from_location(Subject **all_subjects, int all_subjects_count, char *location, Subject **subjects) {
    int count = 0;
    for (int i = 0; i < all_subjects_count; i++) {
        if (all_subjects[i]->location != NULL && strcmp(all_subjects[i]->location, location) == 0) {
            subjects[count] = all_subjects[i];
            count++;
        }
    }
    return count;
}

/**
 * Prints the inventory of the given subject.
 * If the subject has no items, prints "NOTHING".
 */
void print_inventory(Subject *s) {
    int item_type_number = 0;

    for (int i = 0; i < MAX_ITEM_NAME; i++) {
        if (s->items[i].count != 0) {
            item_type_number++;
        }
    }
    int not_0_indexes[item_type_number];
    int m = 0;
    for (int i = 0; i < MAX_ITEM_NAME; i++) {
        if (s->items[i].count != 0) {
            not_0_indexes[m] = i;
            m++;
        }
    }
    if (item_type_number == 0) {
        printf("NOTHING\n");
    } else {
        for (int i = 0; i < item_type_number; i++) {
            if (i == item_type_number - 1) {
                printf("%d %s\n", s->items[not_0_indexes[i]].count, s->items[not_0_indexes[i]].name);
            } else {
                printf("%d %s and ", s->items[not_0_indexes[i]].count, s->items[not_0_indexes[i]].name);
            }
        }
    }
}

/**
 * Logs the current state of the subjects and their items for debugging purposes.
 * Prints all subjects with their locations and their inventory.
 */
void log_current_state(Subject **all_subjects, int all_subjects_count) {
    printf("\n\n\n----- Current State -----\n");
    for (int i = 0; i < all_subjects_count; i++) {
        printf("\033[0;32m");
        printf("Subject: %s, Location: %s\n", all_subjects[i]->name, get_subject_location(all_subjects[i]));
        for (int j = 0; j < MAX_ITEMS; j++) {
            if (all_subjects[i]->items[j].name[0] != '\0') {
                printf("\033[0;35m");
                printf("→ Item: %s, Count: %d\n", all_subjects[i]->items[j].name, all_subjects[i]->items[j].count);
            }
        }
    }
    printf("\033[0m");
    printf("-------------------------\n\n\n");
}