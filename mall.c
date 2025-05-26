#include <stdio.h>
#include <stdlib.h>

int main() {
  system("gcc -o vmvuniz-vunix kernel/main.c kernel/init_kernel.c kernel/panic.c kernel/shell.c kernel/vfs.c kernel/rescue.c");
  system("gcc -o tinyboot boot/tinyboot.c");
  system("gcc -o create_disk create_disk.c");
  return 0;
}
