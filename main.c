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
            short result;
            short size = strlen(*argv)-3;
            char* file_name;
            /* check if the file is .as file  */
            if(strncmp((*argv + size), ".as", 3)){printf("File %s isn't a .as file\n", *argv); continue;}
            /* if he is continue and save the file name without the .as ending */
            file_name = malloc(size+1);
            memcpy(file_name, *argv, size);
            file_name[size] = '\0';
            /* start encoding the given file */
            result = phase1(fp, file_name);
            if(result == 0)printf("\nFailed to encode file %s in phase 1\n", *argv);
            else if(result == -1)printf("\nFailed to encode file %s in phase 2\n", *argv);
            else printf("\nSuccses in encoding file %s\n", *argv);
        }
    }

    return 0;
}