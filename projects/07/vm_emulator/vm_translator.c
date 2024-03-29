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
enum MEMORY_SEGMENT { M_ARG, M_LOCAL, M_STATIC, M_POINTER, M_THIS, M_THAT, M_TEMP, M_CONSTANT };

int cmp_label_index = 0;
int ret_address_index = 0;
typedef struct vm_instruction {
    char* instruction_text;
    char* arg1;
    char* arg2;
    enum INSTRUCTION_TYPE i_type;
    enum MEMORY_SEGMENT m_segment;
} vm_instruction;

typedef struct list_node {
   void* data;
   struct list_node* next;
} list_node;

typedef struct list {
   list_node* head;
   size_t size;
} list;

list_node* new_list_node(void* data)
{
    list_node* ln = calloc(1, sizeof(list_node));
    ln->data = data;
    return ln;
}

list* new_list(list_node* head)
{
    list* l = calloc(1, sizeof(list));
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

void list_foreach(list* l, void (*fn)(void*, int))
{
    if (l == NULL) return;
    int i = 0;
    list_node *n = l->head;
    while (n) {
        fn(n->data, i++);
        n = n->next;
    }
}

void free_vm_instruction (void* i) {
    if (i == NULL) return;
    vm_instruction* vmi = (vm_instruction*) i;
    free(vmi->arg1);
    free(vmi->arg2);
    free(vmi->instruction_text);
    free(vmi);
}

void free_list_nodes(list_node* n, void (*free_data_fn)(void*))
{
    if (n == NULL) return;
    if (n->next) {
        free_list_nodes(n->next, free_data_fn);
    }
    free_data_fn(n->data);
    free(n);
}

void free_list(list* l, void (*free_ptr)(void*))
{
    if (l == NULL) {
        return;
    }

    list_node* head = l->head;
    if (head == NULL) {
        free(l);
        return;
    }
    free_list_nodes(head, free_ptr);
    free(l);
}

void print_list(list* l, void (*print_fn)(void *, int))
{
    list_foreach(l, print_fn);
}

void print_string(void *d, int i)
{
   printf("\n%d:  %s", i, (char *)d);
}

void print_vmi(void *d , int i)
{
    vm_instruction* vmi = (vm_instruction*)d;
    printf("\n");
    printf("%d:  instruction text --> %s\narg1 --> %s\narg2 --> %s\ntype -->  %d", i, vmi->instruction_text, vmi->arg1, vmi->arg2, vmi->i_type);
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
    vmi->instruction_text = calloc(1, instr_length + 1);
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
        vmi->arg1 = calloc(1, strlen(arg1) + 1);
        strcpy(vmi->arg1, arg1);
        if (vmi->i_type == I_PUSH || vmi->i_type == I_POP) {
            if (strcmp(arg1,"constant") == 0) {
                vmi->m_segment = M_CONSTANT;
            }
            if (strcmp(arg1,"argument") == 0) {
                vmi->m_segment = M_ARG;
            }
            if (strcmp(arg1,"local") == 0) {
                vmi->m_segment = M_LOCAL;
            }
            if (strcmp(arg1,"static") == 0) {
                vmi->m_segment = M_STATIC;
            }
            if (strcmp(arg1,"pointer") == 0) {
                vmi->m_segment = M_POINTER;
            }
            if (strcmp(arg1,"this") == 0) {
                vmi->m_segment = M_THIS;
            }
            if (strcmp(arg1,"that") == 0) {
                vmi->m_segment = M_THAT;
            }
            if (strcmp(arg1,"temp") == 0) {
                vmi->m_segment = M_TEMP;
            }
        }
    }

    char* arg2 = strtok(NULL, " ");
     if (arg2) {
        vmi->arg2 = calloc(1, strlen(arg2) + 1);
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
                vm_instruction* vmi = calloc(1, vmi_size);
                set_instruction_fields(vmi, line_buff, line_len);
                list_add(l, new_list_node(vmi));
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
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D+M\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "sub") == 0) {
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n"
                    "M=M-D\n";
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
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=D-M\n"
                    "M=0\n" // assume not equal ==> D!=0
                    "@cmp_lbl_%d\n"
                    "D;JNE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "M=-1\n" // equal
                    "(cmp_lbl_%d)\n";
        fprintf(dest, asm_code, cmp_label_index, cmp_label_index);
        cmp_label_index++;
    }

    if (strcmp(instr, "gt") == 0) {
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=M-D\n"
                    "M=0\n" // assume x<=y ==> D<=0
                    "@cmp_lbl_%d\n"
                    "D;JLE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "M=-1\n" // equal
                    "(cmp_lbl_%d)\n";
        fprintf(dest, asm_code, cmp_label_index, cmp_label_index);
        cmp_label_index++;
    }
    if (strcmp(instr, "lt") == 0) {
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "D=M-D\n"
                    "M=0\n" // assume x>=y ==> D>=0
                    "@cmp_lbl_%d\n"
                    "D;JGE\n"
                    "@SP\n"
                    "A=M-1\n"
                    "M=-1\n" // equal
                    "(cmp_lbl_%d)\n";
        fprintf(dest, asm_code, cmp_label_index, cmp_label_index);
        cmp_label_index++;
    }
    if (strcmp(instr, "and") == 0) {
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D&M\n";
        fprintf(dest, "%s", asm_code);
    }
    if (strcmp(instr, "or") == 0) {
        asm_code =  "@SP\n"
                    "AM=M-1\n"
                    "D=M\n" // load first number
                    "A=A-1\n" // A is now addres of second number
                    "M=D|M\n";
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

char* get_segment_pointer (enum MEMORY_SEGMENT m_segment)
{
    switch (m_segment) {
        case M_ARG:
             return "ARG";
        case M_LOCAL:
             return "LCL";
        case M_THIS:
           return "THIS";
        case M_THAT:
             return "THAT";
        default:
            return NULL;
    }
}

int get_segment_start_address(enum MEMORY_SEGMENT m_segment)
{
    switch (m_segment) {
        case M_TEMP:
            return 5;
        case M_POINTER:
            return 3;
        default:
            return -1;
    }
}

void write_push_pop (enum INSTRUCTION_TYPE i_type, enum MEMORY_SEGMENT m_segment, uint16_t index, char* vm_filename, FILE* dest)
{
    char* asm_code;
    char* segment_p = get_segment_pointer(m_segment);
    int final_address = index + get_segment_start_address(m_segment);

    if (m_segment == M_CONSTANT) { // PUSH contant {number}
        asm_code =  "@%d\n"
                    "D=A\n" // D now has constant `index`
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1\n";
        fprintf(dest, asm_code, index);
        return;
    }
    int is_pointer_segment = !(m_segment == M_POINTER || m_segment == M_TEMP);

    if (i_type == I_PUSH && is_pointer_segment) {
         if (m_segment == M_STATIC) {
            asm_code =  "@%s_static_%d\n" // get final address
                        "D=M\n" // data to push to stack
                        "@SP\n"
                        "A=M\n"
                        "M=D\n"
                        "@SP\n"
                        "M=M+1\n";
            fprintf(dest, asm_code, vm_filename, index);
            return;
        }
        asm_code =  "@%s\n"
                    "D=M\n" // store base address
                    "@%d\n" // load index
                    "A=D+A\n" // get final address
                    "D=M\n" // data to push to stack
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1\n";
        fprintf(dest, asm_code, segment_p, index);
    }

    if (i_type == I_PUSH && !is_pointer_segment) {
        asm_code =  "@%d\n" // get final address
                    "D=M\n" // data to push to stack
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1\n";
        fprintf(dest, asm_code, final_address);
    }

    if (i_type == I_POP && is_pointer_segment) {
        if (m_segment == M_STATIC) {
            asm_code =  "@SP\n"
                        "A=M-1\n"
                        "D=M\n" // data to pop
                        "@%s_static_%d\n"
                        "M=D\n" // put data in desired address
                        "@SP\n"
                        "M=M-1\n";
            fprintf(dest, asm_code, vm_filename, index);
            return;
        }
        asm_code =  "@%s\n"
                    "D=M\n" // store base address
                    "@%d\n" // load index
                    "D=D+A\n" // get final address
                    "@14\n"
                    "M=D\n" // store final address in general purpose register to free reg D
                    "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // data to pop
                    "@14\n"
                    "A=M\n"
                    "M=D\n" // put data in desired address
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, asm_code, segment_p, index);
    }

    if (i_type == I_POP && !is_pointer_segment) {
        asm_code =  "@SP\n"
                    "A=M-1\n"
                    "D=M\n" // data to pop
                    "@%d\n"
                    "M=D\n" // put data in desired address
                    "@SP\n"
                    "M=M-1\n";
        fprintf(dest, asm_code, final_address);
    }
}

