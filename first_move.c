#include "first_move.h"
#include "symbols.h"
#include "charsequence.h"
#include "command.h"


#define CREATE_DATA_THING_IF_POSSIBLE(code_image, i, length, data) \
    if (length == 1) \
        code_image[i++] = new_data_thing(data); \
    else \
        i+=length;

static input_status fmove_process_line(char *,thing **,int *,int *,int *,table *);

/* This section gets two pointers for the beginning and end of opword, if it starts at a point and not at the entrance or outside */
static input_status process_instruction(char *, char *, int *, int *);

/* As in the section above gets two pointers for the beginning and end of the opword, if not starting at the dot */
static input_status process_operation(char *, char *, thing **, int *);

/* Turns the last two pointers to indicate the beginning and end of the icon */
static input_status check_entry_extern_inst(char *, char **, char **);

boolean first_move(FILE *src,char *filename, thing **code_image,int *ic,int *data_image, int *dc,table *symbols_table)
{
    boolean is_success = TRUE;
    boolean first_output = TRUE;
    int line_num;
    char line[MAX_LINE_LENGTH];
    input_status status;
    for (line_num = 1; fgets(line, MAX_LINE_LENGTH, src) != NULL; line_num++)
    {
        status = fmove_process_line(line, code_image, ic, data_image, dc, symbols_table);
        if (!moved(status)) /* Not MOVE or a warning */
            is_success = FALSE;
        if (status != MOVE)
        {
            /* Add new line  */
            if (first_output)
                first_output = FALSE;
            else
                puts("");
            printf(ERROR_FORMAT, filename, ASSEMBLY_POST_MAC_POSTFIX, line_num, get_error(status));

        }
        status = MOVE;
    }
    if (!first_output && is_success)
        puts("");
    return is_success;
}

static input_status fmove_process_line(char *line, thing **code_image, int *ic, int *data_image, int *dc, table *symbols_table)
{
    input_status status;
    char *label_start_maybe, *label_end_maybe, *opstart, *opend, *symb_start, *symb_end;
    boolean label_exists;
    int label_points_to;
    symbol_purpose label_purpose;

    label_start_maybe = next_nonnotewhite(line);
    MOVE_IF_EMPTY_OR_COMMENT(label_start_maybe)
    label_end_maybe = next_notewhite_or_colon(label_start_maybe);
    label_exists = find_opthing(label_start_maybe, label_end_maybe, &opstart, &opend);

    if (str_equal(opstart, opend, ".entry"))
    {
        EXECUTE(status, check_entry_extern_inst(opend, &symb_start, &symb_end))

        return label_exists ? WARN_LABEL_TO_ENTRY : MOVE;
    }

    if (str_equal(opstart, opend, ".extern"))
    {
        EXECUTE(status, check_entry_extern_inst(opend, &symb_start, &symb_end))
        /* Valid extern instruction */
        EXECUTE(status, add_extern_symbol(symbols_table, symb_start, symb_end))
        return label_exists ? WARN_LABEL_TO_EXTERN : MOVE;
    }

    if (*opstart == '.') /* if another instruction */
    {
        label_points_to = *dc;
        label_purpose = DATA;
        EXECUTE(status, process_instruction(opstart, opend, data_image, dc))
    }
    else
    {
        label_points_to = *ic;
        label_purpose = CODE;
        EXECUTE(status, process_operation(opstart, opend, code_image, ic))
    }

    if (label_exists)
        EXECUTE(status, add_symbol(symbols_table, label_start_maybe, label_end_maybe, label_purpose, label_points_to))
    return MOVE;
}

static input_status process_instruction(char *opstart, char *opend, int *data_image, int *dc)
{
    char *str, *argstart, *argend, *curr_char;
    int curr_arg;
    input_status status;

    if (str_equal(opstart, opend, ".data"))
    {
        str = next_nonnotewhite(opend);
        if (!(*str))
            return NO_NUMBERS_FOR_DATA_INST;
        if (*str == ',')
            return ILLEGAL_COMMA_AFTER_INST;

        while (*str)
        {
            EXECUTE(status, find_operand(&str, &argstart, &argend, EXPECTED_NUMBER_AFTER_COMMA))
            if (!str_to_int(argstart, argend, &curr_arg))
                return INVALID_OPERAND_NOT_NUMBER;
            data_image[(*dc)++] = curr_arg;
        }
        if (*argend == ',')
            return EXPECTED_NUMBER_AFTER_COMMA;
        EXECUTE(status, end_of_command(argend))
        return MOVE;
    }

    if (str_equal(opstart, opend, ".string"))
    {
        argstart = next_nonnotewhite(opend);
        if (!(*argstart))
            return STRING_INST_EXPECTS_OPERAND;
        if (*argstart != '"')
            return STRING_MUST_BEGIN_WITH_QUOT;

        argend = next('"', argstart + 1);
        if (!(*argend))
            return STRING_MUST_END_WITH_QUOT;

        for (curr_char = argstart + 1; curr_char < argend; curr_char++)
            data_image[(*dc)++] = *curr_char;
        /* terminate with 0 */
        data_image[(*dc)++] = '\0';
        return MOVE;
    }

    return UNREC_INSTRUCTION;
}

