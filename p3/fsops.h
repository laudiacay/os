#ifndef _FSOPS_H
#define _FSOPS_H

int open_create_filesystem_image(char* fs_img_name);
int mfs_lookup(int fd, int pinum, char* name);
int mfs_creat(int fd, int pinum, int type, char* name);

#endif
