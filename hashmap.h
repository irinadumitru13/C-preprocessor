#ifndef HashMap
#define HashMap

#define TRESHOLD 0.75
#define ERRORNO -12

struct node {
    char *key;
    char *val;
    struct node *next;
};

struct map {
    unsigned int size;
    unsigned int count;
    struct node **list;
};

struct map *create_map(int size);

void free_elem(struct node *n);

void free_map(struct map *m);

struct map *resize(struct map **m, int new_size);

unsigned int hash_fct(char *str);

int has_key(struct map *m, char *key);

int update(struct map **m, char *key, char *val);

int put(struct map **m, char *key, char *val);

char *get(struct map *m, char *key);

void remove_node(struct map **m, char *key);

#endif
