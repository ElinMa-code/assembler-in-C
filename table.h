#ifndef TABLE
#define TABLE
#include <stdio.h>
#include "globals.h"
#include "charsequence.h"

#define KEYOF_NODE_EQUALS(node, keystart, keyend) ((node) != NULL && str_equal((keystart), (keyend), (node)->key))
#define KEYOF_NODE_BEF(node, keyname) ((node) != NULL && strcmp((node)->key, keyname) < 0)
#define KEYOF_NODE_AFTER(node, keyname) ((node) != NULL && strcmp((node)->key, keyname) > 0)

typedef struct symbol_attributes {
    unsigned int is_code: 1;
    unsigned int is_data: 1;
    unsigned int is_extern: 1;
    unsigned int is_entry: 1;
    unsigned int data: 10;
} symbol_attr;

typedef struct mac_attributes {
    unsigned int lines;
    fpos_t position;
} mac_attr;

typedef union row_data_union {
    symbol_attr symbol;
    mac_attr mac;
} row_data;

typedef struct row *table;
typedef struct row {
    char *key;
    table next;
    row_data data;
} table_row;


table search_last_row_bef(table *, char *, char *);

table search_label_entry(table *, char *, char *);

boolean add_entry(table *, char *, char *, row_data);

table search_entry(table *, char *, char *);

/* Free every row in the table. */
void free_table(table *);

table new_row(char *, char *, row_data, table);

#endif
