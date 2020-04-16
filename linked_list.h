#ifndef LinkedList
#define LinkedList

#define ERRORNO -12

struct elem {
    char *data;
    struct elem *next;
};

struct linked_list {
    struct elem *start;
    int count;
};

struct linked_list *create_list();

int insert_at_pos(struct linked_list **l, int pos, char *data_tba);

int insert_at_end(struct linked_list **l, char *data);

struct elem *delete_from_pos(struct linked_list **l, int pos);

void free_list(struct linked_list *l);

#endif
