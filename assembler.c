#include "assembler.h"


const char *R_commands[] = {"add", "sub", "and", "or",
                            "nor", "move", "mvhi", "mvlo"};

const char *I_commands[] = {"addi", "subi", "andi", "ori",
                            "nori", "bne", "beq", "blt", "bgt",
                            "lb", "sb", "lw", "sw", "lh", "sh"};

const char *J_commands[] = {"jmp", "la", "call", "stop"};

const char *instructions_commands[] = {".db", ".dw", ".dh", ".data",
                                       ".asciz", ".entry", ".extern"};

const char *registers[] = {"$0", "$1", "$2", "$3", "$4","$5", "$6", "$7",
                           "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
                           "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
                           "$24", "$25", "$26", "$27", "$28", "$29", "$30","$31"};
                       
unsigned short phase1(FILE *fp){

    /* we read the file line by line */
    unsigned short error_flag = 0;
    char *line = NULL;
    short line_count = 0;
    size_t len = 0;
    binary_file *bf;
    external_file *extf;
    entry_file *entf;
    

    if((bf = malloc(sizeof(binary_file*))) == NULL || (bf->code_head = malloc(sizeof(code*))) == NULL ||
    (bf->data_head = malloc(sizeof(data*))) == NULL || (bf->labels = malloc(sizeof(symbols_table*))) == NULL ||
    (extf = malloc(sizeof(external_file*))) == NULL || (entf = (entry_file*)malloc(sizeof(entry_file*))) == NULL)
    {
        /* failed to allocate memory */
        /* throw an error */
        
        error_flag = 1;
    }
    else{
        code* code_pointer = bf->code_head;
        data* data_pointer = bf->data_head;
        symbols_table* label_pointer = bf->labels;
        bf->code_size = 0;
        bf->data_size = 0;
        bf->code_head->next = NULL;
        bf->data_head->next = NULL;
        bf->labels->next = NULL;
    
    
    

        while (getline(&line, &len, fp) != -1) { 
            
            enum state state = START;
            enum commands cmnd;
            char* temp = line;
            unsigned flag = 1;
            unsigned label_flag = 0;
            line_count++;
            while(flag){
                if (isspace(*temp))
	    	    {
	    		++temp;
	    		continue;
	    	    }else if(temp[0] == ';') break;
                else if(temp[0] == '\0') flag = 0;
                switch(state){
                    case START:{
                        short command;
                        /*check if this is empty line */
                        if(temp[0] == '\0'){flag = 0; break;}
                        /* check for known command */
                        if((command = check_R_command(temp)) != -1){                  
                            /* check if it is an attempt to make a label */
                            if(temp[strlen(R_commands[command])] == ':'){
                                /* error handle - bad label name */

                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            /* it is R typed command */
                            temp += strlen(R_commands[command]);
                            if(command <= 4) cmnd = R_LOGIC;
                            else cmnd = R_COPY;
                            state = RS;
                            /* if we had error in the past its not necessary */
                            if(!error_flag){
                                code_pointer->T = CODE;
                                code_pointer->T_CODE = R;
                                code_pointer->line = IC;
                                code_pointer->original_line = line_count;
                                /* check if the label flag is activated */
                                if(label_flag){

                                    label_pointer->val = IC;
                                    label_pointer->symbol = CODE;
                                    /* set the pointer to the next label */
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        /* throw an error */
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                                                        
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                                bf->code_size += 4;
                                IC+=4;
                                /* assign binary opcode and funct */
                                switch (cmnd)
                                {
                                    case R_LOGIC:{
                                        /* opcode */ 
                                        int_to_binary(code_pointer->opcode, 0, OPCODE_LENGTH-1);
                                        /* funct */
                                        int_to_binary(code_pointer->funct, command+1, FUNCT_LENGTH-1);
                                        break;
                                    }

                                    case R_COPY:{
                                        /* opcode */ 
                                        int_to_binary(code_pointer->opcode, 1, OPCODE_LENGTH-1);
                                        /* funct */
                                        int_to_binary(code_pointer->funct, command-4, FUNCT_LENGTH-1);
                                        break;
                                    }
                                    default:
                                        break;
                                }
                            }
                        }

                        else if((command = check_I_command(temp)) != -1){
                        /* check if it is an attempt to make a label */
                        if(temp[strlen(I_commands[command])] == ':'){
                            /* error handle - bad label name */
                            
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* it is I typed command */
                        temp += strlen(I_commands[command]);
                        if(command <= 4) cmnd = I_LOGIC;
                        else if(command <= 8) cmnd = I_BRANCH;
                        else cmnd = I_MEMORY;
                        /* assign binary opcode */
                        /* if we had error in the past its not necessary */
                        state = RS;
                        /* if we had error in the past its not necessary */
                        if(!error_flag){
                            code_pointer->T = CODE;
                            code_pointer->T_CODE = I;
                            code_pointer->line = IC;
                            code_pointer->original_line = line_count;
                            if(label_flag){
                                    label_pointer->val = IC;
                                    label_pointer->symbol = CODE;
                                    /* set the pointer to the next label */
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        /* throw an error */
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                            bf->code_size += 4;
                            IC+=4;
                            int_to_binary(code_pointer->opcode, 10+command, OPCODE_LENGTH-1); 
                        }
                    }

                        else if((command = check_J_command(temp)) != -1){
                        /* check if it is an attempt to make a label */
                        if(temp[strlen(J_commands[command])] == ':'){
                            /* error handle - bad label name */
                            
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* it is I typed command */
                        temp += strlen(J_commands[command]);
                        state = ADDRESS;
                        if(command == 0) cmnd = J_JUMP;
                        else if(command == 3) state = STOP;
                        else cmnd = J_OTHER;
                        /* assign binary opcode */
                        
                        /* if we had error in the past its not necessary */
                        if(!error_flag){
                            code_pointer->T = CODE;
                            code_pointer->T_CODE = J;
                            code_pointer->line = IC;
                            code_pointer->original_line = line_count;
                            if(label_flag){
                                    label_pointer->val = IC;
                                    label_pointer->symbol = CODE;
                                    /* set the pointer to the next label */
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        /* throw an error */
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                            bf->code_size += 4;
                            IC+=4;
                            if(state!= STOP) int_to_binary(code_pointer->opcode, 30+command, OPCODE_LENGTH-1); 
                            else int_to_binary(code_pointer->opcode, 63, OPCODE_LENGTH-1);
                        }
                    }
                        
                        else{ 
                            unsigned i = 0;
                            /*if there is no known command it must be a label or mistake */
                            while(temp[i]!=':'){
                                if(temp[i] == '\0' || isspace(temp[i])){
                                    /*handle error - unkown command */ 

                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                i++;
                            }
                            if(i == 0){
                                /* handle error - missing label name */

                            }
                            /* first character should be a letter */
                            if((temp[0] > 'Z' && temp[0] < 'a') || temp[0] < 'a' || temp[0] > 'Z'){
                                /* error handle - first character must be a letter*/

                            }
                            i=1;
                            
                            while(temp[i]!=':'){
                                /* validate the characters of the label - only letters and numbers are allowed */
                                if((temp[i] > 'Z' && temp[i] < 'a') || temp[i] > 'z' || (temp[i] > '9' && temp[i] < 'A') || temp[i] < '0'){
                                    /* error handle - illegal character */
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                i++;
                            }
                            
                            /* try to allocate space for the label */
                            if((label_pointer->symbol = malloc(i+1)) == NULL){
                                /* error handle - failed to allocate space */

                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            
                            memcpy(label_pointer->symbol, temp, i);
                            label_pointer->symbol[i] = '\0';
                            label_flag = 1;
                            temp += i+1;
                            break;
                        }
                    }
                    case RS:{
                        short rs;
                                                            
                        if((rs = find_register(temp)) == -1){
                            /* error handle - bad register name */
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* assing the binary value */
                        if(!error_flag) int_to_binary(code_pointer->rs, rs, RS_RT_RD_LENGTH-1);
                        /* move the line pointer by the register name */
                        temp += strlen(registers[rs]);
                        /* after every rs we need a comma */
                        while(1){
                            if(isspace(*temp)){
                                ++temp;
                                continue;
                            }
                            if(*temp != ','){
                                /* error handle - missing comma */
                                error_flag = 1;
                                flag = 0;

                            }
                            ++temp;
                            break;
                        }
                        /* decide which state should come next */
                        switch (cmnd)
                        {
                        case R_LOGIC:{
                            state = RT;
                            break;
                        }
                        case R_COPY:{
                            state = RD;
                            break;
                        }
                        case I_LOGIC:{
                            state = IMMED;
                            break;
                        }
                        case I_BRANCH:{
                            state = RT;
                            break;
                        }
                        case I_MEMORY:{

                            state = IMMED;
                            break;
                        }
                        default:
                            break;
                        }
                        break;
                    }
                    case RT:{
                    short rt;
                    if((rt = find_register(temp)) == -1){
                        
                        /* error handle - bad register name or double comma */
                        /*check if there is a double comma */
                        if(*temp == ','){
                            /* error handle - double comma */
        
                        }
                        else{
                            /* error handle - bad register name */
                            

                        }
                        error_flag = 1;
                        flag = 0;
                        break;
                    }
                    
                    /* assing the binary value */
                    if(!error_flag) int_to_binary(code_pointer->rt, rt, RS_RT_RD_LENGTH-1);
                    /* move the line pointer by the register name */
                    temp += strlen(registers[rt]);
                    /* decide which state should come next */
                    switch (cmnd)
                    {
                    case R_LOGIC:{
                        state = RD;
                        /* we need a comma between RT and RD */
                        while(1){
                            if(isspace(*temp)){
                                ++temp;
                                continue;
                            }
                            if(*temp != ','){
                                /* error handle - missing comma */
                                error_flag = 1;
                                flag = 0;

                            }
                            ++temp;
                            break;
                        }
                        break;
                    }
                    case I_LOGIC:{
                        state = END;
                        break;
                    }
                    case I_BRANCH:{
                        state = IMMED;
                        /* we need a comma between RT and IMMED */
                        while(1){
                            if(isspace(*temp)){
                                ++temp;
                                continue;
                            }
                            if(*temp != ','){
                                /* error handle - missing comma */
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            ++temp;
                            break;
                        }
                        break;
                    }
                    case I_MEMORY:{
                        state = END;
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                }
                    case RD:{
                    short rd;
                    if((rd = find_register(temp)) == -1){
                        /* error handle - bad register name or double comma */
                        /*check if there is a double comma */
                        if(*temp == ','){
                            /* error handle - double comma */
        
                        }
                        else{
                            /* error handle - bad register name */
                            

                        }
                        error_flag = 1;
                        flag = 0;
                        break;
                    }
                    /* assing the binary value */
                    if(!error_flag) int_to_binary(code_pointer->rd, rd, RS_RT_RD_LENGTH-1);
                    
                    /* move the line pointer by the register name */
                    temp += strlen(registers[rd]);
                    state = END;
                    break;
                }
                    case IMMED:{
                    if(*temp == ','){
                        /* error handle - double comma */

                        error_flag = 1;
                        flag = 0;
                        break;
                    }
                    
                    switch(cmnd){
                        case I_BRANCH:{
                            /* the label will be attached in the second phase */
                            code_pointer->immed[0] = '?';
                            while(*temp != '\0' && !isspace(*temp)){
                                /*skip the current label */
                                ++temp;
                            }
                            state = END;
                            break;
                        }
                        default:{
                            char *t;
                            int immed = strtol(temp, &t, 10);
                            if(immed > MAX_INT || immed < MIN_INT){
                                /* error handle - out of range */
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            
                            if(!error_flag) int_to_binary(code_pointer->immed, immed, IMMED_LENGTH-1);
                            /* set the line pointer to be after the immed number */
                            temp = t;
                            if(*temp != ','){
                                /* error handle - missing comma */
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            ++temp;

                            state = RT;
                            break;
                        }
                    }
                    


                    break;
                }
                    case ADDRESS:{

                    switch (cmnd)
                    {
                      case J_JUMP:{
                        /* check if the given address is for a label or for a register */
                        short reg;
                        if(*temp == '$'){
                            /* should be register */
                            if((reg = find_register(temp)) == -1){
                                /* error handle - bad register name */
                                error_flag = 1;
                                flag = 0;

                                break;
                            }else{
                                code_pointer->reg = '1';
                                int_to_binary(code_pointer->address, reg, ADDRESS_LENGTH-1);
                                /* set the line pointer to be after the given reg */
                                temp += strlen(registers[reg]);
                            }
                        }else{
                            /* should be a label */
                            /* we will deal with it in phase 2 */
                            code_pointer->reg = '0';
                            code_pointer->address[0] = '?';

                            /* for now we will skip the label */
                            while(*temp != '\0' && !isspace(*temp)){
                                /*skip the current label */
                                ++temp;
                            }
                        }
                        state = END;
                        break;
                      }
                      default:{
                            /* should be a label */
                            /* we will deal with it in phase 2 */
                            code_pointer->reg = '0';
                            code_pointer->address[0] = '?';
                            break;
                      }
                    }
                    break;
                }
                    case END:{
                        if(*temp != '\0'){
                            /* error handle - junk after end of command */
                            error_flag = 1;
                            flag = 0;
                            break;
                        }else{
                            /* set the pointer to the next code line */
                            code* next = malloc(sizeof(code));
                            if(next == NULL){
                                /* failed to allocate memory */
                                /* throw an error */
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            next->next = NULL;
                            code_pointer->next = next;
                            code_pointer = code_pointer->next;

                            flag = 0;
                            break;
                        }               
                    }
                    case STOP:{
                    code_pointer->reg = '0';
                    int_to_binary(code_pointer->address, 0, ADDRESS_LENGTH-1);
                    state = END;
                    
                }
                    default: break;
                }

            }
            if(state!= END){
                error_flag = 0;
                /*handle error - missing arguments */
                switch (state)
                {
                    case RS: case RT: case RD:{
                        /* handle error - register is missing */ 

                        break;
                    }

                    case ADDRESS:{
                        /* handle error - address is missing */ 

                        break; 
                    }

                    case IMMED:{
                        /* handle error - immed is missing */ 

                        break; 
                    }
                    default: break;

                }
            }
        }
                                            printf("test\n");
        label_pointer = bf->labels;
        while(label_pointer!= NULL){
            printf("label: %s | line: %d | type: %d", label_pointer->symbol, label_pointer->val, label_pointer->T);
            label_pointer = label_pointer->next;
        }
    }
    return error_flag;
}


void int_to_binary(char dest[], int num, short bits){
    unsigned i;
    for(i=0; i<bits; i++)
    {
        unsigned int mask = 1u << (bits - 1 - i);
        dest[i] = (num & mask) ? '1' : '0';
    }
    dest[bits] = '\0';
}

short find_register(char* line){
    unsigned i;
    for(i=REGISTERS-1; i>=0; i--){
        if(!memcmp(line, registers[i], strlen(registers[i]))) return i;
    }
    return -1;
}


short check_R_command(char *line){
    unsigned i;
    for(i=0; i< R_COMMANDS; i++){
        if(!memcmp(line, R_commands[i], strlen(R_commands[i]))) return i;
    }
    return -1;
}

short check_I_command(char *line){
    unsigned i;
    for(i=0; i< I_COMMANDS; i++){
        if(!memcmp(line, I_commands[i], strlen(I_commands[i]))) return i;
    }
    return -1;
}

short check_J_command(char *line){
    unsigned i;
    for(i=0; i< J_COMMANDS; i++){
        if(!memcmp(line, J_commands[i], strlen(J_commands[i]))) return i;
    }
    return -1;
}

unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf){
    /* first go throuh the code typed commands*/
    unsigned short error_flag = 0;
    code* code_temp = bf->code_head;
    while(code_temp!= NULL){
        switch(code_temp->T_CODE){
            case R:{
               break;
            }
            /* I typed commands */
            case I:{
               if(!strncmp(code_temp->immed, "?", 1)){
                   /* check if the label exist in the symbols table */
                   symbols_table* table_temp = bf->labels;
                   unsigned flag = 0;
                   while(table_temp!= NULL){
                      /* check if this is the label */
                      if(!strcmp(table_temp->symbol, code_temp->label)){
                          if(table_temp->T == EXTERN){
                              /* call an error */ 
                              error_flag = 1;
                              break;
                          }else{
                              /* calculate and convert the distance between the command and the label */ 
                              int distance;
                              flag = 1;
                              if(error_flag) break;
                              distance = table_temp->val - code_temp->line;
                              int_to_binary(code_temp->immed, distance, IMMED_LENGTH-1);

                              if(table_temp->T == ENTRY){
                               /* add it to the entry file */
                               if(entf == NULL){
                                if((entf = malloc(sizeof(external_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    entf->line = code_temp->line;
                                    entf->label = table_temp;
                                }
                               }else{
                                entry_file* file_temp = entf;
                                while(file_temp->next != NULL){file_temp = file_temp->next;}
                                /* try to allocate memory for a new line */
                                if((file_temp->next = malloc(sizeof(entry_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    file_temp->next->line = code_temp->line;
                                    file_temp->next->label = table_temp;
                                 }
                                }
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
               if(code_temp->reg == '0'){
                   symbols_table* table_temp = bf->labels;
                   unsigned flag = 0;
                   while(table_temp!= NULL){
                      /* check if this is the label */
                      if(!strcmp(table_temp->symbol, code_temp->label)){
                           /* convert the label line that required in the command to binary */ 
                           flag = 1;
                           /* if we had an error in the past we don't have to bother converting to binary */
                           if(error_flag) break;
                           int_to_binary(code_temp->address, table_temp->val, ADDRESS_LENGTH-1);
                           if(table_temp->T == EXTERN){
                               /* add it to the extern file */
                               if(extf == NULL){
                                if((extf = malloc(sizeof(external_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    extf->line = code_temp->line;
                                    extf->label = table_temp;
                                }
                               }else{
                                external_file* file_temp = extf;
                                while(file_temp->next != NULL){file_temp = file_temp->next;}
                                /* try to allocate memory for a new line */
                                if((file_temp->next = malloc(sizeof(external_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    file_temp->next->line = code_temp->line;
                                    file_temp->next->label = table_temp;
                                }
                               }
                           }else if(table_temp->T == ENTRY){
                               /* add it to the entry file */
                               if(entf == NULL){
                                if((entf = malloc(sizeof(external_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    entf->line = code_temp->line;
                                    entf->label = table_temp;
                                }
                               }else{
                                entry_file* file_temp = entf;
                                while(file_temp->next != NULL){file_temp = file_temp->next;}
                                /* try to allocate memory for a new line */
                                if((file_temp->next = malloc(sizeof(entry_file*))) == NULL){
                                    /* failed to allocate memory */
                                    /* throw an error */
                                    error_flag = 1;
                                }else{
                                    file_temp->next->line = code_temp->line;
                                    file_temp->next->label = table_temp;
                                }
                               }
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
        code_temp = code_temp->next;
    }

    return error_flag;
}