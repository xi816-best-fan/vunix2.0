#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "vfs.h"

void shell() {
	char str[128];
	for(;;) {
		printf("root@vunix: ");
		fgets(str, sizeof(str), stdin);
		str[strcspn(str, "\n")] = '\0';
		if(strcmp(str, "exit") == 0) {
			break;
		} else if(strncmp(str, "echo ", 5) == 0) {
			printf("%s\n", str+5);
		} else if(strcmp(str, "help") == 0) {
			printf("exit - exit\necho <str> - echo\nls - list files\ncat <file> - see file\n\nhelp - help\n");
		} else if(strcmp(str, "ls") == 0) {
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
		} else if(strncmp(str, "cat ", 4) == 0){
			File* f = sys_open(str+4);
			if(f == NULL) {
				printf("File %s doesn`t exist\n", str+4);
			} else {
				printf("%s\n", f->content);
			}
		} else if(strncmp(str, "script", 6) == 0) {
			if(str+6="\0") { 
				script_shell();
			} else {
				script_file(str+6);
			}
		}
		} else {
			printf("Unknown command: %s\n", str);
		}
	}
}
