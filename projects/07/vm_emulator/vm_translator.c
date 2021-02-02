#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <regex.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>

enum INSTRUCTION_TYPE { I_ARITHMETIC, I_PUSH, I_POP, I_LABEL, I_GOTO, I_IF, I_FUNCTION, I_RETURN, I_CALL };

int cmp_label_index = 1;
typedef struct vm_instruction {
    char* instruction_text;
    char* arg1;
    char* arg2;
    enum INSTRUCTION_TYPE i_type;
} vm_instruction;

typedef struct list_node {
   void* data;
   struct list_node* next;
} list_node;

typedef struct list {
   list_node* head;
   size_t size;
} list;

list_node* new_list_node(void* data, size_t data_size)
{
    list_node* ln = malloc(sizeof(list_node));
    ln->data = malloc(data_size);
    memcpy(ln->data, data, data_size);
    ln->next = NULL;
    return ln;
}

list* new_list(list_node* head)
{
    list* l = malloc(sizeof(list));
    l->head = head;
    l->size = head == NULL ? 0 : 1;
    return l;
}

int list_add (list* l, list_node* ln)
{
    list_node* head = l->head;
    if (head == NULL) {
        l->head = ln;
        l->size = 1;
        return 1;
    }
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = ln;
    l->size++;
    return l->size;
}

void free_list_node(list_node* n)
{
    free(n->data);
    free(n);
}

void free_list(list* l)
{
    list_node* head = l->head;
    list_node* cur = head;
    if (l == NULL) {
        return;
    }
    if (head == NULL){
        free(l);
        return;
    }
    while (head != NULL) {
        // head = head->next;
        // cur = head;
        // free(cur);
    }
    free(l);
}

void print_list(list* l, void (*fptr)(void *))
{
    if (l == NULL) {
        return;
    }
    list_node* node = l->head;
    while (node != NULL) {
        (*fptr)(node->data);
        node = node->next;
    }
}

void print_string(void *d)
{
   printf("\n%s", (char *)d);
}

void print_vmi(void *d)
{
    vm_instruction* vmi = (vm_instruction*)d;
    printf("\n");
    printf("instruction text --> %s\narg1 --> %s\narg2 --> %s\ntype -->  %d", vmi->instruction_text, vmi->arg1, vmi->arg2, vmi->i_type);
}

// removes spaces from the ends of the string
// returns new string size
int str_strip_spaces(char* str)
{
    size_t str_len = strlen(str);
    int i = 0;
    int j = str_len-1;
    int stop_start = 0;
    int stop_end = 0;
    int spaces_start = 0;
    int spaces_end = 0;
    while (!stop_start || !stop_end) {
        if (i > j) {
            *str = '\0';
            return 0;
        }
        if (isspace(str[i]) && !stop_start) {
            spaces_start++;
        } else {
            stop_start = 1;
        }
        if (isspace(str[j]) && !stop_end) {
            spaces_end++;
        } else {
            stop_end = 1;
        }
        !stop_start && i++;
        !stop_end && j--;
    }
    str_len -= spaces_end;
    str[str_len] = '\0';
    str_len -= spaces_start;
    memmove(str, str + spaces_start, str_len + 1);
    return str_len;
}

int str_starts_with (char* source, char* test)
{
    size_t t_len = strlen(test);
    if (t_len >= strlen(source)) {
        return strcmp(source, test) == 0;
    }
    char buff[t_len + 1];
    memcpy(buff, source, t_len);
    buff[t_len] = '\0';
    return strcmp(buff, test) == 0;
}

void set_instruction_fields(vm_instruction* vmi, char* instr_text, int instr_length)
{
    vmi->instruction_text = malloc(instr_length + 1);
    strcpy(vmi->instruction_text, instr_text);

    if (str_starts_with(instr_text, "push")) {
        vmi->i_type = I_PUSH;
    } else if (str_starts_with(instr_text, "pop")) {
        vmi->i_type = I_POP;
    } else if (str_starts_with(instr_text, "label")) {
        vmi->i_type = I_LABEL;
    } else if (str_starts_with(instr_text, "goto")) {
        vmi->i_type = I_GOTO;
    } else if (str_starts_with(instr_text, "if-goto")) {
        vmi->i_type = I_IF;
    } else if (str_starts_with(instr_text, "function")) {
        vmi->i_type = I_FUNCTION;
    } else if (str_starts_with(instr_text, "call")) {
        vmi->i_type = I_CALL;
    } else if (strcmp(instr_text, "return") == 0) {
        vmi->i_type = I_RETURN;
    } else {
        vmi->i_type = I_ARITHMETIC;
    }

    strtok(instr_text, " "); // ignore first part

    char* arg1 = strtok(NULL, " ");
    if (arg1) {
        vmi->arg1 = malloc(strlen(arg1) + 1);
        strcpy(vmi->arg1, arg1);
    }

    char* arg2 = strtok(NULL, " ");
     if (arg2) {
        vmi->arg2 = malloc(strlen(arg2) + 1);
        strcpy(vmi->arg2,arg2);
     }
    return;
}

