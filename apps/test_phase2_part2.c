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
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
   
    assert(fs_create("file\0") == 0);
    assert(fs_create("file\0") == -1);
    assert(fs_create("file2\0") == 0);
    assert(fs_create("file3\0") == 0);
    assert(fs_create("file3\0") == -1);
    assert(fs_delete("file2\0") == 0);
    assert(fs_create("file3\0") == -1);

    assert(fs_umount() == 0);

    if (fs_mount("test.fs"))
        die("Cannot remount disk");

    assert(fs_create("file\0") == -1);
    assert(fs_create("file2\0") == 0);
    assert(fs_create("file3\0") == -1);
    assert(fs_delete("file3\0") == 0);

    assert(fs_umount() == 0);

    if (fs_mount("test.fs"))
        die("Cannot remount disk");

    assert(fs_create("file3\0") == 0);
    assert(fs_create("file\0") == -1);

    assert(fs_umount() == 0);


    printf("Test for phase2 part 2: Success!\n");
	return 0;
}