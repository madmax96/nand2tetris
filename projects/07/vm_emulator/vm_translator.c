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
    return 1;
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

list* parse(char* vm_code)
{
   list* l = new_list(NULL);
   size_t vmi_size = sizeof(vm_instruction);
   vm_instruction* vmi = malloc(vmi_size);
   list_add(l, new_list_node(vmi, vmi_size));

   return l;
}

char* gen_asm(list* instructions, char* filename)
{
    list_node* ln = instructions->head;
    while (ln != NULL) {
        vm_instruction* vmi = (vm_instruction*) ln->data;
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

void merge_to_file(char** inputs, char* out_filename)
{

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
    list_node* ln = vm_filenames->head;
    while (ln != NULL) {
        char* content = read_file_full((char *)ln->data);
        char* asm_code = gen_asm(parse(content), extract_filename((char *)ln->data)); // put in list
        ln = ln->next;
    }
    // filenames.forEach(name=> getContent().then(parse).then(gen_asm))
    // merge into file
}
