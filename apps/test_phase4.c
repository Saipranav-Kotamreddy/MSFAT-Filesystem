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
#include <disk.h>

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
    fs_create("file\0");
    fs_create("file2\0");

    int fd = fs_open("file\0");
    for (int i = 0; i < BLOCK_SIZE+1; i++)
        fs_lseek(fd, fs_stat(fd));
    get_current_data_block(fd);

    

    for (int i = 0; i < BLOCK_SIZE+1; i++)
        fs_lseek(fd, fs_stat(fd));
    get_current_data_block(fd);

    //printf("------------------------------------\n");


    int fd2 = fs_open("file2\0");
    for (int i = 0; i < BLOCK_SIZE+1; i++)
        fs_lseek(fd2, fs_stat(fd2));
    get_current_data_block(fd2);

    //printf("------------------------------------\n");

    for (int i = 0; i < BLOCK_SIZE+1; i++)
        fs_lseek(fd, fs_stat(fd));
    get_current_data_block(fd);
   // printf("------------------------------------\n");
    get_current_data_block(fd2);
    //printf("------------------------------------\n");

    fs_lseek(fd, BLOCK_SIZE);
    get_current_data_block(fd);
    //printf("------------------------------------\n");

    for (int i = 0; i < 3*BLOCK_SIZE+1; i++)
        fs_lseek(fd2, fs_stat(fd2));
    get_current_data_block(fd2);

    //printf("------------------------------------\n");
    fs_lseek(fd2, 0);
    get_current_data_block(fd2);
    fs_lseek(fd2, BLOCK_SIZE);
    get_current_data_block(fd2);
    fs_lseek(fd2, BLOCK_SIZE*2);
    get_current_data_block(fd2);

    // for (int i = 0; i < BLOCK_SIZE+1; i++)
    //     fs_lseek(fd, fs_stat(fd2));
    // for (int i = 0; i < BLOCK_SIZE+1; i++)
    //     fs_lseek(fd, fs_stat(fd));
    // get_current_data_block(fd2);
    // get_current_data_block(fd);
	return 0;
}
