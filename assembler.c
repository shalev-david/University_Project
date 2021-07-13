#include "assembler.h"


const char *R_commands[] = {"add", "sub", "and", "or",
                            "nor", "move", "mvhi", "mvlo"};

const char *I_commands[] = {"addi", "subi", "andi", "ori",
                       "nori", "bne", "beq", "blt", "bgt",
                       "lb", "sb", "lw", "sw", "lh", "sh"};

const char *J_commands[] = {"jmp", "la", "call", "stop"};
                       

unsigned short phase2(binary_file *bf){
    /* first go throuh the code typed commands*/
    unsigned short error_flag = 0;
    code* temp = bf->code_head;
    while(temp!= NULL){
        switch(temp->T_CODE){
            case R:{
               temp = temp->next;
               break;
            }
            /* I typed commands */
            case I:{
               if(!strcmp(temp->immed, "?")){
                   /* check if the label exist in the symbols table */
                   symbols_table* table_temp = bf->labels;
                   unsigned flag = 0;
                   while(table_temp!= NULL){
                      /* check if this is the label */
                      if(!strcmp(table_temp->symbol, temp->label)){
                          if(table_temp->T == EXTERN){
                          
                          }else{
                              /* calculate and convert the distance between the command and the label */ 
                              int distance;
                              unsigned int i;
                              flag = 1;
                              if(error_flag) break;
                              distance = table_temp->val - temp->line;
                              for(i=0; i<IMMED_LENGTH; i++)
                              {
                                  unsigned int mask = 1u << (IMMED_LENGTH - 1 - i);
                                  temp->immed[i] = (distance & mask) ? '1' : '0';
                              } 
                              break;
                          }
                      }
                      /* else continue to the next one */
                      table_temp = table_temp->next;
                   }
                   /* validate that we found the label */
                   if(!flag){
                       /* if we didnt found the label throw an error */ 
                      error_flag = 1;
                   }
               }
               break;
           }
           /* J typed commands*/
           case J:{
               if(temp->reg == '0' && !strcmp(temp->address, "?")){
                   symbols_table* table_temp = bf->labels;
                   unsigned flag = 0;
                   while(table_temp!= NULL){
                      /* check if this is the label */
                      if(!strcmp(table_temp->symbol, temp->label)){
                           unsigned int i;
                           /* convert the label line that required in the command to binary */ 
                           flag = 1;
                           /* if we had an error in the past we don't have to bother converting to binary */
                           if(error_flag) break;
                           for(i=0; i<ADDRESS_LENGTH; i++)
                           {
                               unsigned int mask = 1u << (ADDRESS_LENGTH - 1 - i);
                               temp->address[i] = (table_temp->val & mask) ? '1' : '0';
                           }
                           break;
                      }
                      /* else continue to the next one */
                      table_temp = table_temp->next;
                   }
                   /* validate that we found the label */
                   if(!flag){
                       /* if we didnt found the label throw an error */ 
                      error_flag = 1;
                   }
               }
               break;
           }
        }

    }
    return error_flag;
}