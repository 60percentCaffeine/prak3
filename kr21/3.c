/*struct lnode
{
    int k;
    struct lnode *next, *prev;
};*/

#include <stdlib.h>

int count(struct lnode *head)
{
    struct lnode *p = head;
    int cnt = 0;

    while (p != NULL)
    {
        cnt++;
        p = p->next;
    }

    return cnt;
}

struct lnode *getnode(struct lnode *head, int n)
{
    int i = 0;
    struct lnode *p = head;

    for (i = 0; i < n; i++)
        p = p->next;

    return p;
}

int Check_Sym(struct lnode *head)
{
    int cnt = count(head);
    int li = 0;
    int ri = 0;
    int i = 0;
    struct lnode *ln, *rn;

    if (cnt <= 1)
        return 1;

    if (cnt % 2 == 1)
    {
        li = cnt / 2 - 1;
        ri = li + 2;
    }
    else
    {
        li = cnt / 2 - 1;
        ri = li + 1;
    }

    ln = getnode(head, li);
    rn = getnode(head, ri);

    for (i = 0; i < (cnt / 2); i++)
    {
        if (ln->k != rn->k)
            return 0;

        ln = ln->prev;
        rn = rn->next;
    }

    return 1;
}
