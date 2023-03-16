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
    
    int sizes[] = {9034, 1368, 19000};
    char* filenames[] = {"test_file2.txt", "test_file1.txt", "test_file3.txt"};
    char* file_names_on_virtual_disk[] = {"file2", "file", "file3"};

    for (int i = 0; i < 3; i++) {
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
        close(file_fd);
        int fd = fs_open(file_names_on_virtual_disk[i]);
        if (fd == -1) {
            printf("Fd is -1 fail for %d!\n", i);
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

    if (fs_umount())
        die("Cannot unmount disk");
    
    if (fs_mount("test.fs"))
        die("Cannot mount disk");

    /* Test straight up reading */
    for (int i = 0; i < 3; i++) {
        char exp_buf[sizes[i]];
        char actual_buf[sizes[i]];
        int file_fd = open(filenames[i], O_RDONLY);
        if (file_fd < 0) {
            printf("File Fd is -1 fail!\n");
            exit(1);
        }
        int ret = read(file_fd, exp_buf, sizes[i]);
        if (ret == -1) {
            printf("Read failed! - %d\n", ret);
            exit(1);
        }

        int fd = fs_open(file_names_on_virtual_disk[i]);
        if (fd < 0) {
            printf("Fd is -1 fail for %d!\n", i);
            exit(1);
        }
        ret = fs_read(fd, actual_buf, sizes[i]);
        if (ret != sizes[i]) {
            printf("FS Read failed! - %d\n", ret);
            exit(1);
        }
        
        int fail = 0;
        for (int j = 0; j < sizes[i]; j++) {
            if (exp_buf[j] != actual_buf[j]) {
                printf("Fs read does not match at %d for file %s. exp: %c, actual: %c\n", j, filenames[i], exp_buf[j], actual_buf[j]);
                fail += 1;
            }
            if (fail == 10) break;
        }
    }

    /* Test reading in middle of file */

    int seek_am[] = {145, 4099, 7453, 9023, 10005, 14000, 18423};
    for (int i = 0; i < 3; i++) {
        char exp_buf[sizes[i]];
        int file_fd = open(filenames[i], O_RDONLY);
        if (file_fd < 0) {
            printf("File Fd is -1 fail!\n");
            exit(1);
        }
        int ret = read(file_fd, exp_buf, sizes[i]);
        if (ret == -1) {
            printf("Read failed! - %d\n", ret);
            exit(1);
        }

        int fd = fs_open(file_names_on_virtual_disk[i]);
        for(int j = 0; j < 7; j++) {
            if (seek_am[j] >= sizes[i]) break;

            char actual_buf[sizes[i] - seek_am[j]];

            if (fd < 0) {
                printf("Fd is -1 fail for %d!\n", i);
                exit(1);
            }

            ret = fs_lseek(fd, seek_am[j]);
            
            if (ret == -1) {
                printf("Seek of %d failed! - %d\n", seek_am[j], ret);
                exit(1);
            }

            ret = fs_read(fd, actual_buf, sizes[i] - seek_am[j]);
            if (ret != sizes[i] - seek_am[j]) {
                printf("FS Read failed for file %s, seek %d - expected %d, got %d\n", filenames[i], seek_am[j], sizes[i] - seek_am[j], ret);
                exit(1);
            }
            
            int fail = 0;
            for (int k = seek_am[j]; k < sizes[i]; k++) {
                if (exp_buf[k] != actual_buf[k-seek_am[j]]) {
                    printf("Fs read does not match when offset=%d, at ind %d for file %s. exp: %c, actual: %c\n", seek_am[j], k, filenames[i], exp_buf[k], actual_buf[k]);
                    fail += 1;
                }
                if (fail == 10) break;
            }

            if (fail != 0) {
                printf("FS read failed!\n");
                exit(1);
            }
           // printf("FS read succeeded for %s with seek %d\n", file_names_on_virtual_disk[i], seek_am[j]);
        }   
    }

    printf("Reading in middle of files successful!\n");

    /* Testing reading N bytes, then M bytes, etc*/
    int num_bytes[] = {14, 100, 5000, 600, 1345, 224, 4099, 123, 13000};
    for (int i = 0; i < 3; i++) {
        char exp_buf[sizes[i]];
        int file_fd = open(filenames[i], O_RDONLY);
        if (file_fd < 0) {
            printf("File Fd is -1 fail!\n");
            exit(1);
        }
        int ret = read(file_fd, exp_buf, sizes[i]);
        if (ret == -1) {
            printf("Read failed! - %d\n", ret);
            exit(1);
        }

        int sum_bytes = 0;
        int fd = fs_open(file_names_on_virtual_disk[i]);
        for(int j = 0; j < 9; j++) {
            if (sum_bytes > sizes[i]) break;

            char actual_buf[num_bytes[j]];

            if (fd < 0) {
                printf("Fd is -1 fail for %d!\n", i);
                exit(1);
            }

            ret = fs_read(fd, actual_buf, num_bytes[j]);
            int exp_ret = num_bytes[j];
            if (sum_bytes + num_bytes[j] > sizes[i]) {
                exp_ret -= (sum_bytes + num_bytes[j] - sizes[i]);
            }
            if (ret != exp_ret) {
                printf("FS Read failed for file %s, num bytes %d - expected %d, got %d\n", filenames[i], num_bytes[j], exp_ret, ret);
                exit(1);
            }
            
            int fail = 0;
            for (int k = sum_bytes; k < min(sum_bytes+num_bytes[j], sizes[i]); k++) {
                if (exp_buf[k] != actual_buf[k-sum_bytes]) {
                    printf("Fs read does not match when num bytes=%d, at ind %d for file %s. exp: %c, actual: %c\n", num_bytes[j], k, filenames[i], exp_buf[k], actual_buf[k-sum_bytes]);
                    fail += 1;
                }
                if (fail == 10) break;
            }

            if (fail != 0) {
                printf("FS read failed!\n");
                exit(1);
            }
            //printf("FS read succeeded for %s with num bytes %d\n", file_names_on_virtual_disk[i], num_bytes[j]);
            sum_bytes += num_bytes[j];
        }   
    }

    printf("Reading overall successful!\n");

	return 0;
}
