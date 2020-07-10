GCC_PATH :=  /opt/gcc/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/
CROSS_COMPILER := aarch64-none-linux-gnu-

CC := $(CROSS_COMPILER)gcc

all:
	$(GCC_PATH)$(CC) read_temp.c -o read_temp
	$(GCC_PATH)$(CC) mem.c -o mem
clean:
	rm *~ read-temp mem
