#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE 1000
#define MAX_SYMBOL_NAME 50

FILE* fptr;
enum COMMAND_TYPE { A_COMMAND, C_COMMAND, LABEL };
int rom_address = 0;
int ram_address = 16;

typedef struct Command {
    enum COMMAND_TYPE type;
    char* commandText;
    char* label; // if type is label
    char* symbol;
    int value;
    char* dest;
    char* comp;
    char* jump;
} Command;

typedef struct Node {
    Command* cmd;
    struct Node* next;
} Node;

typedef struct SymbolsMap {
    char** keys;
    void** vals;
    int count;
} SymbolsMap;

SymbolsMap* new_map(){
    SymbolsMap* m = malloc(sizeof(SymbolsMap));
    m->count = 0;
    m->keys = NULL;
    m->vals = NULL;
    return m;
}

Command* new_command(){
    Command* c = malloc(sizeof(Command));
    c->value = -1;
    c->symbol = NULL;
    c->label = NULL;
    return c;
}
char* int_to_bin16(int num);
void assemble(Node* head_command, SymbolsMap* m);

// pseudo hash map

void* hash_get(SymbolsMap* map, char* key){
  for (int i = 0; i < map->count; i++){
    if (strcmp(key,map->keys[i]) == 0) {
      return map->vals[i];
    }
  }
  return NULL;
}

