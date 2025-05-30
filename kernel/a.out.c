#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "vfs.h"
#include "panic.h"

typedef struct {
  uint32_t pizdmag;
  uint32_t text;
  uint32_t data;
  uint32_t bss;
  uint32_t syms;
  uint32_t entry;
  uint32_t trsize;
  uint32_t drsize;
} __attribute__((packed)) AOutHeader;

int read_AOut(char* mem) {
  File* f = sys_open(filename);
  AOutHeader header = {0};
  memcpy(&header, f->content, sizeof(DiskHeader));
  printf(": %08X\n", header.pizdmag);
  printf(": %08X\n", header.text);
  printf(": %08X\n", header.data);
  printf(": %08X\n", header.bss);
  printf(": %08X\n", header.syms);
  printf(": %08X\n", header.entry);
  printf(": %08X\n", header.trsize);
  printf(": %08X\n", header.drsize);

  return 0;
}

