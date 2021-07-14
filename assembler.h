#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "error_handle.h"

const char *R_commands[8];
const char *I_commands[15];    
const char *J_commands[4];
const char *instructions_commands[7];


enum state {START, LABEL, RS, RT, RD, IMMED, ADDRESS, END, STOP};
enum commands {R_LOGIC, R_COPY, I_LOGIC, I_BRANCH, I_MEMORY, J_JUMP, J_OTHER};

static unsigned short IC = 100;
static unsigned short DC = 0;


short check_R_command(char *line);
short check_I_command(char *line);
short check_J_command(char *line);

short find_register(char *line);

unsigned short phase1(FILE *fp);
unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf);

void int_to_binary(char dest[], int num, short bits);

void allocate(binary_file *bf);
#endif