int hash_contains(SymbolsMap* map, char* key){
  for (int i = 0; i < map->count; i++){
    if (strcmp(key,map->keys[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

void hash_add(SymbolsMap* map, char* key, void* value){

  map->count++;

  map->keys = realloc(map->keys, sizeof(char*) * map->count);
  map->vals = realloc(map->vals,sizeof(void*) * map->count);

  map->vals[map->count-1] = value;
  map->keys[map->count-1] = malloc(strlen(key) + 1);
  strcpy(map->keys[map->count - 1], key);
}

void insert_predefined_symbols(SymbolsMap* m){

    hash_add(m,"SP",int_to_bin16(0));
    hash_add(m,"LCL",int_to_bin16(1));
    hash_add(m,"ARG",int_to_bin16(2));
    hash_add(m,"THIS",int_to_bin16(3));
    hash_add(m,"THAT",int_to_bin16(4));

    hash_add(m,"R0",int_to_bin16(0));
    hash_add(m,"R1",int_to_bin16(1));
    hash_add(m,"R2",int_to_bin16(2));
    hash_add(m,"R3",int_to_bin16(3));
    hash_add(m,"R4",int_to_bin16(4));
    hash_add(m,"R5",int_to_bin16(5));
    hash_add(m,"R6",int_to_bin16(6));
    hash_add(m,"R7",int_to_bin16(7));
    hash_add(m,"R8",int_to_bin16(8));
    hash_add(m,"R9",int_to_bin16(9));
    hash_add(m,"R10",int_to_bin16(10));
    hash_add(m,"R11",int_to_bin16(11));
    hash_add(m,"R12",int_to_bin16(12));
    hash_add(m,"R13",int_to_bin16(13));
    hash_add(m,"R14",int_to_bin16(14));
    hash_add(m,"R15",int_to_bin16(15));

    hash_add(m,"SCREEN",int_to_bin16(16384));
    hash_add(m,"KBD",int_to_bin16(24576));
}

char* get_comp_inst(char* comp_cmd) {
    if (strcmp(comp_cmd, "0") == 0){
        return "0101010";
    }
    if (strcmp(comp_cmd, "1") == 0){
        return "0111111";
    }
    if (strcmp(comp_cmd, "-1") == 0){
        return "0111010";
    }
    if (strcmp(comp_cmd, "D") == 0){
        return "0001100";
    }
    if (strcmp(comp_cmd, "A") == 0){
        return "0110000";
    }
    if (strcmp(comp_cmd, "!D") == 0){
        return "0001101";
    }
    if (strcmp(comp_cmd, "!A") == 0){
        return "0110001";
    }
    if (strcmp(comp_cmd, "-D") == 0){
        return "0001111";
    }
    if (strcmp(comp_cmd, "-A") == 0){
        return "0110011";
    }
    if (strcmp(comp_cmd, "D+1") == 0){
        return "0011111";
    }
    if (strcmp(comp_cmd, "A+1") == 0){
        return "0110111";
    }
    if (strcmp(comp_cmd, "D-1") == 0){
        return "0001110";
    }
    if (strcmp(comp_cmd, "A-1") == 0){
        return "0110010";
    }
    if (strcmp(comp_cmd, "D+A") == 0){
        return "0000010";
    }
    if (strcmp(comp_cmd, "D-A") == 0){
        return "0010011";
    }
    if (strcmp(comp_cmd, "A-D") == 0){
        return "0000111";
    }
    if (strcmp(comp_cmd, "D&A") == 0){
        return "0000000";
    }
    if (strcmp(comp_cmd, "D|A") == 0){
        return "0010101";
    }

    if (strcmp(comp_cmd, "M") == 0){
        return "1110000";
    }
    if (strcmp(comp_cmd, "!M") == 0){
        return "1110001";
    }
    if (strcmp(comp_cmd, "-M") == 0){
        return "1110011";
    }
    if (strcmp(comp_cmd, "M+1") == 0){
        return "1110111";
    }
    if (strcmp(comp_cmd, "M-1") == 0){
        return "1110010";
    }
    if (strcmp(comp_cmd, "D+M") == 0){
        return "1000010";
    }
    if (strcmp(comp_cmd, "D-M") == 0){
        return "1010011";
    }
    if (strcmp(comp_cmd, "M-D") == 0){
        return "1000111";
    }
    if (strcmp(comp_cmd, "D&M") == 0){
        return "1000000";
    }
    if (strcmp(comp_cmd, "D|M") == 0){
        return "1010101";
    }
    // return 0
    return "0101010";
}

char* get_dest_inst(char* dest_cmd) {

    if(dest_cmd == NULL){
        return "000";
    }
    if (strcmp(dest_cmd, "M") == 0){
        return "001";
    }
    if (strcmp(dest_cmd, "D") == 0){
        return "010";
    }
    if (strcmp(dest_cmd, "MD") == 0){
        return "011";
    }
    if (strcmp(dest_cmd, "A") == 0){
        return "100";
    }
    if (strcmp(dest_cmd, "AM") == 0){
        return "101";
    }
    if (strcmp(dest_cmd, "AD") == 0){
        return "110";
    }
    if (strcmp(dest_cmd, "AMD") == 0){
        return "111";
    }
    return "000";
}

char* get_jmp_inst(char* jmp_cmd) {

    if(jmp_cmd == NULL){
        return "000";
    }
    if (strcmp(jmp_cmd, "JGT") == 0){
        return "001";
    }
    if (strcmp(jmp_cmd, "JEQ") == 0){
        return "010";
    }
    if (strcmp(jmp_cmd, "JGE") == 0){
        return "011";
    }
    if (strcmp(jmp_cmd, "JLT") == 0){
        return "100";
    }
    if (strcmp(jmp_cmd, "JNE") == 0){
        return "101";
    }
    if (strcmp(jmp_cmd, "JLE") == 0){
        return "110";
    }
    if (strcmp(jmp_cmd, "JMP") == 0){
        return "111";
    }
    return "000";
}

void list_add(Node* head, Command* cmd) {
    Node* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    Node* new = malloc(sizeof(struct Node));
    new->cmd = cmd;
    new->next = NULL;
    temp->next = new;
}

void print_commands(Node* head){
    Node* temp = head;
    int i = 0;
    while(temp != NULL){
        printf("%d. %s\n", i, temp->cmd->commandText);
        temp = temp->next;
        i++;
    }
}

char* int_to_bin16(int num) {

    char* binary = malloc(sizeof(char) * 17);
    int i = 15;
    while(num >= 1) {
        binary[i] = (num % 2 >= 1) ? '1' : '0';
        num /= 2;
        i--;
    }
    for (int j = 0; j <= i; j++) {
        binary[j] = '0';
    }
    binary[16] = '\0';
    return binary;
}

Command* parse_line (char line[], int count, SymbolsMap* m) {

    if (!count) return NULL;
    Command* c = new_command();
    c->commandText = malloc(strlen(line) + 1);
    strcpy(c->commandText, line);
    if (line[0] == '@') {
        c->type = A_COMMAND;
        if (isdigit(line[1])) { // error handling: check that if first digit is 0 there is nothing after
            char digits[5];
            int j = 0;
            for (int i = 1; i < count; i++) {
                digits[i-1] = line[i];
            }
            c->value = atoi(digits);
        } else {
            char* symbol = malloc(sizeof(char) * MAX_SYMBOL_NAME);
            for (int i = 1; i <= count; i++) {
                symbol[i - 1] = line[i];
            }
            c->symbol = symbol;
        }
        rom_address++;
    } else if (line[0] == '(') {
        char* label = malloc(sizeof(char) * MAX_SYMBOL_NAME);
        // error handling: check that last char is ')'
        c->type = LABEL;

        for (int i = 1; i < count - 1; i++) {
            label[i - 1] = line[i];
        }
        label[count-2] = '\0';
        c->label = label;
        if (hash_contains(m, label)){
             return NULL;
        }
        hash_add(m, label, int_to_bin16(rom_address));

    } else {
        c->type = C_COMMAND;
        char* dest = malloc(sizeof(char) * 4);
        char* comp = malloc(sizeof(char) * 4);
        char* jump = malloc(sizeof(char) * 4);
        int i = 0;
        char* comp_ptr;
        char* jmp_ptr;
        if ((comp_ptr = strstr(line, "="))) {
            while(line[i] != '=') {
                dest[i] = line[i];
                i++;
            }
        }
        if ((jmp_ptr = strstr(line,";"))) {
            for (i=1; i < 4; i++) {
                jump[i-1] = *(jmp_ptr + i);
            }
        }
        if(!comp_ptr) {
            // we dont have dest
            i = 0;
            while(line[i] != ';'){
                comp[i] = line[i];
                i++;
            }
        } else if (jmp_ptr) {
            // we have both dest and jump
            i = 1;
            while ((comp_ptr + i) != jmp_ptr){
                comp[i] = *(comp_ptr + i);
                i++;
            }
        } else {
            // we dont have jump
            i = 1;
            while (*(comp_ptr + i) != '\0') {
                comp[i-1] = *(comp_ptr + i);
                i++;
            }
        }

        jump[3] = '\0';
        comp[3] = '\0';
        dest[3] = '\0';

        c->jump = jump;
        c->comp = comp;
        c->dest = dest;
        rom_address++;
    }

   return c;
}
int main (int argc, char** argv){

    Node* head = NULL;
    SymbolsMap* m = new_map();
    insert_predefined_symbols(m);
    char* source = argv[1];
    fptr = fopen(source,"r");
    if (fptr == NULL){
        printf("File '%s' does not exist.\n", source);
        exit(1);
    }
    char c, line[MAX_LINE];
    int i = 0;

    while ((c=fgetc(fptr)) != EOF) {

        Command* cmd = NULL;
        if (c == '/' && i > 0 && line[i-1] == '/') {
            line[--i] = '\0';
            cmd = parse_line(line,i, m);
            i = 0;
            while(fgetc(fptr) != '\n'){} // go to the end of the line
        } else if (c == '\n') {
            line[i] = '\0';
            cmd = parse_line(line,i, m);
            i = 0;
        } else if (!iscntrl(c) && c != ' ' && c!= '\t') {
            line[i++] = c;
        }
        if (cmd != NULL){
            if (head == NULL){
                head = malloc(sizeof(Node));
                head->cmd = cmd;
                head->next = NULL;
            } else {
                list_add(head,cmd);
            }
        }
    }
    fclose(fptr);
    //print_commands(head);
    assemble(head, m);
    return 0;
}

void write_instruction(FILE *fptr, char* command){
    fprintf(fptr, "%s\n", command);
}

void assemble(Node* head, SymbolsMap* m) {
    Node* temp = head;
    FILE* fptr = fopen("out.hack", "w");
    while(temp!=NULL) {
        Command* cmd = temp->cmd;
        char* instruction;
        switch(cmd->type) {
            case A_COMMAND:
                if (cmd->value != -1) {
                    instruction = int_to_bin16(cmd->value);
                } else {
                    char* symbol = cmd->symbol;
                    if (hash_contains(m, symbol)) {
                        instruction = (char*)hash_get(m, symbol);
                    } else {
                       instruction = int_to_bin16(ram_address++);
                       hash_add(m, symbol, instruction);
                    }
                }
                write_instruction(fptr, instruction);
                break;
            case C_COMMAND:
                instruction = malloc(sizeof(char) * 17);
                instruction[0] = instruction[1] = instruction[2] = '1';
                memcpy(&instruction[3], get_comp_inst(cmd->comp), 7 * sizeof(char));
                memcpy(&instruction[10], get_dest_inst(cmd->dest), 3 * sizeof(char));
                memcpy(&instruction[13], get_jmp_inst(cmd->jump), 3 * sizeof(char));
                instruction[16] = '\0';
                write_instruction(fptr, instruction);
                break;
            case LABEL:
                // do nothing
                break;
        }
        // instruction = NULL;
        temp=temp->next;
    }
    fclose(fptr);
}