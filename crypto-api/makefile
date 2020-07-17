GCC_PATH :=  /opt/gcc/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/

all:
	# SHA-256
	$(GCC_PATH)aarch64-none-linux-gnu-gcc crypto-api-sha256.c -o crypto-api-sha256-aarch64
	gcc crypto-api-sha256.c -o crypto-api-sha256-x86

	# SHA3-384
	$(GCC_PATH)aarch64-none-linux-gnu-gcc crypto-api-sha3-384.c -o crypto-api-sha3-384-aarch64

	# AES-CBC-256
	$(GCC_PATH)aarch64-none-linux-gnu-gcc crypto-api-aes-cbc.c -o crypto-api-aes-cbc-aarch64
	gcc crypto-api-aes-cbc.c -o crypto-api-aes-cbc-x86

	# AES-GCM-256
	$(GCC_PATH)aarch64-none-linux-gnu-gcc crypto-api-aes-gcm.c -o crypto-api-aes-gcm-aarch64

	# AES-GCM-256 zynqmp
	gcc crypto-api-aes-gcm-256-zynqmp.c -o crypto-api-aes-gcm-256-zynqmp-x86

clean:
	rm *~ *-aarch64 *-x86
