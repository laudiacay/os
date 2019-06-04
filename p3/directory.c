
void goto_dirent(int bnum, int boff) {
    lseek(fd, dblock_loc(bnum), SEEK_SET);
    lseek(fd, boff * SUBDIR_SIZE, SEEK_CUR);
}

int dirent_in_use(int inum, int dirent) {
    MFS_INode_t* inode = get_inode(inum);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    assert(n_blocks > 0);
    goto_dblock(inode->block_nums[0]);
    free(inode);
    return bitmap_get_bit(dirent);
}

int get_first_free_dirent(int inum) {
    MFS_INode_t* inode = get_inode(inum);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    assert(n_blocks > 0);
    goto_dblock(inode->block_nums[0]);
    free(inode);
    return bitmap_find_first_free(SUBDIR_BITMAP_SIZE);
}

void set_dirent_use(int inum, int dirent, int val) {
    MFS_INode_t* inode = get_inode(inum);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    assert(n_blocks > 0);
    goto_dblock(inode->block_nums[0]);
    free(inode);
    bitmap_set_bit(dirent, val);
}

// adds direntry to p's directory table
void add_direntry(int pinum, int inum, char* name) {
    MFS_INode_t* inode = get_inode(pinum);

    int n_blocks = inode->stat_info.blocks;

    int first_free = get_first_free_dirent(pinum);


    int dir_bnum = (first_free + 1) / 16;
    int dir_boff = (first_free + 1) % 16;

    assert(dir_bnum < 10);

    if (dir_bnum >= n_blocks) {
        int new_dblock = dblock_find_space();
        dblock_set_use(new_dblock, 1);
        inode->block_nums[n_blocks] = new_dblock;
        inode->stat_info.blocks += 1;
        inode->stat_info.size += SUBDIR_SIZE;
    }

    MFS_DirEnt_t dirent;
    dirent.inum = inum;
    strcpy(dirent.name, name, MAX_FILENAME_SIZE);

    // write direntry
    goto_dirent(inode->block_nums[dir_bnum], dir_boff);
    write(fd, &dirent, sizeof(MFS_DirEnt_t));

    // write inode
    inode_seek_loc(pinum);
    write(fd, &inode, sizeof(MFS_INode_t));

    free(inode);
    fsync(fd);
}

// TODO: implement me
void defrag_directory(int inum) {
    return;
}

void init_first_block() {
    // create first block bitmap, write first block
    char* bitmap = calloc(SUBDIR_BITMAP_SIZE, sizeof(char));
    int dblock = dblock_find_space();

    goto_dblock(dblock);
    write(fd, bitmap, SUBDIR_BITMAP_SIZE);
    fsync(fd);

    return dblock;
}

void init_dir_inode(int inum) {
    inode_set_use(inum, 1);

    MFS_INode_t inode;
    inode.stat_info.type = MFS_DIRECTORY;
    inode.stat_info.size = 3*SUBDIR_SIZE;
    inode.stat_info.blocks = 1;

    int first_dblock = init_first_block();

    inode.blocks[0] = first_dblock;

    inode_seek_loc(inum);
    write(fd, &inode, sizeof(MFS_INode_t));
    fsync(fd);
}

void mkroot() {
    init_dir_inode(0);

    add_direntry(0, 0, ".");
    add_direntry(0, 0, "..");
}

void mkdir(int pinode, char* name) {
    int new_inode = inode_find_space();
    init_dir_inode(new_inode);

    add_direntry(new_inode, new_inode, ".");
    add_direntry(new_inode, pinode, "..");
}

int dir_is_empty(int inum) {
    MFS_INode_t* inode = get_inode(inum);
    assert(inode->stat_info.type == MFS_DIRECTORY);
    dblock_seek_loc(inode->block_nums[0]);
    return bitmap_is_empty(SUBDIR_BITMAP_SIZE);
}

int dir_unlink(int pinode, char* name) {
    MFS_INode_t* inode = get_inode(pinum);
    MFS_DirEnt_t dirent;
    if (!inode) return -1;
    if (!inode->stat_info.type == MFS_DIRECTORY) return -1;
    if (!dir_is_empty(pinode)) return -1;
    int bnum, boff;
    for (int i = 0; i < inode->stat_info.size/SUBDIR_SIZE - 1; i++) {
        if (!dirent_in_use(inum)) continue;
        else {
            bnum = (i + 1) / 16;
            boff = (i + 1) % 16;
            goto_dirent(inode->block_nums[bnum], boff);
            read(fd, &dirent, sizeof(MFS_DirEnt_t));
            if (strcmp(dirent.name, name) != 0) continue;
            set_dirent_use(pinode, i, 0);
            return 0;
        }
    }

}
