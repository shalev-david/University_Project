#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "error_handle.h"

const char *R_commands[8];
const char *I_commands[15];    
const char *J_commands[4];
const char *instructions_commands[7];


enum state {START, LABEL, RS, RT, RD, IMMED, ADDRESS, END, STOP};
enum commands {R_LOGIC, R_COPY, I_LOGIC, I_BRANCH, I_MEMORY, J_JUMP, J_OTHER};



short check_R_command(char *line);
short check_I_command(char *line);
short check_J_command(char *line);

short build_binary_file(binary_file *bf, char *file_name);

short find_register(char *line);

unsigned short phase1(FILE *fp, char* file_name);
unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf, char *file_name);

void int_to_binary(char dest[], int num, short bits);

void allocate(binary_file *bf);
#endif