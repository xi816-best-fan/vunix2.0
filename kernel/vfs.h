#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define MAGIC_NUMBER 0xdeadbabe
#define FILE_START   0xFE
#define FILE_META    0xFD
#define DIR_START    0xFC
#define DIR_META     0xFB
#define DISK_END     0xFF

#define MAX_DISKS 8
#define MAX_FILES 256
#define MAX_DIRS  256

typedef struct {
    uint32_t magic;
    uint64_t random;
    uint8_t data[];
} __attribute__((packed)) DiskHeader;

typedef struct {
    char name[256];
    char path[256];
    char content[8192];
} File;

typedef struct {
    char name[256];
    char path[256];
} Dir;

extern File files[MAX_FILES];
extern Dir dirs[MAX_DIRS];
extern int file_count;
extern int dir_count;

void write_file(FILE* disk, const char* name, const char* path, const char* content);
void write_dir(FILE* disk, const char* name, const char* path);
void sys_mount(const char* filename);
File* sys_open(const char* path);

#endif
