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
    }

    int fd = open(fs_img_name, 0);
    assert(fd != -1);
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
    for (int i = 0; i < inode->stat_info.size/SUBDIR_SIZE - 1; i++) {
        if (!dirent_in_use(fd, inode, i)) continue;
        else {
            bnum = (i + 1) / 16;
            boff = (i + 1) % 16;
            dirent_seek_loc(fd, inode->block_nums[bnum], boff);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
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
    MFS_INode_t* pinode = get_inode(fd, pinum);
    if (!pinode) return -1;
    if (pinode->stat_info.type != MFS_DIRECTORY) {
        free(pinode);
        return -1;
    }

    int inum = inode_find_space(fd);

    if (type == MFS_REGULAR_FILE) {
        printf("creating normal file %s in inum %d\n", name, inum);
        inode_set_use(fd, inum, 1);
        printf("usage of inum %d: %d\n", 0, inode_get_use(fd, 0));
        printf("usage of inum %d: %d\n", inum, inode_get_use(fd, inum));
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
