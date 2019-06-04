#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void print_bits(char c)
{
    for (int i = 7; i >= 0; --i)
    {
        putchar( (c & (1 << i)) ? '1' : '0' );
    }
    putchar('\n');
}

// file descriptor should be set to start of bitmap in question
// TODO test me
void bitmap_flip_bit(int fd, int offset) {
    int byte_num = offset / 8;
    int byte_offset = offset % 8;

    // TODO add assertion here
    lseek(fd, byte_num, SEEK_CUR);

    char byte_buf;
    // TODO add assertion here
    read(fd, &byte_buf, 1);
    //print_bits(byte_buf);
    byte_buf ^= 1 << (7-byte_offset);

    //print_bits(byte_buf);

    // TODO add assertion here
    lseek(fd, -1, SEEK_CUR);
    // TODO add assertion here
    write(fd, &byte_buf, 1);
    fsync(fd);
}

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
// TODO test me
void bitmap_set_bit(int fd, int offset, int val) {
    assert(val == 0 || val == 1);

    int byte_num = offset/8;
    int byte_offset = offset % 8;
    //errno = 0;
    //printf("setting bit at offset %d\n", offset);
    //printf("byte_num: %d\n", byte_num);
    //printf("byte_offset %d\n", byte_offset);
    off_t save_offset = lseek(fd, byte_num, SEEK_CUR);
    //printf("starting offset: %ld\n", save_offset);

    char byte_buf;
    read(fd, &byte_buf, 1);
    //printf("%s\n", strerror(errno));
    //printf("read byte in question into byte_buf: ");
    fsync(fd);
    //print_bits(byte_buf);
    //exit(0);
    if (val == 0) byte_buf &= ~(1 << (7-byte_offset));
    else byte_buf |= (1<<(7-byte_offset));

    //printf("writing byte_buf: ");
    //print_bits(byte_buf);

    lseek(fd, save_offset, SEEK_SET);
    write(fd, &byte_buf, 1);

    fsync(fd);
}

// file descriptor should be set to start of bitmap in question
// max_size should be bits, multiple of 8!
// return -1 if nothing found
// TODO test me
int bitmap_find_first_free(int fd, int max_size) {
    int bytes = max_size / 8;
    if (max_size % 8) bytes += 1;
    char byte_buf;
    int b_num, b_off;
    for (b_num = 0; b_num < bytes; b_num++) {
        // TODO add assertion here
        read(fd, &byte_buf, 1);
        //printf("read byte:\n");
        //print_bits(byte_buf);
        if (byte_buf == 0xF) continue;
        //printf("found something open in bit %d! ", b_num);
        //print_bits(byte_buf);
        break;
    }
    if (byte_buf != 0xF) {
        //printf("yeahhh\n");
        for (b_off = 0; b_off < 8; b_off++) {
            if (!((byte_buf >> (7-b_off)) & 1)) {
                assert(b_num * 8 + b_off < max_size);
                return b_num * 8 + b_off;
            }
        }
    }
    // failed, raise assertion
    assert(0);
    return -1;
}

// file descriptor should be set to start of bitmap in question
// 1 if empty, 0 otherwise
// TODO test me
int bitmap_is_empty(int fd, int bytes) {
    char byte_buf;
    for (int b_num = 0; b_num < bytes; b_num++) {
        read(fd, &byte_buf, 1);
        if (byte_buf != 0x0) return 0;
    }
    return 1;
}
