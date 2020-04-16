#ifndef Parser
#define Parser

#define BUFSIZE 256

int parse_args(int argc, char *argv[], FILE **f_in, FILE **f_out,
                struct map **m, char ***keys, char ***dir_name,
                int *nr_dir, int *nr_out, int *nr_in)
{
    int i;
    char *key, *val, *arg, *token;

    if (argc != 1) {
        for (i = 1; i < argc; i++) {
            if (strncmp(argv[i], "-D", 2) == 0) {
                if (strlen(argv[i]) > 2) {
                    arg = malloc((strlen(argv[i]) - 1) * sizeof(char));
                    if (arg == NULL)
                        return -ERRORNO;

                    memcpy(arg, argv[i] + 2, strlen(argv[i]) - 2);
                    memcpy(arg + strlen(argv[i]) - 2, "\0", 1);
                } else {
                    i += 1;

                    arg = malloc((strlen(argv[i]) + 1) * sizeof(char));
                    if (arg == NULL)
                        return -ERRORNO;

                    memcpy(arg, argv[i], strlen(argv[i]));
                    memcpy(arg + strlen(argv[i]), "\0", 1);
                }

                token = strtok(arg, (const char *)"=");
                key = malloc((strlen(token) + 1) * sizeof(char));
                if (key == NULL) {
                    free(arg);
                    return -ERRORNO;
                }
                memcpy(key, token, strlen(token));
                memcpy(key + strlen(token), "\0", 1);

                token = strtok(NULL, (const char *)"=");
                if (token != NULL) {
                    val = malloc((strlen(token) + 1) * sizeof(char));
                    if (val == NULL) {
                        free(arg);
                        free(key);
                        return -ERRORNO;
                    }
                    memcpy(val, token, strlen(token));
                    memcpy(val + strlen(token), "\0", 1);
                    put(m, key, val);

                    free(val);
                } else {
                    put(m, key, "");
                }

                *keys = realloc(*keys, (*m)->count * sizeof(char *));
                if (*keys == NULL) {
                    free(key);
                    free(val);
                    return -ERRORNO;
                }

                (*keys)[(*m)->count - 1] = malloc((strlen(key) + 1)
                                                    * sizeof(char));
                if ((*keys)[(*m)->count - 1] == NULL) {
                    free(key);
                    free(arg);
                    return -ERRORNO;
                }
                memcpy((*keys)[(*m)->count - 1], key, strlen(key));
                memcpy((*keys)[(*m)->count - 1] + strlen(key), "\0", 1);

                free(key);
                free(arg);
            } else if (strncmp(argv[i], "-I", 2) == 0) {
                if (strlen(argv[i]) > 2) {
                    (*dir_name)[*nr_dir] = malloc((strlen(argv[i]) - 1)
                                                    * sizeof(char));
                    if ((*dir_name)[*nr_dir] == NULL)
                        return -ERRORNO;

                    memcpy((*dir_name)[*nr_dir], argv[i] + 2,
                            strlen(argv[i]) - 2);
                    memcpy((*dir_name)[*nr_dir] + (strlen(argv[i]) - 2),
                            "\0", 1);
                } else {
                    i += 1;
                    (*dir_name)[*nr_dir] = malloc((strlen(argv[i]) + 1)
                                                    * sizeof(char));
                    if ((*dir_name)[*nr_dir] == NULL)
                        return -ERRORNO;

                    memcpy((*dir_name)[*nr_dir], argv[i], strlen(argv[i]));
                    memcpy((*dir_name)[*nr_dir] + strlen(argv[i]), "\0", 1);
                }
                *nr_dir += 1;

            } else if (strncmp(argv[i], "-o", 2) == 0) {
                if (*nr_out == 0) {
                    *nr_out = 1;
                    if (strlen(argv[i]) > 2) {
                        arg = malloc((strlen(argv[i]) - 1) * sizeof(char));
                        if (arg == NULL)
                            return -ERRORNO;

                        memcpy(arg, argv[i] + 2, strlen(argv[i]) - 2);
                        memcpy(arg + strlen(argv[i]) - 2, "\0", 1);
                    } else {
                        i += 1;

                        arg = malloc((strlen(argv[i]) + 1) * sizeof(char));
                        if (arg == NULL)
                            return -ERRORNO;

                        memcpy(arg, argv[i], strlen(argv[i]));
                        memcpy(arg + strlen(argv[i]), "\0", 1);
                    }

                    *f_out = fopen(arg, "w");
                    if (*f_out == NULL) {
                        printf("Failed to open output file\n");
                        return -1;
                    }
                    free(arg);
                } else {
                    printf("Too many output files\n");
                    return -1;
                }
            } else if (strncmp(argv[i], "-", 1) == 0) {
                printf("Bad argument\n");
                return -1;
            } else {
                if (*nr_in == 0) {
                    *f_in = fopen(argv[i], "r");
                    if (*f_in == NULL) {
                        printf("Failed to open input file\n");
                        return -1;
                    }
                    *nr_in += 1;
                } else if (*nr_out == 0) {
                    *f_out = fopen(argv[i], "w");
                    if (*f_out == NULL) {
                        printf("Failed to open output file\n");
                        return -1;
                    }
                    *nr_out += 1;
                } else {
                    printf("Too many input files\n");
                    return -1;
                }
            }
        }
    }
    return 0;
}

