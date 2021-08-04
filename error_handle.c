#include "error_handle.h"

unsigned short valid_label(char* label, size_t size, symbols_table* table){
    unsigned i;
    symbols_table* ptr = table;

    if(size >=LABEL_LENGTH) return 4;
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
    /* if it is check if its not an entry label thats wating for value */
    while (ptr->next != NULL){
        if(!strncmp(label, ptr->symbol, size))
            if(size == strlen(ptr->symbol)){
                if(!ptr->unassigned)
                    return 3;
                /* the given label is already in the table and waiting to be filled */
                else if(ptr->ext) return 6;
                else return 5;
            }
        ptr = ptr->next;
    }
    
    return 0;
}

void error_handle(short err_code, char line[], short l, short c, char* file_name){
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Error in phase %d of the encoding of file %s\n\n", err_code < 100 ? 1: (err_code < 200 ? 2: 3) , file_name);
    switch (err_code)
    {
        case -1:{
            printf("-- Failed to allocate space. Please try to execute again --\n\n");
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
        case 4:{
            printf("-- There should be a space after the command --\n\n");
            break;
        }
        case 5:{
            printf("-- There shouldn't be a comma after the command --\n\n");
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
        case 15:{
            printf("-- A label can have 31 characters at maximum --\n\n");
            break;
        }
        case 16:{
            printf("-- The given label already declared as extern and can not be declared in this file --\n\n");
            break;
        }
        case 17:{
            printf("-- I Branch command can't be executed with extern label --\n\n");
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
            printf("-- Missing Integer --\n\n");
            break;
        }
        case 33:{
            printf("-- Immediate can only be Integers --\n\n");
            break;
        }
        case 34:{
            printf("-- Missing String --\n\n");
            break;
        }
        case 35:{
            printf("-- Missing Label --\n\n");
            break;
        }
        case 40:{
            printf("-- Missing quotation mark --\n\n");
            break;
        }
        case 50:{
            printf("-- Label can't be identified as extern and entry at the same time --\n\n");
            break;
        }
        case 101:{
            printf("-- The label isn't declared in this file --\n\n");
            break;
        }
        case 102:{
            printf("-- The label is set to entry and isn't declared in this file --\n\n");
            break;
        }
        case 200:{
            printf("-- Filed to create files. Please try to execute again --\n\n");
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