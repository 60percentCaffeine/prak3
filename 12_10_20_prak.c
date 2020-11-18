//
// Объединения.
//

// union - переменная любого из указанных типов
// sizeof(union name) = max(sizeof(...))
// "шаблон"
union name
{
    float var;
    int d;
    char c[6];
};

// само объявление переменной
union name nm;
nm.var = 3.14;

union Int
{
    int d;
    char c[4];
} k;

k.d = 257;

char ch;

ch = k.c[3];
k.c[3] = k.c[0];
k.c[0] = ch;

// Structures.

struct lnode
{
    char str[20];
    struct lnode *prev;
    struct lnode *next;
};

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    struct lnode *head = NULL;
    head = (struct lnode*) malloc(sizeof(struct lnode));

    if (head == NULL)
        exit(1);

    strcpy(head->str, "begin");
    head->next = NULL;
    head->prev = NULL;
}

// NOTE: (cur != NULL) is the same as (cur)

void print_list(struct lnode* cur)
{
    if (cur == NULL)
        return;

    printf("%s", cur->str);
    print_list(cur->next);
}

int add_end(struct lnode* cur, char* str)
{
    while (cur->next)
        cur = cur->next;

    struct lnode* new;
    new = (struct lnode*) malloc(sizeof(struct lnode));

    if (new == NULL)
        return 1;

    strcpy(new->str, str);
    cur->next = new;
    new->prev = cur;
    new->next = NULL;

    return 0;
}
