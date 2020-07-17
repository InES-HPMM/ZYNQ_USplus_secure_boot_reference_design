# Crypto API

Short description crypto api.



# Crypto API Implementation

This section describes how to implement an application, which uses the Linux Crypto API.

## Procedure and Interface Configuration

The Linux Crypto API uses sockets and systemcalls to communicate between the user space and kernel space. Depending on the algorithm, the application has to set different attributes like the key and other settings. The rough procedure is for all algorithms the same. The detailed procedure for SHA and AES are described in Section~\ref{sec.crypto-api_hash_procedure} and Section~\ref{sec.crypto-api_aes_procedure}.

The entire session to encrypt or hash data with the Linux Crypto API is done with a socket. It has to be created in the first step and closed in the last step. After the socket is created, it can be used to send data or configuration to the Linux Crypto API or to read the processed data back. At first, the desired algorithm has to be selected. After the algorithm is chosen, the algorithm can be configured. The data is sent to the Linux Crypto API via the socket. Finally, the encrypted or hashed data can be read from the Linux Crypto API. The simplified steps to access the Linux Crypto API look like this:

1. Open a socket.
2. Select the desired algorithm.
3. Set the configuration for the algorithm (when required).
4. Send the data to process to the socket.
5. Read the processed data.

### SHA Hash Procedure

The procedure to hash data with the Linux Crypto API is done in the following steps. The SHA algorithm has no configuration except for the selection of the algorithm. The working example for hash algorithms can be found in Section~\ref{impl.crypto-api_examples}.

#### Open a socket of type AF_ALG

To start the communication with the Linux Crypto API a socket has to be created. All further steps reference to this socket. Therefore, the Linux Crypto API can distinguish between different accesses.

`socket_desc = socket(AF_ALG, SOCK_SEQPACKET, 0);`

#### Bind the socket to the desired algorithm name and type

After the socket is created, it has to be bound to the desired algorithm. To do this, the `type` and `name` of the algorithm has to be selected. For a hash algorithm the `type` is `hash` and the `name` `sha256` for a normal SHA-256. For the Xilinx implementation of the SHA3-384 algorithm, the name is `\code{xilinx-keccak-384`.

`struct sockaddr_alg sa = {
.salg_family = AF_ALG,
.salg_type = "hash",
.salg_name = "sha256",
};
bind(socket_desc, (struct sockaddr*)&sa, sizeof(sa));`

#### Send the data, which needs to be hashed, to the Linux Crypto API

Create a file descriptor to transfer the data with the `accept` systemcall and send the data with the `write` systemcall.

`fd = accept(socket_desc, NULL, 0);
write(fd, data_to_hash, sizeof(data_to_hash));`

#### Get the calculated hash from the Linux Crypto API

The `read` systemcall can be used to get back the calculated hash. The hash size of the algorithm has to be known.

`read(fd, hash, SHA_HASH_SIZE);`

### AES Encryption/Decryption Procedure

The procedure to use the Linux Crypto API to encrypt or decrypt data with the AES CBC algorithm is listed below. The procedure for AES GCM is similar, but uses different configurations for the additional functions of GCM. The examples for CBC and GCM in Section~\ref{impl.crypto-api_examples} show a complete implementation.

#### Open a socket of type AF_ALG

To start the communication with the Linux Crypto API a socket has be created. All further steps reference to this socket. Therefore, the Linux Crypto API can distinguish between different accesses.

`socket_desc = socket(AF_ALG, SOCK_SEQPACKET, 0);`

#### Bind the socket to the desired algorithm name and type

After the socket is created it has to be bound to the desired algorithm. To do this, the `type` and `name` of the algorithm has to be selected. For AES CBC the `type` is `skcipher` and the `name` is `cbc(aes)`.

`struct sockaddr_alg sa = {
.salg_family = AF_ALG,
.salg_type = "skcipher",
.salg_name = "cbc(aes)"
};
bind(socket_desc, (struct sockaddr*)&sa, sizeof(sa));`

#### Set key and key length

The `setsockopt` systemcall is used to set the key and key length. The key length selects if AES 128, 192 or 256 is used.


`#define AES_KEY_LENGTH 32 /* 16 byte -> 128 bit, 24 byte -> 192, 32 byte -> 256 bit */
char aes_key[] = "aes key ...";
setsockopt(sd, SOL_ALG, ALG_SET_KEY, aes_key, AES_KEY_LENGTH);`


