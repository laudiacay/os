#ifndef __INODE_H
#define __INODE_H

int inode_loc(int inum);
void inode_seek_loc(int inum);
int inode_get_use(int inum);
void inode_set_use(int inum, int val);
int inode_find_space();
MFS_INode_t* get_inode(int inum);

#endif
