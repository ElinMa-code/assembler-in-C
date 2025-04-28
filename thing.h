#ifndef THING
#define THING
#include "globals.h"

#define BASE_OP_LENGTH 1

typedef enum thing_types {
    OPDATA,
    REGDATA,
    INTDATA
} thing_type;

/* Define the three separate structs */
struct OP_DATA_WORD {
    unsigned int ARE: 2;
    unsigned int type: 2;
    unsigned int length: 2;
    unsigned int src_adrs: 3; /* Source address */
    unsigned int opdata: 4;
    unsigned int dest_adrs: 3; /* Destination address */
};

struct REGISTER_WORD {
    unsigned int ARE: 2;
    unsigned int type: 2;
    unsigned int src_reg: 5; /* Source address */
    unsigned int dest_reg: 5; /* Destination address */
};

struct INTEGER_WORD {
    unsigned int ARE: 2;
    unsigned int type: 2;
    int data: 10;

};

/* Create the new union of the three structs */
typedef union thing_union {
    struct OP_DATA_WORD op_data_word;
    struct REGISTER_WORD register_word;
    struct INTEGER_WORD integer_word;
} thing;



/* This method creates and allocates memory for a new opdata thing.
And defines values according to the arguments given */
thing *new_opdata_thing(addressing_method, opcode, addressing_method, int);

/* This nonsense creates and assigns a memory to a new thing of an register. */
thing *new_register_thing(reg, reg);

/* This nonsense creates and assigns a memory to a new thing of an integer. */
thing *new_int_thing(int, memory_status);

/* Checks the type of thing, and fills the array accordingly */
void get_bits(char *, thing *);

/* This method creates and assigns two things defined for the DATA type,
With values of the base of the address given */
void alloc_address_thing(thing**, int, memory_status);

/* This method goes over everything in the array, according to the count given in the second argument,
And frees it from memory */
void free_thing_arr(thing **, int);


#endif
