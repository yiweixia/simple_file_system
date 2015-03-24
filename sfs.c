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
	int mode;
	int link_cnt;
	int uid;
	int gid;
	int size;
} Inode;


/*bitmap that stores whether or not block is used*/
unsigned char bitmap[(BLOCK_NUM/8) + 1]

void create_fresh_disk() {

	Superblock s;
	Superblock * super_ptr;

	/*initializes 5 mb virtual disk*/
	init_fresh_disk("dat_virtual_filesystem_doe", BLOCK_SIZE, BLOCK_NUM);

	/*creating superblock*/
	s = {0, BLOCK_SIZE, BLOCK_NUM, INODE_NUM, 0};
	super_ptr = &s;
	write_superblock(super_ptr);

	/*creating bitmap*/
	bitmap_flip(0); /*superblock*/
	bitmap_flip(1); /*inode 1 for root*/
	bitmap_flip(BLOCK_NUM-1);/*bitmap*/



}

/*writes the information in s to the superblock*/
void write_superblock(Superblock * s){

	void* tempblock = malloc(BLOCK_SIZE);
	void* blockinc = tempblock;

	*blockinc = s.magic;
	*blockinc +=4;
	*blockinc = s.block_size;
	*blockinc +=4;
	*blockinc = s.file_system_size;
	*blockinc +=4;
	*blockinc = s.inode_table_length;
	*blockinc +=4;
	*blockinc = root_directory_inode;

	write_blocks(0, 1, tempblock)

	free(tempblock); 

}

/*switches the bit at block*/
void bitmap_flip(int block) {


	int bitholder = bitmap[block/8];
	int offset = block % 8;

	bitholder = bitholder ^ (1 << offset);

	bitmap[block/8] = bitholder;

	write(BLOCK_NUM-1, 1, &bitmap)
}