list* parse(char* vm_code)
{
    // use strtok ?
    list* l = new_list(NULL);
    size_t vmi_size = sizeof(vm_instruction);
    char c;
    int i = 0;
    char line_buff[100];
    line_buff[0] = '\0';
    int line_len = 0;
    int should_parse = 0;

    while (1) {
        c = vm_code[i++];
        if (c == '/') {
            while ((vm_code[i++]) != '\n');
            line_buff[line_len] = '\0';
            should_parse = line_len;
        } else  if (c == '\n' || c == '\0') {
            line_buff[line_len] = '\0';
            should_parse = line_len;
        } else {
            line_buff[line_len++] = c;
        }
        if (should_parse) {
            if ((line_len = str_strip_spaces(line_buff))) {
                vm_instruction* vmi = malloc(vmi_size);
                set_instruction_fields(vmi, line_buff, line_len);
                list_add(l, new_list_node(vmi, vmi_size));
                free(vmi);
            }
            line_buff[0] = '\0';
            line_len = 0;
            should_parse = 0;
        }
        if (c == '\0') break;
    }
    return l;
}


void write_arithmetic(char* instr, FILE* dest)
{
    char* asm_code;
    if (strcmp(instr, "add") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D+M\n"
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "sub") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n"
                    "M=M-D\n"
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "neg") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "M=-D\n"; // negate
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "eq") == 0) {

        char* asm_code;

        int i_len = snprintf(NULL, 0, "%d", cmp_label_index);
        char* a_instr = malloc(i_len + 10);
        char* label = malloc(i_len + 11);
        snprintf(a_instr, i_len + 10, "@cmp_lbl_%d", cmp_label_index);
        snprintf(label, i_len + 11, "(cmp_lbl_%d)", cmp_label_index);

        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=D-M\n"
                    "M=0\n" // assume not equal ==> D!=0
                    "%s\n" // @cmp_lbl_%d
                    "D;JNE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "A=A-1\n"
                    "M=-1\n" // equal
                    "%s\n" // (cmp_lbl_%d)
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, asm_code, a_instr, label);
        cmp_label_index++;
    }

    if (strcmp(instr, "gt") == 0) {
        char* asm_code;

        int i_len = snprintf(NULL, 0, "%d", cmp_label_index);
        char* a_instr = malloc(i_len + 10);
        char* label = malloc(i_len + 11);
        snprintf(a_instr, i_len + 10, "@cmp_lbl_%d", cmp_label_index);
        snprintf(label, i_len + 11, "(cmp_lbl_%d)", cmp_label_index);

        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=M-D\n"
                    "M=0\n" // assume x<=y ==> D<=0
                    "%s\n" // @cmp_lbl_%d
                    "D;JLE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "A=A-1\n"
                    "M=-1\n" // equal
                    "%s\n" // (cmp_lbl_%d)
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, asm_code, a_instr, label);
        cmp_label_index++;
    }
    if (strcmp(instr, "lt") == 0) {
        char* asm_code;

        int i_len = snprintf(NULL, 0, "%d", cmp_label_index);
        char* a_instr = malloc(i_len + 10);
        char* label = malloc(i_len + 11);
        snprintf(a_instr, i_len + 10, "@cmp_lbl_%d", cmp_label_index);
        snprintf(label, i_len + 11, "(cmp_lbl_%d)", cmp_label_index);

        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=M-D\n"
                    "M=0\n" // assume x>=y ==> D>=0
                    "%s\n" // @cmp_lbl_%d
                    "D;JGE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "A=A-1\n"
                    "M=-1\n" // equal
                    "%s\n" // (cmp_lbl_%d)
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, asm_code, a_instr, label);
        cmp_label_index++;
    }
    if (strcmp(instr, "and") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D&M\n"
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "or") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D|M\n"
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "not") == 0) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // load first number
                    "M=!D\n"; // negate
        fprintf(dest, "%s", asm_code);
    }
    if (!asm_code) {
        printf("\n Error: invalid arithmetic instruction: %s", instr);
        exit(1);
    }
}

