#ifndef SECOND_H_INCLUDED
#define SECOND_H_INCLUDED
#include <stdio.h>
#include "table.h"
#include "thing.h"

int is_empty(char* line, int index);

int is_comment(char* line);

int check_label(char* line);

int get_guiding_sentence(char* line, int index);

int get_opcode(char* str);

int addressing_type(char* operand);

int second_pass(FILE* fp, table* tb, thing** code_image, int* data_image, char* file_name, int *);

#endif /* SECOND_H_INCLUDED */
