#ifndef __BITMAP_H
#define __BITMAP_H

void bitmap_flip_bit(int offset);
int bitmap_get_bit(int offset);
void bitmap_set_bit(int offset, int val);
int bitmap_find_first_free(int max_size);

#endif
