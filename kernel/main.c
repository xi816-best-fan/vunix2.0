#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "init_kernel.h"
#include "shell.h"

int main(int argc, char** argv) {
	if(argc < 1) return -1;
	init_kernel(argc, argv);
	shell();
	puts("System will power off right now");
	usleep(634891);
	fputs("\033[H\033[2J", stdout);
	return 0;
}
