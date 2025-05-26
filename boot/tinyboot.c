#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define KERNEL_SIZE_MAX (1024*1024) //1mb
#define MENU_ENTRIES 2

typedef struct {
	uint8_t code[KERNEL_SIZE_MAX];
	size_t size;
	char cmdline[256];
} KrnlImg;

static const char* menu_items[MENU_ENTRIES] = {
	"Vunix Default",
	"Vunix Rescue",
};

static uint8_t fake_disk[2*1024*1024];

void read_disk(uint32_t lba, uint8_t* buffer, uint32_t count) {
	printf("[TinyBoot] Reading LBA %u, sectors %u\n", lba, count);
	memcpy(buffer, fake_disk + (lba * 512), count*512);
}

void init_fake_disk() {
	fake_disk[510] = 0x55;
	fake_disk[511] = 0xAA;
	const char* kernel_data = "KERNEL_START\0\0\0\0KERNEL_END";
	memcpy(fake_disk + (100 * 512), kernel_data, strlen(kernel_data));
}

int show_boot_menu() {
	printf("\n\n=== TinyBoot v0.1 ===\n");
	for(int i = 0; i < MENU_ENTRIES; i++) {
		printf("%d) %s\n", i+1, menu_items[i]);
	}
	int choice = 0;
	while(choice < 1 || choice > MENU_ENTRIES) {
		printf("choice 1-%d: ", MENU_ENTRIES);
		scanf("%i", &choice);
	}
	return choice;
}

int load_kernel(KrnlImg* kernel) {
	printf("[TinyBoot] Loading Kernel...");
	uint8_t header[512];
	read_disk(100, header, 1);
	if(memcmp(header, "KERNEL_START", 12) != 0) {
		printf("\n[TinyBoot] Error: error in kernel signature\n");
		return -1;
	}
	kernel->size = 256*1024;
	printf("done\n Kernel loaded in memory to address %p\n", (void*)kernel->code);
	strcpy(kernel->cmdline, "disk.img ram.img");
	return 0;
}

int main() {
	KrnlImg kernel;
	init_fake_disk();
	printf("=== TinyBoot ===");
	printf("\nIniting devices... ");
	printf("done\n");
	printf("Finding bootable device... ");
	printf("done\n");
	int choice = show_boot_menu();
	if(load_kernel(&kernel) != 0) {
		printf("Error while loading! System stopped.\n");
		return 1;
	}
	printf("\nTransfer control to the kernel with parameters %s\n", kernel.cmdline);
	printf("Running kernel...\n\n");
	if(choice == 1) {
	  if(execve("./kernel.elf", (char*[]){"1488", "disk.img", NULL}, NULL) == -1) {
	  	printf("vmvuniz-vunix not found");
	  	return -1;
	  }
	} else {
	  if(execve("./kernel.elf", (char*[]){"1488", "rescue"}, NULL) == -1) {
	  	printf("vmvuniz-vunix not found");
	  	return -1;
	  }
	}
	return 0;
}
