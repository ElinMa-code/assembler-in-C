#include <ctype.h>
#include "command.h"
#include "charsequence.h"


boolean find_opthing(char *label_start_maybe, char *label_end_maybe,
                char **opstart, char **opend)
{
    if (*label_end_maybe == ':')
    {
        *opstart = next_nonnotewhite(label_end_maybe + 1);
        *opend = next_notewhite(*opstart);
        return TRUE;
    }

    *opstart = label_start_maybe;
    *opend = label_end_maybe;
    return FALSE;
}

struct cmd_lookup_element{
	char *cmd;
	opcode op;
};

static struct cmd_lookup_element lookup_table[] = {
		{"mov", MOV_OP},
		{"cmp",CMP_OP},
		{"add",ADD_OP},
		{"sub",SUB_OP},
        {"not",NOT_OP},
		{"clr",CLR_OP},
        {"lea",LEA_OP},
		{"inc",INC_OP},
		{"dec",DEC_OP},
		{"jmp",JMP_OP},
		{"bne",BNE_OP},
        {"red",RED_OP},
        {"prn",PRN_OP},
		{"jsr",JSR_OP},
		{"rts",RTS_OP},
		{"stop",STOP_OP},
};

input_status str_to_opcode_funct(char *start, char *end, opcode *op_out)
{
    int i = 0;
    struct cmd_lookup_element *row;
    *op_out = NONE;


    if (end - start < MIN_OPERATION_LENGTH || end - start > MAX_OPERATION_LENGTH)
        return UNREC_OPERATION;

    for (row = lookup_table; i < 16 ; row++)
    {
        if (str_equal(start, end, row->cmd))
        {
            *op_out = row->op;
            return MOVE;
        }
        i++;
    }
    return UNREC_OPERATION;
}

input_status find_operand(char **str, char **start, char **end, input_status not_found_status)
{
    *start = next_nonnotewhite(*str);
    if (!(**start))
        return not_found_status;
    if ((**start) == ',')
        return MULTIPLE_CONSECUTIVE_COMMAS;

    *end = next_notewhite_or_comma(*start);
    *str = next_nonnotewhite(*end);

    if (!(**str))
        return MOVE;
    if ((**str) == ',')
    {
        (*str)++;
        return MOVE;
    }

    return MISSING_COMMA_BETWEEN_OPERANDS;
}

int num_of_operands(opcode op)
{
    if ((op >= MOV_OP && op <= SUB_OP) || op == LEA_OP)
        return 2;

    if (op >= NOT_OP && op <= JSR_OP)
        return 1;
    /* Else, no operands */
    return 0;
}

input_status get_operand_data(char *start, char *end, addressing_method *addr_out, reg *reg_out, int *int_out)
{
    reg temp_reg;

    if ((*start >= '0' && *start <= '9') || *start == '-')
    {
        *addr_out = IMMEDIATE;
        return str_to_int(start, end, int_out) ? MOVE : INVALID_NUMBER_FOR_IMMD_ADDRESSING;
    }

    /* Check register direct addressing method */
    if (*start == '@')
    {
        *addr_out = REGISTER_DIRECT;
        if ((temp_reg = str_to_reg(start, end)) != NON_REG)
        {
            *reg_out = temp_reg;
            return MOVE;
        } else return INVALID_REGISTER_NAME_ADDRESSING;
    }
    *addr_out = DIRECT;
    return MOVE;
}

input_status validate_symbol_name(char *start, char *end)
{
    opcode op;
    /* Check length */
    if (end - start > MAX_SYMBOL_LENGTH)
        return TOO_LONG_SYMBOL_NAME;

    /* Check starts with letter */
    if (!isalpha(*start))
        return ILLEGAL_CHAR_IN_BEGINNING_OF_SYMBOL;


    if (next_non_alnum(start) < end)
        return ILLEGAL_CHARS_IN_SYMBOL_NAME;

    /* Check name of register */
    if (str_to_reg(start, end) != NON_REG)
        return SYMBOL_NAMED_LIKE_REGISTER;

    /* Check name of operation */
    if (str_to_opcode_funct(start, end, &op) != UNREC_OPERATION
        || str_equal(start, end, "mcro:")
        || str_equal(start, end, "endmcro:"))
        return SYMBOL_NAMED_LIKE_OPERATION;

    return MOVE;
}

struct reg_lookup_element {
	char *name;
	reg rg;
};

static struct reg_lookup_element reg_lookup_table[] = {
		{"@r0", r0},
        {"@r1", r1},
        {"@r2", r2},
        {"@r3", r3},
        {"@r4", r4},
        {"@r5", r5},
        {"@r6", r6},
        {"@r7", r7},
};

