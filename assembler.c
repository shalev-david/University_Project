#include "assembler.h"


const char *R_commands[] = {"add", "sub", "and", "or",
                            "nor", "move", "mvhi", "mvlo"};

const char *I_commands[] = {"addi", "subi", "andi", "ori",
                            "nori", "bne", "beq", "blt", "bgt",
                            "lb", "sb", "lw", "sw", "lh", "sh"};

const char *J_commands[] = {"jmp", "la", "call", "stop"};

const char *instructions_commands[] = {".db", ".dw", ".dh",".asciz",
                                       ".entry", ".extern"};

const char *registers[] = {"$0", "$1", "$2", "$3", "$4","$5", "$6", "$7",
                           "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
                           "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
                           "$24", "$25", "$26", "$27", "$28", "$29", "$30","$31"};
                       
unsigned short phase1(FILE *fp, char* file_name){

    /* we read the file line by line */
    unsigned short error_flag = 0;
    char line[MAX_CHARACTERS];
    short line_count = 0;
    
    binary_file *bf;
    external_file *extf;
    entry_file *entf;
    code* code_pointer;
    data* data_pointer;
    data_val* data_val_pointer;
    symbols_table* label_pointer;
    symbols_table* end_of_table_pointer;
    
    bf = (binary_file*)allocate_pointer(0, file_name);
    entf = (entry_file*)allocate_pointer(1, file_name);
    extf = (external_file*)allocate_pointer(2, file_name);
    bf->code_head = (code*)allocate_pointer(3, file_name);
    bf->data_head = (data*)allocate_pointer(4, file_name);
    bf->data_head->val = (data_val*)allocate_pointer(5, file_name);
    bf->labels = (symbols_table*)allocate_pointer(6, file_name);

    
    label_pointer = bf->labels;
    end_of_table_pointer = label_pointer;
    bf->IC = 100;
    bf->DC = 0;
    bf->code_size = 0;
    bf->data_head->bytes_occupied = 0;
    bf->data_head->line = 0;
    bf->labels->ext = 0;
    bf->labels->ent = 0;
    bf->labels->unassigned = 0;
    bf->code_head->next = NULL;
    bf->data_head->next = NULL;
    bf->data_head->val->next = NULL;
    bf->labels->next = NULL;
    entf->next = NULL;
    extf->next = NULL;
    code_pointer = bf->code_head;
    data_pointer = bf->data_head;
    data_val_pointer = bf->data_head->val;

    while (fgets(line, MAX_CHARACTERS, fp) != NULL) { 
        enum state state = START;
        enum commands cmnd;
        char* temp = line;
        unsigned flag = 1;
        unsigned ent = 0;
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
                        /* check if there is at least one space after the command*/
                        if(!isspace(temp[strlen(R_commands[command])])){
                            /* error handle - no space after the command*/
                            error_handle(4, line, line_count, c, file_name);
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
                            code_pointer->T_CODE = R;
                            code_pointer->line =bf->IC;
                            code_pointer->original_line = line_count;
                            /* check if the label flag is activated */
                            if(label_flag){
                                symbols_table* next;
                                label_pointer->val = bf->IC;
                                label_pointer->T = CODE;
                                if(ent){
                                    ent = 0;
                                    label_pointer->unassigned = 0;
                                    label_pointer = end_of_table_pointer;
                                }else{
                                    /* set the pointer to the next label */
                                    next = (symbols_table*)allocate_pointer(6, file_name);
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                    end_of_table_pointer = label_pointer;
                                    label_pointer->ext = 0;
                                    label_pointer->ent = 0;
                                    label_pointer->unassigned = 0;
                                }
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
                    /* check if there is at least one space after the command*/
                        if(!isspace(temp[strlen(I_commands[command])])){
                            /* error handle - no space after the command*/
                            error_handle(4, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            state = END;
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
                        code_pointer->T_CODE = I;
                        code_pointer->line = bf->IC;
                        code_pointer->original_line = line_count;
                        if(label_flag){
                                symbols_table* next;
                                label_pointer->val =bf->IC;
                                label_pointer->T = CODE;
                                /* if the label was already exist just return to the end of the symbol table */
                                if(ent){
                                    ent = 0;
                                    label_pointer->unassigned = 0;
                                    label_pointer = end_of_table_pointer;
                                }else{
                                    /* set the pointer to the next label */
                                    next = (symbols_table*)allocate_pointer(6, file_name);
                                    next->next = NULL;
                                    label_pointer->next = next;
                                    label_pointer = label_pointer->next;
                                    end_of_table_pointer = label_pointer;
                                    label_pointer->ext = 0;
                                    label_pointer->ent = 0;
                                    label_pointer->unassigned = 0;
                                }
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
                            /* check if there is at least one space after the command*/
                            /* if it is a stop command no space needed */
                            if(!isspace(temp[strlen(J_commands[command])]) && command != 3){
                                /* error handle - no space after the command*/
                                error_handle(4, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                state = END;
                                break;
                            }
                            /* it is J typed command */
                            temp += strlen(J_commands[command]);
                            c += strlen(J_commands[command]);
                            state = ADDRESS;
                            if(command == 0) cmnd = J_JUMP;
                            else if(command == 3) state = STOP;
                            else cmnd = J_OTHER;

                            /* assign binary opcode */
                            /* if we had error in the past its not necessary */
                            if(!error_flag){
                                code_pointer->T_CODE = J;
                                code_pointer->line =bf->IC;
                                code_pointer->original_line = line_count;
                                if(label_flag){
                                        symbols_table* next; 
                                        label_pointer->val = bf->IC;
                                        label_pointer->T = CODE;
                                        /* if the label was already exist just return to the end of the symbol table */
                                        if(ent){
                                            ent = 0;
                                            label_pointer->unassigned = 0;
                                            label_pointer = end_of_table_pointer;
                                        }else{
                                            /* set the pointer to the next label */
                                            next = (symbols_table*)allocate_pointer(6, file_name);
                                            next->next = NULL;
                                            label_pointer->next = next;
                                            label_pointer = label_pointer->next;
                                            end_of_table_pointer = label_pointer;
                                            label_pointer->ext = 0;
                                            label_pointer->ent = 0;
                                            label_pointer->unassigned = 0;
                                        }
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
                    
                    else if((command = check_Instruction_command(temp)) != -1){
                            /* check if it is an attempt to make a label */
                            if(temp[strlen(instructions_commands[command])] == ':'){
                                /* error handle - bad label name */
                                error_handle(10, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            
                            /* check if there is at least one space after the command*/
                            if(!isspace(temp[strlen(instructions_commands[command])])){
                                /* error handle - no space after the command*/
                                error_handle(4, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                state = END;
                                break;
                            }
                            
                            /* move to pointer to be after the command */
                            temp += strlen(instructions_commands[command]);
                            c += strlen(instructions_commands[command]);
                            
                            switch (command)
                            {
                            case 0:{
                                cmnd = DB;
                                state = DBHW;
                                break;
                            }
                            case 1:{
                                cmnd = DW;
                                state = DBHW;
                                break;
                            }
                            case 2:{
                                cmnd = DH;
                                state = DBHW;
                                break;
                            }
                            case 3:{
                                cmnd = ASCIZ;
                                state = STRING;
                                break;
                            }
                            case 4:{
                                cmnd = ENTRY;
                                state = LABEL;
                                break;
                            }
                            case 5:{
                                cmnd = EXTERN;
                                state = LABEL;
                            }
                            default:
                                break;
                            }

                            /* if we had error in the past its not necessary */
                            if(!error_flag){
                                if(label_flag){
                                        symbols_table* next;
                                        label_pointer->val = bf->DC;
                                        label_pointer->T = DATA;
                                        /* if the label was already exist just return to the end of the symbol table */
                                        if(ent){
                                            ent = 0;
                                            label_pointer->unassigned = 0;
                                            label_pointer = end_of_table_pointer;
                                        }else{
                                            /* set the pointer to the next label */
                                            next = (symbols_table*)allocate_pointer(6, file_name);
                                            next->next = NULL;
                                            label_pointer->next = next;
                                            label_pointer = label_pointer->next;
                                            end_of_table_pointer = label_pointer;
                                            label_pointer->ext = 0;
                                            label_pointer->ent = 0;
                                            label_pointer->unassigned = 0;
                                        }
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
                            if((v = valid_label(temp, i, bf->labels)) && v != 5){
                                if(v == 1)
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                else if(v==2)
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                else if(v==3)
                                    /* error handle - label already exist */
                                    error_handle(13, line, line_count, c, file_name);
                                else if(v==4)
                                    /* error handle - label size greater than 31 */
                                    error_handle(15, line, line_count, c, file_name);
                                else if(v==6)
                                    /* error handle - labled declared as extern */
                                    error_handle(16, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(v==5){
                                /* the label exist and wating for value */
                                ent = 1;
                                /* set the pointer to the specific label */
                                label_pointer = get_label(temp, i, bf->labels);
                            }
                            /* if we had an error in the past its not necessary */
                            if(!error_flag && !v){
                                /* try to allocate space for the label */
                                if((label_pointer->symbol = malloc(i+1)) == NULL){
                                    /* error handle - failed to allocate space */
                                    error_handle(-1, "", 0, 0, file_name);
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
                        /* check if there is a comma */
                        if(*temp == ','){
                            /* error handle - unwanted comma */
                            error_handle(5, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
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
                        if(!flag){state = END; break;}
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
                    if(!flag){state = END; break;}
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
                            if(v && v != 3 && v!= 5){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else if(v==2)
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                else if(v==4)
                                    /* error handle - label size greater than 31 */
                                    error_handle(15, line, line_count, c, file_name);
                                else if(v==6)
                                    /* error handle - I Branch command can't be executed with extern label */
                                    error_handle(17, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
                                }
                                memcpy(code_pointer->label, temp, i);
                                code_pointer->label[i] = '\0';
                                
                            }
                            temp += i+1;
                            c += i+1;
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
                            if(immed > MAX_INT_16 || immed < MIN_INT_16){
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
                                if(*temp == '.'){
                                    error_handle(33, line, line_count, c, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                }else if(*temp != ','){
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
                    if(!flag){state = END; break;}
                    break;
                }
                case ADDRESS:{

                    switch (cmnd)
                    {
                      case J_JUMP:{           
                        short reg;
                        /* check if there is a comma */
                        if(*temp == ','){
                            /* error handle - unwanted comma */
                            error_handle(5, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* check if the given address is for a label or for a register */
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
                                if(!error_flag) int_to_binary(code_pointer->address, reg, ADDRESS_LENGTH-1);
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
                            if(v && v != 3 && v!= 5 && v!= 6){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else if(v==2)
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                else if(v==4)
                                    /* error handle - label size greater than 31 */
                                    error_handle(15, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
                                }
                                memcpy(code_pointer->label, temp, i);
                                code_pointer->label[i] = '\0';

                            }
                                temp += i+1;
                                c += i+1;
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
                            if(v && v != 3 && v != 5 && v != 6){
                                if(v == 1){
                                    /* error handle - first character must be a letter */
                                    error_handle(11, line, line_count, c, file_name);
                                }else if(v==2)
                                    /* error handle - illegal character */
                                    error_handle(12, line, line_count, c, file_name);
                                else if(v==4)
                                    /* error handle - label size greater than 31 */
                                    error_handle(15, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            if(!error_flag){
                                /* try to allocate space for the label */
                                if((code_pointer->label = malloc(i)) == NULL){
                                    /* error handle - failed to allocate memory */
                                    error_handle(-1, "", 0, 0, file_name);
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
                    if(!flag){state = END; break;}
                    break;
                }
                case DBHW:{
                        char *t;
                        long immed;
                        /* first check if its a number */
                        if((*temp > '9' || *temp < '0') && *temp != '-' && *temp!='+'){
                            error_handle(33, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            state = END;
                            break;
                        }

                        
                        immed = strtol(temp, &t, 10);
                        switch (cmnd)
                        {
                        case DB:{
                            if(immed > MAX_INT_8 || immed < MIN_INT_8){
                            /* error handle - out of range */
                            error_handle(20, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                            }

                            /* assing the binary value */
                            if(!error_flag){
                                int_to_binary(data_val_pointer->val, immed, 8);
                                data_pointer->bytes_occupied++;
                                data_val_pointer->size = 1;
                            }
                            break;
                        }
                        case DH:{
                            if(immed > MAX_INT_16 || immed < MIN_INT_16){
                            /* error handle - out of range */
                            error_handle(20, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                            }
                            /* assing the binary value */
                            if(!error_flag) {
                                int_to_binary(data_val_pointer->val, immed, 16);
                                data_pointer->bytes_occupied += 2;
                                data_val_pointer->size = 2;
                            }
                            break;
                        }
                        case DW:{
                            if(immed > MAX_INT_32 || immed < MIN_INT_32 -1){
                            /* error handle - out of range */
                                error_handle(20, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                            /* assing the binary value */
                            if(!error_flag) {
                                int_to_binary(data_val_pointer->val, immed, 32);
                                data_pointer->bytes_occupied +=4;
                                data_val_pointer->size = 4;
                            }
                            break;
                        }
                        default:
                            break;
                        }
                        
                        if(!flag) break;
                        /* set the line pointer to be after the number */
                        c += t-temp;
                        temp = t;
                        while(isspace(*temp)){
                            temp++;
                            c++;
                        }
                        
                        if(*temp == ','){                            
                            /* create the next value pointer */
                            if(!error_flag){
                                data_val* next = (data_val*)allocate_pointer(5, file_name);
                                next->next = NULL;
                                data_val_pointer->next = next;
                                data_val_pointer = data_val_pointer->next;
                            }
                            /* we don't change the state because we need another number */
                            temp++;
                            c++;
                        }
                        else if(*temp == '\n' || *temp == '\0'){
                            /* end of command */
                            state = END;
                        }
                        break;
                    }
                case STRING:{
                        /* check if the first character after all the spaces isn't a quotation mark */
                        if(*temp != '"'){
                            /*error handle - missing quotation mark */
                            error_handle(40, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* else continue */
                        temp++;
                        c++;

                        while(*temp!= '"' && *temp != '\0' && *temp != '\n'){
                            if(!error_flag){
                                data_val* next ;
                                int_to_binary(data_val_pointer->val, (int)*temp, BYTE_LENGTH);
                                data_val_pointer->size = 1;
                                data_pointer->bytes_occupied++;
                                /* create new data value pointer */                
                                next= (data_val*)allocate_pointer(5, file_name);
                                next->next = NULL;
                                data_val_pointer->next = next;
                                data_val_pointer = data_val_pointer->next;
                            }
                            temp++;
                            c++;
                        }                        /* check if the last character is indeed a quotation mark */
                        if(*temp != '"'){
                            /*error handle - missing quotation mark */
                            error_handle(40, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }

                        /* else continue */
                        /* add the last character - \0 */
                        if(!error_flag){
                            int_to_binary(data_val_pointer->val, 0, BYTE_LENGTH);
                            data_val_pointer->size = 1;
                            data_pointer->bytes_occupied++;
                            
                        }
                        temp++;
                        c++;
                        state = END;
                        break;
                    }
                case LABEL:{
                        unsigned short i = 0, v;      
                        /* we will keep the label */
                        while(temp[i] != '\0' && !isspace(temp[i])){
                            i++;
                        }
                        /* first validate the label */
                        v = valid_label(temp, i, bf->labels);
                        if(v && v != 3 && v!= 5 && v!= 6){
                            if(v == 1){
                                /* error handle - first character must be a letter */
                                error_handle(11, line, line_count, c, file_name);
                            }else if(v==2)
                                /* error handle - illegal character */
                                error_handle(12, line, line_count, c, file_name);
                            else if(v==4)
                                /* error handle - label size greater than 31 */
                                error_handle(15, line, line_count, c, file_name);
                            error_flag = 1;
                            flag = 0;
                            break;
                        }
                        /* label exist */
                        if(v == 3){
                            label_pointer = get_label(temp, i, bf->labels);
                            if(cmnd == ENTRY){
                                if(label_pointer->ext){
                                    /* error handle - label already exist as extern */
                                    error_handle(50, line, line_count, c, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                if(!error_flag){
                                    label_pointer->ent = 1;
                                    label_pointer = end_of_table_pointer;
                                }
                            }else{
                                if(label_pointer->ent){
                                    /* error handle - label already exist as entry */
                                    error_handle(50, line, line_count, c, file_name);
                                    error_flag = 1;
                                    flag = 0;
                                    break;
                                }
                                if(!error_flag){
                                    label_pointer->ent = 1;
                                    label_pointer = end_of_table_pointer;
                                }
                            }
                        }else if(v==5){
                            if(cmnd == EXTERN){
                                /* error handle - label already exist as entry */
                                error_handle(50, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                        else if(v==6){
                            if(cmnd == EXTERN){
                                /* error handle - label already exist as extern */
                                error_handle(50, line, line_count, c, file_name);
                                error_flag = 1;
                                flag = 0;
                                break;
                            }
                        }
                        }else if(v == 0){
                            symbols_table *next;
                            /* create a new label for entry */
                            /* try to allocate space for the label */
                            if((label_pointer->symbol = malloc(i+1)) == NULL){
                                /* error handle - failed to allocate space */
                                error_handle(-1, "", 0, 0, file_name);
                            }
                            memcpy(label_pointer->symbol, temp, i);
                            label_pointer->symbol[i] = '\0';
                            if(cmnd == ENTRY){
                                label_pointer->ent = 1;
                                label_pointer->unassigned = 1;
                            }else label_pointer->ext = 1;
                            
                            label_pointer->val = 0;
                            /* set the pointer to the next label */
                            next = (symbols_table*)allocate_pointer(6, file_name);
                            next->next = NULL;
                            label_pointer->next = next;
                            label_pointer = label_pointer->next;
                            end_of_table_pointer = label_pointer;
                            label_pointer->ext = 0;
                            label_pointer->ent = 0;
                            label_pointer->unassigned = 0;
                            
                        }
                        temp += i+1;
                        c+= i+1;
                        state = END;
                        break;
                    }
                case END:{
                        if(*temp != '\0'){

                            /* error handle - junk after end of command */
                            error_handle(21, line, line_count, c, file_name);
                            error_flag = 1;
                        }else if(cmnd<7){

                            if(!error_flag){
                                /* set the pointer to the next code line */
                                code* next = (code*)allocate_pointer(3, file_name);
                                next->next = NULL;
                                code_pointer->next = next;
                                code_pointer = code_pointer->next;
                            }
                        }else if(cmnd < 11){
                            if(!error_flag){
                                data* next;
                                data_val* val;
                                /* create the next data pointer */
                                next = (data*)allocate_pointer(4, file_name);
                                bf->DC += data_pointer->bytes_occupied;
                                next->next = NULL;
                                data_pointer->next = next;
                                data_pointer = data_pointer->next;
                                data_pointer->bytes_occupied = 0;
                                data_pointer->line = bf->DC;
                                
                                /* create the next data value pointer */
                                val = (data_val*)allocate_pointer(5, file_name);
                                val->next = NULL;
                                data_pointer->val = val;                 
                                data_val_pointer = data_pointer->val;
                            }
                        }
                        flag =0;
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

                    case IMMED: case DBHW:{
                        /* handle error - immed is missing */
                        error_flag = 1; 
                        error_handle(32, line, line_count, c, file_name);
                        break; 
                    }
                    default: break;

                }
            }  
    }
    /* assing the new line values for data segments */
    data_pointer = bf->data_head;
    while(data_pointer->next != NULL){
            data_pointer->line += bf->IC;
            data_pointer = data_pointer->next;
        }   
    label_pointer = bf->labels;
    while(label_pointer->next != NULL){      
            if(label_pointer->T == DATA) label_pointer->val += bf->IC;     
            label_pointer = label_pointer->next;
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

short check_Instruction_command(char *line){
    short i;
    for(i=INSTRUCTION_COMMANDS-1; i>=0 ; i--){
        if(!memcmp(line, instructions_commands[i], strlen(instructions_commands[i]))){
                /* check the next character after the match */
                if(!isspace(line[strlen(instructions_commands[i])]) && !(line[strlen(instructions_commands[i])] == '\0') && !(line[strlen(instructions_commands[i])] == ':'))
                    /* might be a label */
                    return -1;

            return i;
        }
    }
    return -1;
}

unsigned short phase2(binary_file *bf, external_file *extf, entry_file *entf, char *file_name){
    /* go throuh the code typed commands*/
    short error_flag = 0;
    code* code_temp = bf->code_head;
    while(code_temp!= NULL){
        switch(code_temp->T_CODE){
            case R:{
               break;
            }
            /* I typed commands */
            case I:{
               if(*code_temp->immed == '?'){    
                    /* check if the label exist in the symbols table */
                    symbols_table* table_temp = get_label(code_temp->label, strlen(code_temp->label), bf->labels);
                    if(table_temp != NULL)
                    {
                        /* calculate and convert the distance between the command and the label */ 
                        int distance;
                        if(error_flag) break;
                        distance = table_temp->val - code_temp->line;
                        int_to_binary(code_temp->immed, distance, IMMED_LENGTH-1);
                        if(table_temp->ent){
                            entry_file* file_temp;
                            /* firt check that the label is readly declraed in the file */
                            if(table_temp->unassigned){
                                /* error handle - empty label */
                                error_handle(102, "", code_temp->original_line, 0, file_name);
                                error_flag = -1;
                                break;
                            }
                            /* add it to the entry file */
                            file_temp = entf;
                            while(file_temp->next != NULL){file_temp = file_temp->next;}
                            /* try to allocate memory for a new line */
                            file_temp->next = (entry_file*)allocate_pointer(1, file_name);
                            file_temp->next->line = table_temp->val;
                            file_temp->next->label = table_temp;
                            file_temp->next->next = NULL;
                        }
                        break;
                    }else{
                       error_handle(101, "" ,code_temp->original_line, 0, file_name);
                       error_flag = -1;
                   }
               }
               break;
           }
           /* J typed commands*/
           case J:{

               /* check if the command is not stop and if it use a label */
               if(code_temp->reg == '0' && strcmp(code_temp->opcode, "111111")){
                   symbols_table* table_temp = get_label(code_temp->label, strlen(code_temp->label), bf->labels);
                        

                   if(table_temp != NULL){                        /* convert the label line that required in the command to binary */      
                        /* if we had an error in the past we don't have to bother converting to binary */
                        if(error_flag) break;
                        int_to_binary(code_temp->address, table_temp->val, ADDRESS_LENGTH-1);
                        if(table_temp->ext){
                            /* add it to the extern file */
                            external_file* file_temp = extf;
                            while(file_temp->next != NULL){file_temp = file_temp->next;}
                            file_temp->line = code_temp->line;
                            file_temp->label = table_temp;
                            /* try to allocate memory for a new line */
                            file_temp->next = (external_file*)allocate_pointer(2, file_name);
                            file_temp->next->next = NULL;
                        }else if(table_temp->ent){
                            entry_file* file_temp;
                            /* firt check that the label is readly declraed in the file */
                            if(table_temp->unassigned){
                                /* error handle - empty label */
                                error_handle(102, "", code_temp->original_line, 0, file_name);
                                error_flag = -1;
                                break;
                            }
                            
                            /* add it to the entry file */
                            file_temp = entf;
                            while(file_temp->next != NULL){
                                file_temp = file_temp->next;
                            }
                            file_temp->line = table_temp->val;
                            file_temp->label = table_temp;
                            /* try to allocate memory for a new line */
                            file_temp->next = (entry_file*)allocate_pointer(1, file_name);
                            file_temp->next->next = NULL;

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
    else return build_binary_file(bf, file_name) && build_entery_file(entf, file_name) && build_extern_file(extf, file_name);

    
}

short build_binary_file(binary_file *bf, char* file_name){
    FILE *fp;
    code *code_pointer = bf->code_head;
    data *data_pointer = bf->data_head;
    short bytes_remaining;
    unsigned short line=0;

    /* create a file with the correct name */
    char *name = malloc(strlen(file_name) + 4);
    if(name == NULL) error_handle(-1, "", 0, 0, file_name);
    sprintf(name, "%s.ob", file_name);
    fp = fopen(name, "w");
    if(fp == NULL){
        error_handle(200, "", 0, 0, file_name);
        return 0;
    }    
    /* start with the head */
    /* size of code commands | size of data commands */
    fprintf(fp, "     %d %d\n", bf->code_size, bf->DC);
    
    /* then start by adding the code segments */
    while(code_pointer != NULL){
        
        convert_code_to_binary(code_pointer);

        binary_code_to_hex(code_pointer);

        fprintf(fp, "%04d %s %s %s %s\n", code_pointer->line, code_pointer->hex[3], code_pointer->hex[2], code_pointer->hex[1], code_pointer->hex[0]);
        
        line = code_pointer->line;
        code_pointer = code_pointer->next;
    }

    /* new line counter for the binary file */
    if(line!=0) line +=4;
    if(data_pointer->next != NULL) {fprintf(fp, "%04d", line); line +=4;}

    /* after that add the data segments */
    bytes_remaining = 4;
    while(data_pointer->next != NULL){

        data_val *ptr = data_pointer->val;

        binary_data_to_hex(data_pointer);

        while(ptr != NULL){ 

            if(bytes_remaining >= ptr->size){
                /* add each hex to the file */

                short count = ptr->size - 1;
                while(count >= 0) {fprintf(fp, " %s", ptr->hex[count--]); bytes_remaining--;}
                /* if we fill all the remaining cells create a new line if needed */
                if(bytes_remaining == 0 && (ptr->next != NULL || data_pointer->next->next != NULL)){
                    fprintf(fp, "\n%04d", line);
                    line +=4;
                    bytes_remaining = 4;
                }
                ptr = ptr->next;
            }else{

                short count = ptr->size - 1;
                while(bytes_remaining > 0){
                    fprintf(fp, " %s", ptr->hex[count--]);
                    bytes_remaining--;
                }
                /* create a new line and continue */
                fprintf(fp, "\n%04d", line);
                line += 4;
                bytes_remaining = 4;
                /* in the next run start with a new size and complete the remaining hex cells */
                ptr->size = count + 1;
            }
        }

        data_pointer = data_pointer->next;
    }

    fclose(fp);

    return 1;
}

short build_entery_file(entry_file *entf, char* file_name){
    FILE *fp;
    entry_file *ptr = entf;
    char *name;
    if(ptr->next == NULL){
        /* entry table is empty - don't create file */
        return -1;
    }

    /* create a file with the proper name */
    name = malloc(strlen(file_name) + 5);
    if(name == NULL) error_handle(-1, "", 0, 0, file_name);
    sprintf(name, "%s.ent", file_name);
    fp = fopen(name, "w");
    if(fp == NULL){
        error_handle(200, "", 0, 0, file_name);
        return 0;
    }

    while(ptr->next != NULL){
        
        fprintf(fp, "%s %04d\n", ptr->label->symbol, ptr->line);
        
        ptr = ptr->next;
    }
    fclose(fp);
    return 1;
}

short build_extern_file(external_file *extf, char* file_name){
    FILE *fp;
    external_file *ptr = extf;
    char *name;
    if(ptr->next == NULL){
        /* entry table is empty - don't create file */
        return -1;
    }

    /* create a file with the proper name */
    name = malloc(strlen(file_name) + 5);
    if(name == NULL) error_handle(-1, "", 0, 0, file_name);
    sprintf(name, "%s.ext", file_name);
    fp = fopen(name, "w");
    if(fp == NULL){
        error_handle(200, "", 0, 0, file_name);
        return 0;
    }

    while(ptr->next != NULL){
        
        fprintf(fp, "%s %04d\n", ptr->label->symbol, ptr->line);
        
        ptr = ptr->next;
    }
    fclose(fp);
    return 1;

}

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

void binary_code_to_hex(code *w){
    /* convert the given word in binary to hex */ 
    int i, j, count = 0;
    for(i = 0; i<BYTE_LENGTH*4; i+= BYTE_LENGTH){
        unsigned char hex;
        /* convert string of btyes to hex */
        for(j = 0; j<BYTE_LENGTH; j++){
            unsigned char t = w->word[i+j] =='1'?1:0;
            hex = (hex<<1)|t; 

        }

        /* copy the string in hex format */
        sprintf(w->hex[count], "%02X", hex);

        /* add endline in the end */
        w->hex[count][2] = '\0';
        count++;
    }

}

void binary_data_to_hex(data *w){

    data *data_ptr = w;
    data_val *data_val_ptr = w->val;
    while(data_ptr->next != NULL){


        while(data_val_ptr != NULL){
            unsigned i, j, count = 0;
            data_val_ptr->size = strlen(data_val_ptr->val)/BYTE_LENGTH;
            for(i = 0; i<BYTE_LENGTH*data_val_ptr->size; i+= BYTE_LENGTH){
                
                unsigned char hex;
                /* convert string of btyes to hex */
                for(j = 0; j<BYTE_LENGTH; j++){
                    unsigned char t = data_val_ptr->val[i+j] == '1' ? 1:0;
                    hex = (hex<<1)|t; 

                }

                /* copy the string in hex format */
                sprintf(data_val_ptr->hex[count], "%02X", hex);

                /* add endline in the end */
                data_val_ptr->hex[count][2] = '\0';
                count++;
            }
            data_val_ptr = data_val_ptr->next;
        }
        
        data_ptr = data_ptr->next;
    }
    
}

symbols_table* get_label(char *label, size_t size, symbols_table* table){
    symbols_table* ptr = table;
    while(ptr->next != NULL){
        if(!strncmp(label, ptr->symbol, size))
            if(size == strlen(ptr->symbol))
                break;
        ptr = ptr->next;
    }
    return ptr;
}

void* allocate_pointer(short option, char* file_name){
    void* ptr;
    switch (option)
    {
    case 0:{
        /* binary file */
        
        if((ptr = malloc(sizeof(binary_file))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
        break;
    }
    case 1:{
        /* entry file */
        if((ptr = malloc(sizeof(entry_file))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    case 2:{
        /* extern file */
        if((ptr = malloc(sizeof(external_file))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    case 3:{
        /* code */
        if((ptr = malloc(sizeof(code))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    case 4:{
        /* data */
        if((ptr = malloc(sizeof(data))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    case 5:{
        /* data val */
        if((ptr = malloc(sizeof(data_val))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    case 6:{
        /* symbols_table */
        if((ptr = malloc(sizeof(symbols_table))) == NULL){
            /* failed to allocate memory */
            error_handle(-1, "", 0, 0, file_name);
            return NULL;
        }
            break;
    }
    default:
        break;
    }
    return ptr;
}