#ifndef SYMBOLS
#define SYMBOLS
#include "globals.h"
#include "command.h"
#include "table.h"

#define ADDR_NOT_FOUND (-1)
#define ADDR_EXTERN 0

#define ATTR(row) ((row)->data).symbol
#define IS_CODE(row) ATTR(row).is_code
#define IS_DATA(row) ATTR(row).is_data
#define IS_ENTRY(row) ATTR(row).is_entry
#define IS_EXTERN(row) ATTR(row).is_extern
#define ROW_DATA(row) ATTR(row).data

typedef enum symbol_purposes { CODE, DATA, EXTERN } symbol_purpose;

/* This method gets 3 pointers and a pointer for the table and two pointers for the beginning and end of a sub-string which
Represents the symbol key, along with the purpose of the symbol and the data of the symbol.
We will add the icon to the table
And if the name of the symbol is valid.*/
input_status add_symbol(table *, char *, char *, symbol_purpose, int);

/* The method gets a sum of 3 pointers to the table and two pointers to the beginning and end of a string which
Represents the icon key
And we will add the icon to the table if the name is valid.
 */
input_status add_extern_symbol(table *, char *, char *);

/* This method gets a table and the beginning and end of an icon key
And it searches for the symbol and gives its value to a pointer given as a given
 */
int find_symbol_addr(table *, char *, char *);

/* The method gets a table and the beginning and end of an icon key
And looking for the icon */
input_status mark_entry_symbol(table *, char *, char *);

void relocate_data_symbols(table *, int);

/* This method adds a row to the outer word table,
With the specified key and the specified address */
void add_extern_reference_word(table *, char *, char *, int);

#endif