#### Create a msghdr structure for further configurations
	
	
The `msghdr` structure holds the IV, the IV length, the data and the direction if the data needs to be encrypted or decrypted. Only the data is directly contained in the `msghdr` structure. The rest is in `cmsg` structures inside the `msghdr` structure.
The `CMSG_SPACE(4)` and `CMSG\_SPACE(20)` are the size for the two `cmsg` structures in the `msghdr` structure described below.

`struct msghdr msg = {};

/* buffer for op, iv and aad data length, used for msghdr (msg) */
char cbuf[CMSG_SPACE(4) + CMSG_SPACE(20)] = {0};
msg.msg_control = cbuf;
msg.msg_controllen = sizeof(cbuf);

/* pointer for elements from cbuf */
struct cmsghdr *cmsg;`





	\item Select the encryption/decryption direction. \newline
	Create the first `cmsg` structure in the `msghdr` structure and select if the data should be encrypted or decrypted. The flag to select the direction is a 4 byte variable, therefore \code{CMSG\_LEN(4)}.~\cite{crypto_api_docu}
	\begin{lstlisting}[style=cstyle]
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(4);
	/* select encryption / decryption (ALG_OP_ENCRYPT / ALG_OP_DECRYPT) */
	*(__u32 *)CMSG_DATA(cmsg) = ALG_OP_ENCRYPT;
	\end{lstlisting}

	\item Select IV and IV length. \newline
	Create the second `cmsg` structure in the `msghdr` structure to set the IV and IV length. The length of the IV is always 128 bit (16 byte) for AES CBC, but can be different for other algorithms. The size of the structure is 20 bytes, 16 bytes for the IV and 4 bytes for the length.~\cite{crypto_api_docu}
	\begin{lstlisting}[style=cstyle]
	char iv[] = "iv ...";

	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(20);
	ivp = (void *)CMSG_DATA(cmsg);
	ivp->ivlen = 16;

	/* set iv for cbc algorithm */
	memcpy(ivp->iv, iv, 16);
	\end{lstlisting}

	\item Send the data to the Linux Crypto API. \newline
	Add the data to encrypt or decrypt to the `msghdr` structure and send it to the Linux Crypto API.~\cite{crypto_api_docu}
	\begin{lstlisting}[style=cstyle]
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
	\end{lstlisting}
\end{enumerate}

Table \ref{tab:crypto-api_aes-cbc} shows the configuration, to use the Linux Crypto API with AES CBC. The data needs to be padded to the block size, the input and output data size is the same.

\begin{table}[h]
	\centering
	\setlength\extrarowheight{5pt}
	\begin{tabularx}{1\textwidth}{|p{3cm}|p{6cm}|p{4cm}|p{1cm}|}
		\hline
		\textbf{Name} & \textbf{Description} & \textbf{Value} & \textbf{Type} \\
		\hline
		Algorithm Type & Type of the used cipher. & \code{skicipher} & string \\
		\hline
		Algorithm Name & Name of the hardware under which the driver is registered. & \code{cbc(aes)} & string \\
		\hline
		AES Key Length & Select the AES key length. & \code{128}, \code{192}, \code{256} & int \\
		\hline
		AES Key & Set the AES key for encryption or decryption. & 128 / 192 / 256 byte \code{key} & \\
		\hline
		IV Length & Select IV length. & \code{16} byte (128 bit) & int  \\
		\hline
		IV & \acrfull{iv} & 16 byte (128 bit) \code{iv} & \\
		\hline
		Encrypt/Decrypt & Select if the data should be encrypted or decrypted. & "\code{ALG\_OP\_ENCRYPT}" "\code{ALG\_OP\_DECRYPT}" & int \\
		\hline
		Data Length & Length of the data to encrypt or decrypt. Must be a multiple of block size. & Length in bytes & int \\
		\hline
	\end{tabularx}
	\caption{Configuration for AES CBC~\cite{crypto_api_docu}~\cite{nist_aes}}
	\label{tab:crypto-api_aes-cbc}
\end{table}

Table \ref{tab:crypto-api_aes-gcm} shows the configuration, to use the Linux Crypto API with AES GCM.

