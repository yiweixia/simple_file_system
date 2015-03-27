#include "disk_emu.h"
#define BLOCK_SIZE 512
#define BLOCK_NUM 14000
#define INODE_NUM 100
#define INODE_PER_BLOCK 7

typedef struct Superblocks{
	int magic;
	int block_size;
	int file_system_size;
	int inode_table_length;
	int root_directory_inode;
} Superblock;

typedef struct Inodes{
	int mode; /*1 for directory, 0 for file*/
	int link_cnt;
	int uid;
	int gid;
	int size;
	int block_ptr[12];
	int ind_ptr;
} Inode;

typedef struct Directorytable{
	char name[16];
	int inode;
} Dir;

typedef struct FileDescriptor{
	int inode_num;
	void* read_ptr;
	void* write_ptr;

}

/*

GLOBALS BABY

*/


/*for instantiating new inodes*/
const Inode newnode{0, 0, 0, 0, 0, {NULL}, NULL};

/*global variables*/
Superblock s;
Superblock * super_ptr;

/*array that stores association between name and inode*/
Dir association[INODE_NUM];

/*bitmap that stores whether or not block is used*/
unsigned char bitmap[(BLOCK_NUM/8) + 1];

int fileDescriptorTable[]

int current; /*for sfs_fetnextfilename*/
int freeinodes = -1; 
int freedblocks = -1;

Inode inode_cache[3];

void* read_ptr;
void* write_ptr;



/*creates fresh sfs*/
void create_fresh_disk() {
	
	Inode i;

	/*initializes 5 mb virtual disk*/
	init_fresh_disk("dat_virtual_filesystem_doe", BLOCK_SIZE, BLOCK_NUM);

	/*creating superblock*/
	s = {0, BLOCK_SIZE, BLOCK_NUM, INODE_NUM, 0};
	super_ptr = &s;
	write_superblock(super_ptr);

	/*creating root*/
	i.mode = 1;
	i.block_ptr[0] = INODE_NUM + 1;
	write_inode(i, 1);

	/*creating bitmap*/
	zeroblock(BLOCK_NUM - 1)
	bitmap_flip(0); /*superblock*/
	bitmap_flip(1); /*inode 1 for root*/
	bitmap_flip(INODE_NUM + 1) /*directory file*/
	bitmap_flip(BLOCK_NUM - 1);/*bitmap*/

	freeinodes = INODE_NUM - 1;
	freedblocks = BLOCK_NUM - INODE_NUM - 2;

}

/*restores sfs using data on "disk"*/
void boot_disk() {

	void* tempblock = malloc(BLOCK_SIZE);
	read_blocks(0, 1, tempblock);
	memcpy(s, tempblock, sizeof(s));
	read_blocks(1, 1, tempblock);
	memcpy(association, tempblock, sizeof(association));
	read_blocks(BLOCK_NUM-1, 1, tempblock);
	memcpy(bitmap, tempblock, sizeof(bitmap));

	free(tempblock);
	
	/*calculating free blocks*/
	int i, j, switch = 0;
	for (i = 0; i < BLOCK_NUM / 8 + 1; i++) {
		for (j = 0; j < 8; j++) {
			if ((bitmap[i] >> j) %2 == 1) {
				if (switch > 16)
					freeinodes++;
				else
					freedblocks++;
			}
			switch++;
		}
	}

}


/*writes the information in s to the superblock*/
static void write_superblock(Superblock * s){

	void* tempblock = malloc(BLOCK_SIZE);
	void* blockinc = tempblock;

	zeroblock(0);

	*blockinc = s.magic;
	*blockinc +=4;
	*blockinc = s.block_size;
	*blockinc +=4;
	*blockinc = s.file_system_size;
	*blockinc +=4;
	*blockinc = s.inode_table_length;
	*blockinc +=4;
	*blockinc = root_directory_inode;
	*blockinc +=4;
	*blockinc += EOF; 


	write_blocks(0, 1, tempblock)

	free(tempblock); 

}