void write_program_flow (enum INSTRUCTION_TYPE i_type, char* label_name, char* func_name, FILE* dest)
{
    char* asm_code = NULL;
    if (!func_name) {
       func_name = "f";
    }
    switch (i_type) {
        case I_LABEL:
            asm_code = "(%s$%s)\n";
            fprintf(dest, asm_code, func_name, label_name);
            break;
        case I_GOTO:
            asm_code =  "@%s$%s\n"
                        "0;JMP\n";
            fprintf(dest, asm_code, func_name, label_name);
            break;
        case I_IF:
            asm_code =  "@SP\n"
                        "AM=M-1\n"
                        "D=M\n"
                        "@%s$%s\n"
                        "D;JNE\n";
            fprintf(dest, asm_code, func_name, label_name);
            break;
        default:
            printf("\ninvalid program flow instruction");
            exit(1);
    }
}

void write_function(char* name, int var_count, FILE* dest)
{
  char* asm_code =  "@SP\n"
                    "A=M\n"
                    "M=0\n"
                    "@SP\n"
                    "M=M+1\n";

  fprintf(dest, "(%s)\n", name);
  for (int i = var_count; i > 0; i--) {
      fprintf(dest, "%s", asm_code);
  }
}

void write_call(char* f, int arg_count, FILE* dest)
{
    char* asm_code= "@ret_address_%d\n"
                    "D=A\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D // push RETURN address\n"
                    "@SP\n"
                    "M=M+1 // inc SP\n"

                    "@LCL\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D // push LCL\n"
                    "@SP\n"
                    "M=M+1 // inc SP\n"

                    "@ARG\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D // push ARG\n"
                    "@SP\n"
                    "M=M+1 // inc SP\n"

                    "@THIS\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D // push THIS\n"
                    "@SP\n"
                    "M=M+1 // inc SP\n"

                    "@THAT\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D // push THAT\n"
                    "@SP\n"
                    "MD=M+1 // inc SP\n"

                    "@LCL\n"
                    "M=D // LCL=SP\n"

                    "@5\n"
                    "D=D-A // SP-=5\n"
                    "@%d // number of args\n"
                    "D=D-A // SP-=arg_count\n"
                    "@ARG\n"
                    "M=D // ARG = SP-5-arg_count\n"

                    "@%s\n"
                    "0;JMP\n"
                    "(ret_address_%d)\n";

    fprintf(dest, asm_code, ret_address_index, arg_count, f, ret_address_index);
    ret_address_index++;
}

