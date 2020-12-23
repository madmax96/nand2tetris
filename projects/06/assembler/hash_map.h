#define HASHSIZE 101

typedef struct Node {
    struct Node* next;
    char* key;
    char* value;
} Node;

typedef Node** Map;

Map new_map (void);
char* map_get(Map map, char* key);
int map_add(Map map, char* key, char* value);
