#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "hash_map.h"

#define MAX_LINE 1000

enum COMMAND_TYPE { A_COMMAND, C_COMMAND, LABEL };
enum A_COMMAND_MODE { A_VALUE, A_SYMBOL };
int16_t rom_address = 0;
int16_t ram_address = 16;

typedef struct Command {
    enum COMMAND_TYPE type;
    enum A_COMMAND_MODE a_cmd_mode;
    char* commandText;
    union {
        char* label; // if type is label
        char* symbol;
        int16_t value;
        struct {
            char* dest;
            char* comp;
            char* jump;
        } c_fields;
    } data;
} Command;

typedef struct C_NODE {
    Command* cmd;
    struct C_NODE* next;
} C_NODE;

Command* new_command() {
    Command* c = malloc(sizeof(Command));
    // c->data.symbol = NULL;
    // c->data.label = NULL;
    return c;
}

char* int_to_bin16(int16_t num);

void assemble(C_NODE* head_command, Map m, char* dest);

void install_predefined_symbols(Map m){

    map_add(m,"SP",int_to_bin16(0));
    map_add(m,"LCL",int_to_bin16(1));
    map_add(m,"ARG",int_to_bin16(2));
    map_add(m,"THIS",int_to_bin16(3));
    map_add(m,"THAT",int_to_bin16(4));

    map_add(m,"R0",int_to_bin16(0));
    map_add(m,"R1",int_to_bin16(1));
    map_add(m,"R2",int_to_bin16(2));
    map_add(m,"R3",int_to_bin16(3));
    map_add(m,"R4",int_to_bin16(4));
    map_add(m,"R5",int_to_bin16(5));
    map_add(m,"R6",int_to_bin16(6));
    map_add(m,"R7",int_to_bin16(7));
    map_add(m,"R8",int_to_bin16(8));
    map_add(m,"R9",int_to_bin16(9));
    map_add(m,"R10",int_to_bin16(10));
    map_add(m,"R11",int_to_bin16(11));
    map_add(m,"R12",int_to_bin16(12));
    map_add(m,"R13",int_to_bin16(13));
    map_add(m,"R14",int_to_bin16(14));
    map_add(m,"R15",int_to_bin16(15));

    map_add(m,"SCREEN",int_to_bin16(16384));
    map_add(m,"KBD",int_to_bin16(24576));
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

C_NODE* list_add(C_NODE* head, Command* cmd)
{

    C_NODE* new = malloc(sizeof(C_NODE));
    new->cmd = cmd;
    new->next = NULL;
    if (head == NULL)
    {
        return new;
    }
    C_NODE* temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = new;
    return head;
}

void print_commands(C_NODE* head) {
    C_NODE* temp = head;
    int i = 0;
    while(temp != NULL){
        printf("%d. %s\n", i, temp->cmd->commandText);
        temp = temp->next;
        i++;
    }
}

char* int_to_bin16(int16_t num) {
    int originalNUm = num;
    int i = 16; // number of bits
    char* binary = malloc(i + 1);
    binary[i] = '\0';
    while(i > 0) {
        binary[i-1] = (num & (int16_t)1) ? '1' : '0';
        i--;
        num >>=1;
    }
    return binary;
}

Command* parse_line (char line[], Map m) {

    int len = strlen(line);
    if (!len) return NULL;
    Command* c = new_command();
    c->commandText = malloc(strlen(line) + 1);
    strcpy(c->commandText, line);

    if (line[0] == '@') {
        c->type = A_COMMAND;
        if (isdigit(line[1]) || line[1] == '-') {
            char digits[6];
            for (int i = 1; i <= 6; i++) {
                digits[i-1] = line[i];
            }
            c->data.value = atoi(digits);
            c->a_cmd_mode = A_VALUE;
        } else {
            char* symbol = malloc(len);
            for (int i = 1; i <= len; i++) {
                symbol[i - 1] = line[i];
            }
            c->data.symbol = symbol;
            c->a_cmd_mode = A_SYMBOL;
        }
        rom_address++;
    } else if (line[0] == '(') {
        char* label = malloc(len-1);
        // error handling: check that last char is ')'
        c->type = LABEL;

        for (int i = 1; i < len - 1; i++) {
            label[i - 1] = line[i];
        }
        label[len-1] = '\0';
        c->data.label = label;
        if (map_get(m, label) != NULL){
             return NULL; // should signal error for duplicate labels
        }
        map_add(m, label, int_to_bin16(rom_address));

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

        c->data.c_fields.jump = jump;
        c->data.c_fields.comp = comp;
        c->data.c_fields.dest = dest;
        rom_address++;
    }

   return c;
}

char* get_destination_filename(char* source_filename) {
    size_t substr_len = strlen(source_filename) - 3;
    char* dest = malloc(substr_len + 5);
    memcpy(dest, source_filename, substr_len);
    strcat(dest,"hack");
    return dest;
}

int get_instruction(char* line, FILE* fptr) {
    int i = 0;
    line[i] = '\0'; // empty string
    char c;
    while (1)
    {
        c=fgetc(fptr);
        if (c == EOF) return -1;
        if (c == '/' && i > 0 && line[i-1] == '/')
        {
            line[--i] = '\0';
            while((c = fgetc(fptr)) != '\n' && c != EOF){} // go to the end of the line
            return c==EOF ? -1: 1;
        }
        else if (c == '\n')
        {
            line[i] = '\0';
            return 1;
        }
        else if (!isspace(c))
        {
            line[i++] = c;
        }
    }
}

int main (int argc, char** argv)
{
    clock_t t;
    t = clock();
    char* source_filename = argv[1];
    FILE* fptr = fopen(source_filename,"r");
    if (fptr == NULL)
    {
        printf("File '%s' does not exist.\n", source_filename);
        exit(1);
    }
    C_NODE* head = NULL;
    Map m = new_map();
    char* dest_filename = get_destination_filename(source_filename);
    char c, line[MAX_LINE];
    int i = 0;
    Command* cmd = NULL;

    install_predefined_symbols(m);
    int should_continue;

    do {
        should_continue = get_instruction(line, fptr);
        cmd = parse_line(line,m);
        if (cmd != NULL) {
            head = list_add(head,cmd);
        }
    } while (should_continue != -1);

    fclose(fptr);
    print_commands(head);
    assemble(head, m, dest_filename);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("Time taken: %f\n", time_taken);
    return 0;
}

void write_instruction(FILE *fptr, char* command){
    fprintf(fptr, "%s\n", command);
}

void assemble(C_NODE* head, Map m, char* dest) {
    C_NODE* temp = head;
    FILE* fptr = fopen(dest, "w");
    char* instruction = NULL;
    while (temp != NULL) {
        Command* cmd = temp->cmd;
        switch(cmd->type) {
            case A_COMMAND:
                instruction = cmd->a_cmd_mode == A_SYMBOL ? map_get(m, cmd->data.symbol) : int_to_bin16(cmd->data.value);
                if (instruction == NULL) {
                    instruction = int_to_bin16(ram_address++);
                    map_add(m, cmd->data.symbol, instruction);
                }
                write_instruction(fptr, instruction);
                break;
            case C_COMMAND:
                instruction = malloc(sizeof(char) * 17);
                instruction[0] = instruction[1] = instruction[2] = '1';
                memcpy(&instruction[3], get_comp_inst(cmd->data.c_fields.comp), 7 * sizeof(char));
                memcpy(&instruction[10], get_dest_inst(cmd->data.c_fields.dest), 3 * sizeof(char));
                memcpy(&instruction[13], get_jmp_inst(cmd->data.c_fields.jump), 3 * sizeof(char));
                instruction[16] = '\0';
                write_instruction(fptr, instruction);
                break;
            case LABEL:
                // do nothing
                break;
        }
        if (instruction != NULL) {
            free(instruction);
        }
        instruction = NULL;
        temp=temp->next;
    }
    fclose(fptr);
}