\begin{table}[h]
	\centering
	\setlength\extrarowheight{5pt}
	\begin{tabularx}{1\textwidth}{|p{3cm}|p{6cm}|p{4cm}|p{1cm}|}
		\hline
		\textbf{Name} & \textbf{Description} & \textbf{Value} & \textbf{Type} \\
		\hline
		Algorithm Type & Type of the used cipher. & \code{aead} & string \\
		\hline
		Algorithm Name & Name of the hardware under which the driver is registered. & \code{gcm(aes)} & string \\
		\hline
		AES Key Length & Select the AES key length. & \code{128}, \code{192}, \code{256} & int \\
		\hline
		AES Key & Set the AES key for encryption or decryption. & 128 / 192 / 256 byte key \code{key} & \\
		\hline
		IV Length & Select IV length. & \code{12} byte (96 bit) & int  \\
		\hline
		IV & \acrfull{iv} & 12 byte (96 bit) \code{iv} & \\
		\hline
		Encrypt/Decrypt & Select if the data should be encrypted or decrypted. & "\code{ALG\_OP\_ENCRYPT}" "\code{ALG\_OP\_DECRYPT}" & int \\
		\hline
		AEAD Data Length & Length of the assosiated data, must be multiple of block size. (Data which is only authenticated, not encrypted.) & Length in bytes & int \\
		\hline
		AEAD Data & Assosiated data (Data which is only authenticated, not encrypted.) & aead data &  \\
		\hline
		Data Length & Length of the data to encrypt or decrypt. Must be a multiple of block size. & Length in bytes & int \\
		\hline
	\end{tabularx}
	\caption{Configuration for AES GCM~\cite{crypto_api_docu}~\cite{nist_aes}}
	\label{tab:crypto-api_aes-gcm}
\end{table}

\subsection{Difference of the Xilinx Implementation}\label{impl.crypto-api_xil}

The Linux Crypto API is designed to work platform-independent and presents a consistent interface to the applications. This requires the hardware manufacturer to implement the drivers for the crypto hardware in a certain way. As described in Section~\ref{sec.crypto_api_hw}, the \gls{ultrascale+} has two different crypto hardware blocks implemented. The \gls{crypto-ext} has drivers, who use the generic way to implement the Linux Crypto API. The examples, which use the \gls{crypto-ext}, run on an x86 computer with Linux as well. The drivers for the crypto hardware from Xilinx do not use the usual way. Code, which uses this hardware, will not run on another architecture.~\cite{crypto_api_docu}

\subsubsection{Xilinx SHA Implementation}

The driver for the SHA3-384 hardware of the \gls{ultrascale+} uses the usual approach, but the algorithm is named \code{xilinx-keccak-384}. Apart from the name, the driver is platform-independent.

\subsubsection{Xilinx AES GCM Implementation}

Several points are implemented differently in the driver for the Xilinx crypto hardware for AES GCM. As the driver for the Linux Crypto API from Xilinx is not documented, these differences or limitations are not listed. The differences are found by using the interface and analyzing the results. It is possible that some of the missing features are implemented in a completely different way, which was not discovered during the project.

\paragraph{Key Handling} The \gls{ultrascale+} has built-in key handling. One key is selected during boot to decrypt the boot images. This key can be used to encrypt or decrypt further data. To select this key, a zero string must be passed to the Linux Crypto API as key.

\paragraph{Key length} The key length is fixed to 256 bit, usually AES works with key lengths from 128, 192 and 256 bit.

\paragraph{Authentication tag length} The authentication tag length is fixed to 16 bytes. It usually can be adjusted from 0-16 bytes.

\paragraph{AAD data} The associated data of GCM is not implemented.

\paragraph{Buffer sizes} When the buffer with the plaintext/encrypted data and the AAD data is passed to the Linux Crypto API, the length of the buffer has to be specified. Usually, for encryption, this can be calculated by AAD\_LENGTH + DATA\_LENGTH, and for decryption, it is AAD\_LENGTH + DATA\_LENGTH + AUTH\_TAG\_LENGTH which corresponds to the data which is actually passed to the Linux Crypto API.

In the Xilinx implementation, it is always DATA\_LENGTH + AUTH\_TAG\_LENGTH (the AAD data part is missing, as it is not implemented). Although during encryption, only the data to encrypt is passed to the Linux Crypto API, the buffer size of the data has to include the authentication tag size.

The buffers to send and receive data are separate, the size of the buffer to receive data is calculated as usual.

\paragraph{Name and type} AES GCM is an \acrshort{aead} algorithm, therefore the type of the algorithm in the Linux Crypto API to select is \code{aead} and the name is \code{gcm(aes)}. Xilinx uses the type \code{skcipher} and the name \code{xilinx-zynqmp-aes}.


