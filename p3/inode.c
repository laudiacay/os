#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mfs.h"
#include "dblock.h"
#include "bitmap.h"

int inode_loc(int inum) {
    assert(inum < NUM_BLOCKS);
    return INODE_TABLE_LOC + sizeof(MFS_INode_t) * inum;
}

void inode_seek_loc(int fd, int inum) {
    int offset = inode_loc(inum);
    lseek(fd, offset, SEEK_SET);
}

int inode_get_use(int fd, int inum) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    return bitmap_get_bit(fd, inum);
}

void inode_set_use(int fd, int inum, int val) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    bitmap_set_bit(fd, inum, val);
}

int inode_find_space(int fd) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    return bitmap_find_first_free(fd, NUM_BLOCKS);
}

MFS_INode_t* get_inode(int fd, int inum) {
    if (inum < 0 || inum >= NUM_BLOCKS || !inode_get_use(fd, inum)) {
        return NULL;
    }
    MFS_INode_t* mfsinode = malloc(sizeof(MFS_INode_t));
    inode_seek_loc(fd, inum);
    read(fd, mfsinode, sizeof(MFS_INode_t));
    return mfsinode;
}

void pad_file_to_n_blocks(int fd, int inum, int blocks) {
    MFS_INode_t* inode = get_inode(fd, inum);
    assert(inode);
    assert(blocks <= 10);
    assert(inode->stat_info.type == MFS_REGULAR_FILE);
    if (inode->stat_info.blocks >= blocks) return;
    for (int i = inode->stat_info.blocks; i < blocks; i++) {
        inode->block_nums[i] = dblock_find_space(fd);
        dblock_set_use(fd, inode->block_nums[i], 1);
    }
    inode->stat_info.blocks = blocks;
    inode_seek_loc(fd, inum);
    write(fd, inode, sizeof(MFS_INode_t));
    free(inode);
    fsync(fd);
}
