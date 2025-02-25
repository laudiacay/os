#include <stdio.h>
#include "udp.h"
#include "mfs.h"
#include "fsops.h"

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
    int result_buf;
    char blockbuffer_buf[MFS_BLOCK_SIZE];
    char name_buf[MAX_FILENAME_SIZE];
    MFS_Stat_t statinfo_buf;
    fd = open_create_filesystem_image(argv[2]);

    char buffer[BUFFER_SIZE];
    while (1) {
	    int rc = UDP_Read(sd, &s, buffer, BUFFER_SIZE);
	    if (rc > 0) {
	        switch (buffer[0]) {
                case MFS_LOOKUP:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    strncpy(name_buf, &buffer[1+sizeof(int)], MAX_FILENAME_SIZE);
                    result_buf = mfs_lookup(fd, inum_buf, name_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
                case MFS_STAT:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    result_buf = mfs_stat(fd, inum_buf, &statinfo_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    memcpy(&buffer[1+sizeof(int)], &statinfo_buf, sizeof(MFS_Stat_t));
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
                case MFS_WRITE:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&block_buf, &buffer[1+sizeof(int)], sizeof(int));
                    memcpy(blockbuffer_buf, &buffer[1+2*sizeof(int)], MFS_BLOCK_SIZE);
                    result_buf = mfs_write(fd, inum_buf, block_buf, blockbuffer_buf);
                    //printf("result: %d\n", result_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
                case MFS_READ:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&block_buf, &buffer[1+sizeof(int)], sizeof(int));
                    //printf("MFS_Read(%d, %d)\n", inum_buf, block_buf);
                    result_buf = mfs_read(fd, inum_buf, block_buf, blockbuffer_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    if (result_buf != -1)
                        memcpy(&buffer[1+sizeof(int)], blockbuffer_buf, MFS_BLOCK_SIZE);
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
                case MFS_CREAT:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    memcpy(&type_buf, &buffer[1+sizeof(int)], sizeof(int));
                    strncpy(name_buf, &buffer[1+2*sizeof(int)], MAX_FILENAME_SIZE);
                    result_buf = mfs_creat(fd, inum_buf, type_buf, name_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
                case MFS_UNLINK:
                    memcpy(&inum_buf, &buffer[1], sizeof(int));
                    strncpy(name_buf, &buffer[1+sizeof(int)], MAX_FILENAME_SIZE);
                    result_buf = mfs_unlink(fd, inum_buf, name_buf);
                    memcpy(&buffer[1], &result_buf, sizeof(int));
                    rc = UDP_Write(sd, &s, buffer, BUFFER_SIZE);
                    break;
            }
            assert(rc > 0);
	      }
    }

    return 0;
}
