#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "word.h"

const char *R_commands[] = {"add", "sub", "and", "or",
                            "nor", "move", "mvhi", "mvlo"};

const char *I_commands[] = {"addi", "subi", "andi", "ori",
                       "nori", "bne", "beq", "blt", "bgt",
                       "lb", "sb", "lw", "sw", "lh", "sh"};
                  
const char *J_commands[] = {"jmp", "la", "call", "stop"};


void phase1(FILE *fp);

#endif