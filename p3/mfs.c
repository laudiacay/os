#include <stdio.h>
#include "udp.h"
#include "mfs.h"

int inum_buf, type_buf, block_buf;
char blockbuffer_buf[MFS_BLOCK_SIZE];
char name_buf[MAX_FILENAME_SIZE];

char buffer[BUFFER_SIZE];

int sd;
struct sockaddr_in addr, addr2;

int timeout = 5;

// TODO: retries, timeouts
int do_send() {
    int rc = UDP_Write(sd, &addr, buffer, BUFFER_SIZE);
    if (rc > 0) {
	    rc = UDP_Read(sd, &addr2, buffer, BUFFER_SIZE);
	    //printf("CLIENT:: read %d bytes (message: '%s')\n", rc, buffer);
    }
    return rc;
}

int MFS_Init(char *hostname, int port) {
    sd = UDP_Open(CLIENT_PORT);
    assert(sd > -1);

    int rc = UDP_FillSockAddr(&addr, hostname, port);

    return rc;
}

int MFS_Lookup(int pinum, char *name) {
    char cmd_type = MFS_LOOKUP;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &pinum, sizeof(int));
    strncpy(&buffer[1+sizeof(int)], name, MAX_FILENAME_SIZE);
    do_send();
    int ret;
    memcpy(&ret, &buffer[1], sizeof(int));
    return ret;
}

// TODO: parse message out, return code
int MFS_Stat(int inum, MFS_Stat_t *m) {
    char cmd_type = MFS_STAT;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &inum, sizeof(int));
    UNUSED(m);
    do_send();
    return 0;
}

// TODO: parse message out, return code
int MFS_Write(int inum, char *block_buf, int block) {
    char cmd_type = MFS_WRITE;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &inum, sizeof(int));
    memcpy(&buffer[1+sizeof(int)], &block, sizeof(int));
    memcpy(&buffer[1+2*sizeof(int)], block_buf, MFS_BLOCK_SIZE);
    do_send();
    return 0;
}

// TODO: parse message out, return code
int MFS_Read(int inum, char *buffer, int block) {
    char cmd_type = MFS_READ;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &inum, sizeof(int));
    memcpy(&buffer[1+sizeof(int)], &block, sizeof(int));
    do_send();
    return 0;
}

int MFS_Creat(int pinum, int type, char *name) {
    char cmd_type = MFS_CREAT;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &pinum, sizeof(int));
    memcpy(&buffer[1+sizeof(int)], &type, sizeof(int));
    strncpy(&buffer[1+2*sizeof(int)], name, MAX_FILENAME_SIZE);
    do_send();
    int ret;
    memcpy(&ret, &buffer[1], sizeof(int));
    return ret;
    return 0;
}

// TODO: parse message out, return code
int MFS_Unlink(int pinum, char *name) {
    char cmd_type = MFS_UNLINK;
    memcpy(&buffer[0], &cmd_type, 1);
    memcpy(&buffer[1], &pinum, sizeof(int));
    strncpy(&buffer[1+sizeof(int)], name, MAX_FILENAME_SIZE);
    do_send();
    return 0;
}
