#include "word.h"

int main(){
    word *w = malloc(sizeof(word));
    w->T = I_WORD;
    memcpy(w->funct, "00001", FUNCT_LENGTH);
    memcpy(w->rd, "01001", RS_RT_RD_LENGTH);
    memcpy(w->rt, "00010", RS_RT_RD_LENGTH);
    memcpy(w->rs, "01001", RS_RT_RD_LENGTH);
    memcpy(w->opcode, "001101", OPCODE_LENGTH);
    w->reg = '0';
    memcpy(w->immed, "1111111111111011", IMMED_LENGTH);
    memcpy(w->address, "0000000000000000010100001", ADDRESS_LENGTH);

    convert_to_binary(w);
    binary_to_hex(w);
    int i = 0;
    for(; i<4; i++){
        printf("%s ", w->hex[i]);
    }
    return 0;
}