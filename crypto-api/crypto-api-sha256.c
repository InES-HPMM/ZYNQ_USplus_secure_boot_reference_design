/* 
 * Crypto API example for SHA-256
 *
 * Calculate a SHA-256 hash with the Linux Crypto API.
 *
 * Author: Tobias Vögeli, Thierry Delafontaine
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_alg.h>


#define SHA_HASH_SIZE 32 /* sha256 hash is 32 byte long */

int main () {
	int ret;
	int socket_desc;
	int fd;
	int i;
	
	char hash[SHA_HASH_SIZE];

	/* set data to hash */
	char data_to_hash[] = "abc\n";

	/* create structure and select the hash algorithm sha256 */
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha256",
	};

	/*  create socket of type AF_ALG */
	socket_desc = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (socket_desc == -1) {
		printf("Creating socket failed\n");
		return -1;
	}

	/* bind socket to selected algorithm */
	ret = bind(socket_desc, (struct sockaddr*)&sa, sizeof(sa));
	if (ret == -1) {
		printf("bind failed %s\n", strerror(errno));
		close(socket_desc);
		return -1;
	}

	/* get a file descriptor for data transfer */
	fd = accept(socket_desc, NULL, 0);
	if (fd == -1) {
		printf("accept failed: %s\n", strerror(errno));
		close(socket_desc);
		return -1;
	}

	/* send data to kernel */
	ret = write(fd, data_to_hash, sizeof(data_to_hash) - 1);
	if (ret == -1) {
		printf("write failed: %s\n", strerror(errno));
		close(socket_desc);
		close(fd);
		return -1;
	} else if (ret != (sizeof(data_to_hash) - 1)) {
		printf("write failed: Number of written bytes wrong\n");
		close(socket_desc);
		close(fd);
		return -1;
	}      

	/* get calculated hash from kernel */
	ret = read(fd, hash, SHA_HASH_SIZE);
	if (ret == -1) {
		printf("read failed: %s\n", strerror(errno));
		close(socket_desc);
		close(fd);
		return -1;
	} else if (ret != SHA_HASH_SIZE) {
		printf("read failed: Number of written bytes wrong\n");
		close(socket_desc);
		close(fd);
		return -1;
	}

	/* print hash */
	for (i = 0; i < SHA_HASH_SIZE; i++) {
		printf("%02x", (unsigned char)hash[i]);
	}
	printf("\n");
	
	close(fd);
	close(socket_desc);
	
	return 0;
}

