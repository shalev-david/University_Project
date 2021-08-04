#ifndef STRUCTURES_H
#define STRUCTURES_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

/* constant lengths */
#define WORD_LENGTH 33
#define OPCODE_LENGTH 7
#define RS_RT_RD_LENGTH 6
#define FUNCT_LENGTH 6
#define IMMED_LENGTH 17
#define ADDRESS_LENGTH 26
#define NOT_IN_USE_LENGTH 6
#define BYTE_LENGTH 8
#define HEX_SIZE 3
#define LABEL_LENGTH 32
#define MAX_INT_8 127
#define MIN_INT_8 -128
#define MAX_INT_16 32767
#define MIN_INT_16 -32768
#define MAX_INT_32  2147483647
#define MIN_INT_32 -2147483647
#define MAX_CHARACTERS 80
#define R_COMMANDS 8
#define I_COMMANDS 15
#define J_COMMANDS 4
#define INSTRUCTION_COMMANDS 6
#define REGISTERS 32

enum type {CODE, DATA};
enum code_type {R, I, J};

/* define the structures */
typedef struct binary_file binary_file;
/* Linked Lists */
typedef struct code code;
typedef struct data data;
typedef struct data_val data_val;
typedef struct symbols_table symbols_table;
typedef struct external_file external_file;
typedef struct entry_file entry_file;


struct code{
    /* Code typed command */

    /* Pointer to the next node */
    code* next;
    /* Contain the specific code type command */
    enum code_type T_CODE;

    /* All of the variables below are binary strings */
    char word[WORD_LENGTH];
    char opcode[OPCODE_LENGTH];
    char rs[RS_RT_RD_LENGTH];
    char rt[RS_RT_RD_LENGTH];
    char rd[RS_RT_RD_LENGTH];
    char funct[FUNCT_LENGTH];
    char immed[IMMED_LENGTH];
    char reg;
    char address[ADDRESS_LENGTH];

    /* label - contain the label's name in case of command who uses a label */
    /* hex - contain 4 hex strings that were converted from the code->word[33] binary string */
    /* line - contain the line number that should be in the .obj file */
    /* original_line - contain the original line in the given file */
    char *label;
    char hex[4][3];
    unsigned short line;
    unsigned short original_line;
    
};

struct data{
    /* Data typed command */

    /* Pointer to the next node */
    data* next;
    /* Pointer to a data_val linked list */
    data_val * val;

    /* bytes_cooupied - contain the number of bytes that data->val contains */
    /* line - contain the line number that should be in the .obj file */
    /* original_line - contain the original line in the given file */
    unsigned short bytes_occupied;
    unsigned short line;
    unsigned short original_line;
};

struct data_val{
    /* Pointer to the next node */
    data_val *next;

    /* Binary string */
    char val[WORD_LENGTH];

    /* Number of bytes that the binary string occupies - 1, 2 or 4 bytes */
    size_t size;

    /* Contain [size] hex strings that were converted from the data_val->val[33] binary string */
    char hex[4][3];
    
};

struct symbols_table{
    /* Symbol Table */

    /* Pointer to the next node */
    symbols_table *next;
    /* save the type of the flag */
    enum type T;
    

    /* The label's name */
    char *symbol;

    /* The line where the label is set */
    unsigned short val;

    /* If the label is external or entry the flag will turn up */
    unsigned short ext;
    unsigned short ent;

    /* If the label is set to entry and still wasn't defined the flag will turn up */
    unsigned short unassigned;
    
};

struct binary_file{
    /* Binary File */

    /* contain the number of bytes occupied by the code commands */
    unsigned short code_size;

    /* code line counter */
    unsigned short IC;

    /* data line counter - also the size of bytes occupied by the data commands */
    unsigned short DC;

    /* pointer to the code, data, symbol_data structures */
    code* code_head;
    data* data_head;
    symbols_table* labels;

};


struct external_file{
    /* External File */

    /* Pointer to the next node */
    external_file* next;

    /* pointer to the label */
    symbols_table* label;
    
    /* the line where the label was used */
    unsigned short line;
};

struct entry_file{
    /* Entry File */

    /* Pointer to the next node */
    entry_file* next;

    /* pointer to the label */
    symbols_table* label;
    
    /* the line where the label were defined */
    unsigned short line;
    
};


#endif