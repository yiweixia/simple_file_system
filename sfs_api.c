#define MAXFILENAME 16
#include "sfs.h"


int mksfs(int fresh) {

	assert(fresh == 0 | fresh == 1)

	/*make a new filesystem*/
	if (fresh == 1) {

		create_fresh_disk();
	}

	else {
		boot_disk();
	}

}


int sfs_fopen(char *name) {

	get_file(inodenum(name));

}

int sfs_fclose(int fileID) {

}

int sfs_fwrite(int fileID, const char *buf, int length);
int sfs_fread(int fileID, char *buf, int length);
int sfs_fseek(int fileID, int offset);
int sfs_remove(char *file);
int sfs_get_next_filename(char* filename);
int sfs_GetFileSize(const char* path);
