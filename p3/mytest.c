#include "mfs.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    UNUSED(argc);
  MFS_Init(argv[2], atoi(argv[1]));

  // should be -1
  printf("%d\n", MFS_Lookup(0, "file.txt"));
  // should be 0
  printf("%d\n", MFS_Creat(0, MFS_REGULAR_FILE, "file.txt"));
  // should be >0
  printf("%d\n", MFS_Lookup(0, "file.txt"));
  return 0;

}
