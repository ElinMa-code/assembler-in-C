#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void skip_white(char* str, int* index)
{

    /* increase the index until the char isn't a white char. */
    for (; str[*index] == ' ' || str[*index] == '\t'; (*index)++);
}

int equals(char* s1, char* s2)
{
/*
    checks if two strings are equal.
    return 't' if equal, 'f' if not.

*/
    /* index for both strings */
    int i = 0;

    /* while every item is equal in both strings */
    while (s1[i] != '\0' && s1[i] == s2[i])
        i ++;
    /* if ended with equal chars */
    if (s1[i] == s2[i])
        return 1;
    /* if not */
    return 0;
}

void getWord(char* str, char* line, int* index)
{
/*
    get a word from the line starting from the given index.
    the result wil be in the inputted string "str".
*/
    /* length of the word */
    int length_str = 0;

    /* skip spaces */
    skip_white(line, index);

    /* for every char in the line starting from the index until the end of the line. */
    for (; line[*index] != '\n' && line[*index] != '\0'; (*index)++){

        /* if the char is a white char (spaces and tabs) */
        if(line[*index] == ' ' || line[*index] == '\t' || line[*index] == ',')
        {
            *(str + length_str) = '\0';
            return;
        }
        else
        {
            /* if not a white char */
            /* increase the length */
            length_str += 1;
            /* reallocate the string to make space for another char */
            str = (char*)realloc(str, length_str);

            /* if memory allocation failed, exit */
            if (str == NULL)
            {
                perror("");
                exit(0);
            }
            /* add the char and change the digits flag to 't'(true) */
            str[length_str-1] = line[*index];
        }
    }
    /* end the string. */
    *(str + length_str) = '\0';
    return;
}

double getNumber(char* line, int index)
{
/* returns a single integer from the line. returns 0.5(not an integer) if didn't get enough numbers before EOF. */

    /*the whole number */
    int number = 0;
    char digits = 'f';

    /* flag to multiply the number by (if  number is negative, it will be -1.)n*/
    short flagN = 1;

    if (line[index] == '-')
        flagN = -1;

    while (1)
    {
        /*  if char is a digit */
        if(line[index] <= '9' && line[index] >= '0')
        {
            /* add the digit to the number and change the flag. */
            number *= 10;
            number += (line[index] - '0');
            digits = 't';
        }
            /* if char isn't a digit */
        else
        {
            if (digits == 'f')
                return 0.5;
            return flagN * number;
        }
        /* next char in line */
        index++;
    }
}


