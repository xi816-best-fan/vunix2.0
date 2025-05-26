#include <stdio.h>
#include <string.h>
#include "vfs.h"
#include "exec.h"
#include "panic.h"

int current_pid = 0;
process processes[MAX_PROCESSES];

short cpu_read(risc_gc cpu, short address) {
	if(address >= MEMORY_SIZE - 1) {
		printf("Segmentation fault\n");
		return -1; 
	}
}

int mkproc(char* filename) {
	if(current_pid++ >= MAX_PROCESSES) panic("too much processes");
}

int exec(char* content) {
	process proc;
	proc->pid = current_pid;
	proc->cpu->pc = 0;
	proc->cpu->ir = 0;
	proc->cpu->flags = 0;
	for(int i = 0; i < NUM_REGS; i++) {
		proc->cpu->reg[i] = 0;
	}
	memcpy(content, cpu->memory, MEMORY_SIZE);
	
}
