#include <stdio.h>
#include "first_pass.h"
#define REQ_FILLING(adrs) ((adrs) == DIRECT)


int first_pass(FILE* fp, thing** code_image, int* data_image, table* symbols, int *dc)
{
    int ic = IC_INIT_VALUE, icb;
    boolean is_success;

    is_success = first_move(fp, "input", code_image, &ic, data_image, dc, symbols);

    if (!is_success)
    {
        free_thing_arr(code_image, ic);
        free_table(symbols);
        return 0;
    }

    icb = ic;
    ic = IC_INIT_VALUE;
    rewind(fp);
    relocate_data_symbols(symbols, icb);
/*
    write_files("output", code_image, icb, data_image, dc, *symbols, NULL);
    free_thing_arr(code_image, icb);
    free_table(symbols);
*/
    return 0;
}



