#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "second.h"
#include "utilities.h"
#include "mac.h"
#include "table.h"
#include "first_pass.h"


int main(int argc, char** argv)
{
    /* index for pointer of arguments */
    int i;

    int dc;

    int icf = IC_INIT_VALUE;

    FILE* macro_file;

    FILE* source_file;

    table symbol_table = NULL;

    thing* code_image[MAX_CODE_IMAGE_SIZE];

    int data_image[MAX_CODE_IMAGE_SIZE];

    char source_file_name[100];

    char macro_file_name[100];



    /* check if got at least 1 file */
    if (argc < 2)
    {
        printf("no file inputted!\n");
        exit(0);
    }

    /* for every file name argument */
    for (i=1; i < argc; i++)
    {

        /* add ending to filename */
        snprintf(source_file_name, sizeof(source_file_name), "%s.as", argv[i]);

        /*printf("filename: %s\n", file_name);*/

        /* try to open the file for reading only ("r")*/
        source_file = fopen(source_file_name, "r");

        /* if couldn't open file */
        if (source_file == NULL)
        {
            printf("%s - can't be opened!\n", source_file_name);
            exit(0);
        }
            /* if opened the file */
        else
        {
            strcpy(macro_file_name, source_file_name);
            /* run both passes on the file */
            macro_file_name[strlen(macro_file_name)-2] = 'm';

            /* open or create macro file */
            macro_file = fopen(macro_file_name, "w");

            /* if macro spreading goes successfully */
            if (mac_move(source_file, macro_file))
            {
                macro_file = fopen(macro_file_name, "r+");
                printf("passed mcro on %s\n", macro_file_name);
                first_pass(macro_file, code_image, data_image, &symbol_table, &dc);
                second_pass(macro_file, &symbol_table, code_image, data_image, macro_file_name, &icf);
                write_files(macro_file_name, code_image, icf, data_image, dc, symbol_table , symbol_table);
            }
            fclose(macro_file);
            fclose(source_file);
        }
    }

    printf("\n");
    return 0;

}