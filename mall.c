#include <stdio.h>
#include <stdlib.h>

int main() {
  system("gcc -o kernel.elf kernel/main.c kernel/init_kernel.c kernel/panic.c kernel/shell.c kernel/vfs.c");
  system("gcc -o tinyboot boot/tinyboot.c");
  system("gcc -o create-disk create-disk.c");
  return 0;
}
