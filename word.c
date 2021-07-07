#include "word.h"


void convert_to_binary(word *w){
    switch(w->T){
        /* all given binaries should be reversed because of the project conditions */
        /* for example opcode should be in indexes 31-25 | the given binary number start from index 25 and not from index 31 */
        /* so we reverse the given binary so that it will start from index 31 */
        int i, place;
        case R_WORD:{
            /* first 6 bits are 0 in R typed words*/
            for(i=0, place = 0; i< NOT_IN_USE_LENGTH; i++, place++){
                w->word[place] = '0';
            }
            /* next 5 bits are the funct */
            for(i=0; i < FUNCT_LENGTH; i++, place++){
                w->word[place] = w->funct[FUNCT_LENGTH-1-i];
            }
            /* next 5 bits are rd */
            for(i=0; i < RS_RT_RD_LENGTH; i++, place++){
                w->word[place] = w->rd[RS_RT_RD_LENGTH-1-i];
            }
            /* next 5 bits are rt */
            for(i=0; i < RS_RT_RD_LENGTH; i++, place++){
                w->word[place] = w->rt[RS_RT_RD_LENGTH-1-i];
            }

            /* next 5 bits are rs */
            for(i=0; i < RS_RT_RD_LENGTH; i++, place++){
                w->word[place] = w->rs[RS_RT_RD_LENGTH-1-i];
            }

            /* last 6 bits are for the opcode */
            for(i=0; i < OPCODE_LENGTH; i++, place++){
                w->word[place] = w->opcode[OPCODE_LENGTH-1-i];
            }
            /* add endline in the end */
            w->word[place] = '\0';
            break;
        }
        case I_WORD:{
            /* first 16 bits are for the immediate */
            for(i=0, place = 0; i < IMMED_LENGTH; i++, place++){
                w->word[place] = w->immed[IMMED_LENGTH-1-i];
            }

            /* next 5 bits are for rt */
            for(i=0; i < RS_RT_RD_LENGTH; i++, place++){
                w->word[place] = w->rt[RS_RT_RD_LENGTH-1-i];
            }
            
            /* next 5 bits are for rs */
            for(i=0; i < RS_RT_RD_LENGTH; i++, place++){
                w->word[place] = w->rs[RS_RT_RD_LENGTH-1-i];
            }

            /* last 6 bits are for the opcode */
            for(i=0; i < OPCODE_LENGTH; i++, place++){
                w->word[place] = w->opcode[OPCODE_LENGTH-1-i];
            }
            /* add endline in the end */
            w->word[place] = '\0';
            break;
        }
        case J_WORD:{
            /* first 25 bits are for the address */
            for(i=0, place = 0; i< ADDRESS_LENGTH; i++, place++){
                w->word[place] = w->address[ADDRESS_LENGTH-1-i];
            }

            /* next bit is for the reg flag */
            w->word[place] = w->reg;
            place++;

            /* last 6 bits are for the opcode */
            for(i=0; i < OPCODE_LENGTH; i++, place++){
                w->word[place] = w->opcode[OPCODE_LENGTH-1-i];
            }
            /* add endline in the end */
            w->word[place] = '\0';
            break;
        }
    }
}

void binary_to_hex(word *w){
    /* convert the given word in binary to hex */ 
    int i, j, count = 0;
    for(i = 0; i<BYTE_LENGTH*4; i+= BYTE_LENGTH){
        unsigned char hex;
        /* the binary is reversed so we work from the end to the start */
        for(j = BYTE_LENGTH-1; j>=0; j--){
            unsigned char t = w->word[i+j] =='1'?1:0;
            hex = (hex<<1)|t; 

        }
        /* copy the string in hex format */
        sprintf(w->hex[count], "%02X", hex);
        /* add endline in the end */
        w->hex[count][3] = '\0';
        count++;
    }
}