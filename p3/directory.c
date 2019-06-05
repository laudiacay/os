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

int dirent_in_use(int fd, MFS_INode_t* inode, int dnum) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    int bnum = dnum / 16;
    int boff = dnum % 16;
    if (bnum > inode->stat_info.blocks) return 0;
    dirent_seek_loc(fd, inode->block_nums[bnum], boff);
    MFS_DirEnt_t dirent;
    read(fd, &dirent, sizeof(MFS_DirEnt_t));
    printf("dirent.inum: %d\n", dirent.inum);
    return dirent.inum != -1;
}

int get_first_free_dirent(int fd, MFS_INode_t* inode) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    for (unsigned int i = 0; i < inode->stat_info.size/sizeof(MFS_DirEnt_t); i++) {
        if (dirent_in_use(fd, inode, i)) continue;
        return i;
    }
    if (inode->stat_info.size/sizeof(MFS_DirEnt_t) == MAX_NUM_SUBDIRS)
        return -1;

    return inode->stat_info.size/sizeof(MFS_DirEnt_t);
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

    int dir_bnum = first_free / 16;
    int dir_boff = first_free % 16;

    assert(dir_bnum < 10);

    //printf("bnum: %d, boff: %d\n", dir_bnum, dir_boff);

    if (dir_bnum >= n_blocks) {
        //printf("incrementing blocks\n");
        int new_dblock = dblock_find_space(fd);
        dblock_set_use(fd, new_dblock, 1);
        inode->block_nums[n_blocks] = new_dblock;
        inode->stat_info.blocks += 1;
        //printf("num blocks is %d\n", inode->stat_info.blocks);
    }
    if ((first_free+1)*sizeof(MFS_DirEnt_t) > (unsigned int) inode->stat_info.size) {
        //printf("incrementing size\n");
        inode->stat_info.size += SUBDIR_SIZE;
        //printf("size is %d\n", inode->stat_info.size);
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

void defrag_directory(int fd, int inum) {
    MFS_INode_t* inode = get_inode(fd, inum);
    MFS_DirEnt_t dirent;
    int empty_dirent_inds[160];
    int dest;
    int last_empty_dirent = 0;
    int num_empty_dirent_inds = 0;
    int new_num_dirents = inode->stat_info.size/sizeof(MFS_DirEnt_t);
    for (unsigned int i = 2; i < inode->stat_info.size/sizeof(MFS_DirEnt_t); i++) {
        printf("checking dirent %u\n", i);
        if (dirent_in_use(fd, inode, i)) {
            printf("in use\n");
            if (last_empty_dirent == num_empty_dirent_inds) continue;
            dirent_seek_loc(fd, inode->block_nums[i/16], i % 16);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            dest = empty_dirent_inds[last_empty_dirent];
            dirent_seek_loc(fd, inode->block_nums[dest/16], dest % 16);
            write(fd, &dirent, sizeof(MFS_DirEnt_t));
            printf("copied into %d\n", dest);
            last_empty_dirent++;
        } else {
            empty_dirent_inds[num_empty_dirent_inds] = i;
            num_empty_dirent_inds++;
            new_num_dirents--;
        }
    }
    // set new size and blocks
    inode->stat_info.size = new_num_dirents * sizeof(MFS_DirEnt_t);
    int old_blocks = inode->stat_info.blocks;
    inode->stat_info.blocks = (new_num_dirents/16) + 1;
    inode_seek_loc(fd, inum);
    write(fd, inode, sizeof(MFS_INode_t));

    // free blocks
    for (int i = inode->stat_info.blocks; i < old_blocks; i++) {
        dblock_set_use(fd, inode->block_nums[i], 0);
    }

    fsync(fd);
    return;
}

int init_first_block(int fd) {
    //printf("in init_first_block\n");
    // create first block bitmap, write first block
    int dblock = dblock_find_space(fd);
    //printf("using dblock %d\n", dblock);
    dblock_set_use(fd, dblock, 1);
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
    inode.stat_info.size = 0;
    inode.stat_info.blocks = 0;

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
    for (unsigned int i = 2; i < inode->stat_info.size/sizeof(MFS_DirEnt_t); i++) {
        printf("checking dirent %u\n", i);
        if (dirent_in_use(fd, inode, i)) return 0;
    }
    return 1;
}
