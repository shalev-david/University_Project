#ifndef ERROR_HANDLE_H
#define ERROR_HANDLE_H

#include "structures.h"

/* Check if label is valid                                          */
/* ---- Input ----                                                  */
/* label - the label's name                                         */
/* size - the size of the label's name                              */
/* table - pointer for the head of the  symbol table linked list    */
/* ---- Return Value ----                                           */
/* 0 - the label is valid                                           */
/* 1 - first character isn't a letter                               */
/* 2 - the label have illegal character                             */
/* 3 - the label exist and defined                                  */
/* 4 - the label length is way to big                               */
/* 5 - the label exist but not defined (entry)                      */
/* 6 - the label exist and defined as external                      */
unsigned short valid_label(char* label, size_t size, symbols_table *table);

/* Handle each error and print the specific problem     */
/* ---- Input ----                                      */
/* err_code - which error has occured                   */
/* line - the given line                                */
/* l - the number of the row the error occured on       */
/* c - the place in the line where the error occured    */
/* file_name - the name of the file                     */
void error_handle(short err_code, char line[], short l, short c, char* file_name);


#endif