int include_dir(struct linked_list **l, char **dir_name, int nr_dir)
{
    struct elem *elem = (*l)->start;
    int i, j, nr_rep;
    char *data, *temp, *sub_file, *path_file, *token;
    FILE *f_inc;
    char buffer[BUFSIZE];

    for (i = 0; i < (*l)->count; i++) {
        data = malloc((strlen(elem->data) + 1) * sizeof(char));
        if (data == NULL)
            return -ERRORNO;

        memcpy(data, elem->data, strlen(elem->data));
        memcpy(data + strlen(elem->data), "\0", 1);

        if (strncmp(data, "#include", 8) == 0) {
            temp = malloc((strlen(data) + 1) * sizeof(char));
            if (temp == NULL) {
                free(data);
                return -ERRORNO;
            }
            memcpy(temp, data, strlen(data));
            memcpy(temp + strlen(data), "\0", 1);

            token = strtok(temp, "#\" ");
            token = strtok(NULL, "#\" ");

            sub_file = malloc((strlen(token) + 1) * sizeof(char));
            if (sub_file == NULL) {
                free(data);
                free(temp);
                return -ERRORNO;
            }
            memcpy(sub_file, token, strlen(token));
            memcpy(sub_file + strlen(token), "\0", 1);

            path_file = malloc((strlen(sub_file) + 1) * sizeof(char));
            if (path_file == NULL) {
                free(data);
                free(temp);
                free(sub_file);
                return -ERRORNO;
            }
            memcpy(path_file, sub_file, strlen(sub_file));
            memcpy(path_file + strlen(sub_file), "\0", 1);

            f_inc = fopen(sub_file, "r");
            if (f_inc == NULL) {
                for (j = 0; j < nr_dir; j++) {
                    free(path_file);
                    path_file = malloc((strlen(dir_name[j])
                                        + strlen(sub_file) + 2)
                                            * sizeof(char));
                    if (path_file == NULL) {
                        free(data);
                        free(temp);
                        free(sub_file);
                        return -ERRORNO;
                    }
                    memcpy(path_file, dir_name[j], strlen(dir_name[j]));
                    memcpy(path_file + strlen(dir_name[j]), "/", 1);
                    memcpy(path_file + strlen(dir_name[j]) + 1,
                            sub_file, strlen(sub_file) + 1);

                    f_inc = fopen(path_file, "r");
                    if (f_inc != NULL)
                        break;

                }

                if (f_inc == NULL) {
                    free(path_file);
                    free(sub_file);
                    free(data);
                    free(temp);
                    printf("Error opening file\n");
                    return -1;
                }
            }

            nr_rep = i;
            while (fgets(buffer, BUFSIZE, f_inc))
                if (insert_at_pos(l, i, buffer) == ERRORNO)
                    return -ERRORNO;
                else
                    i += 1;

            i = nr_rep;
            elem = delete_from_pos(l, i);

            free(temp);
            free(path_file);
            free(sub_file);
            fclose(f_inc);
        }
        free(data);
        elem = elem->next;
    }
    return 0;
}

