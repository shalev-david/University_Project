#include "error_handle.h"

unsigned short double_comma(char c, unsigned short *error_flag, unsigned short *flag){
    if(c == ','){
        /* error handle - double comma */

        *error_flag = 1;
        *flag = 0;
        return 1;
    }
    return 0;
}