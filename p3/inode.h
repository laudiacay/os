#ifndef __INODE_H
#define __INODE_H

#include "mfs.h"

int inode_loc(int inum);
void inode_seek_loc(int fd, int inum);
int inode_get_use(int fd, int inum);
void inode_set_use(int fd, int inum, int val);
int inode_find_space(int fd);
MFS_INode_t* get_inode(int fd, int inum);
void pad_file_to_n_blocks(int fd, int inum, int blocks);

#endif