static input_status process_operation(char *opstart, char *opend, thing **code_image, int *ic)
{
    input_status status, notfound_status;
    opcode oc;
    reg reg1 = NON_REG, reg2 = NON_REG;
    addressing_method addr1 = NONE, addr2 = NONE;
    int data1, data2, i, length, opcount;
    char *str, *op1_start, *op1_end, *op2_start, *op2_end;

    EXECUTE(status, str_to_opcode_funct(opstart, opend, &oc))
    opcount = num_of_operands(oc);

    if (opcount == 0)
    {
        EXECUTE(status, end_of_command(opend))
        code_image[*ic - IC_INIT_VALUE] = new_opdata_thing(0, oc, 0, 1);
        (*ic)++;
        return MOVE;
    }
    i = (*ic) - IC_INIT_VALUE; /* Next available index in code image */

    str = next_nonnotewhite(opend);
    if (*str == ',')
        return ILLEGAL_COMMA_AFTER_OPERATION;

    /* At least one operand must be found */
    notfound_status = opcount > 1 ? MISSING_OPERAND_REQUIRED_TWO : MISSING_OPERAND_REQUIRED_ONE;
    EXECUTE(status, find_operand(&str, &op1_start, &op1_end, notfound_status))
    EXECUTE(status, get_operand_data(op1_start, op1_end, &addr1, &reg1, &data1))

    if (opcount > 1)
    {
        EXECUTE(status, find_operand(&str, &op2_start, &op2_end, notfound_status))
        EXECUTE(status, get_operand_data(op2_start, op2_end, &addr2, &reg2, &data2))
        EXECUTE(status, end_of_command(op2_end))
    }
    else
    {
        EXECUTE(status, end_of_command(op1_end))
        length = 2;

        if (addr1 == IMMEDIATE && oc == PRN_OP)
        {
            code_image[i++] = new_opdata_thing(0, oc, IMMEDIATE, length);
            code_image[i++] = new_int_thing(data1, Absolute);
        } else if (addr1 == DIRECT)
        {
            code_image[i++] = new_opdata_thing(0, oc, DIRECT, length);
            code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
        } else if (addr1 == REGISTER_DIRECT)
        {
            code_image[i++] = new_opdata_thing(0, oc, REGISTER_DIRECT, length);
            code_image[i++] = new_register_thing(0, reg1);
        }

    }

    if(opcount == 2)
    {
        if (addr1 == REGISTER_DIRECT && addr2 == REGISTER_DIRECT && oc != LEA_OP)
        {
            length = 2;
            code_image[i++] = new_opdata_thing(REGISTER_DIRECT, oc, REGISTER_DIRECT, length);
            code_image[i++] = new_register_thing(reg1, reg2);
        }
        else
        {
            length = 3;
            if (addr1 == REGISTER_DIRECT && addr2 == DIRECT && oc != LEA_OP)
            {
                code_image[i++] = new_opdata_thing(REGISTER_DIRECT, oc, DIRECT, length);
                code_image[i++] = new_register_thing(reg1, 0);
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
            } else if (addr1 == REGISTER_DIRECT && addr2 == IMMEDIATE && oc == CMP_OP)
            {
                code_image[i++] = new_opdata_thing(REGISTER_DIRECT, oc, IMMEDIATE, length);
                code_image[i++] = new_register_thing(reg1, 0);
                code_image[i++] = new_int_thing(data2, Absolute);
            } else if (addr1 == DIRECT && addr2 == DIRECT)
            {
                code_image[i++] = new_opdata_thing(DIRECT, oc, DIRECT, length);
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
            } else if (addr1 == DIRECT && addr2 == REGISTER_DIRECT)
            {
                code_image[i++] = new_opdata_thing(DIRECT, oc, REGISTER_DIRECT, length);
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
                code_image[i++] = new_register_thing(0, reg2);
            } else if (addr1 == DIRECT && addr2 == IMMEDIATE && oc == CMP_OP)
            {
                code_image[i++] = new_opdata_thing(DIRECT, oc, IMMEDIATE, length);
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
                code_image[i++] = new_int_thing(data2, Absolute);
            } else if (addr1 == IMMEDIATE && addr2 == REGISTER_DIRECT && oc != LEA_OP)
            {
                code_image[i++] = new_opdata_thing(IMMEDIATE, oc, REGISTER_DIRECT, length);
                code_image[i++] = new_int_thing(data1, Absolute);
                code_image[i++] = new_register_thing(0, reg2);
            } else if (addr1 == IMMEDIATE && addr2 == DIRECT && oc != LEA_OP)
            {
                code_image[i++] = new_opdata_thing(IMMEDIATE, oc, DIRECT, length);
                code_image[i++] = new_int_thing(data1, Absolute);
                code_image[i++] = new_int_thing(0, Relocatable); /*the 10 left bits will fill in second_pass */
            } else if (addr1 == IMMEDIATE && addr2 == IMMEDIATE && oc == CMP_OP)
            {
                code_image[i++] = new_opdata_thing(IMMEDIATE, oc, IMMEDIATE, length);
                code_image[i++] = new_int_thing(data1, Absolute);
                code_image[i++] = new_int_thing(data2, Absolute);
            }
        }
    }
    *ic = *ic + length;
    return MOVE;
}

static input_status check_entry_extern_inst(char *opend, char **symb_start, char **symb_end)
{
    input_status status;
    char *str = next_nonnotewhite(opend);
    if (*str == ',')
        return ILLEGAL_COMMA_AFTER_INST;

    EXECUTE(status, find_operand(&str, symb_start, symb_end, INST_REQ_OPERAND))
    EXECUTE(status, end_of_command(*symb_end))

    return MOVE;
}
