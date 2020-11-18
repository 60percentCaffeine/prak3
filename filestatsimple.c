#include <stdio.h>
#include "60lib.c"

int is_word_break(char c)
{
    return (c == ' ' || c == '.' || c == ',' || c == ';' || c == '\n');
}

int is_visible(char c)
{
    return (c && c != '\n' && c != EOF);
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
        error("Can't open the file.\n", 1);

    // 
    // Count stuff
    //

    char c = fgetc(f);
    long chars = 0;
    long lines = 0;
    long words = 0;

    while (c && c != EOF)
    {
        // Increment counters
        if (is_visible(c))
            chars++;

        if (c == '\n')
            lines++;

        if (is_word_break(c))
            words++;

        // Next character
        c = fgetc(f);
    }

    // 
    // Output
    //

    printf("Lines: %ld, words: %ld, characters: %ld\n", lines, words, chars);
    return 0;
}
