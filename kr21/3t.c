struct lnode
{
    int k;
    struct lnode *next, *prev;
};

#include "3.c"
#include <stdio.h>

void print_list(struct lnode* cur)
{
    if (cur == NULL)
        return;

    printf("%d", cur->k);
    print_list(cur->next);
}

int add_end(struct lnode* cur, int str)
{
    while (cur->next)
        cur = cur->next;

    struct lnode* new;
    new = (struct lnode*) malloc(sizeof(struct lnode));

    if (new == NULL)
        return 1;

    new->k = str;
    cur->next = new;
    new->prev = cur;
    new->next = NULL;

    return 0;
}

int a[] = {3, 4, 6, 7, -8, 0, -8, 7, 6, 4, 3};

int main()
{
    struct lnode *head = NULL;
    head = (struct lnode*) malloc(sizeof(struct lnode));

    if (head == NULL)
        exit(0);

    head->k = 0;
    head->next = NULL;
    head->prev = NULL;


    for (int i = 0; i < sizeof(a)/sizeof(int); i++)
        add_end(head, a[i]);

    head->next->prev = NULL;
    head = head->next;

    printf("%d\n", Check_Sym(head));
    return 0;
}
