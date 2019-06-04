#include "file.h"
#include "inode.h"
#include "mfs.h"
#include "dblock.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void pad_file_to_n_blocks(int fd, int inum, int blocks) {
    MFS_INode_t* inode = get_inode(fd, inum);
    printf("padding to %d blocks\n", blocks);
    assert(inode);
    assert(blocks <= 10);
    assert(inode->stat_info.type == MFS_REGULAR_FILE);
    if (inode->stat_info.blocks >= blocks) return;
    for (int i = inode->stat_info.blocks; i < blocks; i++) {
        printf("i=%d\n", i);
        inode->block_nums[i] = dblock_find_space(fd);
        dblock_set_use(fd, inode->block_nums[i], 1);
    }
    inode->stat_info.blocks = blocks;
    inode_seek_loc(fd, inum);
    write(fd, inode, sizeof(MFS_INode_t));
    free(inode);
    fsync(fd);
}