void write_return(FILE* dest)
{
    char* asm_code= "@5\n"
                    "D=A\n"
                    "@LCL\n"
                    "A=M-D\n"
                    "D=M // D=return_address\n"
                    "@R13\n"
                    "M=D // saving return_address\n"

                    "@SP\n"
                    "A=M-1\n"
                    "D=M\n"
                    "@ARG\n"
                    "A=M\n"
                    "M=D // *ARG = *(SP - 1)\n"

                    "@ARG\n"
                    "D=M\n"
                    "@SP\n"
                    "M=D+1 // SP=ARG+1\n"

                    "@LCL\n"
                    "AM=M-1\n"
                    "D=M\n"
                    "@THAT\n"
                    "M=D // restoring THAT\n"

                    "@LCL\n"
                    "AM=M-1\n"
                    "D=M\n"
                    "@THIS\n"
                    "M=D // restoring THIS\n"

                    "@LCL\n"
                    "AM=M-1\n"
                    "D=M\n"
                    "@ARG\n"
                    "M=D // restoring ARG\n"

                    "@LCL\n"
                    "A=M-1\n"
                    "D=M\n"
                    "@LCL\n"
                    "M=D // restoring LCL\n"

                    "@R13\n"
                    "A=M\n"
                    "0;JMP\n";

    fprintf(dest,"%s", asm_code);
}

