// replaces min int with max int in file with ints

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h> // exit
#include <limits.h>

int main()
{
    char *file = "file.bin";
    int fp = open(file, O_RDWR);
    if (fp == -1) exit(1);

    // find min & max
    int cur;
    int max = INT_MIN, min = INT_MAX;
    long max_p, min_p;

    if (read(fp, &cur, sizeof(int)) <= 0) exit(2); 
    do
    {
        if (cur > max)
        {
            max = cur;
            max_p = lseek(fp, 0, SEEK_CUR) - sizeof(int);
        }

        if (min > cur)
        {
            min = cur;
            min_p = lseek(fp, 0, SEEK_CUR) - sizeof(int);
        }
    } while (read(fp, &cur, sizeof(int)) > 0);

    lseek(fp, min_p, SEEK_SET);
    write(fp, &max, sizeof(int));

    lseek(fp, max_p, SEEK_SET);
    write(fp, &min, sizeof(int));

    close(fp);
    return 0;
}
