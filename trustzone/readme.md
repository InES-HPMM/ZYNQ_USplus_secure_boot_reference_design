# TrustZone Implementation

This section describes the procedure to implement TrustZone in a project. The first section shows the implementation of the ATF, the second section shows the implementation of OP-TEE. In the folder [images](images) are example binaries, create with this guide.

## ARM Trusted Firmware (ATF) Implementation

The ARM Trusted Firmware (ATF) is the Secure Monitor from ARM. As it is required to boot the Zynq Ultrascale+, the ATF is included in every project. If TrustZone is not needed in a project, the ATF is not used after boot. To add custom SMC, the source code of the ATF needs to be modified and recompiled. The ATF can either be compiled on its own or as part of a Petalinux project.

### ATF in Petalinux project

The ATF is included in every Petalinux project for the Zynq Ultrascale+ by default. The generated configuration can be found under the following path:

	<plnx-proj-root>/project-spec/meta-plnx-generated/recipes-bsp/arm-trusted-firmware/

After the project is built, the binary file for the ATF can be found in:

	<plnx-proj-root>/images/linux/bl31.elf

### ATF standalone project

To compile the ATF outside of a Petalinux project, the source code is needed. The ATF source code from Xilinx can be found on their Github page: https://github.com/Xilinx/arm-trusted-firmware.

The compiler for aarch64 bare-metal target is needed (aarch64-none-elf). It can be downloaded from ARM: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads.

To compile the ATF the following command has to be executed in the root directory of the ATF source:

    make CROSS\_COMPILE=<compiler-path>/bin/aarch64-none-elf- PLAT=zynqmp bl31

The finished binary is placed at the following path:

	<atf-source-root>/build/zynqmp/release/bl31/bl31.elf

### Package ATF

The ATF binary has to be packaged with the other software binaries, in order to be executed on the Zynq Ultrascale+. If Petalinux is used to compile the ATF, the normal command to create the package can be used. The ATF binary is added automatically.

	petalinux-package -{}-boot -{}-fsbl <FSBL-ELF> -{}-fpga <BITSTREAM> -{}-u-boot -{}-pmufw <PMUFW-ELF>}

To generate the package with an ATF binary outside from the Petalinux project, the following command can be used:

	petalinux-package -{}-boot -{}-fsbl <FSBL-ELF> -{}-fpga <BITSTREAM> -{}-u-boot -{}-pmufw <PMUFW-ELF> -{}-atf <atf-source-root>/build/zynqmp/release/bl31/bl31.elf


## OP-TEE Implementation

The following section describes how to create a Petalinux project with OP-TEE. It is further explained how to compile and load a Trusted Application (TA). The OP-TEE project only worked on the development board zcu102 from Xilinx. It does not boot on the Mercury board from Enclustra. The problem for this could not be found during this work. Therefore, this implementation describes only the solution for the zcu102 board.

### Setup Petalinux project with OP-TEE

