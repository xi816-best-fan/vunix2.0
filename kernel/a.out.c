#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "vfs.h"
#include "a.out.h"
#include "panic.h"

int read_AOut(uint8_t* mem, char* filename) {
  File* f = sys_open(filename);
  AOutHeader header = {0};
  memcpy(&header, f->content, 0x24);
  // a.out: 0x20 - 0x3F у линкера мать сдохла
  // v.out: 0x24 - 0x3F у линкера мать сдохла
  uint16_t textoffs = header.tooffs;
  uint16_t dataoffs = header.dooffs;
  memcpy(mem+textoffs, f->content+0x40, header.text);
  printf("  .text loaded at 0x%04X\n", header.tooffs);
  printf("  .data loaded at 0x%04X\n", header.dooffs);
  printf(header.small ? "  .data loaded right after .text\n" : "  .data loaded separately\n");
  memcpy(mem+dataoffs+(header.text * (header.small ? 1 : 0)), f->content+header.text+0x40, header.data);
  if (0) {
    printf("magic number (1488) for vunix: %08X\n", header.pizdmag);
    printf("text section size: %08X\n", header.text);
    printf("data section size: %08X\n", header.data);
    printf("bss section size: %08X\n", header.bss);
    printf("symbol table offset: %08X\n", header.syms);
    printf("program entry offset (offset from .text): %08X\n", header.entry);
    printf("text section start: %08X\n", header.tooffs);
    printf("data section start: %08X\n", header.dooffs);
  }

  return 0;
}

