/*struct tnode
{
    int key;
    struct tnode *left, *right;
};*/

#include <stdlib.h>
#include <stdio.h>

struct tnode *min(struct tnode *n, void *stp)
{
    if (n == stp)
        return n->right;

    if (n->left == NULL)
        return n;

    return (n->left);
}

void Min2Key(struct tnode *tnode)
{
    struct tnode *min1 = min(tnode, tnode);
    struct tnode *min2 = min(tnode, min1);
    printf("%d %d\n", min1->key, min2->key);
    fflush(stdout);
}
