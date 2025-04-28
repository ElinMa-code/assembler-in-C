#include "functioning.h"

static char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void *malloc_safe(int bytes)
{
    void *result = malloc(bytes);
    if (result == NULL)
    {
        printf("Fatal: memory allocation failed");
        exit(1);
    }

    return result;
}

char *strcat_safe(char *str1, char *str2)
{
    int length1, length2;
    char *result;

    length1 = strlen(str1);
    length2 = strlen(str2);
    result = malloc_safe(SIZEOF_STR(length1 + length2));
    memcpy(result, str1, length1);
    memcpy(result+length1, str2, length2);
    result[length1 + length2] = '\0';
    return result;
}

char *strcpy_safe(char *start, char *end)
{
    char *result;
    int length = end - start;
    result = malloc_safe(SIZEOF_STR(length));
    memcpy(result, start, length);
    result[length] = '\0';
    return result;
}

boolean fopen_check(FILE **out, char *filename, char *postfix, char *permissions)
{
    char *full_filename = strcat_safe(filename, postfix);
    *out = fopen(full_filename, permissions);
    free(full_filename);
    return *out != NULL;
}

FILE *fopen_safe(char *filename, char *postfix, char *permissions)
{
    FILE *target;
    boolean open_success = fopen_check(&target, filename, postfix, permissions);
    if (!open_success)
    {
        printf("Fatal: Opening file %s%s failed", filename, postfix);
        exit(1);
    }
    return target;
}

int encode_registers(reg src, reg dest)
{
    int encodedValue = 0;

    /* Encode src (first 5 bits) */
    encodedValue |= (src & 0x1F) << 7;

    /* Encode dest (second 5 bits) */
    encodedValue |= (dest & 0x1F) << 2;

    /* Encode ARE (last 2 bits) */
    encodedValue |= (00 & 0x03);

    return encodedValue;
}

int encode_data(addressing_method src, opcode opdata, addressing_method dest, memory_status ARE)
{
    int encodedValue = 0;

    /* Encode src (first 3 bits) */
    encodedValue |= (src & 0x07) << 9;

    /* Encode opdata (4 bits after src) */
    encodedValue |= (opdata & 0x0F) << 5;

    /* Encode dest (3 bits after opdata) */
    encodedValue |= (dest & 0x07) << 2;

    /* Encode ARE (last 2 bits) */
    encodedValue |= (ARE & 0x03);

    return encodedValue;
}

int encode_int(int data, memory_status ARE)
{
    int encodedValue = 0;

    /* Set the left 10 bits to data */
    encodedValue |= (data & 0x3FF) << 2;

    /* Set the right 2 bits to ARE */
    encodedValue |= (ARE & 0x03);

    return encodedValue;

}

void get_integer_base64(base64 bits64, int data)
{
    /* extract the first 6 bits and last 6 bits */
    int firstSixBits = (data >> 6) & 0x3F;
    int lastSixBits = data & 0x3F;

    /* convert the extracted 6 bits into base 64 characters */
    bits64[0] = base64_table[firstSixBits];
    bits64[1] = base64_table[lastSixBits];
}