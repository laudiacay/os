#include "mfs.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//
// Test timeout for stats
//

int main(int argc, char* argv[]) {
    UNUSED(argc);
  MFS_Init(argv[2], atoi(argv[1]));


  MFS_Stat_t m;
  int rc = MFS_Stat(0, &m);
  if (rc == -1) {
    printf("FAILED 1\n");
    return -1;
  }
  if (m.size != sizeof(MFS_DirEnt_t) * 2 || m.type != MFS_DIRECTORY ||
      m.blocks != 1) {
    printf("FAILED 2\n");
    return -1;
  }
  printf("PASSED\n");
  return 0;
}
