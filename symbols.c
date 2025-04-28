#include <string.h>
#include "symbols.h"


static void make_symbol_attr(row_data *, symbol_purpose, int);

input_status add_symbol(table *label, char *start, char *end, symbol_purpose purpose, int addr)
{
    input_status status;
    boolean key_exists;
    row_data symb_data;
    if ((status = validate_symbol_name(start, end)) != MOVE)
        return status;

    make_symbol_attr(&symb_data, purpose, addr);

    key_exists = !add_entry(label, start, end, symb_data);

    if (key_exists)
        return SYMBOL_ALREADY_DEFINED;
    return MOVE;
}

input_status add_extern_symbol(table *label, char *start, char *end)
{
    input_status status;
    table prior_row, next_row;
    row_data symb_data;
    if ((status = validate_symbol_name(start, end)) !=MOVE)
        return status;

    make_symbol_attr(&symb_data, EXTERN, ADDR_EXTERN);

    prior_row = search_last_row_bef(label, start, end);
    if (!prior_row)
        add_entry(label, start, end, symb_data);
    else
    {
        next_row = prior_row->next;
        if (KEYOF_NODE_EQUALS(next_row, start, end))
        {

            if (!IS_EXTERN(next_row))
                return SYMBOL_ALREADY_DEFINED;

        }
        else
        {

            prior_row->next = new_row(start, end, symb_data, next_row);
        }
    }

    return MOVE;
}

int find_symbol_addr(table *label, char *start, char *end)
{
    table target_row;

    target_row = search_label_entry(label, start, end);

    if (!target_row)
        return ADDR_NOT_FOUND;
    return ROW_DATA(target_row);
}

input_status mark_entry_symbol(table *label, char *start, char *end)
{
    table target_row = search_entry(label, start, end);

    if (!target_row)
        return UNREC_SYMBOL_FOR_ENTRY;

    if (IS_EXTERN(target_row))
        return EXTERNAL_ENTRY_SYMBOL;

    IS_ENTRY(target_row) = TRUE;
    return MOVE;
}

void relocate_data_symbols(table *label, int ic)
{
    if (label != NULL)
    {
        table current_row = *label;
        int prior_value;
        while (current_row != NULL)
        {
            if (IS_DATA(current_row))
            {
                prior_value = ROW_DATA(current_row);
                ROW_DATA(current_row) = prior_value + ic;
            }
            current_row = current_row->next;
        }
    }
}

void add_extern_reference_thing(table *label, char *start, char *end, int base_addr)
{
    table prior_row;
    row_data thing_data;

    make_symbol_attr(&thing_data, EXTERN, base_addr);

    prior_row = search_last_row_bef(label, start, end);

    if (!prior_row)
        add_entry(label, start, end, thing_data);
    else
        prior_row->next = new_row(start, end, thing_data, prior_row->next);
}

static void make_symbol_attr(row_data *row_out, symbol_purpose purpose, int addr)
{
    (*row_out).symbol.data = addr;
    (*row_out).symbol.is_extern = purpose == EXTERN;
    (*row_out).symbol.is_code = purpose == CODE;
    (*row_out).symbol.is_data = purpose == DATA;
    (*row_out).symbol.is_entry = FALSE;
}
