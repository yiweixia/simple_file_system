#include "disk_emu.h"
#define BLOCK_SIZE 512
#define BLOCK_NUM 118
#define INODE_NUM 15

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

/*inode name association*/
typedef struct inameass{
	int inode;
	char name[16];

} Ins;

/*global variables*/
Superblock s;
Superblock * super_ptr;

/*array that stores association between name and inode*/
Ins association[INODE_NUM];

/*bitmap that stores whether or not block is used*/
unsigned char bitmap[(BLOCK_NUM/8) + 1]

int current; /*for sfs_fetnextfilename*/
int freeinodes = -1;
int freeblocks = -1;


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
	freeblocks = BLOCK_NUM - INODE_NUM - 2;

}

void boot_disk() {

	void* tempblock = malloc(BLOCK_SIZE);
	read_blocks(0, 1, tempblock);
	memcpy(s, tempblock, 20);
	read_blocks(1, 1, tempblock);
	memcpy(a, tempblock, 300);
	read_blocks(BLOCK_NUM-1, 1, tempblock);
	memcpy(bitmap, tempblock, (BLOCK_NUM/8) + 1);

	free(tempblock);
	
	/*calculating free blocks*/
	int i, j, switch = 0;
	for (i = 0; i < BLOCK_NUM / 8 + 1; i++) {
		for (j = 0; j < 8; j++) {
			if ((bitmap[i] << j) %2 == 1) {
				if (switch > 16)
					freeinodes++;
				else
					freeblocks++;
			}
			switch++;
		}
	}
}

/*writes the information in s to the superblock*/
void write_superblock(Superblock * s){

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


Inode* read_inode(int block) {

	void* tempblock = malloc(BLOCK_SIZE);

	read_blocks(block, 1, tempblock);

	return tempblock;


}

void write_inode(Inode i, int block) {
	
}


void create_inode(char* filename, int block) {



	write_inode()
}

/*switches the bit at block*/
void bitmap_flip(int block) {

	int bitholder = bitmap[block/8];
	int offset = block % 8;

	bitholder = bitholder ^ (1 << offset);

	bitmap[block/8] = bitholder;

	write(BLOCK_NUM-1, 1, &bitmap)
}

/*formats block to 0s*/
void zeroblock (int block) {

	char* tempblock = malloc(BLOCK_SIZE);
	char* blockinc = tempblock;
	int i;

	for (i = 0; i < BLOCK_SIZE; i++) {
		*blockinc = 0;
		blockinc ++;
	}

	write(block, 1, tempblock);

	free(tempblock);
}
