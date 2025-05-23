#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define MAX_DISK_SIZE 10240 // десот келобайт
#define MAGIC      0xDEADBABE
#define DISK_END   0xFF
#define FILE_START 0xFE
#define FILE_META  0xFD
#define DIR_START  0xFC
#define DIR_META   0xFB

uint32_t ins_U8(uint8_t* a, uint8_t val, uint32_t offset) {
  a[offset] = val;
  return offset+1;
}

uint32_t ins_U32(uint8_t* a, uint32_t val, uint32_t offset) {
  a[offset]   = (uint8_t)(val & 0xFF);
  a[offset+1] = (uint8_t)((val >> 8) & 0xFF);
  a[offset+2] = (uint8_t)((val >> 16) & 0xFF);
  a[offset+3] = (uint8_t)((val >> 24) & 0xFF);
  return offset+4;
}

uint32_t ins_U64(uint8_t* a, uint64_t val, uint32_t offset) {
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

uint32_t write_file(char* buf, char* filename, char* path, char* contents, uint32_t offset) {
  offset = ins_U8(buf, FILE_START, offset);
  offset = ins_str(buf, filename, offset);
  offset = ins_U8(buf, FILE_META, offset);
  offset = ins_str(buf, path, offset);
  offset = ins_U8(buf, FILE_META, offset);
  offset = ins_str(buf, contents, offset)-1;
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

uint32_t main(uint32_t argc, char** argv) {
  srand(time(NULL));
  FILE* f = fopen("disk.img", "r+b");
  if (!f) {
    fprintf(stderr, "ты чё даун нет диска\n");
    return 0;
  }
  char buf[MAX_DISK_SIZE];
  uint32_t offs = 0;
  uint64_t bits = rand();
  bits <<= 31;
  bits |= rand();
  offs = ins_U32(buf, MAGIC, offs);
  offs = ins_U64(buf, bits, offs);

write_shit:
  offs = write_file(buf, "hello.txt", "/hello", "Hello World!", offs);
  offs = write_dir(buf, "hello", "/hello", offs);
  offs = ins_U8(buf, DISK_END, offs);

  fseek(f, SEEK_SET, 0);
  fwrite(buf, 1, MAX_DISK_SIZE, f);
  fclose(f);
  return 0;
}