void write_push_pop (enum INSTRUCTION_TYPE i_type, char* segment, uint16_t index, FILE* dest)
{
     char* asm_code;
     int i_len = snprintf(NULL, 0, "%d", index);
     char* a_instr = malloc(i_len + 2);
     snprintf(a_instr, i_len + 2, "@%d", index);
    if (i_type == I_PUSH && strcmp(segment, "constant") == 0) {
        asm_code =  "D=A\n" // D now has constant `index`
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1\n";
        fprintf(dest, "%s\n%s", a_instr, asm_code);
    }

    free(a_instr);
    //  fprintf(dest, "%s", asm_code);
}

char* write_asm(list* instructions, char* vm_filename, FILE* dest)
{
    list_node* ln = instructions->head;
    while (ln != NULL) {
        vm_instruction* vmi = (vm_instruction*) ln->data;
        // fprintf(dest, "%s - %d\n", vmi->instruction_text, vmi->i_type);
        if (vmi->i_type == I_ARITHMETIC){
            write_arithmetic(vmi->instruction_text, dest);
        }
        if (vmi->i_type == I_PUSH || vmi->i_type == I_POP){
            write_push_pop(vmi->i_type, vmi->arg1, atoi(vmi->arg2), dest);
        }
        ln = ln->next;
    }
    return "asm";
}

char* read_file_full(char* filename)
{
    FILE *f = fopen(filename, "rb");
    struct stat st;
    stat(filename, &st);
    size_t fsize = (size_t) st.st_size;
    char *content = malloc(fsize + 1);
    size_t r_count = fread(content, 1, fsize, f);
    fclose(f);
    if (r_count != fsize) {
        printf("\nError while reading file: %s", filename);
        free(content);
        return NULL;
    }
    content[fsize] = 0;
    return content;
}

int is_valid_vm_file (char* filename, struct stat st)
{
    int is_reg_file = S_ISREG(st.st_mode);
    size_t len = strlen(filename);
    int is_valid = filename[len - 3] == '.' && filename[len - 2] == 'v' && filename[len - 1] == 'm'; // check that filename ends with .vm
    return is_reg_file && is_valid;
}

list* get_vm_filenames(char* filename)
{
    int file_no = 0;
    char** res;
    struct stat st;
    stat(filename, &st);
    int is_dir = S_ISDIR(st.st_mode);
    int is_vm_file = is_valid_vm_file(filename, st);

    if (!is_dir && !is_vm_file) {
        printf("\nError: path: %s is neither a vm file nor directory", filename);
        return NULL;
    }
    if (is_vm_file) {
        return new_list(new_list_node(filename, strlen(filename) + 1));
    }

    list* l = new_list(NULL);
    DIR *d;
    struct dirent *dir;

    struct stat f_st;
    d = opendir(filename);
    if (!d) return l;
    while ((dir = readdir(d)) != NULL) {
        char* f_name = (char *) malloc(strlen(dir->d_name) + strlen(filename) + 2);
        *f_name = 0;
        strcat(f_name, filename);
        strcat(f_name, "/");
        strcat(f_name, dir->d_name);

        stat(f_name, &f_st);
        if (is_valid_vm_file(f_name, f_st)) {
            list_add(l, new_list_node(f_name, strlen(f_name) + 1));
        }
        free(f_name);
    }
    closedir(d);
    return l;
}

char* extract_filename(char* path)
{
    int p_len = strlen(path);
    int f_len = 0;
    char temp[p_len + 1];
    temp[p_len] = '\0';

    for (int i = p_len - 1; i >= 0; i--) {
        if (path[i] == '/') {
            break;
        }
        temp[i] = path[i];
        f_len++;
    }
    if (!f_len) {
        return "";
    }
    char* str = malloc(f_len + 1);
    int offset = p_len - f_len;
    memcpy(str, temp + offset, f_len + 1);
    return str;
}

char* get_dest_filename(char* f)
{
    size_t ln = strlen(f);
    char* temp;
    if (strstr(f, ".vm") != NULL) {
        temp = malloc(ln + 2);
        strcpy(temp,f);
        memcpy(temp + ln - 3, ".asm", 5);
        return temp;
    }
    temp = malloc(ln + 5);
    strcpy(temp,f);
    strcat(temp, ".asm");
    return temp;
}

int main (int argc, char** argv)
{
    char* filename = *(argv + 1);
    if (filename == NULL) {
       return printf("\nError: Missing filename\n");
    }
    list* vm_filenames = get_vm_filenames(filename);
    if (vm_filenames == NULL) {
        return 0;
    }
    char* dest_filename = get_dest_filename(filename);
    FILE* fptr = fopen(dest_filename, "w");
    list_node* ln = vm_filenames->head;
    while (ln != NULL) {
        char* content = read_file_full((char *)ln->data);
        list* parsed_instructions = parse(content);
        write_asm(parsed_instructions, extract_filename((char *)ln->data), fptr);
        ln = ln->next;
    }
    fclose(fptr);
    return 0;
}
