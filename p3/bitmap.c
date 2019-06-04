// file descriptor should be set to start of bitmap in question
// TODO test me
void bitmap_flip_bit(int offset) {
    int byte_num = offset / 8;
    int byte_offset = offset % 8;
    // TODO add assertion here
    lseek(fd, byte_num, SEEK_CUR);

    char byte_buf;
    // TODO add assertion here
    read(fd, &byte_buf, 1);
    byte_buf ^= 1 << byte_offset;

    // TODO add assertion here
    lseek(fd, -1, SEEK_CUR);
    // TODO add assertion here
    write(fd, &byte_buf, 1);
    fsync(fd);
}

// file descriptor should be set to start of bitmap in question
// TODO test me
int bitmap_get_bit(int offset) {
    int byte_num = offset / 8;
    int byte_offset = offset % 8;
    // TODO add assertion here
    lseek(fd, byte_num, SEEK_CUR);

    char byte_buf;
    // TODO add assertion here
    read(fd, &byte_buf, 1);
    byte_buf &= 1 << byte_offset;

    return !!byte_buf;
}

// file descriptor should be set to start of bitmap in question
// TODO test me
void bitmap_set_bit(int offset, int val) {
    off_t save_pos = lseek(fd, 0, SEEK_CUR);
    if (bitmap_get_bit(offset) != val) {
        lseek(fd, save_pos, SEEK_SET);
        bitmap_flip_bit(offset);
    }
}

// file descriptor should be set to start of bitmap in question
// max_size should be bits
// return -1 if nothing found
// TODO test me
int bitmap_find_first_free(int max_size) {
    int bytes = max_size / 8 + 1;
    char byte_buf;
    int b_num, b_off;
    for (b_num = 0; b_num < bytes; b_num++) {
        // TODO add assertion here
        read(fd, &byte_buf, 1);
        if (byte_buf != 0xFF) break;
    }
    if (byte_buf != 0xFF) {
        for (b_off = 0; b_off < 8; b_off++) {
            if (byte_buf >> (7-b_off) & 1) {
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
int bitmap_is_empty(int bytes) {
    for (int b_num = 0; b_num < bytes; b_num++) {
        // TODO add assertion here
        read(fd, &byte_buf, 1);
        if (byte_buf != 0x00) return 0;
    }
    return 1;
}
