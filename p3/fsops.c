#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <libexplain/open.h>

#include "fsops.h"
#include "inode.h"
#include "mfs.h"
#include "dblock.h"
#include "file.h"
#include "directory.h"

void create_filesystem_image(char* fs_img_name) {
    // create file
    int fd = open(fs_img_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    assert(fd >= 0);

    char* buf = calloc(MFS_BLOCK_SIZE, sizeof(char));

    // write out inode bitmap as all zeros
    write(fd, buf, BITMAP_SIZE);

    // write out data block bitmap as all zeros
    write(fd, buf, BITMAP_SIZE);

    // write out inode table as all zeros
    for (int i = 0; i < NUM_BLOCKS; i++) {
        write(fd, buf, sizeof(MFS_INode_t));
    }

    // write out data region as all zeros
    for (int i = 0; i < NUM_BLOCKS; i++) {
        write(fd, buf, MFS_BLOCK_SIZE);
    }

    // create root directory at inode 0
    // mark inode bitmap and data bitmap as needed
    make_root(fd);

    // flush to disk, close newly initialized FS
    assert(!fsync(fd));
    assert(!close(fd));
}

int validate_filesystem() {
    // TODO
    return 0;
}

// return file descriptor of fs image
// otherwise break on assertion
int open_create_filesystem_image(char* fs_img_name) {
    // if the filesystem doesn't exist, initialize it!
    if (access(fs_img_name, R_OK|W_OK) == -1) {
        create_filesystem_image(fs_img_name);
        //printf("CREATED FILESYSTEM!\n");
    }

    int fd = open(fs_img_name, O_RDWR);
    assert(fd != -1);

    //printf("FILESYSTEM READY!\n");
    return fd;
}


int mfs_lookup(int fd, int pinum, char* name) {
    MFS_INode_t* inode = get_inode(fd, pinum);
    MFS_DirEnt_t dirent;
    int ret = -1;
    if (!inode) {
        //printf("returning -1\n");
        return ret;
    }
    if (!inode->stat_info.type == MFS_DIRECTORY) {
        free(inode);
        return ret;
    }
    int bnum, boff;
    for (int i = 0; i < inode->stat_info.size/SUBDIR_SIZE; i++) {
        //printf("dirent %d\n", i);
        if (!dirent_in_use(fd, inode, i)) {
            //printf("not in use\n");
            continue;
        }
        else {
            bnum = i / 16;
            boff = i % 16;
            dirent_seek_loc(fd, inode->block_nums[bnum], boff);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            //printf("%s\n", dirent.name);
            if (strcmp(dirent.name, name) != 0) continue;
            ret = dirent.inum;
            break;
        }
    }
    free(inode);
    return ret;
}

int mfs_creat(int fd, int pinum, int type, char* name) {
    assert(type == MFS_DIRECTORY || type == MFS_REGULAR_FILE);

    if (mfs_lookup(fd, pinum, name) >= 0) return 0;

    //printf("creating something!\n");
    MFS_INode_t* pinode = get_inode(fd, pinum);
    if (!pinode) return -1;
    if (pinode->stat_info.type != MFS_DIRECTORY) {
        free(pinode);
        return -1;
    }

    int inum = inode_find_space(fd);

    if (type == MFS_REGULAR_FILE) {
        //printf("creating normal file %s in inum %d\n", name, inum);
        inode_set_use(fd, inum, 1);
        //printf("usage of inum %d: %d\n", 0, inode_get_use(fd, 0));
        //printf("usage of inum %d: %d\n", inum, inode_get_use(fd, inum));
        //printf("** adding file to pinum's directory\n");
        add_direntry(fd, pinum, inum, name);
        MFS_INode_t* inode = calloc(1, sizeof(MFS_INode_t));
        inode->stat_info.type = MFS_REGULAR_FILE;
        inode->stat_info.size = 0;
        inode->stat_info.blocks = 0;
        inode_seek_loc(fd, inum);
        write(fd, inode, sizeof(MFS_INode_t));
        free(pinode);
        free(inode);
        return 0;
    }
    free(pinode);
    make_dir(fd, pinum, inum, name);
    return 0;
}

int mfs_stat(int fd, int inum, MFS_Stat_t* statinfo_buf) {
    if (inode_get_use(fd, inum) != 1) return -1;
    inode_seek_loc(fd, inum);
    read(fd, statinfo_buf, sizeof(MFS_Stat_t));
    //printf("read in!\n");
    //printf("type: %d, size: %d, blocks %d\n", statinfo_buf->type, statinfo_buf->size, statinfo_buf->blocks);
    return 0;
}


/*int MFS_Write(int inum, char *buffer, int block): MFS_Write() writes a block of size 4096 bytes at the block offset specified by block . Returns 0 on success, -1 on failure. Failure modes: invalid inum, invalid block, not a regular file (you can't write to directories).
*/
int mfs_write(int fd, int inum, int block, char* buffer) {
    if (block >= 10) return -1;
    if (block < 0) return -1;
    MFS_INode_t* inode = get_inode(fd, inum);
    //printf("ptr: %ld\n", (long int)inode);
    if (!inode) return -1;
    if (inode->stat_info.type == MFS_DIRECTORY) {
        free(inode);
        return -1;
    }
    free(inode);
    pad_file_to_n_blocks(fd, inum, block+1);
    inode = get_inode(fd, inum);
    inode->stat_info.size = (block + 1) * MFS_BLOCK_SIZE;

    inode_seek_loc(fd, inum);
    write(fd, inode, sizeof(MFS_INode_t));

    dblock_seek_loc(fd, inode->block_nums[block]);
    write(fd, buffer, MFS_BLOCK_SIZE);
    free(inode);
    fsync(fd);
    return 0;
}


/*int MFS_Read(int inum, char *buffer, int block): MFS_Read() reads a block specified by block into the buffer from file specified by inum . The routine should work for either a file or directory; directories should return data in the format specified by MFS_DirEnt_t. Success: 0, failure: -1. Failure modes: invalid inum, invalid block.
*/
int mfs_read(int fd, int inum, int block, char* buffer) {
    if (block >= 10) return -1;
    if (block < 0) return -1;
    if (inum < 0) return -1;
    if (inum >= NUM_BLOCKS) return -1;

    MFS_INode_t* inode = get_inode(fd, inum);
    if (!inode) return -1;
    if (inode->stat_info.blocks <= block) return -1;

    dblock_seek_loc(fd, inode->block_nums[block]);
    read(fd, buffer, MFS_BLOCK_SIZE);
    free(inode);
    return 0;
}

/*
int MFS_Unlink(int pinum, char *name): MFS_Unlink() removes the file or directory name from the directory specified by pinum . 0 on success, -1 on failure. Failure modes: pinum does not exist, pinum does not represent a directory, the to-be-unlinked directory is NOT empty. Note that the name not existing is NOT a failure by our definition (think about why this might be).
*/
int mfs_unlink(int fd, int pinum, char* name) {
    printf("UNLINKING %s FROM INODE %d\n", name, pinum);
    if (pinum < 0) return -1;
    MFS_INode_t* inode = get_inode(fd, pinum);

    if (!inode) return -1;
    if (!inode->stat_info.type == MFS_DIRECTORY) {
        free(inode);
        return -1;
    }
    MFS_DirEnt_t dirent;

    int bnum, boff;
    int inum_to_remove = -1;
    for (int i = 0; i < inode->stat_info.size/SUBDIR_SIZE; i++) {
        if (!dirent_in_use(fd, inode, i)) {
            printf("dirent i unused\n");
            continue;
        }
        else {
            bnum = i / 16;
            boff = i % 16;
            dirent_seek_loc(fd, inode->block_nums[bnum], boff);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            if (strcmp(dirent.name, name) != 0) continue;
            inum_to_remove = dirent.inum;
            break;
        }
    }
    if (inum_to_remove == -1) {
        printf("either not found or not in use\n");
        free(inode);
        return 0;
    }

    // if it's a dir, check if it's empty- if no, return -1
    MFS_INode_t* inode_to_remove = get_inode(fd, inum_to_remove);
    if (inode_to_remove->stat_info.type == MFS_DIRECTORY) {
        if (!dir_is_empty(fd, inum_to_remove)) {
            free(inode_to_remove);
            free(inode);
            return -1;
        }
    }

    // set to -1 in the parent's dir table
    int unused = -1;
    dirent_seek_loc(fd, inode->block_nums[bnum], boff);
    write(fd, &unused, sizeof(int));

    // mark the child's blocks as unused
    for (int i = 0; i < inode_to_remove->stat_info.blocks; i++) {
        dblock_set_use(fd, inode_to_remove->block_nums[i], 0);
    }

    // mark the child as unused
    inode_set_use(fd, inum_to_remove, 0);

    // TODO: consolidate the parent's dir table?
    defrag_directory(fd, pinum);
    free(inode);
    free(inode_to_remove);
    return 0;
}
