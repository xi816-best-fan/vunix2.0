#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "init_kernel.h"
#include "shell.h"

int main(int argc, char** argv) {
	init_kernel(argc, argv);
	shell();
	printf("System will power off right now");
	usleep(634891);
	system("clear");
	return 0;
}
