# Crypto API

Short description crypto api.



# Crypto API Implementation

This section describes how to implement an application, which uses the Linux Crypto API.

## Procedure and Interface Configuration

The Linux Crypto API uses sockets and systemcalls to communicate between the user space and kernel space. Depending on the algorithm, the application has to set different attributes like the key and other settings. The rough procedure is for all algorithms the same.

The entire session to encrypt or hash data with the Linux Crypto API is done with a socket. It has to be created in the first step and closed in the last step. After the socket is created, it can be used to send data or configuration to the Linux Crypto API or to read the processed data back. At first, the desired algorithm has to be selected. After the algorithm is chosen, the algorithm can be configured. The data is sent to the Linux Crypto API via the socket. Finally, the encrypted or hashed data can be read from the Linux Crypto API. The simplified steps to access the Linux Crypto API look like this:

1. Open a socket.
2. Select the desired algorithm.
3. Set the configuration for the algorithm (when required).
4. Send the data to process to the socket.
5. Read the processed data.

### SHA Hash Procedure

The procedure to hash data with the Linux Crypto API is done in the following steps. The SHA algorithm has no configuration except for the selection of the algorithm.

#### Open a socket of type AF_ALG

To start the communication with the Linux Crypto API a socket has to be created. All further steps reference to this socket. Therefore, the Linux Crypto API can distinguish between different accesses.

	socket_desc = socket(AF_ALG, SOCK_SEQPACKET, 0);

#### Bind the socket to the desired algorithm name and type

