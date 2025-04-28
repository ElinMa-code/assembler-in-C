#include <stdio.h>
#include "output.h"
#include "functioning.h"
#include "symbols.h"
#define LINE_FORMAT "%c%c"
#define BITSOF(bits) bits[0], bits[1]
#define NEWLINE_IF_NEEDED(not_needed, file, newline) \
    if (not_needed) \
        not_needed = FALSE; \
    else \
        fputs(newline, file);


static boolean write_ob(char *, thing **, int, int *, int);

static boolean write_ext(char *, table);

static boolean write_ent(char *, table);

boolean write_files(char *filename, thing **code_image, int icf,
    int *data_image, int dcf, table symbols, table externals)

{
    return write_ob(filename, code_image, icf, data_image, dcf)
        && write_ext(filename, symbols)
        && write_ent(filename, symbols);
}

boolean write_ob(char *filename, thing **code_image, int icf, int *data_image, int dcf)
{
    base64 bits;
    int i, code_image_length = icf - IC_INIT_VALUE;
    FILE *out = fopen_safe(filename, OBJECT_POSTFIX, "w");


    fprintf(out, "%d %d", code_image_length, dcf);

    /* Print code image */
    for (i = 0; i < code_image_length; i++)
    {
        fputs("\n", out);
        get_bits(bits, code_image[i]);
        fprintf(out, LINE_FORMAT, BITSOF(bits));
    }

    /* Print data image */
    for (i = 0; i < dcf; i++)
    {
        fputs("\n", out);
        get_integer_base64(bits, data_image[i]);
        fprintf(out, LINE_FORMAT, BITSOF(bits));
    }

    fclose(out);
    return TRUE;
}

boolean write_ext(char *filename, table externals)
{

    FILE *out;
    table current;
    int addr_loc;
    boolean is_first = TRUE;
    if (externals == NULL)
        return TRUE; /*  if Nothing to write */

    out = fopen_safe(filename, EXTERN_POSTFIX, "w");
    current = externals;

    while (current != NULL)
    {
        if (IS_EXTERN(current))
        {
            NEWLINE_IF_NEEDED(is_first, out, "\n")
            addr_loc = ROW_DATA(current);
            if (IS_EXTERN(current))
                fprintf(out, "%s  %d", current->key, addr_loc);
        }
        current = current->next;
    }

    fclose(out);
    return TRUE;
}

boolean write_ent(char *filename, table symbols)
{
    FILE *out;
    table current;
    int addr_loc;
    boolean is_first = TRUE;
    current = symbols;
    while (current != NULL && !IS_ENTRY(current))
        current = current->next;

    if (current == NULL) /* if you not find entry symbols */
        return TRUE;

    out = fopen_safe(filename, ENTRY_POSTFIX, "w");

    while(current != NULL)
    {
        if (IS_ENTRY(current))
        {
            NEWLINE_IF_NEEDED(is_first, out, "\n")
            addr_loc = ROW_DATA(current);
            if (IS_ENTRY(current))
            fprintf(out, "%s  %d", current->key, addr_loc);
        }
        current = current->next;
    }
    fclose(out);
    return TRUE;
}
