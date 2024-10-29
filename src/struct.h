#ifndef STRUCT
#define STRUCT

#define MAX_SUBJECTS 1024
#define MAX_ITEMS 1024
#define MAX_ITEM_NAME 1024
#define MAX_SUBJECT_NAME 1024

// Globally defined structs
typedef struct {
    char name[MAX_ITEM_NAME];
    int count;
} Item;

typedef struct {
    char name[MAX_SUBJECT_NAME];
    char *location;
    Item items[MAX_ITEMS];
} Subject;

typedef enum {
    GO_TO,
    SELL,
    SELL_TO,
    BUY,
    BUY_FROM,
} ActionType;

typedef enum {
    AT,
    HAS,
    HAS_MORE,
    HAS_LESS,
} ConditionType;

typedef struct {
    ActionType type;
    Subject **subjects;
    int subject_count;
    Subject *from;  // only for BUY_FROM
    Subject *to;    // only for SELL_TO
    Item *item;     // only for BUY, BUY_FROM, SELL, SELL_TO
    int item_count; // only for BUY, BUY_FROM, SELL, SELL_TO, Note that "2 pencils and 3 erasers" are 2 items since "2 pencil" IS an item.
    char *location; // only for GO_TO
} Action;

typedef struct {
    ConditionType type;
    Subject **subjects;
    int subject_count;
    char *location; // only for AT
    Item *item;     // only for HAS, HAS_MORE, HAS_LESS
    int item_count; // only for HAS, HAS_MORE, HAS_LESS
} Condition;

typedef struct {
    Action **actions;
    int action_count;
    Condition **conditions;
    int condition_count;
} Sentence;

#endif
