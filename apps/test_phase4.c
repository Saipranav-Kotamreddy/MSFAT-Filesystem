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
    int fd = fs_open("file\0");
    if (fd == -1) {
        printf("Fd is -1 fail!\n");
        exit(1);
    }

    int size = 10;
    char* buf = "1234567890";
    int res = fs_write(fd, buf, size);
    if (res != size) {
        printf("Write failed: %d vs %d\n", res, size);
        exit(1);
    } else {
        printf("Write succeeded\n");
    }
    fs_close(fd);

    char* buf2 = malloc(sizeof(char) * BLOCK_SIZE);
    block_read(4, buf2);
    printf("Expected '97': %d\n", buf2[0]);

    if (fs_umount())
        die("Cannot unmount disk");
    
    /* =============== Testing get block at offset =========================
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
    fs_create("file\0");
    fs_create("file2\0");

    int fd = fs_open("file\0");
    int ret = fs_test(fd);
    printf("Expected 1: %d\n", ret);
    printf("----------------------------------\n");
    for (int i = 0; i < BLOCK_SIZE+1; i++)
        fs_lseek(fd, fs_stat(fd));
    ret = fs_test(fd);
    printf("Expected 2: %d\n", ret);

    for (int i = 0; i < BLOCK_SIZE*5+1; i++)
        fs_lseek(fd, fs_stat(fd));
    ret = fs_test(fd);
    printf("Expected 7: %d\n", ret);

    fs_lseek(fd, BLOCK_SIZE*3 + 100);
    ret = fs_test(fd);
    printf("Expected 4: %d\n", ret);

    fs_lseek(fd, BLOCK_SIZE*0 + 100);
    ret = fs_test(fd);
    printf("Expected 1: %d\n", ret);

    fs_lseek(fd, BLOCK_SIZE-1);
    ret = fs_test(fd);
    printf("Expected 1: %d\n", ret);

    fs_lseek(fd, BLOCK_SIZE);
    ret = fs_test(fd);
    printf("Expected 2: %d\n", ret);
    
    int fd2 = fs_open("file2\0");
    ret = fs_test(fd2);
    printf("Expected 8: %d\n", ret);

    for (int i = 0; i < BLOCK_SIZE*3+1; i++)
        fs_lseek(fd2, fs_stat(fd2));

    ret = fs_test(fd2);
    printf("Expected 11: %d\n", ret);

    fs_lseek(fd, fs_stat(fd));
    ret = fs_test(fd);
    printf("Expected 7: %d\n", ret);

    for (int i = 0; i < BLOCK_SIZE*1; i++)
        fs_lseek(fd, fs_stat(fd));
    ret = fs_test(fd);
    printf("Expected 12: %d\n", ret);
    */

//     for (int i = 0; i < BLOCK_SIZE+1; i++)
//         fs_lseek(fd, fs_stat(fd));
//     fs_write(fd);

//     //printf("------------------------------------\n");


//     int fd2 = fs_open("file2\0");
//     for (int i = 0; i < BLOCK_SIZE+1; i++)
//         fs_lseek(fd2, fs_stat(fd2));
//     fs_write(fd2);

//     //printf("------------------------------------\n");

//     for (int i = 0; i < BLOCK_SIZE+1; i++)
//         fs_lseek(fd, fs_stat(fd));
//     fs_write(fd);
//    // printf("------------------------------------\n");
//     get_current_data_block(fd2);
//     //printf("------------------------------------\n");

//     fs_lseek(fd, BLOCK_SIZE);
//     get_current_data_block(fd);
//     //printf("------------------------------------\n");

//     for (int i = 0; i < 3*BLOCK_SIZE+1; i++)
//         fs_lseek(fd2, fs_stat(fd2));
//     get_current_data_block(fd2);

//     //printf("------------------------------------\n");
//     fs_lseek(fd2, 0);
//     get_current_data_block(fd2);
//     fs_lseek(fd2, BLOCK_SIZE);
//     get_current_data_block(fd2);
//     fs_lseek(fd2, BLOCK_SIZE*2);
//     get_current_data_block(fd2);
	return 0;
}
