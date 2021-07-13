#ifndef STRUCTURES_H
#define STRUCTURES_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define WORD_LENGTH 33
#define OPCODE_LENGTH 6
#define RS_RT_RD_LENGTH 5
#define FUNCT_LENGTH 5
#define IMMED_LENGTH 16
#define ADDRESS_LENGTH 25
#define NOT_IN_USE_LENGTH 6
#define BYTE_LENGTH 8
#define HEX_SIZE 3
#define LABEL_SIZE 25
#define MAX_INT 32767
#define MIN_INT -32768

enum type {CODE, DATA, ENTRY, EXTERN};
enum code_type {R, I, J};
enum data_type {DB, DH, DW, ASCIZ};

/* define the structures */
typedef struct code code;
typedef struct data data;
typedef struct symbols_table symbols_table;
typedef struct binary_file binary_file;

struct code{
    /* code typed command */
    code* next;
    enum type T;
    enum code_type T_CODE;
    char word[WORD_LENGTH];
    char opcode[OPCODE_LENGTH];
    char rs[RS_RT_RD_LENGTH];
    char rt[RS_RT_RD_LENGTH];
    char rd[RS_RT_RD_LENGTH];
    char funct[FUNCT_LENGTH];
    char immed[IMMED_LENGTH];
    char label[LABEL_SIZE];
    char reg;
    char address[ADDRESS_LENGTH];
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



void convert_code_to_binary(code *ptr);
void binary_to_hex(code *ptr);

#endif