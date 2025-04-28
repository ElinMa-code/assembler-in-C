#include <stdlib.h>
#include "thing.h"
#include "functioning.h"


static thing *new_thing(thing_type, memory_status);

thing *new_opdata_thing(addressing_method src_adrs, opcode op, addressing_method dest_adrs, int length)
{
    thing *result = new_thing(OPDATA, Absolute);
    result->op_data_word.opdata = op;
    result->op_data_word.src_adrs = src_adrs;
    result->op_data_word.dest_adrs = dest_adrs;
    result->op_data_word.length = length; /* the amount rows this word need */

    return result;
}

thing *new_register_thing(reg src_reg, reg dest_reg)
{
    thing *result = new_thing(REGDATA, Absolute);
    /* when the bits are not used they will contain 0 */
    result->register_word.src_reg = src_reg == NON_REG ? r0 : src_reg;
    result->register_word.dest_reg = dest_reg == NON_REG ? r0 : dest_reg;

    return result;
}

thing *new_int_thing(int data, memory_status ARE)
{
    thing *result = new_thing(INTDATA, Absolute);
    result->integer_word.data = data;
    result->integer_word.ARE = ARE;

    return result;
}

void get_bits(base64 bits, thing *source)
{

    switch (source->integer_word.type)
    {
        case OPDATA:
            get_integer_base64(bits,encode_data
                    (source->op_data_word.src_adrs, source->op_data_word.opdata, source->op_data_word.dest_adrs, source->op_data_word.ARE));
            break;
        case REGDATA:
            get_integer_base64(bits,encode_registers
            (source->register_word.src_reg, source->register_word.dest_reg));
            break;
        case INTDATA:
            get_integer_base64(bits,encode_int
                    (source->integer_word.data, source->integer_word.ARE));
            break;
    }
}

void free_thing_arr(thing **things, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        free(things[i]);
    }
}

thing *new_thing(thing_type type, memory_status ARE)
{
    thing *result = malloc_safe(sizeof(thing));

    switch (type)
    {
        case REGDATA:
            result->register_word.type = type;
            result->register_word.ARE = ARE;
            break;
        case OPDATA:
            result->op_data_word.type = type;
            result->op_data_word.ARE = ARE;
            break;
        case INTDATA:
            result->integer_word.type = type;
            result->integer_word.ARE = ARE;
            break;
    }
    return result;
}

void alloc_address_thing(thing **op_address, int address, memory_status ARE)
{
    *op_address = new_thing(INTDATA, ARE);
    (*op_address)->integer_word.data = address;
}