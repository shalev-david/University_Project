#ifndef WORD_H
#define WORD_H

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


enum type {R_WORD, I_WORD, J_WORD};


typedef struct{
    enum type T;
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
}word;

void convert_to_binary(word *ptr);
void binary_to_hex(word *ptr);

#endif