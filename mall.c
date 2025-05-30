#include <stdio.h>
#include <stdlib.h>

void compile(char* command, char* name) {
  printf("compiling %s\n", name);
  system(command);
}

int main() {
  compile("gcc -pedantic -Wextra -Wall -o mall mall.c", "mall");
  compile("gcc -pedantic -Wextra -Wall -o vmvuniz-vunix kernel/main.c kernel/init_kernel.c kernel/panic.c kernel/shell.c kernel/vfs.c kernel/rescue.c kernel/exec.c kernel/a.out.c", "kernel");
  compile("gcc -pedantic -Wextra -Wall -o tinyboot boot/tinyboot.c", "tinyboot");
  compile("gcc -pedantic -Wextra -Wall -o create_disk create_disk.c", "create_disk");
  compile("gcc -o writevfs writevfs.c", "writevfs");
  return 0;
}
