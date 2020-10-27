#include <stdio.h>

// Files.

int main(int argCount, char** argValues);
// program name is an argument too
// char** is array of strings

int i = atoi(argValues[2]);
float pi = atof(argValues[3]);

// files fr

FILE *f;

f = fopen("file_name", "mode");

// modes: r, w, a(ppend), r+ (read&write), w+ (write&read), a+ (append&read)
// diff r+/w+: if file doesn't exist, r+ will error
//
// fopen returns NULL on error

fclose(f);

int fscanf(FILE* f, char* formar_str, ...);

// every UNIX process automatically gets 3 threads: stdin, stdout, stderr
// these names are autodefined constants
//
// prev we threw errors in stdout. Should be fprintf(stderr, "error");

// ./a.out < input.txt
// ./a.out > out.txt
// ./a.out < input.txt > out.txt
//
int getchar();
// EOF - error
int fgetc(FILE* f);
int fputc(int c, FILE* f);

char* gets(char* str); // bad
// when it encounters \n gets replaces it with \0 and stops

int fputs(char* str, FILE* f);

// fgets(buf, 99, stdin); // safe gets

// binary read
int fread(void* p, size_t size, size_t nobj, FILE* f);
double var;
fread(&var, sizeof(double), 1, f);

// binary write
int fwrite(void* p, size_t size, size_t nobj, FILE* f);
fwrite(&var, sizeof(double), 1, f);

int fseek(FILE* f, long offset, int from);
// from SEEK_SET - start of file
// SEEK_CUR - current
// SEEK_END - eof

long ftell(FILE* f); // returns file pos

int a(FILE* f)
{
    int size;

    fseek(f, 0, SEEK_END); // go to eof
    int n = ftell(f);
    size = n / sizeof(double);

    return size;
}

// Trees
//
// Search tree.
//     7
//   6 _ 8
// 8 _ _ _ 10
// _ _ _ 9 _

#include <stdlib.h>

struct tnode
{
    int key;
    struct tnode *left, *right;
};

struct tnode* add_tnode(struct tnode* p, int key)
{
    if (p == NULL)
    {
        p = (struct tnode*) malloc(sizeof(struct tnode));
        if (!p) return NULL;

        p->key = key;
        p->left = NULL;
        p->right = NULL;

        return p;
    }

    if (p->key < key)
        p->right = add_tnode(p->right, key);
    else if (p->key > key)
        p->left = add_tnode(p->left, key);

    return p;
}

int main()
{
    struct tnode* root = NULL;
    int tkey;

    while (scanf("%d", &tKey) != EOF)
        root = add_tnode(root, tKey);

    return 0;
}
