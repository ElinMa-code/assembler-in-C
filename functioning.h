#ifndef FUNCTIONING
#define FUNCTIONING
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#define BASE64_IN_WORD 2
#define STORABLE(integer) ((integer) & 0xFFFF)
#define SIZEOF_STR(length) (sizeof(char) * ((length) + 1))

typedef char base64[BASE64_IN_WORD];

/* The method actually allocates the size of the houses needed in memory, and checks whether the allocation was successful
And if not then she's out */
void *malloc_safe(int);

/* Gets 2 strings and connects the two strings i.e. the contents of a new one with the contents of the second and assigns a new string with the contents of the 2 strings */
char *strcat_safe(char *, char *);

/* The method receives two pointers and copies to the assigned string
Memory location */
char *strcpy_safe(char *, char *);

/* The method opens a file with the specified name and permissions
And assigns it to the FILE pointer
And returns TRUE if opened successfully, and if not then returns false */
boolean fopen_check(FILE **, char *, char *, char *);

/* Opens a file with a name,postfix and permissions specified safely */
FILE *fopen_safe(char *, char *, char *);

/* This method converts an integer that can be sequenced by 2
And put it in 2 LSBs of the 64base array */
void get_integer_base64(base64, int);

int encode_registers(reg, reg);

int encode_data(addressing_method, opcode, addressing_method, memory_status);

int encode_int(int, memory_status);

#endif
