#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "disk.h"
#include "fs.h"

#define FAT_EOC 0xFFFF

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

/* ---------------- Root Directory Data Structure ---------------- */

struct  __attribute__((__packed__)) root_directory{
	char filename[16];
	uint32_t size;
	uint16_t data_index;
	uint8_t padding[10];
};

bool no_file_exists(struct root_directory entry) {
	return entry.filename[0] == '\0';
}

bool has_same_filename(struct root_directory entry, const char *filename) {
	if (no_file_exists(entry)) {
		return false;
	}
	return strcmp(entry.filename, filename) == 0;
}

bool filename_is_invalid(const char *filename) {
	return filename[strlen(filename)] != '\0';
}

/* ---------------- File System Data Structure ---------------- */

struct file_system* fs;

struct  __attribute__((__packed__)) data_block{
	uint8_t data[4096];
};

struct  __attribute__((__packed__)) file_system{
	struct superblock* superblock;
	struct fat_block* fat[4];
	struct root_directory root_dir[128];
	struct data_block* data_blocks[8192];
};


int fs_find_empty_entry(const char *filename) {
	int ind = 0;
	while (ind < FS_FILE_MAX_COUNT && !no_file_exists(fs->root_dir[ind])) {
		if (has_same_filename(fs->root_dir[ind], filename)) {
			return -1;
		}
		ind++; 
	}

	if (ind == FS_FILE_MAX_COUNT) {
		return -1;
	}
	return ind;
}

int fs_find_matching_entry(const char *filename) {
	int ind = 0;
	while (ind < FS_FILE_MAX_COUNT && !has_same_filename(fs->root_dir[ind], filename)) {
		ind++;
	}

	if (ind == FS_FILE_MAX_COUNT) {
		return -1;
	}
	
	return ind;
}

/* ---------------- Open File Table Data Structure ---------------- */

struct  __attribute__((__packed__)) fd_entry{
	bool empty;
	int index_in_rootdir;
	int offset;
};

struct fd_entry open_files[FS_OPEN_MAX_COUNT];

bool open_table_has_filename(const char *filename) {
	for (int i = 0; i < FS_OPEN_MAX_COUNT; i++) {
		if (open_files[i].empty) {
			continue;
		}
		struct root_directory root_dir_entry = fs->root_dir[open_files[i].index_in_rootdir];
		if (has_same_filename(root_dir_entry, filename)) {
			return true;
		}
	}
	return false;
}

int open_table_find_empty_entry() {
	int ind_to_open = 0;
	while (ind_to_open < FS_OPEN_MAX_COUNT && !open_files[ind_to_open].empty) {
		ind_to_open++;
	}
	if (ind_to_open == FS_OPEN_MAX_COUNT) {
		return -1;
	}
	return ind_to_open;
}

bool fd_is_invalid(int fd) {
	return fd < 0 || fd >= FS_OPEN_MAX_COUNT;
}

/* ---------------- File System Methods ---------------- */

int fs_mount(const char *diskname)
{
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
	char* signature = malloc(9);
	strcpy(signature, fs->superblock->signature);
	//strcpy(comparison, signature);
	signature[8]='\0';
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
	if(strcmp(signature,"ECS150FS")!=0 || (1+fat_size)!=root_index || (root_index+1)!=data_index){
		free(signature);
		free(fs->superblock);
		free(fs);
		return -1;
	}
	free(signature);

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
	if(fs->fat[0]->fat_array[0]!=FAT_EOC){
		free(fs->superblock);
		free(fs);
		return -1;
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

	// initialize open files
	for (int i = 0; i < FS_OPEN_MAX_COUNT; i++) {
		open_files[i].empty = true;
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
	if(fs == NULL || filename == NULL || strlen(filename) >= FS_FILENAME_LEN || filename_is_invalid(filename)){
		return -1;
	}	

	int ind_to_add = fs_find_empty_entry(filename);
	if(ind_to_add == -1) {
		return -1;
	} else {
		strncpy(fs->root_dir[ind_to_add].filename, filename, strlen(filename));
		fs->root_dir[ind_to_add].filename[strlen(filename)] = '\0';
		fs->root_dir[ind_to_add].size = 0;
		fs->root_dir[ind_to_add].data_index = FAT_EOC;
		return 0;
	}
}

int fs_delete(const char *filename)
{
	if(fs == NULL || filename == NULL || filename_is_invalid(filename) || open_table_has_filename(filename)){
		return -1;
	}
	
	int ind_to_delete = fs_find_matching_entry(filename);
	if (ind_to_delete == -1) {
		return -1;
	}

	fs->root_dir[ind_to_delete].filename[0] = '\0';
	return 0;
}

int fs_ls(void)
{
	if (fs == NULL) {
		return -1;
	}

	printf("FS Ls:\n");
	for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
		if (no_file_exists(fs->root_dir[i])) {
			continue;
		}
		struct root_directory entry = fs->root_dir[i];
		printf("file: %s, size: %d, data_blk: %d\n", entry.filename, entry.size, entry.data_index);
	}
	
	return 0;
}

int fs_open(const char *filename)
{
	if(fs == NULL || filename == NULL || filename_is_invalid(filename)){
		return -1;
	}
	int index_in_rootdir = fs_find_matching_entry(filename);
	if (index_in_rootdir == -1) {
		return -1;
	}

	int ind_to_open = open_table_find_empty_entry();
	if (ind_to_open == -1) {
		return -1;
	}

	open_files[ind_to_open].empty = false;
	open_files[ind_to_open].index_in_rootdir = index_in_rootdir;
	open_files[ind_to_open].offset = 0;
	return ind_to_open;
}

int fs_close(int fd)
{
	if (fs == NULL || fd_is_invalid(fd) || open_files[fd].empty) {
		return -1;
	}

	open_files[fd].empty = true;
	return 0;
}

int fs_stat(int fd)
{
	if (fs == NULL || fd_is_invalid(fd) || open_files[fd].empty) {
		return -1;
	}

	return fs->root_dir[open_files[fd].index_in_rootdir].size;
}

int fs_lseek(int fd, size_t offset)
{
	if (fs == NULL || fd_is_invalid(fd) || open_files[fd].empty) {
		return -1;
	}

	struct root_directory* entry = &(fs->root_dir[open_files[fd].index_in_rootdir]);
	if (offset < entry->size) {
		open_files[fd].offset = offset;
	} else if (offset == entry->size) {
		entry->size += 1;
		open_files[fd].offset = offset;
	} else {
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