After the socket is created, it has to be bound to the desired algorithm. To do this, the `type` and `name` of the algorithm has to be selected. For a hash algorithm the `type` is `hash` and the `name` `sha256` for a normal SHA-256. For the Xilinx implementation of the SHA3-384 algorithm, the name is `\code{xilinx-keccak-384`.

	struct sockaddr_alg sa = {
	 .salg_family = AF_ALG,
	 .salg_type = "hash",
	 .salg_name = "sha256",
	};

	bind(socket_desc, (struct sockaddr*)&sa, sizeof(sa));`

#### Send the data, which needs to be hashed, to the Linux Crypto API

Create a file descriptor to transfer the data with the `accept` systemcall and send the data with the `write` systemcall.

	fd = accept(socket_desc, NULL, 0);
	write(fd, data_to_hash, sizeof(data_to_hash));

#### Get the calculated hash from the Linux Crypto API

The `read` systemcall can be used to get back the calculated hash. The hash size of the algorithm has to be known.

	read(fd, hash, SHA_HASH_SIZE);

### AES Encryption/Decryption Procedure

The procedure to use the Linux Crypto API to encrypt or decrypt data with the AES CBC algorithm is listed below. The procedure for AES GCM is similar, but uses different configurations for the additional functions of GCM. The examples for CBC and GCM show a complete implementation.

#### Open a socket of type AF_ALG

To start the communication with the Linux Crypto API a socket has be created. All further steps reference to this socket. Therefore, the Linux Crypto API can distinguish between different accesses.

	socket_desc = socket(AF_ALG, SOCK_SEQPACKET, 0);

#### Bind the socket to the desired algorithm name and type

After the socket is created it has to be bound to the desired algorithm. To do this, the `type` and `name` of the algorithm has to be selected. For AES CBC the `type` is `skcipher` and the `name` is `cbc(aes)`.

	struct sockaddr_alg sa = {
	 .salg_family = AF_ALG,
	 .salg_type = "skcipher",
	 .salg_name = "cbc(aes)"
	};

	bind(socket_desc, (struct sockaddr*)&sa, sizeof(sa));

#### Set key and key length

The `setsockopt` systemcall is used to set the key and key length. The key length selects if AES 128, 192 or 256 is used.

	#define AES_KEY_LENGTH 32 /* 16 byte -> 128 bit, 24 byte -> 192, 32 byte -> 256 bit */
	char aes_key[] = "aes key ...";
	setsockopt(sd, SOL_ALG, ALG_SET_KEY, aes_key, AES_KEY_LENGTH);

#### Create a msghdr structure for further configurations
	
The `msghdr` structure holds the IV, the IV length, the data and the direction if the data needs to be encrypted or decrypted. Only the data is directly contained in the `msghdr` structure. The rest is in `cmsg` structures inside the `msghdr` structure.
The `CMSG_SPACE(4)` and `CMSG\_SPACE(20)` are the size for the two `cmsg` structures in the `msghdr` structure described below.

	struct msghdr msg = {};

	/* buffer for op, iv and aad data length, used for msghdr (msg) */
	char cbuf[CMSG_SPACE(4) + CMSG_SPACE(20)] = {0};
	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);

	/* pointer for elements from cbuf */
	struct cmsghdr *cmsg;

#### Select the encryption/decryption direction

Create the first `cmsg` structure in the `msghdr` structure and select if the data should be encrypted or decrypted. The flag to select the direction is a 4 byte variable, therefore `CMSG\_LEN(4)`.

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);

	/* select encryption / decryption (ALG_OP_ENCRYPT / ALG_OP_DECRYPT) */
	*(__u32 *)CMSG_DATA(cmsg) = ALG_OP_ENCRYPT;


#### Select IV and IV length
	
Create the second `cmsg` structure in the `msghdr` structure to set the IV and IV length. The length of the IV is always 128 bit (16 byte) for AES CBC, but can be different for other algorithms. The size of the structure is 20 bytes, 16 bytes for the IV and 4 bytes for the length.

	char iv[] = "iv ...";
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(20);
	ivp = (void *)CMSG_DATA(cmsg);
	ivp->ivlen = 16;

	/* set iv for cbc algorithm */
	memcpy(ivp->iv, iv, 16);


#### Send the data to the Linux Crypto API. \newline
	
Add the data to encrypt or decrypt to the `msghdr` structure and send it to the Linux Crypto API.

	char iov_buf[AES_MSG_LENGTH];
	struct af_alg_iv *ivp;
	struct iovec iov = {iov_buf, AES_MSG_LENGTH};

	char data[] = "Data to encrypt or decrypt ...";

	memcpy(iov_buf, data, AES_MSG_LENGTH);
	iov.iov_len = AES_MSG_LENGTH;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	sendmsg(fd, &msg, 0);
	\end{lstlisting}

	\item Get the encrypted or decrypted data from the Linux Crypto API.~\cite{crypto_api_docu}
	\begin{lstlisting}[style=cstyle]
	/* buffer to reveive data from kernel */
	char buf[AES_MSG_LENGTH];

	read(fd, buf, AES_MSG_LENGTH);


The following Table shows the configuration, to use the Linux Crypto API with AES CBC. The data needs to be padded to the block size, the input and output data size is the same.

| Name | Description | Value | Type |
| --- | --- | --- | --- |
| Algorithm Type | Type of the used cipher. | skicipher | string |
Algorithm Name | Name of the hardware under which the driver is registered. | cbc(aes) | string |
AES Key Length | Select the AES key length. | 128, 192, 256 | int |
AES Key | Set the AES key for encryption or decryption. | 128 / 192 / 256 byte key | |
IV Length | Select IV length. | 16 byte (128 bit) | int  |
IV | IV | 16 byte (128 bit) iv | |
Encrypt/Decrypt | Select if the data should be encrypted or decrypted. | ALG_OP_ENCRYPT / ALG_OP_DECRYPT | int |
Data Length | Length of the data to encrypt or decrypt. Must be a multiple of block size. | Length in bytes | int |


The next Table shows the configuration, to use the Linux Crypto API with AES GCM.

| Name | Description | Value | Type |
| --- | --- | --- | --- |
Algorithm Type | Type of the used cipher. | aead | string |
Algorithm Name | Name of the hardware under which the driver is registered. | gcm(aes) | string |
AES Key Length | Select the AES key length. | 128, 192, 256 | int |
AES Key | Set the AES key for encryption or decryption. | 128 / 192 / 256 byte key key | |
IV Length | Select IV length. | 12 byte (96 bit) | int |
IV | Initalization Vector (IV) | 12 byte (96 bit) iv | |
Encrypt/Decrypt | Select if the data should be encrypted or decrypted. | ALG_OP_ENCRYPT/  ALG_OP_DECRYPT | int |
AEAD Data Length | Length of the assosiated data, must be multiple of block size. (Data which is only authenticated, not encrypted.) | Length in bytes | int |
AEAD Data | Assosiated data (Data which is only authenticated, not encrypted.) | aead data | |
Data Length | Length of the data to encrypt or decrypt. Must be a multiple of block size. | Length in bytes | int |


## Difference of the Xilinx Implementation

The Linux Crypto API is designed to work platform-independent and presents a consistent interface to the applications. This requires the hardware manufacturer to implement the drivers for the crypto hardware in a certain way. Tthe Zynq Ultrascale+ has two different crypto hardware blocks implemented. The ARM Cryptography Extension has drivers, who use the generic way to implement the Linux Crypto API. The examples, which use the ARM Cryptography Extension, run on an x86 computer with Linux as well. The drivers for the crypto hardware from Xilinx do not use the usual way. Code, which uses this hardware, will not run on another architecture.

### Xilinx SHA Implementation

The driver for the SHA3-384 hardware of the Zynq Ultrascale+ uses the usual approach, but the algorithm is named `xilinx-keccak-384`. Apart from the name, the driver is platform-independent.

### Xilinx AES GCM Implementation

Several points are implemented differently in the driver for the Xilinx crypto hardware for AES GCM. As the driver for the Linux Crypto API from Xilinx is not documented, these differences or limitations are not listed. The differences are found by using the interface and analyzing the results. It is possible that some of the missing features are implemented in a completely different way, which was not discovered during the project.

#### Key Handling

The Zynq Ultrascale+ has built-in key handling. One key is selected during boot to decrypt the boot images. This key can be used to encrypt or decrypt further data. To select this key, a zero string must be passed to the Linux Crypto API as key.

#### Key length

The key length is fixed to 256 bit, usually AES works with key lengths from 128, 192 and 256 bit.

#### Authentication tag length

The authentication tag length is fixed to 16 bytes. It usually can be adjusted from 0-16 bytes.

#### AAD data

The associated data of GCM is not implemented.

#### Buffer sizes

When the buffer with the plaintext/encrypted data and the AAD data is passed to the Linux Crypto API, the length of the buffer has to be specified. Usually, for encryption, this can be calculated by AAD_LENGTH + DATA_LENGTH, and for decryption, it is AAD_LENGTH + DATA_LENGTH + AUTH_TAG_LENGTH which corresponds to the data which is actually passed to the Linux Crypto API.

In the Xilinx implementation, it is always DATA_LENGTH + AUTH_TAG_LENGTH (the AAD data part is missing, as it is not implemented). Although during encryption, only the data to encrypt is passed to the Linux Crypto API, the buffer size of the data has to include the authentication tag size.

The buffers to send and receive data are separate, the size of the buffer to receive data is calculated as usual.

#### Name and type

AES GCM is an AEAD algorithm, therefore the type of the algorithm in the Linux Crypto API to select is `aead` and the name is `gcm(aes)`. Xilinx uses the type `skcipher` and the name `xilinx-zynqmp-aes`.


## Examples

In this section, the different examples for the Linux Crypto API are described. The following table lists the different examples.

|Example | Hardware | Compatibility | Example file |
| --- | --- | --- | --- |
SHA-256 | ARM Cryptography Extension | compatible with other architectures | crypt-api-sha256.c |
SHA3-384 | Xilinx crypto hardware | only on Zynq Ultrascale+ | crypt-api-sha3-384.c |
AES CBC | ARM Cryptography Extension | compatible with other architectures | crypt-api-aes-cbc.c |
AES GCM | | x86 (general GCM implementation) | crypt-api-aes-gcm.c |
AES GCM 256 | Xilinx crypto hardware | only on Zynq Ultrascale+ | crypt-api-aes-gcm-256-zynqmp.c |


### Security Reminder

These examples show how to access the Linux Crypto API and are not a complete tool to encrypt files or data. Algorithms like AES are only secure if used correctly. In general, the key has to be kept in secret and needs a high entropy. The IV must be generated randomly for every encryption. Do not use the key or IV provided in these examples in a production environment.

### Requirements

The compiler aarch64-none-linux-gnu-gcc Linux applications on the aarch64 architecture is required to compile the examples. It can be downloaded from https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads}{Arm\textregistered}. The provided makefile expects to find the compiler binaries under the following path. If the path is different, it has to be changed in the makefile.

	/opt/gcc/gcc-arm-9.2-2019.12-x86\_64-aarch64-none-linux-gnu/bin/

Some of the examples can run on the x86 architecture. Therefore, they can be tested on a host computer running Linux. The makefile contains the instructions to compile the compatible examples for the host. It requires the gcc and make-utils package for the host. If Petalinux is installed, these packages are already installed.

It is also possible to add the application in \gls{petalinux} and build it with the Linux image.

### SHA Examples

Two examples are made for SHA. One is for SHA-256 and the other for SHA3-384. The example for SHA-256 uses the ARM Cryptography Extension and should run on most Linux systems.

The example for SHA3-384 uses the Xilinx crypto hardware from the Zynq Ultrascale+. Most of the implementation is similar, but the name of the algorithm is `xilinx-keccak-384` instead of the generic name.

In both examples the data, which is hashed, is stored in the `data_to_hash` variable at the beginning of the code. The resulting hash is printed out to the terminal.

### AES Examples

There are three examples for AES. One example for CBC and two for GCM.

The example for AES CBC  uses the ARM Cryptography Extension and should run on most Linux systems.

For AES GCM two examples are available. The first one shows the usual implementation of the Linux Crypto API for AES GCM and runs on the x86 architecture. The second example shows how Xilinx implemented the driver for their crypto hardware in the Zynq Ultrascale+.

All the options can be set at the beginning of the examples in the sections marked `SETTINGS`. Some of the settings in the AES GCM for the Zynq Ultrascale+ are missing, as they are not implemented by Xilinx.
