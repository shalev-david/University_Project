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
                       
unsigned short phase1(FILE *fp, char* file_name){

    /* we read the file line by line */
    unsigned short error_flag = 0;
    char *line = NULL;
    short line_count = 0;
    
    ssize_t line_size;
    size_t len = 0;
    binary_file *bf;
    external_file *extf;
    entry_file *entf;
    

    if((bf = malloc(sizeof(binary_file*))) == NULL || (bf->code_head = malloc(sizeof(code*))) == NULL ||
    (bf->data_head = malloc(sizeof(data*))) == NULL || (bf->labels = malloc(sizeof(symbols_table*))) == NULL ||
    (extf = malloc(sizeof(external_file*))) == NULL || (entf = (entry_file*)malloc(sizeof(entry_file*))) == NULL)
    {
        /* failed to allocate memory */
        error_handle(-1, "", 0, 0, file_name);
        error_flag = 1;
    }
    else{
        code* code_pointer = bf->code_head;
        data* data_pointer = bf->data_head;
        symbols_table* label_pointer = bf->labels;
        bf->IC = 100;
        bf->DC = 0;
        bf->code_size = 0;
        bf->data_size = 0;
        bf->code_head->next = NULL;
        bf->data_head->next = NULL;
        bf->labels->next = NULL;
    

        while ((line_size = getline(&line, &len, fp)) != -1) { 
            enum state state = START;
            enum commands cmnd;
            char* temp = line;
            unsigned flag = 1;
            unsigned label_flag = 0;
            short c = 0;
            line_count++;
            while(flag){
                if (isspace(*temp))
	    	    {
	    		    ++temp;
                    c++;
	    		    continue;
	    	    }
                else if(*temp == ';') {flag = 0; state = END; break;}
                else if(*temp == '\0' && state != END) {break;}
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
                                error_handle(10, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                state = END;
                                break;
                            }
                            /* it is R typed command */
                            temp += strlen(R_commands[command]);
                            c+=strlen(R_commands[command]);
                            if(command <= 4) cmnd = R_LOGIC;
                            else cmnd = R_COPY;
                            state = RS;
                            /* if we had error in the past its not necessary */
                            if(!error_flag){
                                code_pointer->T = CODE;
                                code_pointer->T_CODE = R;
                                code_pointer->line =bf->IC;
                                code_pointer->original_line = line_count;
                                /* check if the label flag is activated */
                                if(label_flag){
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    label_pointer->val =bf->IC;
                                    label_pointer->T = CODE;
                                    /* set the pointer to the next label */
                                    
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        error_handle(-1, "", 0, 0, file_name);
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                                                        
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                                bf->code_size += 4;
                                bf->IC+=4;
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
                                break;
                            }
                            
                        }

                        else if((command = check_I_command(temp)) != -1){
                        /* check if it is an attempt to make a label */
                        if(temp[strlen(I_commands[command])] == ':'){
                            /* error handle - bad label name */
                            error_handle(10, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* it is I typed command */
                        temp += strlen(I_commands[command]);
                        c += strlen(I_commands[command]);
                        if(command <= 4) cmnd = I_LOGIC;
                        else if(command <= 8) cmnd = I_BRANCH;
                        else cmnd = I_MEMORY;
                        state = RS;
                        /* assign binary opcode */
                        /* if we had error in the past its not necessary */
                        if(!error_flag){
                            code_pointer->T = CODE;
                            code_pointer->T_CODE = I;
                            code_pointer->line = bf->IC;
                            code_pointer->original_line = line_count;
                            if(label_flag){
                                    /* set the pointer to the next label */
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    label_pointer->val =bf->IC;
                                    label_pointer->T = CODE;
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        error_handle(-1, "", 0, 0, file_name);
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                            bf->code_size += 4;
                            bf->IC+=4;
                            int_to_binary(code_pointer->opcode, 10+command, OPCODE_LENGTH-1); 
                        }
                    }

                        else if((command = check_J_command(temp)) != -1){
                        /* check if it is an attempt to make a label */
                        if(temp[strlen(J_commands[command])] == ':'){
                            /* error handle - bad label name */
                            error_handle(10, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* it is I typed command */
                        temp += strlen(J_commands[command]);
                        c += strlen(J_commands[command]);
                        state = ADDRESS;
                        if(command == 0) cmnd = J_JUMP;
                        else if(command == 3) state = STOP;
                        else cmnd = J_OTHER;

                        /* assign binary opcode */
                        /* if we had error in the past its not necessary */
                        if(!error_flag){
                            code_pointer->T = CODE;
                            code_pointer->T_CODE = J;
                            code_pointer->line =bf->IC;
                            code_pointer->original_line = line_count;
                            if(label_flag){
                                    /* set the pointer to the next label */
                                    symbols_table* next = malloc(sizeof(symbols_table));
                                    label_pointer->val = bf->IC;
                                    label_pointer->T = CODE;
                                    if(next == NULL){
                                        /* failed to allocate memory */
                                        error_handle(-1, "", 0, 0, file_name);
                                        error_flag = 1;
                                        flag = 0;
                                        break;
                                    }
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                }
                            bf->code_size += 4;
                            bf->IC+=4;
                            if(state!= STOP) int_to_binary(code_pointer->opcode, 30+command, OPCODE_LENGTH-1); 
                            else {
                                int_to_binary(code_pointer->opcode, 63, OPCODE_LENGTH-1);
                                code_pointer->reg = '0';
                                int_to_binary(code_pointer->address, 0, ADDRESS_LENGTH-1);
                                state = END;
                            }
                            
                        }
                    }
                        
                        else{
                            unsigned i = 0, v;
                            /*if there is no known command it must be a label or mistake */
                            while(temp[i]!=':'){
                                if(temp[i] == '\0' || isspace(temp[i])){
                                    /*handle error - unkown command */ 
                                    error_handle(0, line, line_count, c, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                i++;
                            }
                            if(i == 0){
                                /* handle error - missing label name */
                                error_handle(14, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if((v = valid_label(temp, i, bf->labels))){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else if(v==2){
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                }else{
                                    /* error handle - label already exist */
                                    error_handle(13, line, line_count, c, file_name);
                                }
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            /* if we had an error in the past its not necessary */
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((label_pointer->symbol = malloc(i+1)) == NULL){
                                    /* error handle - failed to allocate space */
                                    error_handle(-1, "", 0, 0, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }

                                memcpy(label_pointer->symbol, temp, i);
                                label_pointer->symbol[i] = '\0';
                            }
                            label_flag = 1;
                            temp += i+1;
                            c+= i+1;
                            break;
                        }
                        break;
                    }
                    case RS:{
                        short rs;
                        if((rs = find_register(temp)) == -1){
                            /* error handle - bad register name */
                            error_handle(1, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        
                        /* assing the binary value */
                        if(!error_flag) int_to_binary(code_pointer->rs, rs, RS_RT_RD_LENGTH-1);
                        /* move the line pointer by the register name */
                        temp += strlen(registers[rs]);
                        c+=strlen(registers[rs]);
                        /* after every rs we need a comma */
                        while(1){
                            if(isspace(*temp)){
                                ++temp;
                                continue;
                            }
                            if(*temp != ','){
                                /* error handle - missing comma */
                                error_handle(2, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                            }
                            ++temp;
                            break;
                        }
                        if(!flag){state = END; break;}

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
                        if(*temp == ',')
                            /* error handle - double comma */
                            error_handle(3, line, line_count, c, file_name);
                        else
                            /* error handle - bad register name */
                            error_handle(1, line, line_count, c, file_name);      
                        error_flag = 1;
                        flag = 0;
                        break;
                    }
                    
                    /* assing the binary value */
                    if(!error_flag) int_to_binary(code_pointer->rt, rt, RS_RT_RD_LENGTH-1);
                    /* move the line pointer by the register name */
                    temp += strlen(registers[rt]);
                    c += strlen(registers[rt]);
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
                                error_handle(1, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
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
                                error_handle(2, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                            }
                            ++temp;
                            break;
                        }
                        if(!flag){ state = END; break;}
                        break;  
                    }
                    case I_MEMORY:{
                        state = END;
                        break;
                    }
                    default: break;
                    }
                    break;
                }
                    case RD:{
                    short rd;
                    if((rd = find_register(temp)) == -1){

                        /* error handle - bad register name or double comma */
                        /*check if there is a double comma */
                        if(*temp == ',')
                            /* error handle - double comma */
                            error_handle(3, line, line_count, c, file_name);
                        else
                            /* error handle - bad register name */
                            error_handle(1, line, line_count, c, file_name);
                        error_flag = 1;
                        flag = 0;
                        break;
                    }
                    /* assing the binary value */
                    if(!error_flag) int_to_binary(code_pointer->rd, rd, RS_RT_RD_LENGTH-1);
                    if(cmnd == R_COPY) int_to_binary(code_pointer->rt, 0, RS_RT_RD_LENGTH-1);
                    /* move the line pointer by the register name */
                    temp += strlen(registers[rd]);
                    c += strlen(registers[rd]);
                    state = END;
                    break;
                }
                    case IMMED:{
                    if(*temp == ','){
                        /* error handle - double comma */
                        error_handle(3, line, line_count, c, file_name);
                        error_flag = 1;
                        flag = 0;
                        state = END;
                        break;
                    }
                    switch(cmnd){
                        case I_BRANCH:{
                            /* the label will be attached in the second phase */
                            unsigned short i = 0, v;
                            code_pointer->immed[0] = '?';
                            /* we will keep the label */
                            while(temp[i] != '\0' && !isspace(temp[i])){
                               i++;
                            }
                            /* first validate the label */
                            v = valid_label(temp, i, bf->labels);
                            if(v && v != 3){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else{
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                }
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                memcpy(code_pointer->label, temp, i);
                                code_pointer->label[i] = '\0';
                                temp += i+1;
                                c += i+1;
                            }
                            state = END;
                            break;
                        }
                        default:{
                            char *t;
                            int immed = strtol(temp, &t, 10);
                            /* first check if it a number */
                            if((*temp > '9' || *temp < '0') && *temp != '-' && *temp!='+'){
                                error_handle(33, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                state = END;
                                break;
                            }
                            if(immed > MAX_INT || immed < MIN_INT){
                                /* error handle - out of range */
                                error_handle(20, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            
                            if(!error_flag) int_to_binary(code_pointer->immed, immed, IMMED_LENGTH-1);
                            /* set the line pointer to be after the immed number */
                            c += t-temp;
                            temp = t;
                            while(1){
                                if(isspace(*temp)){
                                    ++temp;
                                    c++;
                                    continue;
                                }
                                if(*temp != ','){
                                    /* error handle - missing comma */
                                    error_handle(2, line, line_count, c, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                }
                                ++temp;
                                c++;
                                break;
                            }
                            state = RT;
                            if(!flag){state = END; break;}
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
                                error_handle(1, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;

                                break;
                            }else{
                                code_pointer->reg = '1';
                                int_to_binary(code_pointer->address, reg, ADDRESS_LENGTH-1);
                                /* set the line pointer to be after the given reg */
                                temp += strlen(registers[reg]);
                                c += strlen(registers[reg]);
                            }
                        }else{
                            /* should be a label */
                            /* we will deal with it in phase 2 */
                            unsigned short i = 0, v;
                            code_pointer->reg = '0';
                            code_pointer->address[0] = '?';
                            
                            /* we will keep the label */
                            while(temp[i] != '\0' && !isspace(temp[i])){
                                i++;
                            }
                            /* first validate the label */
                            v = valid_label(temp, i, bf->labels);
                            if(v && v!=3){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else{
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                }
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                memcpy(code_pointer->label, temp, i);
                                code_pointer->label[i] = '\0';
                                temp += i+1;
                                c += i+1;
                            }
                        }
                        state = END;
                        break;
                      }
                      default:{
                            /* should be a label */
                            /* we will deal with it in phase 2 */
                            unsigned short i = 0, v;
                            code_pointer->reg = '0';
                            code_pointer->address[0] = '?';
                            /* we will keep the label */
                            while(temp[i] != '\0' && !isspace(temp[i])){
                               i++;
                            }
                            /* first validate the label */
                            v = valid_label(temp, i, bf->labels);
                            if(v && v != 3){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);

                                }else{
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);

                                }
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                memcpy(code_pointer->label, temp, i);
                                code_pointer->label[i] = '\0';
                                temp += i+1;
                                c += i+1;
                            }
                            state = END;
                            break;
                      }
                    }
                    break;
                }
                    case END:{
                        if(*temp != '\0'){
                            /* error handle - junk after end of command */
                            error_handle(21, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }else{
                            /* set the pointer to the next code line */
                            code* next = malloc(sizeof(code));
                            if(next == NULL){
                                /* failed to allocate memory */
                                error_handle(-1, "", 0, 0, file_name);
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
                        break;              
                    }
                    default: break;

                }
            }
            if(state!= END){
                /*handle error - missing arguments */
                switch (state)
                {
                    case RS: case RT: case RD:{
                        /* handle error - register is missing */ 
                        error_flag = 1;
                        error_handle(30, line, line_count, c, file_name);
                        break;
                    }

                    case ADDRESS:{
                        /* handle error - address is missing */
                        error_flag = 1;
                        error_handle(31, line, line_count, c, file_name);
                        break; 
                    }

                    case IMMED:{
                        /* handle error - immed is missing */
                        error_flag = 1; 
                        error_handle(32, line, line_count, c, file_name);
                        break; 
                    }
                    case START:{
                        error_flag=0;
                        break;
                    }
                    default: break;

                }
            }  
        }
        
        
        
    }
    if(error_flag) return 0;
    else return phase2(bf, extf, entf, file_name);
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
    short i;
    for(i=REGISTERS-1; i>=0; i--){
        if(!memcmp(line, registers[i], strlen(registers[i]))){
            /* check the next character after the match */
            if((!isspace(line[strlen(registers[i])]) &&  !(line[strlen(registers[i])] == ',') ) && !(line[strlen(registers[i])] == '\0'))
                /* might be a label */
                return -1;
        
            
            return i;
        }
    }
    return -1;
}

short check_R_command(char *line){
    short i;
    for(i=R_COMMANDS-1; i>=0 ; i--){
        if(!memcmp(line, R_commands[i], strlen(R_commands[i]))){
                /* check the next character after the match */
                if(!isspace(line[strlen(R_commands[i])]) && !(line[strlen(R_commands[i])] == '\0') && !(line[strlen(R_commands[i])] == ':'))
                    /* might be a label */
                    return -1;

            return i;
        }
    }
    return -1;
}

short check_I_command(char *line){
    short i;
    for(i=I_COMMANDS-1; i>=0 ; i--){
        if(!memcmp(line, I_commands[i], strlen(I_commands[i]))){
                /* check the next character after the match */
                if(!isspace(line[strlen(I_commands[i])]) && !(line[strlen(I_commands[i])] == '\0') && !(line[strlen(I_commands[i])] == ':'))
                    /* might be a label */
                    return -1;

            return i;
        }
    }
    return -1;
}

short check_J_command(char *line){
    short i;
    for(i=J_COMMANDS-1; i>=0 ; i--){
        if(!memcmp(line, J_commands[i], strlen(J_commands[i]))){
                /* check the next character after the match */
                if(!isspace(line[strlen(J_commands[i])]) && !(line[strlen(J_commands[i])] == '\0') && !(line[strlen(J_commands[i])] == ':'))
                    /* might be a label */
                    return -1;

            return i;
        }
    }
    return -1;
}

unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf, char *file_name){
    /* first go throuh the code typed commands*/
    unsigned short error_flag = 0;
    code* code_temp = bf->code_head;
    while(code_temp->next!= NULL){
        switch(code_temp->T_CODE){
            case R:{
               break;
            }
            /* I typed commands */
            case I:{
               if(*code_temp->immed == '?'){    
                   /* check if the label exist in the symbols table */
                   symbols_table* table_temp = label_exist(code_temp->label, bf->labels);
                   if(table_temp != NULL)
                   {
                       if(table_temp->T == EXTERN){
                            /* call an error */ 
                            error_handle(100, "", code_temp->original_line, 0, file_name); 
                            error_flag = -1;
                            break;
                        }
                        else{
                            /* calculate and convert the distance between the command and the label */ 
                            int distance;
                            if(error_flag) break;
                            distance = table_temp->val - code_temp->line;
                            int_to_binary(code_temp->immed, distance, IMMED_LENGTH-1);

                            if(table_temp->T == ENTRY){
                             /* add it to the entry file */
                             if(entf == NULL){
                              if((entf = malloc(sizeof(external_file*))) == NULL){
                                  /* failed to allocate memory */
                                  error_handle(-1, "", 0, 0, file_name);
                                  error_flag = -1;
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
                                  error_handle(-1, "", 0, 0, file_name);
                                  error_flag = -1;
                              }else{
                                  file_temp->next->line = code_temp->line;
                                  file_temp->next->label = table_temp;
                               }
                              }
                            } 
                            break;
                          }
                   }else{
                       error_handle(101, "" ,code_temp->original_line, 0, file_name);
                   }
    
               }
               break;
           }
           /* J typed commands*/
           case J:{

               /* check if the command is not stop and if it use a label */
               if(code_temp->reg == '0' && strcmp(code_temp->opcode, "111111")){
                   symbols_table* table_temp = label_exist(code_temp->label, bf->labels);
                        

                   if(table_temp != NULL){                        /* convert the label line that required in the command to binary */      
                        /* if we had an error in the past we don't have to bother converting to binary */
                        if(error_flag) break;
                        int_to_binary(code_temp->address, table_temp->val, ADDRESS_LENGTH-1);
                        if(table_temp->T == EXTERN){
                            /* add it to the extern file */
                            if(extf == NULL){
                                if((extf = malloc(sizeof(external_file*))) == NULL){
                                    /* failed to allocate memory */
                                    error_handle(-1, "", 0, 0, "");
                                    error_flag = -1;
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
                                    error_flag = -1;
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
                                    error_flag = -1;
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
                                    error_flag = -1;
                                }else{
                                    file_temp->next->line = code_temp->line;
                                    file_temp->next->label = table_temp;
                                }
                            }
                        }
                   }else{
                        /* if we didnt found the label throw an error */ 
                        error_handle(101, "" ,code_temp->original_line, 0, file_name);
                        error_flag = -1;
                   }
               }
               break;
           }
        }
        code_temp = code_temp->next;
    }
    if(error_flag == -1) return -1;
    else return build_binary_file(bf, file_name);
}

short build_binary_file(binary_file *bf, char* file_name){
    FILE *fp;
    code *code_pointer = bf->code_head;
    char *name = malloc(strlen(file_name) + 4);
    sprintf(name, "%s.ob", file_name);
    fp = fopen(name, "w");
    if(fp == NULL){
        error_handle(200, "", 0, 0, file_name);
        return 0;
    }
    
    /* start with the head */
    /* size of code commands | size of data commands */
    fprintf(fp, "     %d %d\n", bf->code_size, bf->data_size);
    while(code_pointer->next != NULL){
        
        convert_code_to_binary(code_pointer);

        binary_to_hex(code_pointer);

        if(code_pointer->line < 1000) fprintf(fp, "0%d %s %s %s %s\n", code_pointer->line, code_pointer->hex[3], code_pointer->hex[2], code_pointer->hex[1], code_pointer->hex[0]);
        else fprintf(fp, "%d %s %s %s %s\n", code_pointer->line, code_pointer->hex[3], code_pointer->hex[2], code_pointer->hex[1], code_pointer->hex[0]);
        
        code_pointer = code_pointer->next;
    }
    fclose(fp);
    return 1;
}