OP-TEE has an example project for the Zynq Ultrascale+, but some additional steps are required to get the project running. The OP-TEE Documentation (https://optee.readthedocs.io/en/latest/index.html) describes the example.

The OP-TEE example is made for Petalinux 2018.2 and does not work with version 2019.2. The project can be compiles after the version check is removed. However, it does not boot if, OP-TEE was not compiled with version 2018.2.

Since the OP-TEE example for the Zynq Ultrascale+ was created, OP-TEE was developed further and the example no longer works. The OP-TEE realeses from the time, when the example was created, are used to test the example. The following table lists the different parts of OP-TEE and the used releases.

| Name | Link | Git Commit | Date |
| --- | --- | --- | --- |
| OP-TEE \acrshort{os} | https://github.com/OP-TEE/optee_os.git | e61fc00f9643fb55f2b19d1168d86b9b15d8d9c9 | 19.04.2019 |
| OP-TEE Client | https://github.com/OP-TEE/optee_client.git | e9e55969d76ddefcb5b398e592353e5c7f5df198 | 14.05.2020 (latest) |
| OP-TEE Test | https://github.com/OP-TEE/optee_test.git | 895c5caa9070a134bc12acdc6d0ad0354aa1f644 | 28.02.2019 |


This are the required steps to create the OP-TEE project, the description is similar to the guide in the OP-TEE documentation, but has some additional steps.

#### Create a new directory for the project. 

The new directory serves as the root directory for the project
	
#### Place the \acrfull{bsp} in the project root directory

The BSP for Xilinx boards can be downloaded from the Xilinx download page. The file for the zcu102 board is xilinx-zcu102-v2018.2-final.bsp. (As the OP-TEE example was made for Petalinux 2018.2, the BSP for this version needs to be selected.
	
#### Clone the OP-TEE build repository into the project root directory.

	git clone https://github.com/OP-TEE/build
	
#### Build the project

The cloned repository contains a makefile to create the project. By default, the project is generated for the zcu102 board from Xilinx. The makefile creates the project and starts to build, the first build will fail, as some additional configurations need to be made, as described in the next steps.

	cd build/
	make -f zynqmp.mk
	
#### Clone the OP-TEE OS and Test repository into the project root directory

OP-TEE has 3 parts, optee_os, optee_client and optee_test. At the time of the implementation, the latest version of optee\_client worked and only optee_os and optee_test needed older versions.

	git clone https://github.com/OP-TEE/optee_os.git
	git clone https://github.com/OP-TEE/optee_test.git
	
#### Revert the OP-TEE repositories

Revert the repositories to the state, when the example was created.

    cd optee\_os
    git revert e61fc00f9643fb55f2b19d1168d86b9b15d8d9c9
    cd ../optee\_test
    git revert 895c5caa9070a134bc12acdc6d0ad0354aa1f644
	
#### Change the configuration files to use the local repositories for OP-TEE
	
Open file \code{zcu102-2018.2/project-spec/meta-user/recipes-bsp/optee-os/optee-os.bb} and modify line 27:

	REPO ??= "git://<path-to-proj-root-dir>/optee_os/;protocol=file"

Open file \code{zcu102-2018.2/project-spec/meta-user/recipes-apps/optee-test/optee-test.bb} and modify line 21:

	REPO ??= "git://<path-to-proj-root-dir>/optee_test;protocol=file"

	
#### Recompile the Petalinux project

After the configuration is made, the project needs to be recompiled, this time it the build should complete without errors.

    cd <path-to-proj-root-dir>/zcu102-2018.2/
    petalinux-build
	
After the build is finished, the following files should be in folder `<path-to-proj-root-dir>/zcu102-2018.2/images/linux/`:

    zynqmp\_fsbl.elf (FSBL Binary)
    pmufw.elf (PMU Firmware Binary)
    u-boot.elf (U-Boot Binary)
    system.bit (Bitstream)
    bl31.bin (ATF Binary)
    bl32.elf (OP-TEE Binary)
    image.ub (Linux)
	
#### Package the binaries

Create a file named `boot.bif` with the configuration for the package:

	image : {
	[ /* fsbl */
	bootloader,
	destination_cpu         = a53-0
	] zynqmp_fsbl.elf

	[ /* pmufw */
	destination_cpu         = pmu
	] pmufw.elf

	[ /* bitstream */
	destination_device      = pl
	] system.bit

	[ /* trusted firmware */
	trustzone,
	exception_level         = el-3,
	destination_cpu         = a53-0
	] bl31.elf

	[ /* u-boot */
	exception_level         = el-2,
	destination_cpu         = a53-0
	] u-boot.elf

	[ /* op-tee */
	trustzone,
	exception_level		= el-1,
	destination_cpu		= a53-0
	] bl32.elf
	}

With the following command the package can be created:

    bootgen -arch zynqmp -image boot.bif -o i boot.bin
	
#### Boot the Zynq Ultrascale+

The files `boot.bin` and `image.ub` are needed to boot the Zynq Ultrascale+. During boot, OP-TEE is loaded in the secure world and Linux in the non-secure world.
	
#### Start the OP-TEE client and run the self test

The OP-TEE client can be started with the command:

    tee-supplicant &
	
To check if OP-TEE is initialized and started correctly, the self test can be run:

    xtest
	
The system is now ready to load and execute a TA.




### Setup Toolchain for Trusted Application (TA)

After OP-TEE is configured and running, a TA can be loaded and executed. The OP-TEE toolchain has to be set up to build a TA. As OP-TEE is builed with Petalinux, the toolchain was not needed for this step.

The following script is an example of how the OP-TEE toolchain can be installed:

	mkdir -p $HOME/toolchains
	cd $HOME/toolchains

	# Download 32bit toolchain
	wget https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2019.01/gcc-arm-8.2-2019.01-x86_64-arm-linux-gnueabi.tar.xz
	mkdir aarch32
	tar xf gcc-arm-8.2-2019.01-x86_64-arm-linux-gnueabi.tar.xz -C aarch32 --strip-components=1

	# Download 64bit toolchain
	wget https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2019.01/gcc-arm-8.2-2019.01-x86_64-aarch64-linux-gnu.tar.xz
	mkdir aarch64
	tar xf gcc-arm-8.2-2019.01-x86_64-aarch64-linux-gnu.tar.xz -C aarch64 --strip-components=1


It can be installed at any path on the system, it just needs to be sourced before the next steps:

	export PATH=\$PATH:\$HOME/toolchains/aarch32/bin:\$HOME/toolchains/aarch64/bin}~\cite{optee_toolchain

Clone the OP-TEE OS repository and build it:

	git clone https://github.com/OP-TEE/optee\_os.git

	make CFG\_ARM64\_core=y CFG\_TEE\_BENCHMARK=n CFG\_TEE\_CORE\_LOG\_LEVEL=3 CROSS\_COMPILE=aarch64-linux-gnu-  CROSS\_COMPILE\_core=aarch64-linux-gnu- CROSS\_COMPILE\_ta\_arm32=arm-linux-gnueabi- CROSS\_COMPILE\_ta\_arm64=aarch64-linux-gnu- DEBUG=1 O=out/arm PLATFORM=zynqmp

Clone the OP-TEE client repository and built it:

	git clone https://github.com/OP-TEE/optee\_client.git

	make CROSS\_COMPILE=aarch64-linux-gnu-

### Compile a Trusted Application (TA)

The Git repository https://github.com/linaro-swg/optee_examples contains a number of different example TAs. The examples consist of a `host` and a `ta` part. The `ta` part is the TA, which is running in the secure world, while the `host` part is the client application running in Linux, which communicate with the TA.

To prohibit the loading of a malicious TA, the TAs are signed. OP-TEE supports only a hardcoded key. The OP-TEE OS has a folder \code{keys} that contains the key, which is compiled into OP-TEE. During the build, the TA is automatically signed. The default key should be changed if the software is used in production.

Switch to the `host` folder of an example and run the command to compile the `host` part:

	make CROSS\_COMPILE=aarch64-linux-gnu- TEEC\_EXPORT=<optee\_client>/out/export/usr --no-builtin-variables}

Change to the `ta` folder to compile the TA.

	make CROSS\_COMPILE=arm-linux-gnueabi- PLATFORM=zynqmp TA\_DEV\_KIT\_DIR=<optee\_os>/out/arm/export-ta\_arm32}

To execute the TA, copy the new binary inside the `host` folder and the `*.ta` file from the `ta` folder to the Zynq Ultrascale+. Move the `*.ta` file to the folder `/lib/optee\_armtz/` and execute the binary.
