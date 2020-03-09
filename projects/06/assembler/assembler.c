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

void assemble(Node* head_command);

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
char* int_to_bin16(int num){

    return NULL;
}

Command* parse_line (char line[] , int count) {

    if (!count) return NULL;
    Command* c = malloc(sizeof(Command));
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
            char symbol[MAX_SYMBOL_NAME];
            for (int i = 1; i < count; i++) {
                symbol[i - 1] = line[i];
            }
            c->symbol = symbol;
        }
        rom_address++;
    } else if (line[0] == '(') {
        char label[MAX_SYMBOL_NAME];
        // error handling: check that last char is ')'
        c->type = LABEL;

        for (int i = 1; i < count - 1; i++) {
            label[i - 1] = line[i];
        }
        c->label = label;
        if (hash_contains(label)){
             return NULL;
        }
        hash_add(label, rom_address);

    } else {
        c->type = C_COMMAND;
        char dest[3];
        char comp[3];
        char jump[3];
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

        c->jump = jump;
        c->comp = comp;
        c->dest = dest;
        rom_address++;
    }

   return c;
}
int main (int argc, char** argv){

    Node* head = NULL;
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
            cmd = parse_line(line,i);
            i = 0;
            while(fgetc(fptr) != '\n'){} // go to the end of the line
        } else if (c == '\n') {
            line[i] = '\0';
            cmd = parse_line(line,i);
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
    print_commands(head);
    assemble(head);
    return 0;
}

void assemble(Node* head) {
    Node* temp = head;

    while(temp!=NULL) {
        Command* cmd = temp->cmd;
        switch(cmd->type) {
            case A_COMMAND:
                if (cmd->value) {
                     int_to_bin16(cmd->value);
                } else {
                    char* symbol = cmd->symbol;
                    if (hash_contains(symbol)) {
                        hash_get(symbol);
                    } else {
                        hash_add(symbol, int_to_bin16(ram_address++));
                    }
                }
                break;
            case C_COMMAND:
                break;
            case LABEL:
                break;
        }
        temp=temp->next;
    }
}
