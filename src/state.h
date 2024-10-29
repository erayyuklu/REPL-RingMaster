#ifndef STATE_H
#define STATE_H

typedef enum {
    START,
    ACTION,
    CONDITION,
    END
} SentenceState;

typedef enum {
    A_START,
    A_SUBJECT,
    A_VERB,
    A_ITEM,
    A_LOCATION,
    A_O_SUBJECT,
    A_END
} ActionState;

typedef enum {
    C_START,
    C_SUBJECT,
    C_VERB,
    C_ITEM,
    C_LOCATION,
    C_END
} ConditionState;

#endif