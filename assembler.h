#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "error_handle.h"

const char *R_commands[8];
const char *I_commands[15];    
const char *J_commands[4];
const char *instructions_commands[7];


enum state {START, RS, RT, RD, IMMED, ADDRESS, END, STOP, DBHW, STRING, LABEL};
enum commands {R_LOGIC, R_COPY, I_LOGIC, I_BRANCH, I_MEMORY, J_JUMP, J_OTHER, DB, DH, DW, ASCIZ, ENTRY, EXTERN};



short check_R_command(char *line);
short check_I_command(char *line);
short check_J_command(char *line);
short check_Instruction_command(char *line);

short build_binary_file(binary_file *bf, char *file_name);
short build_entery_file(entry_file *entf, char *file_name);
short build_extern_file(external_file *entf, char* file_name);

short find_register(char *line);

unsigned short phase1(FILE *fp, char* file_name);
unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf, char *file_name);

void int_to_binary(char dest[], int num, short bits);

void allocate(binary_file *bf);

void convert_code_to_binary(code *ptr);
void binary_code_to_hex(code *ptr);
void binary_data_to_hex(data *ptr);

symbols_table* get_label(char* label, size_t size, symbols_table* table);

void* allocate_pointer(short option, char* filename);
#endif