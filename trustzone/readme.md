# TrustZone Implementation

This section describes the procedure to implement TrustZone in a project. The
first section shows the implementation of the ATF, the second section shows the
implementation of OP-TEE. In the folder [images](images) are example binaries,
create with this guide.

## ARM Trusted Firmware (ATF) Implementation

The ARM Trusted Firmware (ATF) is the Secure Monitor from ARM. As it is required
to boot the Zynq Ultrascale+, the ATF is included in every project. If TrustZone
is not needed in a project, the ATF is not used after boot. To add custom SMC,
the source code of the ATF needs to be modified and recompiled. The ATF can
either be compiled on its own or as part of a Petalinux project.

### ATF in Petalinux project

The ATF is included in every Petalinux project for the Zynq Ultrascale+ by
default. The generated configuration can be found under the following path:

    <plnx-proj-root>/project-spec/meta-plnx-generated/recipes-bsp/arm-trusted-firmware/

After the project is built, the binary file for the ATF can be found in:

    <plnx-proj-root>/images/linux/bl31.elf

### ATF standalone project

To compile the ATF outside of a Petalinux project, the source code is needed.
The ATF source code from Xilinx can be found on their Github page:
https://github.com/Xilinx/arm-trusted-firmware.

The compiler for aarch64 bare-metal target is needed (aarch64-none-elf). It can
be downloaded from ARM:
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads.

To compile the ATF the following command has to be executed in the root
directory of the ATF source:

    make CROSS\_COMPILE=<compiler-path>/bin/aarch64-none-elf- PLAT=zynqmp bl31

The finished binary is placed at the following path:

    <atf-source-root>/build/zynqmp/release/bl31/bl31.elf

### Package ATF

The ATF binary has to be packaged with the other software binaries, in order to
be executed on the Zynq Ultrascale+. If Petalinux is used to compile the ATF,
the normal command to create the package can be used. The ATF binary is added
automatically.

    petalinux-package -{}-boot -{}-fsbl <FSBL-ELF> -{}-fpga <BITSTREAM>
    -{}-u-boot -{}-pmufw <PMUFW-ELF>}

To generate the package with an ATF binary outside from the Petalinux project,
the following command can be used:

    petalinux-package -{}-boot -{}-fsbl <FSBL-ELF> -{}-fpga <BITSTREAM>
    -{}-u-boot -{}-pmufw <PMUFW-ELF> -{}-atf
    <atf-source-root>/build/zynqmp/release/bl31/bl31.elf


## OP-TEE Implementation

The following section describes how to create a Petalinux project with OP-TEE.
It is further explained how to compile and load a Trusted Application (TA).
Because the prebuilt version of OP-TEE from the built repository did not work on
the Mercury XU5 board, the default meta-linaro layer was added to the project.
OP-TEE has its own documentation on
https://optee.readthedocs.io/en/latest/index.html.

### Sample project from OP-TEE/built

OP-TEE has an example project for the Zynq Ultrascale+ evaluation boards, but
some additional steps are required to get the project running. The project is
not supported anymore and is very delicate to build. Thus, we do not recommend
to use it. Instead, follow the steps in this description to implement OP-TEE.

### Add the linaro layer to the petalinux project

The `meta-linaro` layer is the default layer to implement OP-TEE in yocto
projects. Because petalinux is built upon petalinux, the laer can be added to
the projec, with only some minor adjustments.

1. First the meta-linaro layer is cloned to the folder `<prj_path>/components/ext_sources`
 
    ```sh
    mkdir components/ext_sources
    git clone -b thud git://git.linaro.org/openembedded/meta-linaro.git
    ```
    
    As Petalinux 2019.2 (the version we were using) uses the thud branch, the
    thud branch of the layer has to be cloned. 
    
2. After the layer is clone, it has to be added as an external layers to the
   petalinux project. Therefore, type in `petalinux-config` and add the layer in
   `Yocto settings---> User Layers--->
   ()user layer 0`. Add the layer by typing to its location,
   `${PROOT}/components/ext_sources/meta-linaro/meta-optee`. The `meta-layer` is
   a subfolder to a collection of layers. In our case the `meta-optee` layer. It
   is important for petalinux, that the layer has a conf folder, with a
   `layer.conf` file.
 
3. Next, add the recipes to the project dependencies to automatically build the
   recipes with `petalinux-build`. Add optee-os and optee-client to the
   `EXTRA_IMAGEDEPENDS_append` variable in the `petalinuxbsp.conf` file in the
   `<prj_path>/project-spec/meta-user/conf` directory. To test if OPTEE is
   working on the system add the application `CONFIG_optee-test` to the
   `<prj_path>/project-spec/meta-user/conf/user-rootfsconfig` file, to
   automatically build and include it into the rootfs. If you also want to add
   the example applications provided by OPT-TEE, add `CONFIG_optee-examples`.
   After they are added, the recipes can be activated in the `petalinux-config -c rootfs` 
   menuconfig under `user packages-->`. 

### Configuring OP-TEE and the Arm Trusted Firmware

