#include "assembler.h"

int main(int argc, char* argv[]){
    FILE *fp;
    if(argc == 1){
        /* handle error - no files input */
    }
    while(--argc!=0){
        if((fp = fopen(*++argv, "r")) == NULL){
            /* handle error - couldn't open */
            printf("couldn't open %s\n", *argv);
        }else{
            short result = phase1(fp, *argv);
            if(result == 0)printf("\nFailed to encode file %s in phase 1\n", *argv);
            else if(result == -1)printf("\nFailed to encode file %s in phase 2\n", *argv);
            else printf("\nSucces in encoding file %s\n", *argv);
        }
    }


    return 0;
}