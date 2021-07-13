#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "structures.h"

extern const char *R_commands[8];

extern const char *I_commands[15];
                  
extern const char *J_commands[4];

static unsigned short IC = 100;
static unsigned short DC = 0;


unsigned short phase1(FILE *fp);

unsigned short phase2(binary_file *bf);


#endif