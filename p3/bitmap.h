#ifndef __BITMAP_H
#define __BITMAP_H

void bitmap_flip_bit(int fd, int offset);
int bitmap_get_bit(int fd, int offset);
void bitmap_set_bit(int fd, int offset, int val);
int bitmap_find_first_free(int fd, int max_size);
int bitmap_is_empty(int fd, int bytes);

#endif
