#include "bitmap.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

int main() {
    int bitmap_file = open("bitmap_file", O_CREAT|O_RDWR, S_IRWXU);

    printf("fd: %d\n", bitmap_file);
    printf("error: %s\n", strerror(errno));
    char* buf = calloc(20, sizeof(char));
    for (int i = 0; i < 20; i++)
    {
        if (i > 0) printf(":");
        buf[i] = i;
        printf("%02X", buf[i]);
    }
    printf("\n");
    printf("%d bytes written\n", write(bitmap_file, buf, 20));
    printf("error: %s\n", strerror(errno));

    lseek(bitmap_file, 0, SEEK_SET);
    printf("GETTING\n");
    printf("result: %d\n", bitmap_get_bit(bitmap_file, 10));
    lseek(bitmap_file, 0, SEEK_SET);
    printf("SETTING\n");
    bitmap_set_bit(bitmap_file, 10, 1);
    lseek(bitmap_file, 0, SEEK_SET);
    printf("GETTING\n");
    printf("result: %d\n", bitmap_get_bit(bitmap_file, 10));
    fsync(bitmap_file);
    close(bitmap_file);
    return 0;
}
