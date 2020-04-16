#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

struct map *create_map(int size)
{
    unsigned int i;
    struct map *m;

    m = malloc(sizeof(struct map));
    if (m == NULL)
        return NULL;

    m->list = malloc(sizeof(struct node *) * size);
    if (m->list == NULL) {
        free(m);
        return NULL;
    }

    m->size = size;
    m->count = 0;

    for (i = 0; i < size; i++)
        m->list[i] = NULL;

    return m;
}

void free_elem(struct node *n)
{
    free(n->key);
    free(n->val);
    free(n);
}

void free_map(struct map *m)
{
    unsigned int i;
    struct node *curr_node, *next_node;

    for (i = 0; i < m->size; i++) {
        curr_node = m->list[i];
        while (curr_node != NULL) {
            next_node = curr_node->next;
            free_elem(curr_node);
            curr_node = next_node;
        }
    }

    free(m->list);
    free(m);
}

struct map *resize(struct map **m, int new_size)
{
    int i;
    struct map *new_map;
    struct node *curr_node;

    new_map = create_map(new_size);
    if (new_map == NULL)
        return NULL;

    for (i = 0; i < (*m)->size; i++) {
        curr_node = (*m)->list[i];

        while (curr_node != NULL) {
            if (put(&new_map, curr_node->key, curr_node->val) != 0) {
                free_map(new_map);
                return NULL;
            }
            curr_node = curr_node->next;
        }
    }

    free_map(*m);
    return new_map;
}

/* Hash function based on djb2 from Dan Bernstein
 * http://www.cse.yorku.ca/~oz/hash.html
 *
 * @return computed hash value
 */
unsigned int hash_fct(char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

int has_key(struct map *m, char *key)
{
    unsigned int pos = hash_fct(key) & (m->size - 1);
    struct node *list = m->list[pos];

    while (list != NULL) {
        if (strcmp(list->key, key) == 0)
            return 1;

        list = list->next;
    }

    return 0;
}

int update(struct map **m, char *key, char *val)
{
    unsigned int pos = hash_fct(key) & ((*m)->size - 1);
    struct node *list = (*m)->list[pos];

    while (list != NULL) {
        if (strcmp(list->key, key) == 0) {
            free(list->val);
            list->val = malloc((strlen(val) + 1) * sizeof(char));
            if (list->val == NULL)
                return -ERRORNO;
            memcpy(list->val, val, strlen(val));
            return 0;
        }

        list = list->next;
    }
    return 0;
}

int put(struct map **m, char *key, char *val)
{
    unsigned int pos = hash_fct(key) & ((*m)->size - 1);
    float load = 0.0;
    struct node *new_node;

    new_node = malloc(sizeof(struct node));
    if (new_node == NULL)
        return -ERRORNO;

    if (has_key(*m, key) == 1) {
        free(new_node);
        if (update(m, key, val) == ERRORNO)
            return -ERRORNO;
        return 0;
    }

    new_node->key = malloc((strlen(key) + 1) * sizeof(char));
    if (new_node->key == NULL) {
        free(new_node);
        return -ERRORNO;
    }
    new_node->val = malloc((strlen(val) + 1) * sizeof(char));
    if (new_node->val == NULL) {
        free(new_node);
        return -ERRORNO;
    }
    memcpy(new_node->key, key, strlen(key));
    memcpy(new_node->key + strlen(key), "\0", 1);
    memcpy(new_node->val, val, strlen(val));
    memcpy(new_node->val + strlen(val), "\0", 1);
    new_node->next = (*m)->list[pos];
    (*m)->list[pos] = new_node;
    (*m)->count++;
    load = (float) (*m)->count / (*m)->size;

    if (load >= TRESHOLD) {
        *m = resize(m, (*m)->size * 2);
        if (*m == NULL)
            return -ERRORNO;
    }

    return 0;
}

char *get(struct map *m, char *key)
{
    unsigned int pos = hash_fct(key) & (m->size - 1);
    struct node *curr_node = m->list[pos];

    while (curr_node != NULL) {
        if (strcmp(curr_node->key, key) == 0)
            return curr_node->val;

        curr_node = curr_node->next;
    }

    return NULL;
}

void remove_node(struct map **m, char *key)
{
    unsigned int pos = hash_fct(key) & ((*m)->size - 1);
    struct node *curr_node, *prev_node;

    prev_node = curr_node = (*m)->list[pos];

    while (curr_node != NULL) {
        if (strcmp(curr_node->key, key) == 0) {
            if (prev_node == curr_node)
                (*m)->list[pos] = curr_node->next;
            else
                prev_node->next = curr_node->next;
            free_elem(curr_node);
            (*m)->count--;
            return;
        }
        prev_node = curr_node;
        curr_node = curr_node->next;
    }
}
