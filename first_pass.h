#ifndef FIRST_PASS_H_INCLUDED
#define FIRST_PASS_H_INCLUDED
#include <stdio.h>
#include "table.h"
#include "first_move.h"
#include "thing.h"
#include "symbols.h"
#include "output.h"
#include "command.h"

extern int snprintf(char *, size_t , const char*,...);

extern int first_pass(FILE* , thing** , int* , table*, int *);

#endif /* FIRST_PASS_H_INCLUDED */
