/*creates fresh sfs*/
void create_fresh_disk();

/*restores sfs using data on "disk"*/
void boot_disk();

/*builds file from inode*/
void* read_dir(int inode);

/*writes file using inode*/
void write_dir(int inode);
