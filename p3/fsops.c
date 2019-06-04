#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "lib/fsops.h"
#include "lib/types.h"

// TODO MAKE SURE ALL FAILABLE CALLS ASSERT
// TODO MAKE SURE YOU ALWAYS FLUSH BUFFERS

// return file descriptor of fs image
// otherwise break on assertion
void open_create_filesystem_image(char* fs_img_name) {
    int rc;

    // if the filesystem doesn't exist, initialize it!
    if (access(fs_img_name, R_OK|W_OK) == -1) {
        rc = create_filesystem_image(fs_img_name);
    }

    fd = open(fs_img_name, NULL);
    assert(fd != -1);
}

int create_filesystem_image(char* fs_img_name) {
    // create file
    fd = creat(fs_img_name, S_IRUSR|S_IWUSR);
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
    mkroot();

    // flush to disk, close newly initialized FS
    assert(!fsync(rc));
    assert(!close(rc));
    return 1;
}

int validate_filesystem(int fs_fd) {
    // TODO
    return 0;
}
