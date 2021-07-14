#ifndef STRUCTURES_H
#define STRUCTURES_H
#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define WORD_LENGTH 33
#define OPCODE_LENGTH 7
#define RS_RT_RD_LENGTH 6
#define FUNCT_LENGTH 6
#define IMMED_LENGTH 17
#define ADDRESS_LENGTH 26
#define NOT_IN_USE_LENGTH 6
#define BYTE_LENGTH 8
#define HEX_SIZE 3
#define LABEL_LENGTH 26
#define MAX_INT 32767
#define MIN_INT -32768
#define R_COMMANDS 8
#define I_COMMANDS 15
#define J_COMMANDS 4
#define INSTRUCTION_COMMANDS 7
#define REGISTERS 32

enum type {CODE, DATA, ENTRY, EXTERN};
enum code_type {R, I, J};
enum data_type {DB, DH, DW, ASCIZ};

/* define the structures */
typedef struct code code;
typedef struct data data;
typedef struct symbols_table symbols_table;
typedef struct binary_file binary_file;
typedef struct external_file external_file;
typedef struct entry_file entry_file;

struct code{
    /* code typed command */
    code* next;
    enum type T;
    enum code_type T_CODE;

    /* all of the variables below are binary strings */
    char word[WORD_LENGTH];
    char opcode[OPCODE_LENGTH];
    char rs[RS_RT_RD_LENGTH];
    char rt[RS_RT_RD_LENGTH];
    char rd[RS_RT_RD_LENGTH];
    char funct[FUNCT_LENGTH];
    char immed[IMMED_LENGTH];
    char reg;
    char address[ADDRESS_LENGTH];

    /* these are regular variables */
    char *label;
    char hex[4][3];
    unsigned short line;
    unsigned short original_line;
    
};

struct data{
    /* data typed command */
    data* next;
    enum type T;
    enum data_type T_DATA;

    unsigned short size;
    unsigned short line;
    unsigned short original_line;
};

struct symbols_table{
    /* symbol table */
    char *symbol;
    unsigned short val;
    enum type T;
    symbols_table *next;
};

struct binary_file{
    unsigned short code_size;
    unsigned short data_size;
    code* code_head;
    data* data_head;
    symbols_table* labels;

};

struct external_file{
    unsigned short line;
    symbols_table* label;
    external_file* next;
};

struct entry_file{
    unsigned short line;
    symbols_table* label;
    entry_file* next;
};

void convert_code_to_binary(code *ptr);
void binary_to_hex(code *ptr);

#endif