#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#define LAST_BIT 0xFF
#include <stdio.h>

// debug stuff
/*
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}*/
int read2(int fd, void *buf, off_t pos, int seek)
{
    lseek(fd, pos, seek);
    int bytes_read = read(fd, buf, sizeof(int));
    return bytes_read;
}

int write2(int fd, void *buf, off_t pos, int seek)
{
    lseek(fd, pos, seek);
    int bytes_written = write(fd, buf, sizeof(int));
    return (bytes_written == sizeof(int));
}

off_t getsize(int fd)
{
    off_t fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, fsize, SEEK_SET);
    return fsize;
}

int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
        perror("open failed\n");
    
    off_t size = getsize(fd);
    if (size <= sizeof(int))
    {
        close(fd);
        return 0;
    }

    int a = -1, b = -1;
    off_t pos = 0;

    for (pos = 0; pos < (size / 2); pos += sizeof(int))
    {
        read2(fd, &a, pos, SEEK_SET);
        read2(fd, &b, -pos - sizeof(int), SEEK_END);
        write2(fd, &b, pos, SEEK_SET);
        write2(fd, &a, -pos - sizeof(int), SEEK_END);
   }

    close(fd);
    return 0;
}
