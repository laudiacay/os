int inode_loc(int inum) {
    assert(inum < NUM_BLOCKS);
    return INODE_TABLE_LOC + sizeof(MFS_INode_t) * inum;
}

void inode_seek_loc(int inum) {
    int offset = inode_loc(inum);
    lseek(fd, inum, SEEK_SET);
}

int inode_get_use(int inum) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    return bitmap_get_bit(NUM_BLOCKS);
}

void inode_set_use(int inum, int val) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    bitmap_set_bit(inum, val);
}

int inode_find_space() {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    return bitmap_find_first_free(NUM_BLOCKS);
}

MFS_INode_t* get_inode(int inum) {
    if (!inode_get_use(inum)) return NULL;
    mfsinode = malloc(sizeof(MFS_INode_t));
    inode_seek_loc(inum);
    read(fd, mfsinode, sizeof(MFS_INode_t));
    return mfsinode;
}
