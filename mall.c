#include <stdio.h>
#include <stdlib.h>

int main() {
  system("gcc -g -o vmvuniz-vunix kernel/main.c kernel/init_kernel.c kernel/panic.c kernel/shell.c kernel/vfs.c kernel/rescue.c kernel/exec.c");
  system("gcc -g -o tinyboot boot/tinyboot.c");
  system("gcc -g -o create_disk create_disk.c");
  return 0;
}
