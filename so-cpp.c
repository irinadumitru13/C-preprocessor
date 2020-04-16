#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "linked_list.h"
#include "parser.h"

#define BUFSIZE 256
#define MAPSIZE 16

int main(int argc, char *argv[])
{
    char buffer[BUFSIZE];
    struct map *m = create_map(MAPSIZE);
    int i;
    char **keys = NULL;
    FILE *f_in = stdin, *f_out = stdout;
    char **dir_name;
    int return_value = 0;
    int nr_out = 0, nr_in = 0, nr_dir = 0;
    struct linked_list *l;

    l = create_list();
    dir_name = malloc(2 * sizeof(char *));
    if (dir_name == NULL) {
        return_value = ERRORNO;
        goto exit;
    }

    return_value = parse_args(argc, argv, &f_in, &f_out, &m, &keys,
                                &dir_name, &nr_dir, &nr_out, &nr_in);
    if (return_value != 0)
         goto exit;

    while (fgets(buffer, BUFSIZE, f_in))
        if ((insert_at_end(&l, buffer)) == -ERRORNO) {
            return_value = -ERRORNO;
            goto exit;
        }

    fclose(f_in);

    return_value = include_dir(&l, dir_name, nr_dir);
    if (return_value == -ERRORNO)
        goto exit;

    return_value = ifdef_dir(&l, m);
    if (return_value == -ERRORNO)
        goto exit;

    return_value = define_dir(&l, &m, &keys);
    if (return_value == -ERRORNO)
        goto exit;

    return_value = modify_keys(&l, keys, m);
    if (return_value == -ERRORNO)
        goto exit;

    print_in_file(f_out, l);

exit:
    for (i = 0; i < m->count; i++)
        free(keys[i]);
    free(keys);
    fclose(f_out);

    for (i = 0; i < nr_dir; i++)
        free(dir_name[i]);
    free(dir_name);

    free_list(l);
    free_map(m);
    return return_value;
}
