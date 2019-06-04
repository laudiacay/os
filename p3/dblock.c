int dblock_loc(int dbnum) {
    assert(dbnum < NUM_BLOCKS);
    return DATA_REGION_LOC + MFS_BLOCK_SIZE * dbnum;
}

int dblock_get_use(int dbnum) {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    return bitmap_get_bit(dbnum);
}

void dblock_set_use(int dbnum, int val) {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    bitmap_set_bit(dbnum, val);
}

int dblock_find_space() {
    lseek(fd, DBLOCK_BITMAP_LOC, SEEK_SET);
    return bitmap_find_first_free(NUM_BLOCKS);
}

void dblock_seek_loc(int dbnum) {
    int loc = dblock_loc(dbnum);
    lseek(fd, loc, SEEK_SET);
}
