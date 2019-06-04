#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "mfs.h"
#include "inode.h"
#include "dblock.h"
#include "bitmap.h"

void dirent_seek_loc(int fd, int bnum, int boff) {
    lseek(fd, dblock_loc(bnum), SEEK_SET);
    lseek(fd, boff * SUBDIR_SIZE, SEEK_CUR);
}

int dirent_in_use(int fd, MFS_INode_t* inode, int dirent) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    dblock_seek_loc(fd, inode->block_nums[0]);
    return bitmap_get_bit(fd, dirent);
}

int get_first_free_dirent(int fd, MFS_INode_t* inode) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    dblock_seek_loc(fd, inode->block_nums[0]);
    return bitmap_find_first_free(fd, SUBDIR_BITMAP_SIZE);
}

void set_dirent_use(int fd, MFS_INode_t* inode, int dirent, int val) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    dblock_seek_loc(fd, inode->block_nums[0]);
    bitmap_set_bit(fd, dirent, val);
}

// adds direntry to p's directory table
void add_direntry(int fd, int pinum, int inum, char* name) {
    //printf("in add_direntry pinum %d inum %d name %s\n", pinum, inum, name);

    MFS_INode_t* inode = get_inode(fd, pinum);
    assert(inode);
    //printf("got the pinode\n");
    int n_blocks = inode->stat_info.blocks;

    int first_free = get_first_free_dirent(fd, inode);
    //printf("first free dirent: %d\n", first_free);
    set_dirent_use(fd, inode, first_free, 1);

    int dir_bnum = (first_free + 1) / 16;
    int dir_boff = (first_free + 1) % 16;

    assert(dir_bnum < 10);

    //printf("bnum: %d, boff: %d\n", dir_bnum, dir_boff);

    if (dir_bnum >= n_blocks) {
        //printf("should not be here yet!\n");
        int new_dblock = dblock_find_space(fd);
        dblock_set_use(fd, new_dblock, 1);
        inode->block_nums[n_blocks] = new_dblock;
        inode->stat_info.blocks += 1;
        inode->stat_info.size += SUBDIR_SIZE;
    }

    MFS_DirEnt_t dirent;
    dirent.inum = inum;
    strncpy(dirent.name, name, MAX_FILENAME_SIZE);

    // write direntry
    dirent_seek_loc(fd, inode->block_nums[dir_bnum], dir_boff);
    write(fd, &dirent, sizeof(MFS_DirEnt_t));

    //printf("wrote new dirent!\n");

    // write inode
    inode_seek_loc(fd, pinum);
    write(fd, inode, sizeof(MFS_INode_t));

    //printf("wrote new inode!\n");

    free(inode);
    fsync(fd);
}

// TODO: implement me
void defrag_directory(int fd, int inum) {
    UNUSED(fd);
    UNUSED(inum);
    return;
}

int init_first_block(int fd) {
    //printf("in init_first_block\n");
    // create first block bitmap, write first block
    char* bitmap = calloc(SUBDIR_BITMAP_SIZE, sizeof(char));
    int dblock = dblock_find_space(fd);
    //printf("using dblock %d\n", dblock);
    dblock_set_use(fd, dblock, 1);

    dblock_seek_loc(fd, dblock);
    write(fd, bitmap, SUBDIR_BITMAP_SIZE);
    fsync(fd);
    //printf("initialized first block!\n");
    return dblock;
}

void init_dir_inode(int fd, int inum) {
    //printf("inode set use call:\n");
    inode_set_use(fd, inum, 1);
    //printf("about to check inode use:\n");
    //printf("in use? should be 1: %d\n", inode_get_use(fd, inum));

    MFS_INode_t inode;
    inode.stat_info.type = MFS_DIRECTORY;
    inode.stat_info.size = 3*SUBDIR_SIZE;
    inode.stat_info.blocks = 1;

    int first_dblock = init_first_block(fd);

    inode.block_nums[0] = first_dblock;

    inode_seek_loc(fd, inum);
    write(fd, &inode, sizeof(MFS_INode_t));
    fsync(fd);
}

void make_root(int fd) {
    //printf("file descriptor... %d\n", fd);
    init_dir_inode(fd, 0);
    //printf("in use?: %d\n", inode_get_use(fd, 0));

    add_direntry(fd, 0, 0, ".");
    add_direntry(fd, 0, 0, "..");

    //printf("MADE ROOT BITCH!\n");
}

void make_dir(int fd, int pinum, int inum, char* name) {
    init_dir_inode(fd, inum);

    add_direntry(fd, inum, inum, ".");
    add_direntry(fd, inum, pinum, "..");
    add_direntry(fd, pinum, inum, name);
}

int dir_is_empty(int fd, int inum) {
    MFS_INode_t* inode = get_inode(fd, inum);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    dblock_seek_loc(fd, inode->block_nums[0]);
    return bitmap_is_empty(fd, SUBDIR_BITMAP_SIZE);
}

/*
int dir_unlink(int fd, int pinum, char* name) {
    MFS_INode_t* inode = get_inode(fd, pinum);
    MFS_DirEnt_t dirent;
    if (!inode) return -1;
    if (!inode->stat_info.type == MFS_DIRECTORY) return -1;
    // TODO: FIX ME
    //if (!dir_is_empty(pinum)) return -1;
    int bnum, boff;
    for (int i = 0; i < inode->stat_info.size/SUBDIR_SIZE - 1; i++) {
        if (!dirent_in_use(fd, i)) continue;
        else {
            bnum = (i + 1) / 16;
            boff = (i + 1) % 16;
            goto_dirent(inode->block_nums[bnum], boff);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            if (strcmp(dirent.name, name) != 0) continue;
            set_dirent_use(pinum, i, 0);
            return 0;
        }
    }
    return 0;
}
*/
