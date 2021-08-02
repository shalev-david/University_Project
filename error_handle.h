#ifndef ERROR_HANDLE_H
#define ERROR_HANDLE_H

#include "structures.h"


unsigned short double_comma(char c, unsigned short *error_flag, unsigned short *flag);

unsigned short valid_label(char* label, size_t size, symbols_table *table);

void error_handle(short err_code, char line[], short l, short c, char* file_name);


#endif