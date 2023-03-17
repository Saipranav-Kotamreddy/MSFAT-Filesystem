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
    assert(fs_create("file\0") == -1);
    assert(fs_delete("file\0") == -1);
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
    assert(fs_create("file\0") == 0);
    assert(fs_create("file") == -1);
    assert(fs_create("123456789012345\0") == 0);
    assert(fs_create("1234567890123456\0") == -1);
    assert(fs_delete("file\0") == 0);
    assert(fs_create("file\0") == 0);
    assert(fs_delete("file\0") == 0);
    assert(fs_delete("file\0") == -1);
    assert(fs_delete("file124\0") == -1);
    assert(fs_delete("123456789012345\0") == 0);
    assert(fs_create("file\0") == 0);
    assert(fs_create("file2\0") == 0);
    assert(fs_create("file3\0") == 0);

    for(int i = 0; i < FS_FILE_MAX_COUNT - 2; i++) {
        char *str = malloc(sizeof(char) * 2);
        str[0] = i;
        str[1] = '\0';
        assert(fs_create(str) == 0);
    }

    char *str = malloc(sizeof(char) * 2);
    str[0] = FS_FILE_MAX_COUNT - 2;
    str[1] = '\0';
    assert(fs_create(str) == -1);

    int fd = fs_open("file3\0");
    assert(fd != -1);
    assert(fs_delete("file3\0") == -1);
    assert(fs_stat(fd) == 0);
    assert(fs_delete("file2\0") == 0);
    fs_close(fd);
    assert(fs_delete("file3\0") == 0);

    printf("Opening --------------------- \n");
    for(int i = 40; i < 60; i++) {
        char *str = malloc(sizeof(char) * 2);
        str[0] = i;
        str[1] = '\0';
        //fs_open(str);
        assert(fs_open(str) != -1);
    }

    for(int i = 40; i < 60; i++) {
        char *str = malloc(sizeof(char) * 2);
        str[0] = i;
        str[1] = '\0';
        assert(fs_delete(str) == -1);
        fs_close(i-40);
        assert(fs_delete(str) == 0);
    }


    printf("Test for create and delete: Success!\n");
	return 0;
}