int define_dir(struct linked_list **l, struct map **m, char ***keys)
{
    char *data, *temp, *token, *m_val, *key, *val, *new_val;
    int i, j, len;
    struct elem *elem = (*l)->start;

    for (i = 0; i < (*l)->count; i++) {
        data = malloc((strlen(elem->data) + 1) * sizeof(char));
        if (data == NULL)
            return -ERRORNO;

        memcpy(data, elem->data, strlen(elem->data));
        memcpy(data + strlen(elem->data), "\0", 1);

        if (strncmp(data, "#define", 7) == 0) {
            temp = malloc((strlen(data) + 1) * sizeof(char));
            if (temp == NULL) {
                free(data);
                return -ERRORNO;
            }
            memcpy(temp, data, strlen(data));
            memcpy(temp + strlen(data), "\0", 1);

            token = strtok(temp, (const char *) "\n ");
            token = strtok(NULL, (const char *) "\n ");
            key = malloc((strlen(token) + 1) * sizeof(char));
            if (key == NULL) {
                free(temp);
                free(data);
                return -ERRORNO;
            }
            memcpy(key, token, strlen(token));
            memcpy(key + strlen(token), "\0", 1);

            token = strtok(NULL, (const char *) "\n");
            if (token == NULL) {
                val = malloc(sizeof(char));
                if (val == NULL) {
                    free(key);
                    free(temp);
                    free(data);
                }
                memcpy(val, "\0", 1);
            } else {
                val = malloc((strlen(token) + 1) * sizeof(char));
                if (val == NULL) {
                    free(key);
                    free(temp);
                    free(data);
                    return -ERRORNO;
                }
                memcpy(val, token, strlen(token));
                memcpy(val + strlen(token), "\0", 1);

                for (j = 0; j < (*m)->count; j++) {
                    token = strstr(val, (*keys)[j]);
                    if (token != NULL) {
                        m_val = get(*m, (*keys)[j]);
                        len = strlen(val) - strlen((*keys)[j])
                                + strlen(m_val) + 1;
                        new_val = malloc(len * sizeof(char));
                        if (new_val == NULL) {
                            free(data);
                            free(key);
                            free(val);
                            free(temp);
                            return -ERRORNO;
                        }

                        if (token == val) {
                            memcpy(new_val, m_val, strlen(m_val) + 1);
                            memcpy(new_val + strlen(m_val) - 1,
                                    val + strlen((*keys)[j]),
                                   strlen(val) - strlen((*keys)[j]) + 1);
                        } else {
                            memcpy(new_val, val, token - val);
                            memcpy(new_val + (token - val), m_val,
                                    strlen(m_val) + 1);
                            memcpy(new_val + (token - val) + strlen(m_val) - 1,
                                   token + strlen((*keys)[j]),
                                   strlen(token) - strlen((*keys)[j]) + 1);
                        }

                        free(val);
                        val = malloc((strlen(new_val) + 1) * sizeof(char));
                        if (val == NULL) {
                            free(new_val);
                            free(key);
                            free(temp);
                            free(data);
                            return -ERRORNO;
                        }
                        memcpy(val, new_val, strlen(new_val));
                        memcpy(val + strlen(new_val), "\0", 1);
                        free(new_val);
                        break;
                    }
                }
            }

            free(temp);

            if (has_key(*m, key) == 0) {
                *keys = realloc(*keys, ((*m)->count + 1) * sizeof(char *));
                if (*keys == NULL) {
                    free(data);
                    free(key);
                    free(val);
                    return -ERRORNO;
                }

                (*keys)[(*m)->count] = malloc((strlen(key) + 1) * sizeof(char));
                if ((*keys)[(*m)->count] == NULL) {
                    free(data);
                    free(key);
                    free(val);
                }
                memcpy((*keys)[(*m)->count], key, strlen(key));
                memcpy((*keys)[(*m)->count] + strlen(key), "\0", 1);
            }

            if (put(m, key, val) == -ERRORNO) {
                free(data);
                free(key);
                free(val);
                free(temp);
                return -ERRORNO;
            }

            free(key);
            free(val);
        }

        free(data);
        elem = elem->next;
    }

    return 0;
}