OP-TEE needs further configuration to compile with petalinux and to run on the
Mercury XU5 board. Additionally, the Arm Trusted Firmware (ATF) has to be
configured, such that it start OP-TEE OS.
 
1. OP-TEE requires a machine name to configure OP-TEE OS accordingly.
   Additionally, petalinux requires the built binaries to be in certain directories
   with certain names, to copy them to the known output folder of petalinux
   `<prj_path>/images/linux`. Add a recipe configuration file to the project-specification in
   `<prj_path>/project-spec/meta-user/recipes-security/optee-os/optee-os_%.bbappend`.

    ```
    OPTEEMACHINE = "zynqmp"
    OPTEEOUTPUTMACHINE = "zynqmp"

    do_deploy() {
	    install -d ${DEPLOYDIR}
	    install -d ${TMPDIR}/../../images/linux/
	    install -m 0644 ${B}/out/arm-plat-${OPTEEOUTPUTMACHINE}/core/tee.elf ${DEPLOYDIR}/optee
	    install -m 0644 ${B}/out/arm-plat-${OPTEEOUTPUTMACHINE}/core/tee.elf ${TMPDIR}/../../images/linux/bl32.elf
	    install -m 0644 ${B}/out/arm-plat-${OPTEEOUTPUTMACHINE}/core/tee.bin ${DEPLOYDIR}/optee
	    install -m 0644 ${B}/out/arm-plat-${OPTEEOUTPUTMACHINE}/core/tee.bin ${TMPDIR}/../../images/linux/bl32.bin
    }
    addtask deploy before do_build after do_install
    ```

2. Expand the device tree with the OP-TEE firmware device. This device serves as
   an interface to the TEE. Add the following lines to the file in the directory
   `<prj_path>/project_spec/meta-user/recipes-bsp/devices-tree/files/system-user.dtsi`.

    ``` 
	firmware {
		optee {
			compatible = "linaro,optee-tz";
			method = "smc";
		};
	};
    ```
    
3. Additionally we need to tell the ATF to initialize
   OP-TEE OS. Thus, type `petalinux-config` to the commandline and go to 
   `ARM Trusted Firmware Compilation Configuration --->`. Check 
   `ATF memory settings` and change `ATF MEM SIZE` to `0x16001`.
   Accordingly, the base address has also to be adjusted, else the memory will
   overflow. Therfore, change the `ZYNQ_ATF_MEM_BASE` to `0xFFFE9000`.
   Additionally, add 
   `SPD=opteed ZYNQMP_BL32_MEM_BASE=0x60000000 ZYNQMP_BL32_MEM_SIZE=0x80000 RESET_TO_BL31=1` 
   to the `extra ATF compilation settings`.
   
### Configuring OP-TEE examples

The examples do not compile out of the box. Petalinux will through an error. To
prevent the error add a `optee-examples.bbappend` file to
`<prj_path>/project_spec/meta-user/recipes-security/optee_examples` and add the
follwing content to it.

```
TARGET_CC_ARCH += "${LDFLAGS}"
```

### Enable the Linux device drivers for OP-TEE

The devices have been added to the device tree in the previous step. Now the
drivers in Linux have to be enabled. The driver settings can be simply activated
using the menuconfig, using the `petalinux-config -c kernel` command. The
driver is located under `Device Drivers ---> Trusted Execution Environment support`. 
Enable the driver as built in (`*`) and also enable the additional OP-TEE driver under `TEE drivers --->
OP-TEE`. Although this would suffice, petalinux complains about the
configuration. Also if the project would be cleaned, the changes would not
persist. Thus, to finalize the call `petalinux-build -c kernel -x finish`. This
command generates a config patch file located in the 
`<prj_path>/project-spec/meta-user/recipes-kernel/linux` directory. If the
kernel is rebuilt, the patch is reapplied to the kernel configurations and
enables the drivers again.

### Add OP-TEE to the .bif file

Because OP-TEE is loaded and initialized during the start procedure of the ATF
it also has to be included in the boot image. To do so add the following lines
to your `.bif` file.

```
/* optee-os */
[
    trustzone               = secure,
    exception_level         = el-1,
    destination_cpu         = a53-0,

    authentication          = rsa,
    spk_select              = spk-efuse,
    spk_id                  = 0x46857465,
    sskfile                 = keys/secondary3.pem,

    encryption              = aes,
    aeskeyfile              = keys/aes_p4.nky

] images/linux/bl32.elf
```

OP-TEE OS as secure OS is running in the secure world. Thus, it is defined as
trustzone secure. Also consider to create additional keys. In this case only a new RSA key has to
be generated in (`keys/secondary3.pem`). The AES key will be automatically
generated when the image is built.

### Test OP-TEE

To test OP-TEE start the tee-supplicant with the command `tee-supplicant -d`.
After the initialization run `xtest` run through all test. At the end, the
programm prints aut a summary. For more information on the test programm
consider the wiki page
https://optee.readthedocs.io/en/latest/building/gits/optee_test.html?highlight=test.

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
