#include "assembler.h"

int main(int argc, char* argv[]){
    FILE *fp;
    if(argc == 1){
        /* handle error - no files input */
    }
    while(--argc!=0){
        if((fp = fopen(*++argv, "r")) == NULL){
            /* handle error - couldn't open */
            printf("couldn't open file\n");
        }else{
            phase1(fp);
        }
    }


    return 0;
}