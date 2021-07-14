#include "structures.h"


void convert_code_to_binary(code *w){
    switch(w->T_CODE){
        case R:{
            /* R typed command structure */
            /* |opcode|rs|rt|rd|funct|6 unused bits| */
            sprintf(w->word, "%s%s%s%s%s000000", w->opcode, w->rs, w->rt, w->rd, w->funct);
            w->word[WORD_LENGTH] = '\0';
            break;
        }
        case I:{
            /* I typed command structure */
            /* |opcode|rs|rt|immed| */
            sprintf(w->word, "%s%s%s%s", w->opcode, w->rs, w->rt, w->immed);
            w->word[WORD_LENGTH] = '\0';
            break;
        }
        case J:{
            /* J typed command structure */
            /* |opcode|reg|address| */
            sprintf(w->word, "%s%c%s", w->opcode, w->reg,w->address);
            w->word[WORD_LENGTH] = '\0';
            break;
        }
        default:
            break;
    }
}

void binary_to_hex(code *w){
    /* convert the given word in binary to hex */ 
    int i, j, count = 0;
    for(i = 0; i<BYTE_LENGTH*4; i+= BYTE_LENGTH){
        unsigned char hex;
        for(j = 0; j<BYTE_LENGTH; j++){
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