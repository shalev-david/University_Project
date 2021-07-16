#include "error_handle.h"

unsigned short double_comma(char c, unsigned short *error_flag, unsigned short *flag){
    if(c == ','){
        /* error handle - double comma */

        *error_flag = 1;
        *flag = 0;
        return 1;
    }
    return 0;
}

unsigned short valid_label(char* label, size_t size, symbols_table* table){
    unsigned i;
    symbols_table* ptr = table;
    /* first character should be a letter */
    if((*label > 'Z' && label[0] < 'a') || *label < 'A' || *label > 'z'){
        /* error handle - first character must be a letter */

        return 1;
    }
    i=1;
    while(i<size){
        /* validate the characters of the label - only letters and numbers are allowed */
        if((label[i] > 'Z' && label[i] < 'a') || label[i] > 'z' || (label[i] > '9' && label[i] < 'A') || label[i] < '0'){
            /* error handle - illegal character */

            return 2;
        }
        i++;
    }

    /* now check if the label is already exist in the symbol table */
    while (ptr->next!= NULL){
        if(!strncmp(label, ptr->symbol, size)) if(size == strlen(ptr->symbol))return 3;
        ptr = ptr->next;
    }
    
    return 0;
}

void error_handle(short err_code, char* line, short l, short c, char* file_name){
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Error in phase %d of the encoding of file %s\n", err_code < 100 ? 1: (err_code < 200 ? 2: 3) , file_name);
    switch (err_code)
    {
        case -1:{
            printf("-- Failed to allocate space --\n\n");
            exit(0);
        }
        case 0:{
            printf("-- Unkown command --\n\n");
            break;
        }
        case 1:{
            printf("-- Unkown register --\n\n");
            break;
        }
        case 2:{
            printf("-- There is a missing comma -- \n\n");
            break;
        }
        case 3:{
            printf("-- There is a double comma -- \n\n");
            break;
        }
        case 10:{
            printf("-- The label can not be the same as an existing command --\n\n");
            break;
        }
        case 11:{
            printf("-- First character of a label must be a letter --\n\n");
            break;
        }
        case 12:{
            printf("-- Label must contain letters and numbers only --\n\n");
            break;
        }
        case 13:{
            printf("-- The given label was already declared before --\n\n");
            break;
        }
        case 14:{
            printf("-- Missing label --\n\n");
            break;
        }
        case 20:{
            printf("-- The given numbner is out of range --\n\n");
            break;
        }
        case 21:{
            printf("-- Extraneous text after end of command --\n\n");
            break;
        }
        case 30:{
            printf("-- Missing Register --\n\n");
            break;
        }
        case 31:{
            printf("-- Missing Address --\n\n");
            break;
        }
        case 32:{
            printf("-- Missing Immediate --\n\n");
            break;
        }
        case 33:{
            printf("-- Immediate can only be numbers --\n\n");
            break;
        }
        case 100:{
            printf("-- This command cannot execute with an external label --\n\n");
            break;
        }
        case 101:{
            printf("-- The label isn't declared in this file --\n\n");
            break;
        }
        case 200:{
            printf("-- Filed to create the binary file --\n\n");
            break;
        }
            default:
                break;
    }
    if(err_code < 100){
        short i;
        printf("%d| %s\n", l, line);
        for(i=-3; i<c; i++){
            printf("~");
        }
        printf("^\n\n");
    }else if(err_code < 200) printf("In line: %d\n\n", l);
    printf("ERROR CODE: %d\n", err_code);
}

symbols_table* label_exist(char* label, symbols_table* table){
    symbols_table *ptr = table;
    while(ptr!= NULL){
        if(!strcmp(label, ptr->symbol)){
            return ptr;
        }
        
        ptr = ptr->next;
    }
    
    return NULL;
}