/*LEAVES UNFREED ALLOCATED MEMORY, MUST BE FREED WHEN INODE IS USED*/
static void* read_inode(int inode_num) {

	void* tempblock = malloc(BLOCK_SIZE);
	void* inode = malloc(sizeof(Inode));

	/*finds block containing the inode*/
	int blocknum = inode_num/7 + 1;

	/*reads block into memory*/
	read_blocks(blocknum, 1, tempblock);

	/*copies the one inode into inode for returning*/
	memcpy(inode, tempblock+inode_num%7, sizeof(Inode));

	free(tempblock);

	return inode;

}

static void write_inode(Inode i, int block) {

	void* tempblock = malloc(BLOCK_SIZE);
	memcpy (tempblock, i, sizeof(i));
	write(block, 1, tempblock);
	free(tempblock);
	
}

/*attempts to create inode, returns 1 if successful, 0 if no free inodes*/
static int create_inode(char* filename) {

	if (freeinodes > 0){
		int inc, firstfree;

		firstfree = first_free_inode();

		for(inc = 0; inc < INODE_NUM; inc++){
			if (association[inc] == NULL){
				association[inc] == (Dir){filename, firstfree};
				break;
			}
		}

		Inode i = newnode; 
		write_inode(i, firstfree);
		bitmap_flip(firstfree);
		freeinodes--;
		return 1;

	}
	else {
		printf("sorry, no more inodes </3");
		return 0;
	}
	
}

/*switches the bit at block*/
static void bitmap_flip(int block) {

	int bitholder = bitmap[block/8];
	int offset = block % 8;

	bitholder = bitholder ^ (1 << offset);

	bitmap[block/8] = bitholder;

	write(BLOCK_NUM-1, 1, &bitmap)
}

/*finds first unused inode, must have inode free*/
static int first_free_inode() {

	assert (freeinodes > 0);

	/*searching*/
	int i, j, switch = 0;
	for (i = 0; i < BLOCK_NUM / 8 + 1; i++) {
		for (j = 0; j < 8; j++) {
			if ((bitmap[i] >> j) %2 == 1) {
				if (switch > 0)
					return switch;
			}
			switch++;
		}
	}

}

/*finds first unused data block, must have data block free*/
static int first_free_dblock() {

	assert (freedblocks > 0);

	/*searching*/
	int i, j, switch = 0;
	for (i = 0; i < BLOCK_NUM / 8 + 1; i++) {
		for (j = 0; j < 8; j++) {
			if ((bitmap[i] >> j) %2 == 1) {
				if (switch > INODE_NUM+1);
					return switch;
			}
			switch++;
		}
	}

}

/*builds file from inode*/
void* get_file(int inode) {

	Inode i = *read_inode(inode);
	void* data = malloc((i.size/BLOCK_SIZE + 1)*BLOCK_SIZE);
	char* appender = data; 

	if (i.size > 12*BLOCK_SIZE) {
		memcpy()
	}
	else {
		int inc;
		int numblocks = (i.size/ BLOCK_SIZE) + 1;
		for (inc = 0; inc < numblocks; inc++) {
			read(i.block_ptr[inc], 1, appender);
			appender += BLOCK_SIZE;
		}
	}

	return data;

}

/*writes file using inode. returns -1 if not enough space*/
int put_file(int inode, int size, char* buffer) {

	if (size/BLOCK_SIZE + 1 > freedblocks)
		return -1;




}

/*formats block to 0s*/
static void zeroblock (int block) {

	void* tempblock = malloc(BLOCK_SIZE);
	void* blockinc = tempblock;
	int i;

	for (i = 0; i < BLOCK_SIZE; i++) {
		*blockinc = 0;
		blockinc ++;
	}

	write(block, 1, tempblock);

	free(tempblock);
	
}

/*find inode for for this name. returns -1 if can't be found*/
int inodenum(char* name) {

	int i;
	for(i = 0; i < INODE_NUM; i++) {
		if (strcmp(name, association[i].name) == 0)
			return associaton[i].inode;
	}

	return -1;
}