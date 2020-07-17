/* 
 * Crypto API example for AES CBC
 *
 * Encrypt or decrypt data with AES CBC. This is an example on how the Linux
 * Crypto API works. Don't use the key or iv provided in this example.
 *
 * Author: Tobias Vögeli, Thierry Delafontaine
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <linux/if_alg.h>


/* SETTINGS */
#define AES_MSG_LENGTH 32 /* msg to encrypt is 32 byte long (must be a multiple of 16 bytes */
#define AES_KEY_LENGTH 32 /* 16 byte -> 128 bit, 24 byte -> 192, 32 byte -> 256 bit */



int main() {
	int i;
	int ret;
	int sd;
	int fd;
	/* msghdr structure to send data with settings to kernel */
	struct msghdr msg = {};

	/* buffer for op, iv and aad data length, used for msghdr (msg) */
	char cbuf[CMSG_SPACE(4) + CMSG_SPACE(20)] = {0};
	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);

	/* pointer for elements from cbuf */
	struct cmsghdr *cmsg;

	/* buffer to reveive data from kernel */
	char buf[AES_MSG_LENGTH];



	/* SETTINGS */

	/* aes key, must have length of AES_KEY_LENGTH */
	char aes_key[] = "\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37";
	/* DON'T USE THIS KEY IN PRODUCTION */

	/* set data to encrypt, length must correspond to AES_MSG_LENGTH */
	char data[] = "This is a 32 byte message. abcd\n";

	/* test data for decryption */
	//char data[] = "\x7c\x4b\xae\xd3\x4e\x4f\x34\x2e\x44\xfe\x2c\x54\xfd\x77\x83\xf5\xeb\x99\x45\xd2\x09\x09\xae\xba\x0c\x32\xd4\x04\x29\x6d\x37\xe8";

	/* select encryption or decryption (ALG_OP_ENCRYPT, ALG_OP_DECRYPT) */
	int enc_dir = ALG_OP_ENCRYPT;

	/* set iv (must be 16 byte long for cbc) */
	char iv[] = "\x20\x21\x22\x23\x24\x25\x26\x27\x20\x21\x22\x23\x24\x25\x26\x27";
	/* DON'T USE THIS IV IN PRODUCTION */
	

	
	

	/* prepare buffer to send data to kernel */
	char iov_buf[AES_MSG_LENGTH];
	struct af_alg_iv *ivp;
	struct iovec iov = {iov_buf, AES_MSG_LENGTH};

	/* create structure and select encryption algorithm */
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cbc(aes)"
	};

	/*  create socket of type AF_ALG */
        sd = socket(AF_ALG, SOCK_SEQPACKET, 0);
	if (sd == -1) {
		printf("socket failed: %s\n", strerror(errno));
		return 1;
	}

	/* bind socket to selected algorithm */
	ret = bind(sd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret == -1) {
		printf("bind failed: %s\n", strerror(errno));
		close(sd);
		return 1;
	}

	/* set key for aes encryption */
	ret = setsockopt(sd, SOL_ALG, ALG_SET_KEY, aes_key, AES_KEY_LENGTH);
	if (ret == -1) {
		printf("setsockopt failed: %s\n", strerror(errno));
		close(sd);
		return 1;
	}

	/* get a file descriptor for data transfer */
	fd = accept(sd, NULL, 0);
	if (fd == -1) {
		printf("accept failed: %s\n", strerror(errno));
		close(sd);
		return 1;
	}

	/* settings for encryption */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	*(__u32 *)CMSG_DATA(cmsg) = enc_dir;
	
	/* select encryption / decryption (ALG_OP_ENCRYPT / ALG_OP_DECRYPT) */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(20);
	ivp = (void *)CMSG_DATA(cmsg);
	ivp->ivlen = 16;
	
	/* set iv for cbc algorithm */
	memcpy(ivp->iv, iv, 16);
	
	/* set message to encrypt */
	memcpy(iov_buf, data, AES_MSG_LENGTH);
	iov.iov_len = AES_MSG_LENGTH;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	/* send data to encrypt/decrypt */
	ret = sendmsg(fd, &msg, 0);
	if (ret == -1) {
		printf("sendmsg failed: %s\n", strerror(errno));
		close(sd);
		close(fd);
		return 1;
	}

	/* get encrypted/decrypted data */
	ret = read(fd, buf, AES_MSG_LENGTH);
	if (ret == -1) {
		printf("read failed: %s\n", strerror(errno));
		close(sd);
		close(fd);
		return 1;
	}

	/* print output in console */
	if (enc_dir == ALG_OP_ENCRYPT) {
		for (i = 0; i < AES_MSG_LENGTH; i++) {
			printf("\\x%02x", (unsigned char)buf[i]);
		}
		printf("\n");
	} else {
		printf("%s\n", (unsigned char*)buf);
	}

	close(fd);
	close(sd);

	return 0;
}

