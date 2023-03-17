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
    
    int sizes[] = {1368, 9034, 19000};
    char* filenames[] = {"test_file1.txt", "test_file2.txt", "test_file3.txt"};
    char* file_names_on_virtual_disk[] = {"file", "file2", "file3"};

    for (int i = 0; i < 2; i++) {
        char buf[sizes[i]];
        int file_fd = open(filenames[i], O_RDONLY);
        if (file_fd < 0) {
            printf("File Fd is -1 fail!\n");
            exit(1);
        }
        int ret = read(file_fd, buf, sizes[i]);
        if (ret == -1) {
            printf("Read failed! - %d\n", ret);
            exit(1);
        }
        int fd = fs_open(file_names_on_virtual_disk[i]);
        if (fd == -1) {
            printf("Fd is -1 fail!\n");
            exit(1);
        }
        int res = fs_write(fd, buf, sizes[i]);
        if (res != sizes[i]) {
            printf("Write failed: %d vs %d\n", res, sizes[i]);
            exit(1);
        } else {
            printf("Write succeeded\n");
        }

        int file_size = fs_stat(fd);
        if (file_size != sizes[i]) {
            printf("Mismatched file size: Expected %d, Got %d\n", sizes[i], file_size);
            exit(1);
        }

        fs_close(fd);
    }

    int i = 2;
    char buf[sizes[i]];
    int file_fd = open(filenames[i], O_RDONLY);
    if (file_fd < 0) {
        printf("File Fd is -1 fail!\n");
        exit(1);
    }
    int ret = read(file_fd, buf, sizes[i]);
    if (ret == -1) {
        printf("Read failed! - %d\n", ret);
        exit(1);
    }
    int fd = fs_open(file_names_on_virtual_disk[i]);
    if (fd == -1) {
        printf("Fd is -1 fail!\n");
        exit(1);
    }
    int res = fs_write(fd, buf, sizes[i]);
    assert(res == 4096);
    printf("Result of write: %d\n", res);

    int file_size = fs_stat(fd);
    assert(file_size == 4096);

    fs_close(fd);

    fs_create("file4\0");
    fd = fs_open("file4\0");
    if (fd == -1) {
        printf("Fd is -1 fail!\n");
        exit(1);
    }
    res = fs_write(fd, buf, 10);
    assert(res == 0);
    file_size = fs_stat(fd);
    assert(file_size == 0);

    fs_close(fd);

    printf("------------------------------- Error test successful! ----------------");

    if (fs_umount())
        die("Cannot unmount disk");
    
    /* fs -1 case */
    assert(fs_read(0, buf, 9) == -1);
    if (fs_mount("test.fs"))
        die("Cannot mount disk");
    /* empty file case */
    assert(fs_read(0, buf, 9) == -1);
    /* invalid fd cases */
    assert(fs_read(-1, buf, 9) == -1);
    assert(fs_read(FS_OPEN_MAX_COUNT, buf, 9) == -1);

    fd = fs_open(file_names_on_virtual_disk[0]);
    assert(fs_read(fd, NULL, 3) == -1);

    fs_close(fd);

    if (fs_umount())
        die("Cannot unmount disk");
	return 0;
}
