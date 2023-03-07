#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

struct  __attribute__((__packed__)) superblock{
	char signature[8];
	uint16_t total_blocks;
	uint16_t root_index;
	uint16_t data_index;
	uint16_t data_size;
	uint8_t fat_size;
	uint8_t padding[4079];
};

struct  __attribute__((__packed__)) fat_block{
	uint16_t fat_array[2048]; 
};

struct  __attribute__((__packed__)) root_entry{
	char filename[16];
	uint32_t size;
	uint16_t data_index;
	uint8_t padding[10];
};

struct  __attribute__((__packed__)) data_block{
	uint8_t data[4096];
};

struct  __attribute__((__packed__)) file_system{
	struct superblock* superblock;
	struct fat_block* fat;
	struct root_directory* root_dir;
	struct data_block* data;
};

/* TODO: Phase 1 */

struct file_system* fs;

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
	if(block_disk_open(*diskname)==-1){
		return -1;
	}
	fs = malloc(sizeof(struct file_system));
	fs->superblock = malloc(sizeof(struct superblock));
	if(block_read(0, fs->superblock)==-1){
		return -1;
	}
	//Insert error checking on disk info here

	char* signature = fs->superblock->signature;
	uint8_t fat_size = fs->superblock->fat_size;
	int root_index = fs->superblock->root_index;
	int data_index = fs->superblock->data_index;
	int data_count = fs->superblock->data_size;
	int total_blocks = fs->superblock->total_blocks;
	if(strcmp(signature,"ECS150FS")!=0 || (1+fat_size)!=root_index || (root_index+1)!=data_index){
		free(fs->superblock);
		free(fs);
		return -1;
	}

	if(block_disk_count!=total_blocks || (1+fat_size+1+data_size)!=total_blocks){
		free(fs->superblock);
		free(fs);
		return -1;
	}

	fs->fat = malloc(sizeof(struct fat_block)*fat_size);
	for(int i=0; i<fat_size; i++){
		if(block_read(i+1, fs->fat[i])==-1){
			free(fs->fat);
			free(fs->superblock);
			free(fs);
			return -1;
		}
	}
	fs->root_dir = malloc(FS_FILE_MAX_COUNT*32);
	if(block_read(root_index, fs->root_dir)==-1){
		free(fs->root_dir);
		free(fs->fat);
		free(fs->superblock);
		free(fs);
		return -1;
	}
	fs->data = malloc(data_count*4096);
	for(int i=0; i<data_count; i++){
		if(block_read(data_index+i,fs->data[i])==-1){
			free(fs->data);
			free(fs->root_dir);
			free(fs->fat);
			free(fs->superblock);
			free(fs);
			return -1;
		}
	}
	return 0;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
	free(fs->data);
	free(fs->root_dir);
	free(fs->fat);
	free(fs->superblock);
	free(fs);
	//Add check for open file descriptors
	if(block_disk_close()==-1){
		return -1;
	}
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

