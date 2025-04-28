#ifndef COMMAND
#define COMMAND
#include "globals.h"

#define MAX_SYMBOL_LENGTH 31
#define REGISTER_NAME_LENGTH 3
#define MIN_OPERATION_LENGTH 3
#define MAX_OPERATION_LENGTH 4

#define EXECUTE(status, func) \
    if ((status = func) != MOVE) \
        return status;
#define MOVE_IF_EMPTY_OR_COMMENT(first_nonnotewhite) \
    if (!(*(first_nonnotewhite)) || *(first_nonnotewhite) == ';') \
        return MOVE;
#define ERROR_FORMAT "%s%s:%d - %s"

typedef enum input_statuses {
    MOVE,
    /* Warnings */
    WARN_LABEL_TO_ENTRY,
    WARN_LABEL_TO_EXTERN,

    /*macro errors */
    ILLEGAL_MACRO,

    /* Label name errors */
    TOO_LONG_SYMBOL_NAME,
    ILLEGAL_CHAR_IN_BEGINNING_OF_SYMBOL,
    ILLEGAL_CHARS_IN_SYMBOL_NAME,
    SYMBOL_NAMED_LIKE_OPERATION,
    SYMBOL_NAMED_LIKE_REGISTER,
    SYMBOL_ALREADY_DEFINED,


    LABEL_TO_EMPTY_LINE,
    MULTIPLE_CONSECUTIVE_COMMAS,
    MISSING_COMMA_BETWEEN_OPERANDS,
    EXTRANEOUS_TEXT,

    /* First MOVE - instruction structure */
    UNREC_INSTRUCTION,
    ILLEGAL_COMMA_AFTER_INST,
    INST_REQ_OPERAND,
    STRING_INST_EXPECTS_OPERAND,
    STRING_MUST_BEGIN_WITH_QUOT,
    STRING_MUST_END_WITH_QUOT,
    NO_NUMBERS_FOR_DATA_INST,
    INVALID_OPERAND_NOT_NUMBER,
    EXPECTED_NUMBER_AFTER_COMMA,

    /* operation structure */
    UNREC_OPERATION,
    ILLEGAL_COMMA_AFTER_OPERATION,
    MISSING_OPERAND_REQUIRED_ONE,
    MISSING_OPERAND_REQUIRED_TWO,
    INVALID_ADRS_METHOD_FIRST_OP,
    INVALID_ADRS_METHOD_SECOND_OP,
    INVALID_NUMBER_FOR_IMMD_ADDRESSING,
    INVALID_REGISTER_NAME_ADDRESSING,


    UNREC_SYMBOL_FOR_ENTRY,
    SYMBOL_NOT_FOUND_FIRST_OP,
    SYMBOL_NOT_FOUND_SECOND_OP,
    EXTERNAL_ENTRY_SYMBOL


} input_status;
/* This method gets a pointer to the beginning and end of a sub-string,
And the method compares it to given strings and turns the opcode values and their function into two given pointers.
Returns */
input_status str_to_opcode_funct(char *, char *, opcode *);

/* This method gets a pointer to a pointer to the place along the string,
And assigns the pointer to a character that is not the next space as the second argument,
And the white space or comma following it to the third argument.
Redirects the pointer in the first argument beyond the next comma if found */
input_status find_operand(char **, char **, char **, input_status );

/* This method gets an opcode integer and returns the number of operands that this operation requests in its method */
int num_of_operands(opcode);

/* This peak gets gets two pointers to the beginning and end of a string,
And finds the appropriate address of the operand sub-string and assigns it to the appropriate pointer given. */
input_status get_operand_data(char *, char *, addressing_method *, reg *, int *);

/* This method gets two pointers at the beginning and end of a string,
And looking for a valid icon there */
input_status validate_symbol_name(char *, char *);

boolean split_symbol_index(char *, char **, char **, char **, char **);

/* This method gets two pointers to the beginning and end of a string
And returns the corresponding registration code
If not found, will return NONE_REG */
reg str_to_reg(char *, char *);

/* Receives opcode and two methods of contact,
And validates them against each other using the table shown in the instructions
Returns */
input_status check_addressing_methods(opcode, addressing_method, addressing_method);

/* This method gets an address method,
And calculates the length of the quantity of things */
int things_by_addr(addressing_method);

/* This method gets a pointer to the first space character we want in a sequence of white characters ending in a string,
And looks for non-spaced characters. */
input_status end_of_command(char *);

/* This method gets the count of input countable and returns the corresponding error message */
char *get_error(input_status);

/* This method checks if the input status is MOVE or WARNING */
boolean moved(input_status);

boolean find_opthing(char *, char *, char **, char **);

#endif
