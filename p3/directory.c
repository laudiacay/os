#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mfs.h"
#include "inode.h"
#include "dblock.h"
#include "bitmap.h"

void dirent_seek_loc(int fd, int bnum, int boff) {
    lseek(fd, dblock_loc(bnum), SEEK_SET);
    lseek(fd, boff * SUBDIR_SIZE, SEEK_CUR);
}

int dirent_in_use(int fd, MFS_INode_t* inode, int dnum) {
    if (dnum/16 > inode->stat_info.blocks) return 0;
    dirent_seek_loc(fd, inode->block_nums[dnum/16], dnum%16);
    MFS_DirEnt_t dirent;
    read(fd, &dirent, sizeof(MFS_DirEnt_t));
    return dirent.inum != -1;
}

int get_first_free_dirent(int fd, MFS_INode_t* inode) {
    assert(inode);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    unsigned int num_subdirs = inode->stat_info.size/sizeof(MFS_DirEnt_t);
    for (unsigned int i = 0; i < num_subdirs ; i++) {
        if (dirent_in_use(fd, inode, i)) continue;
        return i;
    }
    if (num_subdirs == MAX_NUM_SUBDIRS)
        return -1;

    return num_subdirs;
}

// adds direntry to p's directory table
void add_direntry(int fd, int pinum, int inum, char* name) {

    MFS_INode_t* inode = get_inode(fd, pinum);
    assert(inode);
    int n_blocks = inode->stat_info.blocks;

    int first_free = get_first_free_dirent(fd, inode);

    int dir_bnum = first_free / 16;
    int dir_boff = first_free % 16;

    assert(dir_bnum < 10);

    if (dir_bnum >= n_blocks) {
        int new_dblock = dblock_find_space(fd);
        dblock_set_use(fd, new_dblock, 1);
        inode->block_nums[n_blocks] = new_dblock;
        inode->stat_info.blocks += 1;
    }
    if ((first_free)*sizeof(MFS_DirEnt_t) == (unsigned int) inode->stat_info.size) {
        inode->stat_info.size += SUBDIR_SIZE;
    }

    MFS_DirEnt_t dirent;
    dirent.inum = inum;
    strncpy(dirent.name, name, MAX_FILENAME_SIZE);

    dirent_seek_loc(fd, inode->block_nums[dir_bnum], dir_boff);
    write(fd, &dirent, sizeof(MFS_DirEnt_t));

    inode_seek_loc(fd, pinum);
    write(fd, inode, sizeof(MFS_INode_t));

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
        if (dirent_in_use(fd, inode, i)) {
            if (last_empty_dirent == num_empty_dirent_inds) continue;
            dirent_seek_loc(fd, inode->block_nums[i/16], i % 16);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            dest = empty_dirent_inds[last_empty_dirent];
            dirent_seek_loc(fd, inode->block_nums[dest/16], dest % 16);
            write(fd, &dirent, sizeof(MFS_DirEnt_t));
            last_empty_dirent++;
        } else {
            empty_dirent_inds[num_empty_dirent_inds] = i;
            num_empty_dirent_inds++;
            new_num_dirents--;
        }
    }

    inode->stat_info.size = new_num_dirents * sizeof(MFS_DirEnt_t);
    int old_blocks = inode->stat_info.blocks;
    inode->stat_info.blocks = (new_num_dirents/16) + 1;
    inode_seek_loc(fd, inum);
    write(fd, inode, sizeof(MFS_INode_t));

    for (int i = inode->stat_info.blocks; i < old_blocks; i++) {
        dblock_set_use(fd, inode->block_nums[i], 0);
    }

    fsync(fd);
    return;
}

int init_first_block(int fd) {
    int dblock = dblock_find_space(fd);
    dblock_set_use(fd, dblock, 1);
    fsync(fd);
    return dblock;
}

void init_dir_inode(int fd, int inum) {
    inode_set_use(fd, inum, 1);

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
    init_dir_inode(fd, 0);
    add_direntry(fd, 0, 0, ".");
    add_direntry(fd, 0, 0, "..");
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
        if (dirent_in_use(fd, inode, i)) return 0;
    }
    return 1;
}
