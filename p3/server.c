#include <stdio.h>
#include "udp.h"
#include "mfs.h"

int fd;

int main(int argc, char *argv[]) {
    assert(INODE_T_SIZE == sizeof(MFS_INode_t));

    if(argc<3) {
        printf("Usage: server server-port-number file-system-image\n");
        exit(1);
    }

    int portid = atoi(argv[1]);
    int sd = UDP_Open(portid); //port #
    assert(sd > -1);
    struct sockaddr_in s;

    int inum_buf, type_buf, block_buf;
    char blockbuffer_buf[MFS_BLOCK_SIZE];
    char name_buf[MAX_FILENAME_SIZE];
    MFS_Stat_t statinfo_buf;

    char buffer[BUFFER_SIZE];

    while (1) {
	    int rc = UDP_Read(sd, &s, buffer, BUFFER_SIZE);
	    if (rc > 0) {
	        switch (buffer[0]) {
                case MFS_LOOKUP:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    strncpy(name_buf, &buffer[1+sizeof(int)], MAX_FILENAME_SIZE);
                    printf("MFS_Lookup(%d, %s)", inum_buf, name_buf);
                    break;
                case MFS_STAT:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    printf("MFS_Stat(%d)", inum_buf);
                    break;
                case MFS_WRITE:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&block_buf, &buffer[1+sizeof(int)], sizeof(int));
                    memcpy(blockbuffer_buf, &buffer[1+2*sizeof(int)], MFS_BLOCK_SIZE);
                    printf("MFS_Write(%d, %d, %s)", inum_buf, block_buf, blockbuffer_buf);
                    break;
                case MFS_READ:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&block_buf, &buffer[1+sizeof(int)], sizeof(int));
                    printf("MFS_Read(%d, %d)", inum_buf, block_buf);
                    break;
                case MFS_CREAT:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&type_buf, &buffer[1+sizeof(int)], sizeof(int));
                    strncpy(name_buf, &buffer[1+2*sizeof(int)], MAX_FILENAME_SIZE);
                    printf("MFS_Creat(%d, %d, %s)", inum_buf, type_buf, name_buf);
                    break;
                case MFS_UNLINK:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    strncpy(name_buf, &buffer[1+sizeof(int)], MAX_FILENAME_SIZE);
                    printf("MFS_Unlink(%d, %s)", inum_buf, name_buf);
                    break;
            }
            strncpy(buffer, "ok\n", BUFFER_SIZE);
	        rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
            assert(rc > 0);
	    }
    }

    return 0;
}