int modify_keys(struct linked_list **l, char **keys, struct map *m)
{
    int i, j, len;
    char *f, *s, *m_val, *data, *new_val;
    struct elem *elem = (*l)->start;

    for (i = 0; i < (*l)->count; i++) {
        data = malloc((strlen(elem->data) + 1) * sizeof(char));
        if (data == NULL)
            return -ERRORNO;

        memcpy(data, elem->data, strlen(elem->data));
        memcpy(data + strlen(elem->data), "\0", 1);

        if (strncmp(data, "#", 1) == 0) {
            free(data);
            elem = elem->next;
            continue;
        } else {
            for (j = 0; j < m->count; j++) {
                f = strstr(data, keys[j]);
                if (f != NULL) {
                    m_val = get(m, keys[j]);
                    len = strlen(data) - strlen(keys[j]) + strlen(m_val) + 1;

                    new_val = malloc(len * sizeof(char));
                    if (new_val == NULL) {
                        free(m_val);
                        free(data);
                        return -ERRORNO;
                    }

                    if (strstr(data, "printf") != NULL) {
                        s = strstr(f + strlen(keys[j]), keys[j]);
                        if (s != NULL) {
                            memcpy(new_val, data, s - data);
                            memcpy(new_val + (s - data), m_val,
                                    strlen(m_val) + 1);
                            memcpy(new_val + (s - data) + strlen(m_val),
                                  s + strlen(keys[j]),
                                  strlen(s) - strlen(keys[j]) + 1);
                        } else {
                            memcpy(new_val, data, f - data);
                            memcpy(new_val + (f - data), m_val,
                                    strlen(m_val) + 1);
                            memcpy(new_val + (f - data) + strlen(m_val),
                                    f + strlen(keys[j]),
                                    strlen(f) - strlen(keys[j]) + 1);
                        }
                    } else {
                        memcpy(new_val, data, f - data);
                        memcpy(new_val + (f - data), m_val, strlen(m_val) + 1);
                        memcpy(new_val + (f - data) + strlen(m_val),
                                f + strlen(keys[j]),
                                strlen(f) - strlen(keys[j]) + 1);
                    }

                    free(elem->data);
                    elem->data = malloc((strlen(new_val) + 1) * sizeof(char));
                    if (elem->data == NULL) {
                        free(data);
                        free(new_val);
                        return -ERRORNO;
                    }
                    memcpy(elem->data, new_val, strlen(new_val));
                    memcpy(elem->data + strlen(new_val), "\0", 1);

                    free(new_val);
                }
            }
        }
        free(data);
        elem = elem->next;
    }

    return 0;
}

