typedef struct {
  uint32_t pizdmag; // magic number (0x1488)
  uint32_t text;    // text section size
  uint32_t data;    // data section size
  uint32_t bss;     // bss section size
  uint32_t syms;    // symbol table offset
  uint32_t entry;   // entry offset from .text
  uint32_t tooffs;  // text load offset
  uint32_t dooffs;  // data load offset
  uint32_t small;   // small memory model
} __attribute__((packed)) AOutHeader;

int read_AOut(uint8_t* mem, char* filename);

