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

struct  __attribute__((__packed__)) root_directory{
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
	struct fat_block* fat[4];
	struct root_directory root_dir[128];
	struct data_block* data_blocks[8192];
};

/* TODO: Phase 1 */

struct file_system* fs;

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
	if(block_disk_open(diskname)==-1){
		return -1;
	}
	fs = malloc(sizeof(struct file_system));
	fs->superblock = malloc(sizeof(struct superblock));
	if(block_read(0, fs->superblock)==-1){
		return -1;
	}
	//Insert error checking on disk info here
	char* correct_name = "ECS150FS";
	char* signature = fs->superblock->signature;
	uint8_t fat_size = fs->superblock->fat_size;
	uint16_t root_index = fs->superblock->root_index;
	uint16_t data_index = fs->superblock->data_index;
	uint16_t data_count = fs->superblock->data_size;
	uint16_t total_blocks = fs->superblock->total_blocks;
	printf("Signature: %s\n", signature);
	printf("FAT count: %d\n", fat_size);
	printf("Root index: %d\n", root_index);
	printf("Data index: %d\n", data_index);
	printf("Data Count: %d\n", data_count);
	printf("Total: %d\n", total_blocks);
	printf("Compare: %d\n", strcmp(signature, correct_name));
	/*if(strcmp(signature,"ECS150FS")!=0 || (1+fat_size)!=root_index || (root_index+1)!=data_index){
		free(fs->superblock);
		free(fs);
		return -1;
	}*/

	if(block_disk_count()!=total_blocks || (1+fat_size+1+data_count)!=total_blocks){
		free(fs->superblock);
		free(fs);
		return -1;
	}

	//fs->fat = malloc(sizeof(struct fat_blocks*));
	for(int i=0; i<fat_size; i++){
		fs->fat[i]=malloc(sizeof(struct fat_block));
	}
	for(int i=0; i<fat_size; i++){
		if(block_read(i+1, fs->fat[i])==-1){
			for(int i=0; i<fat_size; i++){
				free(fs->fat[i]);
			}
			//free(fs->fat);
			free(fs->superblock);
			free(fs);
			return -1;
		}
	}

	if(block_read(root_index, fs->root_dir)==-1){
		for(int i=0; i<fat_size; i++){
			free(fs->fat[i]);
		}
		//free(fs->fat);
		free(fs->superblock);
		free(fs);
		return -1;
	}
	//fs->data_blocks = malloc(sizeof(struct data_block*)*data_count);
	for(int i=0; i<data_count; i++){
		fs->data_blocks[i]=malloc(sizeof(struct data_block));
	}
	for(int i=0; i<data_count; i++){
		if(block_read(data_index+i,fs->data_blocks[i])==-1){
			for(int i=0; i<fat_size; i++){
				free(fs->fat[i]);
			}
			for(int j=0; j<data_count; j++){
				free(fs->data_blocks[i]);
			}
			//free(fs->data_blocks);
			//free(fs->fat);
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
	uint8_t fat_size = fs->superblock->fat_size;
	uint16_t data_count = fs->superblock->data_size;
	for(int i=0; i<fat_size; i++){
		free(fs->fat[i]);
	}
	for(int j=0; j<data_count; j++){
		free(fs->data_blocks[j]);
	}
	//free(fs->data_blocks);
	//free(fs->fat);
	free(fs->superblock);
	free(fs);
	//Add check for open file descriptors
	if(block_disk_close()==-1){
		return -1;
	}
	return 0;
}

int fs_info(void)
{
	printf("FS Info:\n");
	uint8_t fat_size = fs->superblock->fat_size;
	uint16_t root_index = fs->superblock->root_index;
	uint16_t data_index = fs->superblock->data_index;
	uint16_t data_count = fs->superblock->data_size;
	uint16_t total_blocks = fs->superblock->total_blocks;
	int free_fat_blocks=0;
	int free_root_space=0;
	for(int i=0; i<fat_size;i++){
		for(int k=0; k<2048; k++){
			if(fs->fat[i]->fat_array[k]==0){
				free_fat_blocks++;
			}
		}
	}
	for(int j=0; j<128; j++){
		if(fs->root_dir[j].filename[0]=='\0'){
			free_root_space++;
		}
	}
	printf("total_blk_count=%d\n", total_blocks);
	printf("fat_blk_count=%d\n", fat_size);
	printf("rdir_blk=%d\n", root_index);
	printf("data_blk=%d\n", data_index);
	printf("data_blk_count=%d\n", data_count);
	printf("fat_free_ratio=%d/%d\n", free_fat_blocks, fat_size*2048);
	printf("rdir_free_ratio=%d/128\n", free_root_space);
	return -1;
}

int fs_create(const char *filename)
{
	if(filename){
		return -1;
	}
	return 0;
}

int fs_delete(const char *filename)
{
	if(filename){
		return -1;
	}
	return 0;
}

int fs_ls(void)
{
	return -1;
}

int fs_open(const char *filename)
{
	if(filename){
		return -1;
	}
	return -1;
}

int fs_close(int fd)
{
	if(fd){
		return -1;
	}
	return 0;
}

int fs_stat(int fd)
{
	if(fd){
		return -1;
	}
	return -1;
}

int fs_lseek(int fd, size_t offset)
{
	if(fd || offset){
		return -1;
	}
	return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	if(fd || buf || count){
		return -1;
	}
	return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	if(fd || buf || count){
		return -1;
	}
	return 0;
}
