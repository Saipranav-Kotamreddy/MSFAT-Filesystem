#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fs.h>

#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define test_fs_error(fmt, ...) \
	fprintf(stderr, "%s: "fmt"\n", __func__, ##__VA_ARGS__)

#define die(...)				\
do {							\
	test_fs_error(__VA_ARGS__);	\
	exit(1);					\
} while (0)

#define die_perror(msg)			\
do {							\
	perror(msg);				\
	exit(1);					\
} while (0)

int main(int argc, char **argv)
{
    assert(fs_lseek(0, 0) == -1);
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
    assert(fs_create("file\0") == 0);
    //fs_create("file\0");    
    int fd1 = fs_open("file\0");

    for (int i = 0; i < 5; i++) {
        //printf("In loop, fs stat returns: %d\n", fs_stat(fd1));
        //fs_lseek(fd1, fs_stat(fd1));
        assert(fs_stat(fd1) == i);
        assert(fs_lseek(fd1, fs_stat(fd1)) == 0);
    }
    assert(fs_stat(fd1) == 5);
    for (int i = 0; i < 5; i++) {
        assert(fs_lseek(fd1, i) == 0);
    }

    /* Test -1 cases */
    assert(fs_lseek(-1, 0) == -1);
    assert(fs_lseek(32, 0) == -1);
    assert(fs_lseek(fd1, fs_stat(fd1) + 1) == -1);
    fs_close(fd1);
    assert(fs_lseek(fd1, 0) == -1);

    printf("Test for lseek: Success!\n");
	return 0;
}