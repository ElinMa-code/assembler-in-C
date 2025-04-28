#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include "table.h"
#include "symbols.h"
#include "globals.h"
#include "thing.h"


int is_comment(char* line)
{
    int i = 0;
    int result = 0;
    /* increase the index until the char isn't a white char. */
    for (; line[i] == ' ' || line[i] == '\t'; (i)++);

    if (line[i] == ';')
        result = 1;
    return result;
}

int is_empty(char* line, int index)
{
    int result = 0;

    /* increase the index until the char isn't a white char. */
    for (; line[index] == ' ' || line[index] == '\t'; (index)++);

    if (line[index] == '\n' || line[index] == EOF)
        result = 1;

    return result;
}

int check_label(char* line)
{
/* return the index of the end of the label (0 if no label)*/
    int i=0, j=0;

    /* increase the index until the char isn't a white char. */
    for (; line[i] == ' ' || line[i] == '\t'; (i)++);

    /* a label must start with an alphabetic letter */
    if ((line[i] <= 'Z' && line[i] >= 'A') || (line[i] <= 'z' && line[i] >= 'a'))
    {
        j++;

        while (line[i] != '\0' && line[i] != ' ' && line[i] != '\t' && line[i] != ':' && j < 31)
        {
            i++;
            j++;
        }

        if (line[i] == '\0' || line[i] == ' ' || line[i] == '\t' || (j >= 31 && line[i] != ':'))
            return 0;

        return i+1;
    }
    else
        return 0;

}

int get_guiding_sentence(char* line, int index)
{

    int result = -1;

    /* increase the index until the char isn't a white char. */
    for (; line[index] == ' ' || line[index] == '\t'; index++);

    if (line[index] == '.')
    {
        /* if guide is '.data': */
        if (line[index+1] == 'd' && line[index+2] == 'a' && line[index+3] == 't' && line[index+4] == 'a')
        {
            if  (line[index+5] == '\t'  || line[index+5] == ' ')
            {
                result = 1;
            }
        }

        /* if guide is '.string' */
        else if(line[index+1] == 's' && line[index+2] == 't' && line[index+3] == 'r' && line[index+4] == 'i' && line[index+5] == 'n' && line[index+6] == 'g')
        {
            if  (line[index+7] == '\t'  || line[index+7] == ' ')
            {
                result = 2;
            }
        }

        /* if guide is '.entry' */
        else if(line[index+1] == 'e' && line[index+2] == 'n' && line[index+3] == 't' && line[index+4] == 'r' && line[index+5] == 'y')
        {
            if  (line[index+6] == '\t'  || line[index+6] == ' ')
            {
                result = 3;
            }
        }

        /* if guide is '.extern' */
        else if(line[index+1] == 'e' && line[index+2] == 'x' && line[index+3] == 't' && line[index+4] == 'e' && line[index+5] == 'r' && line[index+6] == 'n')
        {
            if  (line[index+7] == '\t'  || line[index+7] == ' ')
            {
                result = 4;
            }
        }
    }

    return result;
}


int get_opcode(char* str)
{
    int opcode = -1;

    if (equals(str, "mov")){
        opcode = 0;
    }
    else if (equals(str, "cmp")){
        opcode = 1;
    }
    else if (equals(str, "add")){
        opcode = 2;
    }
    else if (equals(str, "sub")){
        opcode = 3;
    }
    else if (equals(str, "not")){
        opcode = 4;
    }
    else if (equals(str, "clr")){
        opcode = 5;
    }
    else if (equals(str, "lea")){
        opcode = 6;
    }
    else if (equals(str, "inc")){
        opcode = 7;
    }
    else if (equals(str, "dec")){
        opcode = 8;
    }
    else if (equals(str, "jmp")){
        opcode = 9;
    }
    else if (equals(str, "bne")){
        opcode = 10;
    }
    else if (equals(str, "red")){
        opcode = 11;
    }
    else if (equals(str, "prn")){
        opcode = 12;
    }
    else if (equals(str, "jsr")){
        opcode = 13;
    }
    else if (equals(str, "rts\r")){
        opcode = 14;
    }
    else if (equals(str, "stop\r")){
        opcode = 15;
    }
    return opcode;
}

