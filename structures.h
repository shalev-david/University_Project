#ifndef STRUCTURES_H
#define STRUCTURES_H

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
typedef struct code code;
typedef struct data data;
typedef struct data_val data_val;
typedef struct symbols_table symbols_table;
typedef struct binary_file binary_file;
typedef struct external_file external_file;
typedef struct entry_file entry_file;


struct code{
    /* code typed command */
    code* next;
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
    data_val * val;


    unsigned short bytes_occupied;
    unsigned short line;
    unsigned short original_line;
};

struct data_val{
    char val[WORD_LENGTH];
    data_val *next;
    char hex[4][3];
    size_t size;
};

struct symbols_table{
    /* symbol table */
    char *symbol;
    unsigned short val;
    unsigned short ext;
    unsigned short ent;
    unsigned short unassigned;
    enum type T;
    symbols_table *next;
};

struct binary_file{
    unsigned short code_size;
    unsigned short IC;
    unsigned short DC;
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


#endif