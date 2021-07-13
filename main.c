#include "assembler.h"

int main(int argc, char* argv[]){


    FILE *fp;
    if(argc == 1){
        /* handle error - no files input */
    }
    while(--argc!=1){
        if((fp = fopen(*++argv, "r")) == NULL){
            /* handle error - couldn't open */
        }else{
            /*phase1(fp);*/
        }
    }


    return 0;
}