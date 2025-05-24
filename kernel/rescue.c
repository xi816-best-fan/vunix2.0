#include <stdio.h>
#include <string.h>
#include "vfs.h"
#include "rescue.h"

void rescue() {
	char buf[64];
	printf("Vunix booted in rescue mode.\nFor help write help\n");
	for(;;) {
		printf("rescue> ");
		fgets(buf, 64, stdin);
		buf[strcspn(buf, "\n")] = 0;
		if(!strcmp(buf, "help")) {
			printf("help - get help\nexit - exit\nmount (diskname)- try to mount disk\necho (str) - echo string\n");
		} else if(!strcmp(buf, "exit")) {
			break;
		} else if(!strncmp(buf, "mount ", 6)) {
			sys_mount(buf+6);
		} else if(!strncmp(buf, "echo ", 5)) {
			printf("%s\n", buf+5);
		} else {
			printf("unknown command %s\n", buf);
		}
	}
	for(;;) {}
}
