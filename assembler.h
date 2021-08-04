#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "error_handle.h"

const char *R_commands[8];
const char *I_commands[15];    
const char *J_commands[4];
const char *instructions_commands[7];
const char *registers[32];

/* states to know our place in the command line */
enum state {START, RS, RT, RD, IMMED, ADDRESS, END, STOP, DBHW, STRING, LABEL};
/* the specific command we deal in this line */
enum commands {R_LOGIC, R_COPY, I_LOGIC, I_BRANCH, I_MEMORY, J_JUMP, J_OTHER, DB, DH, DW, ASCIZ, ENTRY, EXTERN};


/* Check if the given text is in one of the constant commands or registers                   */
/* The commands and registers are stored in the arrays above ^                               */
/* R_commands[8] | I_commands[15] | J_commands[4] | instructions_commands[7] | registers[32] */
/* ---- Input ---- */
/* line - pointer to a string                                     */
/* ---- Return Value ----                                         */
/* In case of success return the place of the elemnt in the array */
/* In case of fail return -1                                      */
short check_R_command(char *line);
short check_I_command(char *line);
short check_J_command(char *line);
short check_Instruction_command(char *line);
short find_register(char *line);


/* pahse 1 and 2 of encoding the given file                                   */
/* In case of errors in phase 1 the program won't continue to phase 2         */
/* In case of errors in phase 2 the program won't continue to build the files */
/* ---- Input ---- */
/* fp - file pointer for reading the file                                                                                   */
/* file_name - the name of the file so we can identify errors in each file and to create a new files with the correct names */
/* bf, extf, entf - the pointer of structures - we create with them the files                                               */
unsigned short phase1(FILE *fp, char* file_name);
unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf, char *file_name);


/* Create a file in case of need */
/* ---- Input ---- */
/* bf, extf, entf - the pointer of structures store the values of everything needed to create the files                     */
/* file_name - the name of the file so we can identify errors in each file and to create a new files with the correct names */
short build_binary_file(binary_file *bf, char *file_name);
short build_entery_file(entry_file *entf, char *file_name);
short build_extern_file(external_file *entf, char* file_name);

/* Convert an int to a binary string */
/* ---- Input ---- */
/* dest - we store the result in this char array    */
/* num - the given number                           */
/* bits - the number of bits to convert             */
void int_to_binary(char dest[], long num, short bits);

/* Merge every needed arttibute of |code| to a binary string and stores it in code->word[33] */
/* ---- Input ---- */
/* ptr - the pointer of the structure store every needed value to create the binary string */
void convert_code_to_binary(code *ptr);

/* Convert every byte of code->word[33] to a hex string and stores it in hex[4] */
/* ---- Input ---- */
/* ptr - the code pointer stores the word to convert */
void binary_code_to_hex(code *ptr);

/* Convert every data value byte to hex string */
/* ---- Input ---- */
/* ptr - the data pointer contain a data_value pointer which contain a binary string in one of these sizes - 1 byte, 2 bytes or 4 bytes */
void binary_data_to_hex(data *ptr);

/* Find a specific label from the symbol table*/
/* ---- Input ---- */
/* label - the label's name               */
/* size - the size of the label's name    */
/* table - a pointer to the symbol table  */
/* ---- Return Value ---- */ 
/* In case of success - the requested label */
/* In case of failute - NULL                */
symbols_table* get_label(char* label, size_t size, symbols_table* table);

/* Try to allocate space for a specific structure */
/* ---- Input ---- */
/* option - the option tell the function which structure we want to allocate space for  */
/* filename - the file name in case of an error                                         */
/* ---- Return Value ---- */
/* In case of success return a pointer for the allocated space  */
/* In case of failure throw an error and exit the program       */ 
void* allocate_pointer(short option, char* filename);

/* Check if a given numner is out of the range of Int32 */
/* ---- Input ---- */
/* ptr - pointer to the start of the 10 characters number */
/* ---- Return Value ---- */
/* 0 - if the given number is in range      */
/* 1 - if the given number is out of range  */
short out_of_range32(char* ptr);

#endif