void write_asm(list* instructions, char* vm_filename, FILE* dest)
{
    list_node* ln = instructions->head;
    char* current_function = NULL;
    while (ln != NULL) {
        vm_instruction* vmi = (vm_instruction*) ln->data;
        switch (vmi->i_type) {
            case I_ARITHMETIC:
                write_arithmetic(vmi->instruction_text, dest);
                break;
            case I_PUSH:
            case I_POP:
                write_push_pop(vmi->i_type, vmi->m_segment, atoi(vmi->arg2), vm_filename, dest);
                break;
            case I_LABEL:
            case I_IF:
            case I_GOTO:
                write_program_flow(vmi->i_type, vmi->arg1, current_function, dest);
                break;
            case I_FUNCTION:
                current_function = vmi->arg1;
                write_function(current_function, atoi(vmi->arg2), dest);
                break;
            case I_CALL:
                write_call(vmi->arg1, atoi(vmi->arg2), dest);
                break;
            case I_RETURN:
                write_return(dest);
                break;
            default:
                printf("\nunsupported instruction type");
                exit(1);
        }
        ln = ln->next;
    }
}

char* read_file_full(char* filename)
{
    FILE *f = fopen(filename, "rb");
    struct stat st;
    stat(filename, &st);
    size_t fsize = (size_t) st.st_size;
    char *content = calloc(1, fsize + 1);
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
    struct stat st;
    stat(filename, &st);
    int is_dir = S_ISDIR(st.st_mode);
    int is_vm_file = is_valid_vm_file(filename, st);

    if (!is_dir && !is_vm_file) {
        printf("\nError: path: %s is neither a vm file nor directory", filename);
        return NULL;
    }
    if (is_vm_file) {
        char* t = calloc(strlen(filename) + 1, 1);
        strcpy(t,filename);
        return new_list(new_list_node(t));
    }

    list* l = new_list(NULL);
    DIR *d;
    struct dirent *dir;

    struct stat f_st;
    d = opendir(filename);
    if (!d) return l;
    while ((dir = readdir(d)) != NULL) {
        char* f_name = calloc(1, strlen(dir->d_name) + strlen(filename) + 2);
        strcat(f_name, filename);
        strcat(f_name, "/");
        strcat(f_name, dir->d_name);

        stat(f_name, &f_st);
        if (is_valid_vm_file(f_name, f_st)) {
            list_add(l, new_list_node(f_name));
        }
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
    char* str = calloc(1, f_len + 1);
    int offset = p_len - f_len;
    memcpy(str, temp + offset, f_len + 1);
    return str;
}

char* get_dest_filename(char* f)
{
    size_t ln = strlen(f);
    char* temp;
    if (strstr(f, ".vm") != NULL) {
        temp = calloc(1, ln + 2);
        strcpy(temp,f);
        memcpy(temp + ln - 3, ".asm", 5);
        return temp;
    }
    temp = calloc(1, ln + 5);
    strcpy(temp,f);
    strcat(temp, ".asm");
    return temp;
}

void write_bootstrap(FILE* dest)
{
    char* asm_code =  "@256\n"
                      "D=A\n"
                      "@SP\n"
                      "M=D\n";
    fprintf(dest, "%s", asm_code);
    write_call("Sys.init", 0, dest);
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
    print_list(vm_filenames, print_string);
    char* dest_filename = get_dest_filename(filename);
    FILE* fptr = fopen(dest_filename, "w");
    free(dest_filename);
    list_node* ln = vm_filenames->head;
    write_bootstrap(fptr);
    while (ln != NULL) {
        char* content = read_file_full((char *)ln->data);
        list* parsed_instructions = parse(content);
        free(content);
        char* extracted_filename = extract_filename((char *)ln->data);
        write_asm(parsed_instructions, extracted_filename, fptr);
        free(extracted_filename);
        // print_list(parsed_instructions, print_vmi);
        free_list(parsed_instructions, free_vm_instruction);
        ln = ln->next;
    }
    free_list(vm_filenames, free);
    fclose(fptr);
    return 0;
}
