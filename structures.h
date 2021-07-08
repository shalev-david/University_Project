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

enum type {CODE, DATA, ENTRY, EXTERN};
enum code_type {R, I, J};
enum data_type {D, ASCIZ};

typedef struct {
    /* symbol table */
    char *symbol;
    unsigned short val;
    enum type arttbute;
    symbol_table *next;

}symbol_table;



typedef struct{
    /* code type command */
    enum code_type T;
    char word[WORD_LENGTH];
    char opcode[OPCODE_LENGTH];
    char rs[RS_RT_RD_LENGTH];
    char rt[RS_RT_RD_LENGTH];
    char rd[RS_RT_RD_LENGTH];
    char funct[FUNCT_LENGTH];
    char immed[IMMED_LENGTH];
    char reg;
    char address[ADDRESS_LENGTH];
    char hex[4][3];
}code;




void convert_code_to_binary(code *ptr);
void binary_to_hex(code *ptr);

#endif