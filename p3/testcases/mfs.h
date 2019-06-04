#ifndef __MFS_h__
#define __MFS_h__

#define MFS_DIRECTORY 0
#define MFS_REGULAR_FILE 1

// all these are in bytes
#define BITMAP_SIZE 64
#define NUM_BLOCKS 4096
#define MFS_BLOCK_SIZE 4096
#define MAX_NUM_BLOCKS 10

#define MAX_FILENAME_SIZE 252

#define CLIENT_PORT 1337

#define BUFFER_SIZE 4500
#define MFS_LOOKUP '0'
#define MFS_STAT '1'
#define MFS_WRITE '2'
#define MFS_READ '3'
#define MFS_CREAT '4'
#define MFS_UNLINK '5'

#define CLIENT_TIMEOUT 5

#define INODE_T_SIZE 52

#define INODE_BITMAP_LOC 0
#define DBLOCK_BITMAP_LOC (INODE_BITMAP_LOC + BITMAP_SIZE)
#define INODE_TABLE_LOC (DBLOCK_BITMAP_LOC + BITMAP_SIZE)
#define DATA_REGION_LOC (INODE_TABLE_LOC + INODE_T_SIZE * NUM_BLOCKS)

// start with 20 byte bitmap of what DirEnts are taken
// this will be at start of first 256-byte block
#define SUBDIR_SIZE 256
#define SUBDIR_BITMAP_SIZE 20
#define MAX_NUM_SUBDIRS 159

#define UNUSED(x) (void)(x)

typedef struct __MFS_Stat_t {
  int type;   // MFS_DIRECTORY or MFS_REGULAR
  int size;   // bytes
  int blocks; // number of blocks allocated to file
  // note: no permissions, access times, etc.
} MFS_Stat_t;

typedef struct __MFS_DirEnt_t {
  int  inum;      // inode number of entry (-1 means entry not used)
  char name[MAX_FILENAME_SIZE]; // up to 252 bytes of name in directory (including \0)
} MFS_DirEnt_t;

typedef struct __MFS_INode_t {
    MFS_Stat_t stat_info;
    int block_nums[MAX_NUM_BLOCKS];
} MFS_INode_t;

int MFS_Init(char *hostname, int port);
int MFS_Lookup(int pinum, char *name);
int MFS_Stat(int inum, MFS_Stat_t *m);
int MFS_Write(int inum, char *buffer, int block);
int MFS_Read(int inum, char *buffer, int block);
int MFS_Creat(int pinum, int type, char *name);
int MFS_Unlink(int pinum, char *name);

#endif // __MFS_h__
