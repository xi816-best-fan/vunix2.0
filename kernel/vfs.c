#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "panic.h"
#include "vfs.h"

DiskHeader disks[MAX_DISKS];
File files[MAX_FILES] = {0};  
Dir dirs[MAX_DIRS] = {0};     
int file_count = 0;           
int dir_count = 0;            

void write_file(FILE* disk, const char* name, const char* path, const char* content) {
    if (!disk || !name || !path || !content) return;

    fputc(FILE_START, disk);
    fwrite(name, 1, strlen(name)+1, disk);
    fputc(FILE_META, disk);
    fwrite(path, 1, strlen(path)+1, disk);
    fputc(FILE_META, disk);
    fwrite(content, 1, strlen(content), disk);
    fputc(FILE_START, disk);
}

void write_dir(FILE* disk, const char* name, const char* path) {
    if (!disk || !name || !path) return;

    fputc(DIR_START, disk);
    fwrite(name, 1, strlen(name)+1, disk);
    fputc(DIR_META, disk);
    fwrite(path, 1, strlen(path)+1, disk);
    fputc(DIR_START, disk);
}

File* sys_open(const char* path) {
	for(int i = 0; i < MAX_FILES; i++) {
		if(strcmp(files[i].name, path) == 0 || strcmp(files[i].path, path) == 0) {
			return &files[i];
		}
	}
	return NULL;
}

void sys_mount(const char* filename) {
    if (!filename) panic("Invalid filename");

    FILE* disk = fopen(filename, "rb");
    if (!disk) panic("Failed to mount disk");

    DiskHeader header;
    if (fread(&header, sizeof(DiskHeader), 1, disk) != 1) {
        fclose(disk);
        panic("Failed to read disk header");
    }

    if (header.magic != MAGIC_NUMBER) {
        fclose(disk);
        panic("Invalid disk format!");
    }

    printf("Disk mounted. Random ID: %016lx\n", header.random);

    uint8_t byte;
    char name[256] = {0};
    char path[256] = {0};
    char content[1024] = {0};

    while (fread(&byte, 1, 1, disk) == 1) {
        switch (byte) {
            case FILE_START: {
                if (file_count >= MAX_FILES) {
                    printf("Warning: Maximum files reached (%d)\n", MAX_FILES);
                    while (fread(&byte, 1, 1, disk) == 1 && byte != FILE_START) {}
                    break;
                }
                int i = 0;
                while (i < sizeof(name)-1 && fread(&byte, 1, 1, disk) == 1 && byte != 0) {
                    name[i++] = byte;
                }
                name[i] = '\0';

                if (fread(&byte, 1, 1, disk) != 1 || byte != FILE_META) {
                    panic("Invalid file format: expected FILE_META");
                }
                i = 0;
                while (i < sizeof(path)-1 && fread(&byte, 1, 1, disk) == 1 && byte != 0) {
                    path[i++] = byte;
                }
                path[i] = '\0';
                
                if (fread(&byte, 1, 1, disk) != 1 || byte != FILE_META) {
                    panic("Invalid file format: expected FILE_META");
                }
                i = 0;
                while (i < sizeof(content)-1 && fread(&byte, 1, 1, disk) == 1 && byte != FILE_START) {
                    content[i++] = byte;
                }
                content[i] = '\0';

                printf("File: %s\nPath: %s\nContent: %s\n", name, path, content);
                strncpy(files[file_count].name, name, sizeof(files[file_count].name)-1);
                strncpy(files[file_count].path, path, sizeof(files[file_count].path)-1);
                strncpy(files[file_count].content, content, sizeof(files[file_count].content)-1);
                files[file_count].name[sizeof(files[file_count].name)-1] = '\0';
                files[file_count].path[sizeof(files[file_count].path)-1] = '\0';
                files[file_count].content[sizeof(files[file_count].content)-1] = '\0';
                file_count++;
                break;
            }

            case DIR_START: {
                if (dir_count >= MAX_DIRS) {
                    printf("Warning: Maximum directories reached (%d)\n", MAX_DIRS);
                    while (fread(&byte, 1, 1, disk) == 1 && byte != DIR_START) {}
                    break;
                }
                int i = 0;
                while (i < sizeof(name)-1 && fread(&byte, 1, 1, disk) == 1 && byte != 0) {
                    name[i++] = byte;
                }
                name[i] = '\0';

                if (fread(&byte, 1, 1, disk) != 1 || byte != DIR_META) {
                    panic("Invalid dir format: expected DIR_META");
                }
                i = 0;
                while (i < sizeof(path)-1 && fread(&byte, 1, 1, disk) == 1 && byte != 0) {
                    path[i++] = byte;
                }
                path[i] = '\0';

                if (fread(&byte, 1, 1, disk) != 1 || byte != DIR_START) {
                    panic("Invalid dir format: expected DIR_START");
                }

                printf("Dir: %s\nPath: %s\n", name, path);
                strncpy(dirs[dir_count].name, name, sizeof(dirs[dir_count].name)-1);
                strncpy(dirs[dir_count].path, path, sizeof(dirs[dir_count].path)-1);
                dirs[dir_count].name[sizeof(dirs[dir_count].name)-1] = '\0';
                dirs[dir_count].path[sizeof(dirs[dir_count].path)-1] = '\0';
                dir_count++;
                break;
            }
            
            case DISK_END:
                fclose(disk);
                return;

            default:
                printf("Warning: Unknown marker 0x%02x at offset %ld\n", 
                      byte, ftell(disk)-1);
                break;
        }
    }
    fclose(disk);
}
