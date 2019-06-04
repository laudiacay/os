#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "mfs.h"
#include "bitmap.h"

int inode_loc(int inum) {
    assert(inum < NUM_BLOCKS);
    return INODE_TABLE_LOC + sizeof(MFS_INode_t) * inum;
}

void inode_seek_loc(int fd, int inum) {
    int offset = inode_loc(inum);
    //printf("going to inode loc at %d\n", offset);
    lseek(fd, offset, SEEK_SET);
}

int inode_get_use(int fd, int inum) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    printf("getting usage of inode %d\n", inum);
    return bitmap_get_bit(fd, inum);
}

void inode_set_use(int fd, int inum, int val) {
    printf("setting usage of inode %d to %d\n", inum, val);
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    bitmap_set_bit(fd, inum, val);
}

int inode_find_space(int fd) {
    lseek(fd, INODE_BITMAP_LOC, SEEK_SET);
    return bitmap_find_first_free(fd, NUM_BLOCKS);
}

MFS_INode_t* get_inode(int fd, int inum) {
    //printf("getting use of inode %d: %d\n", inum, inode_get_use(fd, inum));
    if (inum < 0 || inum >= NUM_BLOCKS || !inode_get_use(fd, inum)) {
        //printf("not in use!\n");
        return NULL;
    }
    MFS_INode_t* mfsinode = malloc(sizeof(MFS_INode_t));
    inode_seek_loc(fd, inum);
    read(fd, mfsinode, sizeof(MFS_INode_t));
    return mfsinode;
}