\subsection{Examples}\label{impl.crypto-api_examples}

In this section, the different examples for the Linux Crypto API are described. Table~\ref{tab:overview_sha_examples} lists the different examples. The examples can be found in the Git repository~\ref{int.project_data}.

\begin{table}[h]
	\setlength\extrarowheight{5pt}
	\begin{tabular}{|p{2cm}|p{4cm}|p{4cm}|p{3cm}|}
		\hline
		\textbf{Example} & \textbf{Hardware} & \textbf{Compatibility} & \textbf{Example file} \\
		\hline
		SHA-256 & \gls{crypto-ext} & compatible with other architectures & crypt-api-sha256.c \\
		\hline
		SHA3-384 & Xilinx crypto hardware & only on \gls{ultrascale+} & crypt-api-sha3-384.c \\
		\hline
		AES CBC & \gls{crypto-ext} & compatible with other architectures & crypt-api-aes-cbc.c \\
		\hline
		AES GCM & & x86 (general GCM implementation) & crypt-api-aes-gcm.c \\
		\hline
		AES GCM 256 & Xilinx crypto hardware & only on \gls{ultrascale+} & crypt-api-aes-gcm-256-zynqmp.c \\
		\hline
	\end{tabular}
	\caption{Overview examples}
	\label{tab:overview_sha_examples}
\end{table}

\subsubsection{Security Reminder}

These examples show how to access the Linux Crypto API and are not a complete tool to encrypt files or data. Algorithms like AES are only secure if used correctly. Read the section~\ref{sec_features.encryption} to understand these algorithms. In general, the key has to be kept in secret and needs a high entropy. The IV must be generated randomly for every encryption. Do not use the key or IV provided in these examples in a production environment.~\cite{nist_aes}

\subsubsection{Requirements}

The compiler \code{aarch64-none-linux-gnu-gcc} Linux applications on the aarch64 architecture is required to compile the examples. It can be downloaded from \href{https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads}{Arm\textregistered}. The provided makefile expects to find the compiler binaries under the following path. If the path is different, it has to be changed in the makefile.

\code{/opt/gcc/gcc-arm-9.2-2019.12-x86\_64-aarch64-none-linux-gnu/bin/}

Some of the examples can run on the x86 architecture. Therefore, they can be tested on a host computer running Linux. The makefile contains the instructions to compile the compatible examples for the host. It requires the \code{gcc} and \code{make-utils} package for the host. If \gls{petalinux} is installed, these packages are already installed.

It is also possible to add the application in \gls{petalinux} and build it with the Linux image.

\subsubsection{SHA Examples}\label{impl.crypto-api_examples_sha}

Two examples are made for SHA. One is for SHA-256 and the other for SHA3-384. The example for SHA-256 can be found in \code{Software/crypto-api/crypto-api-sha256.c}. It uses the \gls{crypto-ext} and should run on most Linux systems.

The example for SHA3-384 can be found in \code{Software/crypto-api/crypto-api-sha3-384.c}. It uses the Xilinx crypto hardware from the \gls{ultrascale+}. Most of the implementation is similar, but the name of the algorithm is \code{xilinx-keccak-384} instead of the generic name.

In both examples the data, which is hashed, is stored in the \code{data\_to\_hash} variable at the beginning of the code. The resulting hash is printed out to the terminal.

\subsubsection{AES Examples}

There are three examples for AES. One example for CBC and two for GCM.

The example for AES CBC can be found in \code{Software/crypto-api/crypto-api-aes-cbc.c}. It uses the \gls{crypto-ext} and should run on most Linux systems.

For AES GCM two examples are available, \code{Software/crypto-api/crypto-api-aes-gcm.c} and \\\code{Software/crypto-api/crypto-api-aes-gcm-256-zynqmp.c}. The first one shows the usual implementation of the Linux Crypto API for AES GCM and runs on the x86 architecture. The second example shows how Xilinx implemented the driver for their crypto hardware in the \gls{ultrascale+}. Section~\ref{impl.crypto-api_xil} describes the differences.

Table~\ref{tab:crypto-api_aes-cbc} for AES CBC and Table~\ref{tab:crypto-api_aes-gcm} for GCM list the different options like keys or data length, which can be modified to use the examples. All the options can be set at the beginning of the examples in the sections marked \code{SETTINGS}. Some of the settings in the AES GCM for the \gls{ultrascale+} are missing, as they are not implemented by Xilinx.
