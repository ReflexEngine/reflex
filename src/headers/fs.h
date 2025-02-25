#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <stdlib.h>

// Reads an entire file into a dynamically allocated string
char* fs_read(const char *filename);

// Writes a string to a file (overwrites if exists)
int fs_write(const char *filename, const char *content);

#endif // FS_H
