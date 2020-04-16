#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

struct linked_list *create_list(void)
{
    struct linked_list *l = malloc(sizeof(struct linked_list));

    if (l == NULL)
        return NULL;
    l->start = NULL;
    l->count = 0;
    return l;
}

int insert_at_pos(struct linked_list **l, int pos, char *data_tba)
{
    int i;
    struct elem *new_elem = malloc(sizeof(struct elem));
    struct elem *prev, *curr;

    if (new_elem == NULL)
        return -ERRORNO;

    new_elem->data = malloc((strlen(data_tba) + 1) * sizeof(char));
    if (new_elem->data == NULL) {
        free(new_elem);
        return -ERRORNO;
    }
    memcpy(new_elem->data, data_tba, strlen(data_tba));
    memcpy(new_elem->data + strlen(data_tba), "\0", 1);

    prev = curr = (*l)->start;

    for (i = 0; i < (*l)->count; i++) {
        if (i == pos) {
            curr = curr->next;
            break;
        }
        prev = prev->next;
        curr = curr->next;
    }

    new_elem->next = curr;
    prev->next = new_elem;
    (*l)->count++;

    return 0;
}

int insert_at_end(struct linked_list **l, char *data)
{
    struct elem *new_elem = malloc(sizeof(struct elem));
    struct elem *temp;

    if (new_elem == NULL)
        return -ERRORNO;

    new_elem->data = malloc((strlen(data) + 1) * sizeof(char));
    if (new_elem->data == NULL) {
        free(new_elem);
        return -ERRORNO;
    }
    memcpy(new_elem->data, data, strlen(data));
    memcpy(new_elem->data + strlen(data), "\0", 1);

    (*l)->count++;

    if ((*l)->start == NULL) {
        (*l)->start = new_elem;
        new_elem->next = NULL;
        return 0;
    }

    temp = (*l)->start;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new_elem;
    new_elem->next = NULL;

    return 0;
}

void free_list(struct linked_list *l)
{
    struct elem *curr_elem, *next_elem;

    curr_elem = l->start;

    while (curr_elem != NULL) {
        next_elem = curr_elem->next;
        free(curr_elem->data);
        free(curr_elem);
        curr_elem = next_elem;
    }

    free(l);
}

void delete_from_beginning(struct linked_list **l)
{
    struct elem *temp;

    if ((*l)->start == NULL)
        return;

    temp = (*l)->start->next;
    free((*l)->start->data);
    free((*l)->start);
    (*l)->start = temp;
    (*l)->count--;

    return;
}

struct elem *delete_from_pos(struct linked_list **l, int pos)
{
    struct elem *curr = (*l)->start, *prev = (*l)->start, *next;
    int i;

    if (pos == 0) {
        curr = prev->next;
        free(prev->data);
        free(prev);
        (*l)->start = curr;
        (*l)->count--;
        return (*l)->start;
    }

    for (i = 0; i < (*l)->count; i++) {
        if (i == pos - 1) {
            curr = curr->next;
            break;
        }
        prev = prev->next;
        curr = curr->next;
    }

    next = curr->next;
    prev->next = next;
    free(curr->data);
    free(curr);
    (*l)->count--;

    return prev;
}
