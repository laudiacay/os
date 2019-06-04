#ifndef __DBLOCK_H
#define __DBLOCK_H

int dblock_loc(int dbnum);
int dblock_get_use(int dbnum);
void dblock_set_use(int dbnum, int val);
int dblock_find_space();
void dblock_seek_loc(int dbnum);

#endif