int is_register(char* operand)
{
    /* if its a register */
    if (equals(operand, "@r0") || equals(operand, "@r1") || equals(operand, "@r2") || equals(operand, "@r3") || equals(operand, "@r4") || equals(operand, "@r5") || equals(operand, "@r6") || equals(operand, "@r7") || equals(operand, "@r0\r") || equals(operand, "@r1\r") || equals(operand, "@r2\r") || equals(operand, "@r3\r") || equals(operand, "@r4\r") || equals(operand, "@r5\r") || equals(operand, "@r6\r") || equals(operand, "@r7\r"))
        return 1;
    return 0;
}

int is_label(char* operand)
{
    /* check if operand is a label (not a strait value and not a register) */
    if ((is_register(operand)) || ((operand[0] >= '0' && operand[0] <= '9') || (operand[0] == '-')))
        return 0;

    return 1;
}


int second_pass(FILE* fp, table* tb, thing** code_image, int* data_image, char* file_name, int *ic)
{

    char* str;
    char* first_operand;
    char* second_operand;
    char* line;

    int opcode = 0;

    int opcount = 0;

    char current;

    int pass_section = 1;

    int flag = 1;

    int index_in_line = 0;

    int guiding = 0;

    int error = 0;

    thing* op_data;

    int length, i;

    int address;

    table tb_entry;

    table tb1;

    table tb2;

    thing* addr;

    /* allocate memory for the string. */
    str = (char*)calloc(1,sizeof(char));

    first_operand = (char*)calloc(1, sizeof(char));

    second_operand = (char*)calloc(1, sizeof(char));

    line= (char*)malloc(sizeof(char)*ROW_SIZE);

    printf("1\n");

    if (line == NULL || str == NULL)
    {
        perror("");
        exit(0);
    }

    while (flag)
    {
        switch (pass_section)
        {

            case 1:
            /* read the next line */
            	printf("2\n");
                index_in_line = 0;

                if ((current = fgetc(fp)) == EOF)
                {
                    /* go to 7 if end of file. */
                    pass_section = 6;
                }
                else
                {
                    /* seek 1 byte before */
                    fseek(fp, -1, SEEK_CUR);
                    /* get the line */
                    fgets(line, ROW_SIZE, fp);

                    printf("%s", line);

                    /* continue only if not a comment or if isn't a black line. */
                    if (!(is_comment(line) || is_empty(line, 0)))
                        pass_section++;

                }
                break;

            case 2:
                /* index */
                i = *ic - IC_INIT_VALUE;

            	printf("3\n");
                /* if there's a label, skip it (index_in_line will be 0 if there isn't). */
                index_in_line = check_label(line);
                if (index_in_line != 0)
                {
                    skip_white(line,&index_in_line);
                    if(get_guiding_sentence(line, index_in_line) != -1)
                    {
                        pass_section = 1;
                        break;
                    }
                }

                /* continue only if not a comment or if isn't a blank line. */
                if (is_empty(line, index_in_line))
                    pass_section = 1;
                else
                {
                    length = 0;
                    if (index_in_line == 0 && get_guiding_sentence(line, 0) != (-1))
                    {
                        pass_section++;
                        break;
                    }

                    op_data = code_image[i++];

                    length = op_data->op_data_word.length;
                    if(length > 0)
                        *ic = *ic + length;
                    pass_section++;
                }
                break;

            case 3:
            	printf("4\n");
            	/* get number of guiding sentence */
                guiding = get_guiding_sentence(line, index_in_line);
                switch (guiding)
                {
                    /* if not entry */
                    case -1:
                        pass_section = 5;
                        break;
                    /* if entry */
                    case 3:
                        pass_section++;
                        break;
                    /* error */
                    default:
                        pass_section = 1;
                        break;
                }
                break;

            case 4:
                /* skip one word and read the next one (guiding sentence) */
                getWord(str, line, &index_in_line);
                skip_white(line, &index_in_line);
                /* get the operand */
                getWord(first_operand, line, &index_in_line);

                /* if didnt get an operand */
                if (strlen(first_operand) < 1)
                {
                    error = 1;
                    pass_section = 6;
                    break;
                }

                /* check if the first operand is a label */
                if (is_label(first_operand))
                {
                    /* get symbol in table */
                    tb_entry = search_label_entry(tb, first_operand, &(first_operand[strlen(first_operand)-1]));

                    /* if not in table */
                    if (tb_entry == NULL)
                    {
                        error = 1;
                        pass_section = 6;
                        break;
                    }

                    /* mark as entry */
                    tb_entry->data.symbol.is_entry = 1;
                }
                /* do error if not a label */
                else
                {
                    error = 1;
                    pass_section = 6;
                    break;
                }

                /* continue number 5 */
                pass_section = 1;
                break;

            case 5:

                /* get the function name */
                getWord(str, line, &index_in_line);

                /* skip spaces */
                skip_white(line, &index_in_line);

                /* get opcode */
                opcode = get_opcode(str);

                /* get opcode expected operands number */
                opcount = num_of_operands(opcode);
                /* if gets operands*/
                if (opcode >= 0 && opcode <= 13)
                {
                    /* get the first operand */
                    getWord(first_operand, line, &index_in_line);

                    /* if didnt get an operand */
                    if (strlen(first_operand) < 1)
                    {
                        error = 1;
                        pass_section = 6;
                        break;
                    }

                    /* check if the first operand is a label */
                    if (is_label(first_operand))
                    {
                        tb1 = search_label_entry(tb, first_operand, &(first_operand[strlen(first_operand)-1]));

                        if (tb1 == NULL)
                        {
                            error = 1;
                            pass_section = 6;
                            break;
                        }
                        /* if extern */
                        if (tb1->data.symbol.is_extern)
                        {
                            code_image[i]->integer_word.ARE = 1;
                            code_image[i++]->integer_word.data = 0;
                        }
                        else
                        {
                            /* get address of symbol */
                            address = find_symbol_addr(tb, first_operand, &(first_operand[strlen(first_operand) - 1]));
                            /* get address thing */
                            alloc_address_thing(&addr, address, Relocatable);
                            /* put the label address in correct place that was empty in first_pass */
                            code_image[i++] = addr;
                        }
                    }

                    skip_white(line, &index_in_line);

                    /* Checks for a comma between first operand and second operand */
                    if (line[index_in_line] == ',')
                    {
                        if (opcode > 3 && opcode != 6)
                        {
                            error = 1;
                            pass_section = 6;
                            break;
                        }

                        index_in_line++;

                        /* get the second operand */
                        getWord(second_operand, line, &index_in_line);

                        /* if second operator is label */
                        if (is_label(second_operand))
                        {
                            tb2 = search_label_entry(tb, second_operand, &(second_operand[strlen(second_operand)-1]));
                            if (tb2 == NULL)
                            {
                                error = 1;
                                pass_section = 6;
                                break;
                            }
                            /* if the first operand is label i points to third row in opdata,
                             * if not i points to second row in opdata, and i need to increase 1 more */
                            if(!is_label(first_operand))
                            {
                                i++;
                            }
                            /* if extern */
                            if (tb2->data.symbol.is_extern)
                            {
                                code_image[i]->integer_word.ARE = EXTERN;
                                code_image[i++]->integer_word.data = 0;
                            }
                            else
                            {
                                address = find_symbol_addr(tb, second_operand, &(second_operand[strlen(second_operand) - 1]));
                                /* get address thing */
                                alloc_address_thing(&addr, address, Relocatable);
                                /* add the DATA to the code(if first was label so i++ if not so: */
                                code_image[i++] = addr;
                            }
                            pass_section = 1;
                            break;
                        }
                        pass_section = 1;
                        break;
                    }
                    else if(opcount == 2)
                    {
                        error = 1;
                        pass_section = 6;
                    }
                    pass_section = 1;
                    break;
                }
                else if (opcode == -1)
                {
                    error = 1;
                    pass_section = 6;
                }
                else
                    pass_section = 1;
                break;

            case 6:
                if (error)
                    exit(0);
                else
                    pass_section = 7;
                break;

            case 7:

                flag = 0;
                break;
        }
    }
    return 0;
}
