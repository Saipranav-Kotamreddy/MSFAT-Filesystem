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
    /* Test open when fs is null */
    assert(fs_open("file\0") == -1);
    assert(fs_close(0) == -1);
    assert(fs_stat(0) == -1);
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
    assert(fs_create("file\0") == 0);
    assert(fs_create("file2\0") == 0);
    assert(fs_create("newfile\0") == 0);
    assert(fs_create("newfile") == -1);
    
    /* Test open on invalid file name */
    assert(fs_open("file\0") == 0);
    
    assert(fs_open("file\0") == 1);
    assert(fs_open("file2\0") == 2);
    assert(fs_close(0) == 0);
    
    /* Test close and stat when file not opened*/
    assert(fs_close(0) == -1);
    assert(fs_stat(0) == -1);

    assert(fs_open("newfile") == 0);
    /* Test close on invalid fd's */
    assert(fs_close(32) == -1);
    assert(fs_close(-1) == -1);
    assert(fs_stat(32) == -1);
    assert(fs_stat(-1) == -1);

    assert(fs_stat(0) == 0);
    assert(fs_stat(1) == 0);
    assert(fs_stat(2) == 0);
    assert(fs_stat(3) == -1);
    assert(fs_close(1) == 0);
    assert(fs_stat(1) == -1);
    assert(fs_close(2) == 0);
    assert(fs_close(0) == 0);

    for(int i = 0; i < FS_OPEN_MAX_COUNT; i++) {
        assert(fs_open("file\0") == i);
    }
    /* Test open when open file table full */
    assert(fs_open("file\0") == -1);
    /* Test open when file name does not exist */
    assert(fs_open("filenoexists\0") == -1);


    printf("Test for open, close, and stat: Success!\n");
	return 0;
}
