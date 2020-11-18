// Low-level file access

// FILE *f; fopen; fprintf - high level funcs

// What is FILE *f?
typedef struct { // depends on the compiler
    char *ptr;
    char *base; // buf addr
    int flag;
    int fd;
    // ...
} FILE;

// fprintf doesn't immediately outputs, it puts data in a buffer (*base)
// buffer cleanup and output usually happen on '\n' or buffer overflow
// if program ends preemptively, the buffer might not get processed
// buffer is 512 bytes?

fflush(f); // flush buffer (and output the data)

#include <fcntl.h>

int open(char *filename, int flags; [int params]);

// int flags. Bitflags (can be mixed)
O_RDONLY // read only
O_WRONLY
O_RDWR
O_APPEND // seek to eof
O_TRUNC // truncate file to 0 len (clear it)
O_EXCL // error if file exists
O_CREAT // create file
O_NONBLOCK // open in non-blocking mode

// some flags are incompatible with each other (i.e. O_RDONLY | O_TRUNC)

// open returns a file descriptor
int fd = open("filename", O_RDONLY);

// TOPD - table of open FDs is created for each new process
// default:
// 0 | stdin
// 1 | stdout
// 2 | stderr
// -- open file --
// 3 | "filename" // it's actually an index descriptor, not a filename

// open returns -1 on error

fd = open("file1.dat", O_WRONLY | O_CREAT | O_EXCL, 0644);

// O_CREAT requires perms
// ls -l
// rwx r-x r-x a.out
// rw- r-- r--
// user group other - 3 perm groups

// perms are usually represented by an octal number:
// 111 101 101 a.out
//  7   5   5
//
// chmod 755 a.out
// chmod +x a.out - add x to usr, grp, other
//
// there are actually 4 numbers, not 3. Y? idk

int fd2 = open("file2.dat", O_WRONLY|O_CREAT|O_TRUNC, 0666); // this is actually "rw" mode in fopen
int fd3 = creat("file3.dat", 0666); // same as above

// directories are just special files
// closing files:
close(fd1); // returns -1 on error

// on any error its' code is written to "errno" global var
#include <errno.h> // contains macros of every possible error

#include <unistd.h> // read, write

// rw
int read(int fd, void *buf, int size); // returns amount of bytes actually read (diff in case file is less than size/doesn't exist)
int write(int fd, void *buf, int size); // same

char buf[1024];
read(fd, buf, 102);

int d = 3;
write(fd2, &d, sizeof(int));
// writes d IN BINARY FORM
// convenient command: hexdump -C file.bin

// copy
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char buf[1024];
    int n, fd1, fd2;
    struct stat s;

    if (argc < 3)
    {
        throw_error(1, ">:(")
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1) throw_error(2, "open");

    // get perms of fd1
    stat(argv[1], &s);

    // creaT file with same perms
    fd2 = creat(argv[2], s.st_mode);
    if (fd2 == -1) throw_error(3, "creat");

    while ((n = read(fd1, buf, 1024)) > 0)
        write(fd2, buf, n);

    close(fd1);
    close(fd2);
    return 0;
}

// seek
long lseek(int fd, long offset, int mode); // offset can be neg
// modes:
// SEEK_SET (from beginning)
// SEEK_CUR (from current pos)
// SEEK_END (from eof)

// doesn't error if out of bounds
// errors if you try to read OOB
// doesn't error if you write OOB

lseek(fd, 0, SEEK_CUR); // returns current pos from beginning of file
