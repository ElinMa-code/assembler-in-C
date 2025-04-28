#ifndef OUTPUT
#define OUTPUT
#include "globals.h"
#include "thing.h"
#include "table.h"

#define ENTRY_POSTFIX ".ent"
#define EXTERN_POSTFIX ".ext"
#define OBJECT_POSTFIX ".ob"

extern boolean write_files(char *, thing **, int, int *, int, table, table);


#endif
