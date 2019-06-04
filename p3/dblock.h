#ifndef __DBLOCK_H
#define __DBLOCK_H

int dblock_loc(int dbnum);
int dblock_get_use(int fd, int dbnum);
void dblock_set_use(int fd, int dbnum, int val);
int dblock_find_space(int fd);
void dblock_seek_loc(int fd, int dbnum);

#endif
