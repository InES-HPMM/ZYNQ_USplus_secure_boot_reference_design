# TrustZone Example Binaries

This example contains the binaries to boot the Zynq Ultrascale+ with OP-TEE an run a Trusted Application. These files were created with the implementation guide from [crypto-api](here).

The files `boot.bin` and `image.ub` are to boot the Zynq Ultrascale+ with Linux in the non-secure world and OP-TEE in the secure world. The file `8aaaf200-2450-11e4-abe2-0002a5d5c51b.ta` is a Trusted Application and the binary `optee_example_hello_world` is the client application, which runs in Linux and communicates with the Trusted Application.

To test the binaries, the following steps need to be done:

1. Boot the Zynq Ultrascale+ with the `boot.bin` and `image.ub`.
2. Login in Linux with root as username and password.
3. copy the `8aaaf200-2450-11e4-abe2-0002a5d5c51b.ta` file to the folder `/lib/optee\_armtz/`.
4. Start the OP-TEE client with the command `tee-supplicant &`.
5. Execute the client application `./optee_example_hello_world`.

