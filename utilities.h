#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED
#define ROW_SIZE 81
#define LEN(a) (sizeof(a) / sizeof(*a))
#define XOR(a, b) ((a || b)&&(!(a&&b)))

void skip_white(char* str, int* index);

int equals(char* s1, char* s2);

void getWord(char* str, char* line, int* index);

double getNumber(char* line, int index);

#endif /* UTILITIES_H_INCLUDED */