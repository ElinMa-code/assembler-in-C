#ifndef CHARSEQ
#define CHARSEQ
#include "globals.h"



/* This method returns a pointer to a character that was not the next white in a given string
 Or until the end of the string if not found */
extern char *next_nonnotewhite(char *);

/* This method returns a pointer to the next white character in the existing string
Or until the end of the string if not found */
extern char *next_notewhite(char *);

/* This method returns a pointer to the white character or the next white comma in a given string
 Or until the end of the string if not found */
extern char *next_notewhite_or_comma(char *);

/* This method returns a pointer to the next white character or colon in a given string
Or until the end of the string if not found */
extern char *next_notewhite_or_colon(char *);

/*This method  returns a pointer to the next character that is not a digit
or to the end of string if not found */
extern char *next_non_digital(char *);

/*This method  returns a pointer to the next character that is not a digit or a letter
or to the end of string if not found */
extern char *next_non_alnum(char *);

/*This method  returns a pointer to the next character in a given string that is equal to a given character,
ot to the end of string if not found  */
extern char *next(char, char *);

/* This method gets a pointer to the beginning of a string, and points to its end
And compares it with the existing string.
Returns true if indeed worthwhile, otherwise lie */
boolean str_equal(char *, char *, char *);

/* The method gets a pointer to the beginning of a string, and a pointer to its end
And compares it with a given string.
Return true if before the string compared lexicographically, otherwise false */
boolean str_bef(char *, char *, char *);

/* The method gets a pointer to the beginning of a string, and a pointer to its end
And analyzes it into an integer if it is a valid integer
And assigns the result to a voter given to it.
Returns true if converted to number, false otherwise */
boolean str_to_int(char *, char *, int *);

void eraseLineFromFile(char *, int);
/*
boolean str_equal_symbol(char *, char *, table *);
*/
boolean custom_string_compare(char *, char *);

#endif
