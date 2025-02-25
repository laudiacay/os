#ifndef _DIRECTORY_H
#define _DIRECTORY_H

void add_direntry(int fd, int pinum, int inum, char* name);
void make_root(int fd);
void make_dir(int fd, int pinum, int inum, char* name);
int dirent_in_use(int fd, MFS_INode_t* inode, int dirent);
void dirent_seek_loc(int fd, int bnum, int boff);
int dir_is_empty(int fd, int inum);
void defrag_directory(int fd, int inum);

#endif
