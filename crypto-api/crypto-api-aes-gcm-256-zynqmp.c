/* 
 * Crypto API example for AES GCM on the Xilinx Zynq Ultrascale+
 *
 * Encrypt or decrypt data with AES GCM with the crypto hardware of the Xilinx
 * Zynq Ultrascale+. This is an example on how the Linux Crypto API works. Don't
 * use the key or iv provided in this example.
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

#define ALG_SET_KEY_TYPE		6


#define AES_KEY_LENGTH 32 /* 256 bit key length */
#define AUTH_TAG_LENGTH 16 /* authentication tag length */


/* SETTINGS */
#define AAD_LENGTH 0 /* associated data length */
#define DATA_LENGTH 64 /* encrypted data length */



int main() {
	int i;
	int ret;
	int sd;
	int fd;
	/* msghdr structure to send data with settings to kernel */
	struct msghdr msg = {};
	
	/* buffer for op, iv and aad data length, used for msghdr (msg) */
	char cbuf[CMSG_SPACE(4) + CMSG_SPACE(16) + CMSG_SPACE(4)] = {0};
	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);
	
	/* pointer for elements from cbuf */
	struct cmsghdr *cmsg;
	
	/* buffer to reveive data from kernel */
	char buf[AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH];
	
	/* struct pointer for iv */
	struct af_alg_iv *ivp;

	

	/* SETTINGS */

	/* set aes key (must be 32 byte long for gcm 256 bit) */
	char aes_key[] = "\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37\x30\x31\x32\x33\x34\x35\x36\x37";
	/* DON'T USE THIS KEY IN PRODUCTION */

	/* to select the key from the eFuse/BBRAM of the Ultrascale+, set the key variable to 0 */
        //memset(aes_key, 0, 32);

	/* set iv (must be 12 byte long for gcm 256 bit) */
	char iv[] = "\x20\x21\x22\x23\x24\x25\x26\x27\x20\x21\x22\x23";
	/* DON'T USE THIS IV IN PRODUCTION */

	/* set aad data (not encrypted, only authenticated), length must correspond to AAD_LENGTH */
	char aad[] = "123456789012345\n";

	/* set data (to encrypted/decrypt), length must correspond to DATA_LENGTH */
	char data[] = "This is a 32 byte message. abcd This is a 32 byte message. abcd\n";

	/* test data for decryption */
	//char data[] = "\xed\x25\x03\x63\xb2\xb6\xe0\xd3\xf3\x25\xa8\x04\xdb\x24\x80\xa2\xac\xae\x18\xbd\x41\x91\xf3\xd7\xa8\x74\x67\x52\xb6\x22\x01\x2a\x3d\x5e\x5e\x02\x0c\x4b\xcf\x32\xa8\x34\x37\x51\xf9\xf1\xab\x5d\x65\x0e\x0b\x2e\xe5\x02\x2c\x98\xbc\x8f\x12\xa9\x83\xea\x14\x76";

	/* set authentication tag (only used for decryption), length must correspond to AUTH_TAG_LENGTH */
	char auth_tag[] = "\xb6\x42\x75\x4c\x0f\xa2\xdd\xc0\x04\x5d\x03\x51\x13\xbf\xea\x9e";

	/* select encryption or decryption (ALG_OP_ENCRYPT, ALG_OP_DECRYPT) */
	int enc_dir = ALG_OP_ENCRYPT;




	
	/* prepare buffer to send data to kernel */
	char iov_buf[AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH];
	struct iovec iov;
	iov.iov_base = iov_buf;
	if (enc_dir == ALG_OP_ENCRYPT) {
		iov.iov_len = AAD_LENGTH + DATA_LENGTH;
	} else {
		iov.iov_len = AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH;
	}

	/* select encryption algorithm */
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "xilinx-zynqmp-aes"
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
		printf("setsockopt ALG_SET_KEY failed: %s\n", strerror(errno));
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

	/* select encrypt / decrypt */
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4); /* data size 4 Bytes (uint32) */
	*(__u32 *)CMSG_DATA(cmsg) = enc_dir;

	/* set iv of type struct af_alg_iv */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(16); /* sizeof iv struct (4 Byte len + 12 Byte data) */
	ivp = (void *)CMSG_DATA(cmsg);
	ivp->ivlen = 12;
	memcpy(ivp->iv, iv, 12);

	/* set aad data length */
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_AEAD_ASSOCLEN;
	cmsg->cmsg_len = CMSG_LEN(4); /* data size 4 Bytes (uint32) */
	*(__u32 *)CMSG_DATA(cmsg) = AAD_LENGTH;
	
	/* copy aad and data to iov buffer */
	if (enc_dir == ALG_OP_ENCRYPT) {
		memcpy(iov_buf, aad, AAD_LENGTH);
		memcpy(&iov_buf[AAD_LENGTH], data, DATA_LENGTH);
		iov.iov_len = AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH;
	} else {
		memcpy(iov_buf, aad, AAD_LENGTH);
		memcpy(&iov_buf[AAD_LENGTH], data, DATA_LENGTH);
		memcpy(&iov_buf[AAD_LENGTH + DATA_LENGTH], auth_tag, AUTH_TAG_LENGTH);
		iov.iov_len = AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH;
	}	
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
	if (enc_dir == ALG_OP_ENCRYPT) {
		ret = read(fd, buf, AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH);
	} else {
		ret = read(fd, buf, AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH);
	}	
	if (ret == -1) {
		printf("read failed: %s\n", strerror(errno));
		close(sd);
		close(fd);
		return 1;
	}

	/* print output in console */
	if (enc_dir == ALG_OP_ENCRYPT) {
		printf("AAD:\n");
		for (i = 0; i < AAD_LENGTH; i++) {
			printf("\\x%02x", (unsigned char)buf[i]);
		}
		printf("\nDATA:\n");
		for (i = 0; i < DATA_LENGTH; i++) {
			printf("\\x%02x", (unsigned char)buf[AAD_LENGTH + i]);
		}
		printf("\nAuthentification Tag:\n");
		for (i = 0; i < AUTH_TAG_LENGTH; i++) {
			printf("\\x%02x", (unsigned char)buf[AAD_LENGTH + DATA_LENGTH + i]);
		}
		printf("\n");
	} else {
		buf[AAD_LENGTH + DATA_LENGTH] = 0;
		printf("%s\n", buf);
		for (i = 0; i < DATA_LENGTH + AAD_LENGTH; i++) {
			printf("\\x%02x", (unsigned char)buf[i]);
		}
	}

	close(fd);
	close(sd);

	return 0;
}