int ifdef_dir(struct linked_list **l, struct map *m) {
    struct elem *elem = (*l)->start, *curr;
    int i, nr_rep;
    char *data, *temp, *key, *token;

    for (i = 0; i < (*l)->count; i++) {
        data = malloc((strlen(elem->data) + 1) * sizeof(char));
        if (data == NULL)
            return -ERRORNO;

        memcpy(data, elem->data, strlen(elem->data));
        memcpy(data + strlen(elem->data), "\0", 1);
        if (strncmp(data, "#ifdef", 6) == 0) {
            temp = malloc((strlen(data) + 1) * sizeof(char));
            if (temp == NULL) {
                free(data);
                return -ERRORNO;
            }
            memcpy(temp, data, strlen(data));
            memcpy(temp + strlen(data), "\0", 1);

            token = strtok(temp, " \n");
            token = strtok(NULL, " \n");

            key = malloc(strlen(token) * sizeof(char));
            memcpy(key, token, strlen(token) - 1);
            memcpy(key + strlen(token) - 1, "\0", 1);

            if (has_key(m, key) == 1) {
                elem = delete_from_pos(l, i);
                nr_rep = i + 1;
                curr = (elem->next)->next;
                while (strncmp(curr->data, "#endif", 6) != 0) {
                    curr = delete_from_pos(l, nr_rep);
                    curr = curr->next;
                }
                curr = delete_from_pos(l, nr_rep);
            } else {
                elem = delete_from_pos(l, i);
                elem = delete_from_pos(l, i);

                if (strncmp(elem->next->data, "#else", 5) == 0) {
                    elem = delete_from_pos(l, i);

                    nr_rep = i;
                    curr = elem;
                    elem = elem->next;

                    while (strncmp(elem->data, "#endif", 6) != 0) {
                        elem = elem->next;
                        i += 1;
                    }

                    elem = delete_from_pos(l, i);

                    elem = curr;
                }
            }

            free(key);
            free(temp);
        }

        free(data);
        elem = elem->next;
    }

    return 0;
}

int ifndef_dir(struct linked_list **l, struct map *m) {
    struct elem *elem = (*l)->start, *curr;
    char *data, *temp, *token, *key;
    int i, nr_rep;

    for (i = 0; i < (*l)->count; i++) {
        data = malloc((strlen(elem->data) + 1) * sizeof(char));
        if (data == NULL)
            return -ERRORNO;

        memcpy(data, elem->data, strlen(elem->data));
        memcpy(data + strlen(elem->data), "\0", 1);

        if (strncmp(data, "#ifndef", 7) == 0) {
            temp = malloc((strlen(data) + 1) * sizeof(char));
            if (temp == NULL) {
                free(data);
                return -ERRORNO;
            }
            memcpy(temp, data, strlen(data));
            memcpy(temp + strlen(data), "\0", 1);

            token = strtok(temp, " \n");
            token = strtok(NULL, " \n");

            key = malloc(strlen(token) * sizeof(char));
            memcpy(key, token, strlen(token) - 1);
            memcpy(key + strlen(token) - 1, "\0", 1);

            if (has_key(m, key) == 0) {
                elem = delete_from_pos(l, i);
                nr_rep = i + 1;
                curr = (elem->next)->next;
                while (strncmp(curr->data, "#endif", 6) != 0)
                    curr = curr->next;
                curr = delete_from_pos(l, nr_rep);
            } else {
                elem = delete_from_pos(l, i);
                elem = delete_from_pos(l, i);

                if (strncmp(elem->next->data, "#else", 5) == 0) {
                    elem = delete_from_pos(l, i);

                    nr_rep = i;
                    curr = elem;
                    elem = elem->next;

                    while (strncmp(elem->data, "#endif", 6) != 0) {
                        elem = elem->next;
                        i += 1;
                    }

                    elem = delete_from_pos(l, i);

                    elem = curr;
                }
            }

            free(key);
            free(temp);
        }

        free(data);
        elem = elem->next;
    }
    return 0;
}

void print_in_file(FILE *f_out, struct linked_list *l)
{
    struct elem *elem = l->start;
    int i = 0;

    for (i = 0; i < l->count; i++) {
        if (strncmp(elem->data, "#", 1) == 0) {
            elem = elem->next;
            continue;
        } else {
            fprintf(f_out, "%s", elem->data);
        }
        elem = elem->next;
    }
}

#endif
