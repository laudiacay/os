#ifndef _DIRECTORY_H
#define _DIRECTORY_H

void add_direntry(int pinum, int inum, char* name);
void defrag_directory(int inum);
void mkroot();
void mkdir(int pinode, char* name);

#endif
