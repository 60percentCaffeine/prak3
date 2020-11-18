#include "60lib.c" // read_string, printf

int main()
{
    char *sDestFile;
    char *sSrcFile;
    char *sText;
    FILE *fDest;
    FILE *fSrc;

    printf("Enter filenames separated by newline\nDestination: ");
    sDestFile = read_string('\n');
    printf("Source: ");
    sSrcFile = read_string('\n');

    fDest = fopen(sDestFile, "w+");
    if (fDest == NULL)
        exit(1);

    fSrc = fopen(sSrcFile, "r+");
    if (fSrc == NULL)
        exit(2);

    sText = fread_string(fSrc, EOF);
    fputs(sText, fDest);

    // cleanup
    fclose(fDest);
    fclose(fSrc);
    
    // not really needed but w/e
    free(sDestFile);
    free(sSrcFile);
    free(sText);
}
