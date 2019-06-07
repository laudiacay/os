#ifndef __BITMAP_H
#define __BITMAP_H

int bitmap_get_bit(int fd, int offset);
void bitmap_set_bit(int fd, int offset, int val);
int bitmap_find_first_free(int fd, int max_size);

#endif
