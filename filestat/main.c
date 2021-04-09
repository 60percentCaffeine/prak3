// wc clone

#include <stdio.h>
#include <stdlib.h>
#include "tree.c"

//
// stat helpers
//

int is_word_break(int c)
{
    return (c == ' ' || c == '.' || c == ',' || c == ';' || c == '\n' || c == EOF);
}

int is_visible(int c)
{
    return (c && c != '\n' && c != EOF);
}

char LAST_CHAR_GARBAGE = 1;

// returns 1 on word end
int inc_stats(int c, long *chars, long *lines, long *words)
{
    int res = 0;

    // Increment counters
    if (c == '\n')
        (*lines)++;

    if (!LAST_CHAR_GARBAGE && is_word_break(c))
    {
        (*words)++;
        res = 1;
    }

    (*chars)++; 
    LAST_CHAR_GARBAGE = (is_word_break(c) || !is_visible(c));

    return res;
}

// 
// search helpers
//

// adds a new node to the left or to the right of the given one
void add_tnode(struct tnode *p, char *word)
{
    int cmp = strcmp(p->word, word);

    if (cmp < 0)
    {
        struct tnode *new_right = make_tnode(word);
        new_right->right = p->right;
        p->right = new_right;
    }
    else if (cmp > 0)
    {
        struct tnode *new_left = make_tnode(word);
        new_left->left = p->left;
        p->left = new_left;
    }
}

// returns 0 if doesn't find word
// 1 if does
// res is the last viewed node
int search(struct tnode **res, struct tnode *p, char *word)
{
    int cmp = strcmp(p->word, word);
    *res = p;

    if (cmp < 0)
        p = p->right;
    else if (cmp > 0)
        p = p->left;
    else
        return 1;

    if (p == NULL)
        return 0;

    return search(res, p, word);
}

void reg_word(struct tnode* p, char *word)
{
    struct tnode *res;
    int found = search(&res, p, word);

    if (found)
    {
        res->occurences++;
        return;
    }

    // entry not found, create it
    add_tnode(res, word);

    // no need to increment occurences there, 1 is the default
}

int main(int argc, char **argv)
{
    // 
    // Open the file
    //

    FILE *f;

    if (argc > 1)
    {
        f = fopen(argv[1], "r");
        printf("Using filename\n");
    }
    else
    {
        f = stdin;
        printf("Using stdin\n");
    }

    if (f == NULL)
        error("Can't open the file.\n", LAST_60ERR + 1);

    // 
    // Read the file, count stats and build the tree
    //

    // list head
    struct tnode *tree = make_tnode(""); // empty string can't be a word

    int c = fgetc(f);
    long chars = 0;
    long lines = 0;
    long words = 0;
    char *word = NULL;
    int word_size = 0;

    while (1)
    {
        // Increment stats
        int word_ended = inc_stats(c, &chars, &lines, &words);

        // Add character to word
        if (is_visible(c) && !is_word_break(c))
        {
            word_size++;
            word = realloc(word, word_size + 1);
            word[word_size] = 0;
            word[word_size - 1] = c;
        }

        // Add word to tree
        if (word_ended)
        {
            reg_word(tree, word);
            word_size = 0;
            word[0] = 0;
        }

        int exit_sign = 0;
        if (c == '\n')
            exit_sign = 1;

        // Next character
        c = fgetc(f);

        if (c == '\n' && exit_sign)
            break;

        if (c == EOF)
            break;
    }

    // 
    // Output stats
    //

    printf("Lines: %ld, words: %ld, characters: %ld\n", lines, words, chars);

    // 
    // Search: Reading the needle
    //

    free(word);
    printf("Enter the word:\n");
    word = read_string('\n');

    printf("The word is: \"%s\"\n", word);

    // 
    // Search: searching
    //

    struct tnode *t;
    int found = search(&t, tree, word);

    // 
    // Search: output
    // 

    printf("%d occurences\n", (found ? t->occurences : 0));

    // 
    // Finish the program: clean the memory, etc.
    // 

    destroy_tree(tree);

    return 0;
}
