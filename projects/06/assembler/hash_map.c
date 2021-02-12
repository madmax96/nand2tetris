#include <stdlib.h>
#include <string.h>
#include "hash_map.h"

static Node* new_node(char* key, char* value)
{
    Node* n = (Node*)malloc(sizeof(Node));
    n->next = NULL;
    // n->key = (char*) malloc(strlen(key) + 1);
    // n->value = (char*) malloc(strlen(value) + 1);
    // strcpy(n->key,key);
    // strcpy(n->value,value);
    n->key = key;
    n->value = value;
    return n;
}

static void replace_value(Node* n, char* value)
{
    // free(n->value);
    // n->value = malloc(strlen(value) + 1);
    // strcpy(n->value,value);
    n->value = value;
}

Map new_map (void)
{
    return (Map) malloc(sizeof(Node*) * HASHSIZE);
}

 /* hash: form hash value for string s */
static unsigned int hash(char *s)
{
    unsigned int hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = 31 * hashval + *s;
    }
    return hashval % HASHSIZE;
}

char* map_get(Map map, char* key)
{
    Node* head;
    char* found_value = NULL;
    for (head = map[hash(key)]; head != NULL; head = head->next) {
        if (strcmp(key,head->key) == 0) {
            found_value = malloc(strlen(head->value) + 1);
            return strcpy(found_value,head->value);
        }
    }
    return found_value;
}

int map_add(Map map, char* key, char* value)
{
    unsigned hashval = hash(key);
    Node* head = map[hashval];
    if (head == NULL) {
        map[hashval] = new_node(key,value);
        return 1;
    }
    while (head->next != NULL ) {
        if (strcmp(key,head->key) == 0) {
            replace_value(head,value);
            return 1;
        }
        head=head->next;
    };
    head->next = new_node(key,value);
    return 1;
}