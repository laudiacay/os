#ifndef _FSOPS_H
#define _FSOPS_H

#include "mfs.h"

int open_create_filesystem_image(char* fs_img_name);
int mfs_lookup(int fd, int pinum, char* name);
int mfs_creat(int fd, int pinum, int type, char* name);
int mfs_stat(int fd, int inum, MFS_Stat_t* statinfo_buf);
int mfs_write(int fd, int inum, int block, char* buffer);
int mfs_read(int fd, int inum, int block, char* buffer);
int mfs_unlink(int fd, int pinum, char* name);

#endif
