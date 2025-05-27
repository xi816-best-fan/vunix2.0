#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "vfs.h"
#include "exec.h"

void shell() {
	char str[128];
	for(;;) {
		printf("root@vunix: ");
		fgets(str, sizeof(str), stdin);
		str[strcspn(str, "\n")] = '\0';
		if(!strcmp(str, "exit")) {
			break;
		} else if(!strncmp(str, "echo ", 5)) {
			printf("%s\n", str+5);
		} else if(!strcmp(str, "help")) {
			printf("exit - exit\necho <str> - echo\nls - list files\ncat <file> - see file\nexec - exec binary\nhelp - help\n");
		} else if(!strcmp(str, "ls")) {
			for(int i = 0; i < MAX_FILES; i++) {
				if(strcmp(files[i].name, "\0") != 0) { 
					printf("%s ", files[i].name);
				}
			} 
			for(int i = 0; i < MAX_DIRS; i++) {
				if(strcmp(files[i].name, "\0") == 0) {
					printf("%s ", dirs[i].name);
				}
			}
			putchar('\n');
		} else if(!strncmp(str, "cat ", 4)){
			File* f = sys_open(str+4);
			if(f == NULL) {
				printf("File %s doesn`t exist\n", str+4);
			} else {
				printf("%s\n", f->content);
			}
		} else if(!strncmp(str, "exec ", 5)) {
			exec(str+5);
		} else {
			printf("Unknown command: %s\n", str);
		}
	}
}
