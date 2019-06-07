#include "mfs.h"
#include "bitmap.h"
#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int dblock_loc(int dbnum) {
    assert(dbnum < NUM_BLOCKS);
    return DATA_REGION_LOC + MFS_BLOCK_SIZE * dbnum;
}

int dblock_get_use(int fd, int dbnum) {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    return bitmap_get_bit(fd, dbnum);
}

void dblock_set_use(int fd, int dbnum, int val) {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    bitmap_set_bit(fd, dbnum, val);
}

int dblock_find_space(int fd) {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    return bitmap_find_first_free(fd, NUM_BLOCKS);
}

void dblock_seek_loc(int fd, int dbnum) {
    int loc = dblock_loc(dbnum);
    lseek(fd, loc, SEEK_SET);
}
