#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#define MAX_DISK_SIZE 16384 // много келобайт
#define MAX_FILE_SIZE 8192  // восем келобайт
#define MAGIC      0xDEADBABE
#define DISK_END   0xFF
#define FILE_START 0xFE
#define FILE_META  0xFD
#define DIR_START  0xFC
#define DIR_META   0xFB

uint32_t ins_U8(char* a, uint8_t val, uint32_t offset) {
  a[offset] = val;
  return offset+1;
}

uint32_t ins_U32(char* a, uint32_t val, uint32_t offset) {
  a[offset]   = (uint8_t)(val & 0xFF);
  a[offset+1] = (uint8_t)((val >> 8) & 0xFF);
  a[offset+2] = (uint8_t)((val >> 16) & 0xFF);
  a[offset+3] = (uint8_t)((val >> 24) & 0xFF);
  return offset+4;
}

uint32_t ins_U64(char* a, uint64_t val, uint32_t offset) {
  a[offset]   = (uint8_t)(val & 0xFF);
  a[offset+1] = (uint8_t)((val >> 8) & 0xFF);
  a[offset+2] = (uint8_t)((val >> 16) & 0xFF);
  a[offset+3] = (uint8_t)((val >> 24) & 0xFF);
  a[offset+4] = (uint8_t)((val >> 32) & 0xFF);
  a[offset+5] = (uint8_t)((val >> 40) & 0xFF);
  a[offset+6] = (uint8_t)((val >> 48) & 0xFF);
  a[offset+7] = (uint8_t)((val >> 56) & 0xFF);
  return offset+8;
}

uint32_t ins_str(char* buf, char* str, uint32_t offset) {
  uint32_t len = strlen(str);
  memcpy(buf+offset, str, len);
  buf[offset+len] = 0x00;
  return offset+len+1;
}

uint32_t ins_membuf(char* buf, char* str, uint32_t size, uint32_t offset) {
  memcpy(buf+offset, str, size);
  return offset+size;
}

uint32_t write_file(char* buf, char* filename, char* path, char* contents, uint32_t csize, uint32_t offset) {
  offset = ins_U8(buf, FILE_START, offset);
  offset = ins_str(buf, filename, offset);
  offset = ins_U8(buf, FILE_META, offset);
  offset = ins_str(buf, path, offset);
  offset = ins_U8(buf, FILE_META, offset);
  offset = ins_membuf(buf, contents, csize, offset);
  offset = ins_U8(buf, FILE_START, offset);
  return offset;
}

uint32_t write_dir(char* buf, char* name, char* path, uint32_t offset) {
  offset = ins_U8(buf, DIR_START, offset);
  offset = ins_str(buf, name, offset);
  offset = ins_U8(buf, DIR_META, offset);
  offset = ins_str(buf, path, offset);
  offset = ins_U8(buf, DIR_START, offset);
  return offset;
}

int main(int argc, char** argv) {
  puts("writevfs 1.0");
  if (argc < 5) {
    printf("Usage: %s disk.img file.bin file /dir\n", argv[0]);
    return 1;
  }
  char* diskname     = argv[1];
  char* filename     = argv[2];
  char* new_filename = argv[3];
  char* directory    = argv[4];
  printf("  Disk name:     %s\n", diskname);
  printf("  File name:     %s\n", filename);
  printf("  New file name: %s\n", new_filename);
  printf("  Directory:     %s\n", directory);

  // File
  FILE* f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "error: file `%s` does not exist\n", filename);
    return 1;
  }
  char contents[MAX_FILE_SIZE];
  fread(contents, 1, MAX_FILE_SIZE, f);
  fclose(f);

  // Disk
  FILE* f2 = fopen(diskname, "r+b");
  if (!f2) {
    fprintf(stderr, "error: disk `%s` does not exist\n", diskname);
    return 1;
  }
  unsigned char buf[MAX_DISK_SIZE];
  fread(buf, 1, MAX_DISK_SIZE, f2);
  fseek(f2, 0, SEEK_END);
  uint32_t csize = ftell(f2);
  uint32_t offs = 0;
  while (buf[offs] != 0xFF) offs++;

  buf[offs] = 0x00;
  offs = write_file(buf, new_filename, directory, contents, csize, offs);
  offs = ins_U8(buf, 0xFF, offs);
  for (int i = 0x3C; i < 0x80; i++) {
    printf("%02X ", buf[i]);
  }
  puts("\b");

  fseek(f2, 0, SEEK_SET);
  fwrite(buf, 1, MAX_DISK_SIZE, f2);
  fclose(f2);
  return 0;
}

