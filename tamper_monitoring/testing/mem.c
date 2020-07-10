/*
 * Read out and write tamper monitoring information and manually trigger a tamper event
 *
 * Author: Tobias Vögeli, Thierry Delafontaine
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PAGE_SIZE		0x1000
#define BASE_MASK		0xFFFFF000
#define OFFSET_MASK		0x00000FFF
#define BYTES_PER_LINE	32

static int mount_mem (int *fp, uint8_t **pmem, uint32_t base, uint32_t length) {
	/* open dev mem */
	*fp = open("/dev/mem", O_RDWR);
	if (*fp == 0) {
		printf("ERROR: %s\n", strerror(errno));
		return 1;
	}

	/* mmap memory */
	*pmem = (uint8_t*)mmap(NULL, length,
			PROT_READ | PROT_WRITE,
			MAP_FILE | MAP_PRIVATE, *fp, base);
	if (*pmem == MAP_FAILED) {
		printf("ERROR: %s\n", strerror(errno));
		close(*fp);
		return 1;
	}
	return 0;
}

static int umount_mem (int *fp, uint8_t **pmem, uint32_t length) {
	/* unmap memory */
	if (munmap(*pmem, length) != 0) {
		printf("ERROR: %s\n", strerror(errno));
		close(*fp);
		return 1;
	}

	if (close(*fp) != 0) {
		printf("ERROR: %s\n", strerror(errno));
		return 1;
	}
	return 0;
}

static void write_to_mem (uint32_t in, uint32_t base, uint32_t offset) {
	int mem;
	uint8_t *pmem;

	/* mount memory */
	if (mount_mem(&mem, &pmem, base, PAGE_SIZE) != 0) {
		return;
	}

	printf("Write %#x, at baseaddr: %#x and offset: %#x\n", in, base, offset);

	/* write value to memory */
	memcpy(pmem + offset, &in, 4);

	/* unmount memory */
	umount_mem(&mem, &pmem, PAGE_SIZE);
}

static unsigned int read_mem (uint32_t base, uint32_t offset) {
	int mem;
	uint8_t *pmem;
	uint32_t value = 0;

	if (mount_mem(&mem, &pmem, base, PAGE_SIZE) != 0) {
		return value;
	}

	printf("Read value from baseaddr: %#x and offset: %#x\n", base, offset);

	/*  read memory value */
	memcpy(&value, pmem + offset, 4);

	/* unmount memory */
	umount_mem(&mem, &pmem, PAGE_SIZE);

	return value;
}

/*  Help function
 *
 */
static void help_function (void) {
	printf("USAGE:\n");
	printf("Read data from memory:\n");
	printf("./mem -r <address>\n");
	printf("example: ./mem -r 0xffca0014\n");
	printf("write data to memory:\n");
	printf("./mem -w <address> <mask> <value>\n");
	printf("example: ./mem -w 0xffca0014 0x1 0x1\n");
	printf("./mem -wo <address> <value>\n");
	printf("example: ./mem -w 0xffca0014 0x1\n");
}

int main(int argc, char **argv) {
	uint32_t value = 0, address, write, mask;

	/* read memory */
	if ((argc == 3) && !strcmp(argv[1], "-r")) {
		sscanf(argv[2], "%x", &address);
		value = read_mem(address & BASE_MASK, address & OFFSET_MASK);
		printf("value:\t%#x\n\t%d\n", value, value);
	/* write mem */
	} else if ((argc == 5) && !strcmp(argv[1], "-w")) {
		sscanf(argv[2], "%x", &address);
		sscanf(argv[3], "%x", &mask);
		sscanf(argv[4], "%x", &write);
		value = read_mem(address & BASE_MASK, address & OFFSET_MASK);
		printf("old value: \t%#x\n", value);
		value = (value & ~mask) | write;
		write_to_mem(value , address & BASE_MASK, address & OFFSET_MASK);
		value = read_mem(address & BASE_MASK, address & OFFSET_MASK);
		printf("new value: \t%#x\n", value);
	/* write only mem */
	} else if ((argc == 4) && !strcmp(argv[1], "-wo")) {
		sscanf(argv[2], "%x", &address);
		sscanf(argv[3], "%x", &write);
		value = write ;
		write_to_mem(value , address & BASE_MASK, address & OFFSET_MASK);
		printf("new value: \t%#x\n", value);
	/* write help any other case */
	} else {
		help_function();
	}

    return 0;
}
