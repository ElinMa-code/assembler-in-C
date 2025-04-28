#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "charsequence.h"



#define NOTEWHITE(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')
#define NON_NOTEWHITE(c) (!(NOTEWHITE(c)))
#define NOTEWHITE_OR_COMMA(c) (NOTEWHITE(c) || (c) == ',')
#define NOTEWHITE_OR_COLON(c) (NOTEWHITE(c) || (c) == ':')
#define NON_DIGITAL(c) (!isdigit(c))
#define NON_ALNUM(c) (!isalnum(c))

#define DEFINE_NEXT_FUNCTION(func_name, filter) \
char *func_name(char *str)\
{\
    char *p;\
    for (p = str; *p && !filter(*p); p++);\
    return p;\
}\

DEFINE_NEXT_FUNCTION(next_notewhite, NOTEWHITE)
DEFINE_NEXT_FUNCTION(next_nonnotewhite, NON_NOTEWHITE)
DEFINE_NEXT_FUNCTION(next_notewhite_or_comma, NOTEWHITE_OR_COMMA)
DEFINE_NEXT_FUNCTION(next_notewhite_or_colon, NOTEWHITE_OR_COLON)
DEFINE_NEXT_FUNCTION(next_non_digital, NON_DIGITAL)
DEFINE_NEXT_FUNCTION(next_non_alnum, NON_ALNUM)

char *next(char target, char *str)
{
    char *p;
    for (p = str; *p && *p != target; p++);
    return p;
}

boolean str_equal(char *start, char *end, char *compare_to)
{
    int result;
    char temp = *end;
    *end = '\0';
    result = !strcmp(start, compare_to);
    *end = temp;
    return result ? TRUE : FALSE;
}

boolean custom_string_compare(char *str1, char *str2)
{
    while (*str1 != '\0' && *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return -1;
        }
        str1++;
        str2++;
    }

    return (*str1 == '\0' && *str2 == '\0');
}
/*
boolean str_equal_symbol(char *start, char *end, table *label)
{
        char *current_row;
        table curr = *label;
        char *key_copy;
        while (curr != NULL)
        {
            current_row = (*label)->key;
            key_copy = strcpy_safe(start, end);
            if(custom_string_compare(key_copy,current_row))
                return 1;
            curr =(*label)->next;
        }
    return 0;
}
*/
boolean str_bef(char *start, char *end, char *compare_to)
{
    int result;
    char temp = *end;
    *end = '\0';
    result = strcmp(start, compare_to);
    *end = temp;
    return result < 0 ? TRUE : FALSE;
}

boolean str_to_int(char *start, char *end, int *to_assign)
{
    char temp = *end;

    if ((!isdigit(*start) && *start != '-' && *start != '+')
        || next_non_digital(start + 1) != end)
        return FALSE;

    *end = '\0';
    *to_assign = atoi(start);
    *end = temp;
    return TRUE;
}

void eraseLineFromFile(char *filename, int lineToRemove)
{
    FILE *file;
    FILE *tempFile;
    char buffer[1024];
    int currentLine = 1;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    /* Create a temporary file to write the modified content */
    tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error creating temporary file");
        fclose(file);
        return;
    }

    /* Read and copy lines from the original file to the temporary file */
    while (fgets(buffer, sizeof(buffer), file))
    {
        if (currentLine != lineToRemove)
        {
            fputs(buffer, tempFile);
        }
        currentLine++;
    }

    fclose(file);
    fclose(tempFile);

    /* Replace the original file with the temporary file */
    if (remove(filename) != 0) {
        perror("Error deleting original file");
        return;
    }
    if (rename("temp.txt", filename) != 0)
    {
        perror("Error renaming temporary file");
    }
}
