#include "mfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    UNUSED(argc);
  MFS_Init(argv[2], atoi(argv[1]));

  printf("should not be -1: %d\n", MFS_Creat(0, MFS_REGULAR_FILE, "file.txt"));

  int inode = MFS_Lookup(0, "file.txt");
  printf("should not be -1 or 0: %d\n", inode);

  char buf[MFS_BLOCK_SIZE];
  memset(buf, 99, MFS_BLOCK_SIZE);
  printf("should not be 0: %d\n", MFS_Write(inode, buf, -1));
  printf("should not be 0: %d\n", MFS_Write(inode, buf, 10));
  return 0;

}