reg str_to_reg(char *start, char *end)
{
    int i = 0;
    struct reg_lookup_element *row;

    int length = end - start;
    if (length != REGISTER_NAME_LENGTH)
        return -1;

    for (row = reg_lookup_table; i < 8; row++)
    {
        if (str_equal(start, end, row->name))
            return row->rg;

        i++;
    }

    /* Did not recognize register */
    return -1;
}

input_status check_addressing_methods(opcode op, addressing_method addr1, addressing_method addr2)
{

    if (op == LEA_OP && addr1 != DIRECT)
        return INVALID_ADRS_METHOD_FIRST_OP;

    if (op != CMP_OP && addr2 == IMMEDIATE)
        return INVALID_ADRS_METHOD_SECOND_OP;

    return MOVE;
}

int things_by_addr(addressing_method addr)
{
    return (addr != NONE) ? 1 : 0;
}

input_status end_of_command(char *str)
{
    if (*next_nonnotewhite(str))
        return EXTRANEOUS_TEXT;
    return MOVE;
}


struct err_lookup_element {
    input_status status;
	char *message;
};

static struct err_lookup_element err_lookup_table[] = {
    {WARN_LABEL_TO_ENTRY, "Warning: Label to .entry instruction ignored"},
    {WARN_LABEL_TO_EXTERN, "Warning: Label to .extern instruction ignored"},
    {ILLEGAL_MACRO, "Error: 'endmcro' with no 'mcro' before"},
    {TOO_LONG_SYMBOL_NAME, "Error: Label name should be at most 31 characters"},
    {ILLEGAL_CHAR_IN_BEGINNING_OF_SYMBOL, "Error: Label name should start with a letter"},
    {ILLEGAL_CHARS_IN_SYMBOL_NAME, "Error: Label name should contain only letters and digits"},
    {SYMBOL_NAMED_LIKE_OPERATION, "Error: Label cannot be named like operation"},
    {SYMBOL_NAMED_LIKE_REGISTER, "Error: Label cannot be named like register"},
    {SYMBOL_ALREADY_DEFINED, "Error: Symbol already defined"},
    {LABEL_TO_EMPTY_LINE, "Error: Expects code after label"},
    {MULTIPLE_CONSECUTIVE_COMMAS, "Error: Multiple consecutive commas between operands"},
    {MISSING_COMMA_BETWEEN_OPERANDS, "Error: Missing comma between operands"},
    {EXTRANEOUS_TEXT, "Error: Extraneous text after command end"},
    {UNREC_INSTRUCTION, "Error: Unrecognized instruction"},
    {ILLEGAL_COMMA_AFTER_INST, "Error: Illegal comma after instruction"},
    {INST_REQ_OPERAND, "Error: Required operand for instruction"},
    {STRING_INST_EXPECTS_OPERAND, "Error: Expected string after .string instruction"},
    {STRING_MUST_BEGIN_WITH_QUOT, "Error: String must begin with quotation mark ('\"')"},
    {STRING_MUST_END_WITH_QUOT, "Error: String must end with quotation mark ('\"')"},
    {NO_NUMBERS_FOR_DATA_INST, "Error: Expected data after .data instruction"},
    {INVALID_OPERAND_NOT_NUMBER, "Error: Invalid operand - not a number"},
    {EXPECTED_NUMBER_AFTER_COMMA, "Error: expected number after comma"},
    {UNREC_OPERATION, "Error: Unrecognized operation"},
    {ILLEGAL_COMMA_AFTER_OPERATION, "Error: Illegal comma after operation"},
    {MISSING_OPERAND_REQUIRED_ONE, "Error: Missing operand - required 1"},
    {MISSING_OPERAND_REQUIRED_TWO, "Error: Missing operand(s) - required 2"},
    {INVALID_ADRS_METHOD_FIRST_OP, "Error: Invalid addressing method for first operand"},
    {INVALID_ADRS_METHOD_SECOND_OP, "Error: Invalid addressing method for second operand"},
    {INVALID_NUMBER_FOR_IMMD_ADDRESSING, "Error: Expected a valid number"},
    {INVALID_REGISTER_NAME_ADDRESSING, "Error: Expected a register operand"},
    {UNREC_SYMBOL_FOR_ENTRY, "Error: Unrecognized symbol for .entry instruction"},
    {SYMBOL_NOT_FOUND_FIRST_OP, "Error: Symbol not found in first operand"},
    {SYMBOL_NOT_FOUND_SECOND_OP, "Error: Symbol not found in second operand"},
    {EXTERNAL_ENTRY_SYMBOL, "Error: External symbol cannot be defined as .entry"},
    {MOVE, ""}
};

char *get_error(input_status status)
{
    struct err_lookup_element *row;

    for (row = err_lookup_table; row->status; row++)
        if (row->status == status)
            return row->message;

    return "";
}

boolean moved(input_status status)
{
    return status == MOVE || status == WARN_LABEL_TO_ENTRY || status == WARN_LABEL_TO_EXTERN;
}