#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// file descriptor should be set to start of bitmap in question
// TODO test me
int bitmap_get_bit(int fd, int offset) {
    int byte_num = offset / 8;
    int byte_offset = offset % 8;

    //printf("getting bit at offset %d\n", offset);
    //printf("byte_num: %d\n", byte_num);
    //printf("byte_offset %d\n", byte_offset);

    lseek(fd, byte_num, SEEK_CUR);

    char byte_buf;
    // TODO add assertion here
    read(fd, &byte_buf, 1);
    //printf("read byte in question into byte_buf: ");
    //print_bits(byte_buf);

    byte_buf &= 1 << (7-byte_offset);

    //printf("isolated bit: ");
    //print_bits(byte_buf);

    return !!byte_buf;
}

// file descriptor should be set to start of bitmap in question
void bitmap_set_bit(int fd, int offset, int val) {
    assert(val == 0 || val == 1);

    int byte_num = offset/8;
    int byte_offset = offset % 8;

    off_t save_offset = lseek(fd, byte_num, SEEK_CUR);

    char byte_buf;
    read(fd, &byte_buf, 1);

    fsync(fd);

    if (val == 0) byte_buf &= ~(1 << (7-byte_offset));
    else byte_buf |= (1<<(7-byte_offset));

    lseek(fd, save_offset, SEEK_SET);
    write(fd, &byte_buf, 1);
    fsync(fd);
}

// file descriptor should be set to start of bitmap in question
// max_size should be bits, multiple of 8!
// return -1 if nothing found
int bitmap_find_first_free(int fd, int max_size) {
    int bytes = max_size / 8;
    if (max_size % 8) bytes += 1;
    char byte_buf;
    int b_num, b_off;
    for (b_num = 0; b_num < bytes; b_num++) {
        read(fd, &byte_buf, 1);
        if (byte_buf == ~0) {
            continue;
        }
        break;
    }
    if (byte_buf != 0xF) {
        for (b_off = 0; b_off < 8; b_off++) {
            if (!((byte_buf >> (7-b_off)) & 1)) {
                return b_num * 8 + b_off;
            }
        }
    }
    return -1;
}
