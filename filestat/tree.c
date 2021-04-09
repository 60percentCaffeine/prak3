#include <stdlib.h>
#include "60lib.c"

struct tnode
{
    char *word;
    int occurences;
    struct tnode *left, *right;
};

void destroy_tree(struct tnode *p)
{
    if (p == NULL)
        return;

    destroy_tree(p->left);
    destroy_tree(p->right);

    free(p);
}

// creates a node with word
struct tnode *make_tnode(char *word)
{
    struct tnode *p = malloc(sizeof(struct tnode));
    check_alloc(p, 1);

    p->word = clone_string(word);
    p->occurences = 1;
    p->left = NULL;
    p->right = NULL;

    return p;
}
