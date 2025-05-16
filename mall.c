#include <stdio.h>
#include <stdlib.h>

int main() {
	system("clang -o kernel.elf kernel/main.c kernel/init_kernel.c kernel/panic.c kernel/shell.c kernel/vfs.c");
	system("clang -o tinyboot boot/tinyboot.c");
	return 0;
}
