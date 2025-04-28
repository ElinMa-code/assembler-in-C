#ifndef GLOBALS
#define GLOBALS

#define IC_INIT_VALUE 100
#define MAX_LINE_LENGTH 82
#define BITS_IN_THING 12
#define MAX_CODE_IMAGE_SIZE 1024
#define NON_REG (-1)
#define NONE (-1)
#define NON_OP (-1)
#define ASSEMBLY_POSTFIX ".as"
#define ASSEMBLY_POST_MAC_POSTFIX ".am"

typedef enum { FALSE = 0, TRUE = 1 } boolean;

typedef enum ARE {
    Absolute = 0, /* 00 */
    Extern = 1, /* 01 */
    Relocatable = 2 /* 10 */
} memory_status;

typedef enum  registers {
    r0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7
} reg;

typedef enum addressing_methods {
    IMMEDIATE = 1,
    DIRECT = 3,
    REGISTER_DIRECT = 5
} addressing_method;

typedef enum opcodes {
    MOV_OP = 0,
    CMP_OP = 1,
    ADD_OP = 2,
    SUB_OP = 3,
    NOT_OP = 4,
    CLR_OP = 5,
    LEA_OP = 6,
    INC_OP = 7,
    DEC_OP = 8,
    JMP_OP = 9,
    BNE_OP = 10,
    RED_OP = 11,
    PRN_OP = 12,
    JSR_OP = 13,
    RTS_OP = 14,
    STOP_OP = 15

} opcode;

#endif