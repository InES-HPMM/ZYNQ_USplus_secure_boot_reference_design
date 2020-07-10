# BA20_rosn_02_Secure_Boot_reference_design

Introduction...

[Full Documentation](full_doku.pdf)

[Crypto API](crypto-api/readme.md)

...
Implementation {#chap.implementation_guides}
==============

As Chapter \[chap.security\_features\_analysis\] showed, the supports a
range of features to prevent security incidents. This Chapter shows how
this features are implemented. The first use-case demands for a secure
device. The tools used for that are root of trust for authentication and
encryption for confidentiality. Compared to an insecure device, the
secure device implements the root of trust. This concept is built upon
the fact, that in each step, the or boot-loader authenticates the next
software. An interrupt of this root of trust means, the software running
is not trustworthy. But root of trust does not make the device all
secure. If, aside from authentication, also confidentiality is asked
information stored on easy accessible places has to be encrypted. In
this implementation, the boot image also is encrypted. Data handled in
Linux can be encrypted or decrypted using the .

The second use-case covers the life-cycle and update process of a
device. In this work, the update process takes place in Linux. The
multiboot feature provides security against update failures. The device
boots a second image, in case the regular one fails. Additionally, the
procedure of key revocation is explained. The revocation applies only to
.

At last, the implementation of the tamper monitoring unit will be shown.
The tamper monitoring unit works together with the system monitoring
unit. Therefore, this section also discusses parts of the system
monitoring unit.

Tools and Environment
---------------------

This section describes the tools and the environment used for the
implementation. All features were tested on a Mercury XU5
(ME-XU5-5EV-2I-D12E) module mounted on a Mercury PE1 bard
(ME-PE1-300-W). generation and hardware configurations were made in
2019.2. The official template from Enclustra was used, which can be
downloaded from their website [@downloads_enclustra]. Further
configuration and generating the boot images were made with 2019.2. The
applications were manually compiled, although they can be directly
integrated into the process. Nevertheless, because it is often very time
consuming, the gnu-aarch64-none-linux-gcc compiler (9.2-2019.12) was
used. The operating system of the host computer was based on Ubuntu
18.04.

### Why and not ?

–&gt; TODO: Beschreibung was ist petalinux. (Ein Tool das untendran
sowieso yocto benutzt). Wieso haben wir uns für petalinux entschieden.
evtl. was tun wenn man yocto benutzen will.

### Getting started {#imp.getting_started}

This section describes how to create the first project for a Mercury
Board from Enclustra. A description of how to install and use Petalinux
can be found in the following user manuals and application notes from
Xilinx. The following Guides focuses on the specialities of the Mercury
Board.

-   PetaLinux Tools Documentation Reference Guide
    (UG1144) [@refg_petalinux]

-   PetaLinux Tools Documentation Command Line Reference Guide
    (UG1157) [@cli_man_petalinux]

##### Create project

If further adjustments to the hardware have to be made, it makes sense
to create a project as well. The user guide to the reference design from
Enclustra [@downloads_enclustra] describes how to create a project. In
this case:

-   Mercury XU5 SoC Module Reference Design for Mercury+ PE1 Base Board
    User Manual [@ug_mercury_reference_design]

##### Export Hardware

After synthesizing the project, the hardware information and the have to
be exported. This is done by accessing the in the menu bar. It is
essential to check in the options window, in order to use the export in
.

##### Create Project

In this case, none of the template board support packages from Xilinx
will fit the chip, and an exported hardware will be later imported to
the project. Thus, a project with the template can be created.

##### Import hardware to the project

This step is always necessary, after any changes in hardware. Thus, new
s have to be exported and imported to . During this work, no issues were
detected during the import. Though, since 2019.2 introduce a new file
format for hardware description files () the older ones () can still be
imported. If in previous steps no hardware has been exported, the
exported hardware in the reference design from
Enclustra [@downloads_enclustra] can be imported. For the board used in
this work the file can be found in this path:

It is essential only to have one exported hardware file in the folder,
else will complain.

##### Configure Project

After importing the hardware description, the first configuring window
appears. It can be exited by double-pressing the key. The different
components are configured separately in . There are two ways to
configure components. If available, a can be invoked, as seen after the
hardware import, by typing:

The different components with are:

-   -   -   (default)

Other components can be configured by creating a configuring file. These
components are:

-   -   —

-   

##### Build project

Like when configuring, individual components can be built with the flag.
To build the project use:

##### Packaging the project

After building the project, all individual executables are generated
individually but are not bundled together inside a bootable image. To
generate a basic bootable image use:

With this image, the device will not boot securely. To generate an image
for a secure boot, read the following section.

Secure Boot
-----------

The implementation of secure boot focuses mainly on packaging and
settings in the registers. As explained in
Section \[sec\_features.boot\_flow\] two implementation of secure boot
are possible. By setting the bit in the the “encryption only” secure
boot process is activated. This boot mode is limiting, because this mode
only uses for authentication and confidentiality. Additionally, the
device can only use the master or device key stored in the register.
Therefore, the other secure boot mode “root of trust” has been
implemented. To activate the “root of trust” mode one has to write the
bits in the . For testing purposes Xilinx provides a feature to work
with this secure boot mode, before writing the bits. In this mode
everything works as expected, with authentication, but the
authentication of the never happens. Instead of comparing the hash of
the and the hash stored inside the registers, the bootloader skips this
step and goes right to the authentication process. Therefore, neither
secure boot nor key revocation works, because the checks between and the
boot header are skipped. But the chip is not limited to authenticated
images. It can run any images, as long as those secure boot chips are
not set.

The first to boot securely is by defining the content on the bootable
image and the security features of the different partitions. Instead of
packaging the image only using the command above, which is very limited,
an additional file is introduced. This file is a description file of the
boot image. It defines:

-   The containing partitions

-   Where the partition is executed

-   If the partitions are encrypted, what key to use

-   If the partitions are authenticated, what key to use

-   Where partitions are loaded

A complete list and description of all definitions is available in the
User Guide [@ug_bootgen]. is the tool behind the file. It will be
automatically installed when installing . In case the tool has to be
used separately, it can be installed using the Design Suite Web
installer [@web_vivado].

### Authentication

In order to generate images with authentication, the public keys and
signatures have to be included inside the boot image. Thus, in the file
has to be declared, which key is used. Because is an asymmetric
authentication method with public and private keys. There are multiple
possibilities to do so. The easier way, which is showed in this example,
is by providing the and . The other possibility is by using the public
key. It can be used to protect the secret keys.
Listing \[lst:public\_key\_bif\] shows the settings, which have to be
specified.

      image : {
        /* Key revocation features */
        [auth\_params] ppk\_select=0; spk\_select=spk-efuse; spk\_id=0x00000000

        /* Define primary public key file */
        [ppkfile]primarypublickey.pem

        /* Define secondary public key file for the boot header*/
        [spkfile]secondarypublickey1.pem

        /* Define the signature file for the secondary public key */
        [spksignature]spk\_signature.sig

        /* Define the signature file for the boot header and fsbl as they are together authenticated */
        [bhsignature]bh\_signature.sig

        /* Define the signature file for the header table */
        [headersignature] header\_signature.sig

        /* Enabling authentication and define signature file for individual images */
        [
        authentication    = rsa,
        spkfile           = secondarypublickey2.pem,
        presign           = signature2.sig,
        /* (Optional) Select a different spk\_id and spk\_id source */
        spk\_select        = <spk-efuse/user-efuse>,
        spk\_id            = 0x00000000
        ] image.bin
      }

This way is more complicated than with secret keys, but it is the more
secure way because the secret keys remain unrevealed. To generate the
signatures and key files, a toolkit like can be used. The generated keys
have to have a key length of 4096 bits. A good systematic description is
available in the documentation [@ug_bootgen] in the description of
creating images using a module, starting on page 73. The key revocation
settings will be discussed in detail in Section \[imp.key\_revocation\].

If the secret keys are available during the image creation, the file
looks a bit simpler. Instead of defining public keys and signatures,
only the secret keys have to be defined. automatically generates
signatures and public keys in the process.

      image : {
        /* Key revocation features */
        [auth\_params] ppk\_select=0; spk\_select=spk-efuse; spk\_id=0x00000000

        /* Define primary secret key */
        [pskfile] primarysecretkey.pem

        /* Define secondary secret key for the boot header*/
        [sskfile] secondarysecretkey1.pem

        /* Enabling authentication and define secondary secret key for individual images */
        [
        authentication    = rsa,
        sskfile           = secondarysecretkey2.pem,
        /* (Optional) Select a different spk\_id and spk\_id source */
        spk\_select        = <spk-efuse/user-efuse>,
        spk\_id            = 0x00000000
        ] image.bin
      }

An example of a full file can be found in Listing \[lst:full\_bif\] in
Appendix \[appen.bif\_example\]. In this example, for each partition, a
different key has been used. An additional parameter can be set, to test
authentication and secure boot without blowing the :

With this parameter set, authentication will be enabled, but the will
not be checked with the hash stored in the , as well as the s. If either
the nor the parameter are set, nothing of the boot image will be
authenticated.

##### Important

The boot header and the need to have the same key defined. They usually
are authenticated separately, but during testing, it has been noticed,
that the authentication throws an error, if two separate keys are
defined.

#### Generate Keys for  {#imp.genrsa_keys}

In both cases, at some point, keys have to be generated. Xilinx provides
a way of using .

With this way, will generate secret keys in the defined location.
Attention, the location must exist, else will throw a segmentation
fault. This command is only usable if a primary and one secondary key is
used. In cases, which use more secondary keys, will not generate the
other keys. Thus, the second approach with is easier.

Important is to generate a key with the key length of 4096 bits.

#### Generate Hash

To be able to write the hash from the to the it first has to be
generated. Thus, the tool also provides a feature to generate the hash
automatically.

### Encryption

As is a symmetrical method, it is much easier to implement. However, it
has the disadvantage that the key files have to be available, for the
generation of the image. The operational key method reduces the amount,
in which the device key is used. While with the normal method, all the
images would be decrypted using the device key, with the operational key
method, only a small encrypted part of the boot header is decrypted with
the device key. All following partitions are decrypted using their own
key, saved in the previous partition. To further reduce the risk of a
key leakage, the rolling key method can also be implemented.
Listing \[lst:operational\_rolling\_key\_bif\] shows a basic file,
implementing encryption with the operational key method. An example of a
complete file can be found in Listing \[lst:full\_bif\] in
Appendix \[appen.bif\_example\]. More information about file attributes
can be found in the User Guide [@ug_bootgen].

      image : {
        /* Define source of device key */
        [keysrc\_encryption] bbram\_red\_key

        /* Further options */
        [fsbl\_config] opt\_key

        /* Enabling encryption and defining key file for individual images */
        [
        encryption    = aes,
        aeskeyfile    = aeskeyfile1.nky
        ] image.bin

        /* (Optional) Enabling rolling key method for big image partition */
        [
        encryption    = aes,
        /* Attention!: the keyfile has to have the number of keys required for the amount of blocks */
        aeskeyfile    = aeskyfile2.nky,
        /* Define length and amount of blocks */
        blocks        = 2014(2);2048(2);8192(2);4096(*)
        ] longimage.bit
      }

#### Generate Keys for  {#generate-keys-for}

To generate keys for nothing has to be especially done. When the command
runs, it automatically generates new keys, if it does not find the
defined ones. Additionally, only the part name has to be specified with
the option, to the usual options. The part name can be anything.
Listing \[lst:aes\_key\_file\] shows that the part name is written into
the key file. The key files may vary. This key file has been generated
with the file from the appendix \[appen.bif\_example\].

      Device       <partname>;

      Key 0        50C0F949817F0A00DD3A66117599936D7A14BAB349BB546E1CBBAD32F69278D3;
      IV 0         A2E411F57D6045FE506B994F;

      Key 1        83283F2FCA0F1A2643A8D62811CBB2DB6184C330CF9E926818FA098556ECBBD5;
      IV 1         C28E78DAE84F9BF415FD142D;

      Key Opt      96EEBD73F10EB683E8F4028D609AB86AA90E8D19920AB55BF507878C6DEE3E86;

The first key is the device key, which has to be loaded, to the or .
This key should be the same in all generated key files. Also, the
according should be the same. The second key is the individual key for
each partition. This key and the are different from partition to
partition. The last key is the operational key. This key and are also
identical on all key files.

### Write or 

The main possibility to write the and the is through the xilskey library
provided by Xilinx [@ug_standalone_library]. Xilinx already provides an
example which only has to be complemented with the user-specific data.
To use the example the description in the application note Programming
and s [@xapp_programm_bbram_efuse] can be followed. While the can be
written a limitless amount, the bits in the can only be set once from 0
to 1.

####  Registers {#imp.efuse_registers}

Table \[tab:efuse\_reg\] provides an overview about the most important
registers. A complete list is available in the Devices Register
Reference [@zynq_us+_device_register_ref].

[|l|c|&gt;X|]{}

  ----------
  Register
  Name
  ----------

& Size (bits) & Description\
RSA\_EN & 15 & Enforces to boot with authentication. Enables secure
boot.\
PPK0\_WRLK & 1 & Locks writing to the 0 .\
PPK0\_INVLD & 2 & Revokes the 0.\
PPK0\_0…11 & 12$\times$32 & First hash of\
PPK1\_WRLK & 1 & Locks writing to the 1 .\
PPK1\_INVLD & 2 & Revokes the 1.\
PPK1\_0…11 & 12$\times$32 & Second hash of , in case of key revocation\
SPK\_ID & 32 & id to disable images with old secondary keys\
USER\_0…7 & 8$\times$32 & Eight user fuses. Can be used for enhanced key
revocation or user specific features.\
ENC\_ONLY & 1 & Enforces encrypted boot partitions. Only the key can be
used for . Enables secure boot.\
AES\_RDLK & 1 & Disables the check for the key.\
AES\_WRLK & 1 & Locks writing the .\
AES\_KEY & 256 & register for the key.\
BBRAM\_DIS & 1 & Disables the key.\
JTAG\_DIS & 1 & Disables the controller.\
DFT\_DIS & 1 & Disable .\
SEC\_LOCK & 1 & Disable reboot into mode after secure lockdown.\

Multi-boot {#imp.multiboot}
----------

In our implementation, the device boots from . The system divides the
into different sections. The first section is the boot image section.
This section is where the images are stored. The second section is the
boot environment section. There are the environment variables stored.
The last section is the kernel section, where the image is stored if the
image is stored on the . It is important to make sure that each image
has its place in the storage, and is not intersecting another. In our
example, the first boot image is stored in the section — . The fallback
image is stored in the section — . The remaining sections are located
after the address . Now, because the fallback image comes after the
regular one, it has to be ensured that the update process does not
accidentally overwrite the fallback image. For example, in the provided
update program, it is ensured, that the image size is smaller than to
prevent overwriting the fallback image. In addition to that, the image
sections defined in the device tree have to be adjusted. Else, may
override some data when saving its boot environment variables. The
sections can be adjusted during the configuration of the project. They
are accessed with:

In the opened menuconfig the sections can be adjusted under .

CORRECTED WITH GRAMMARLY UNTIL HERE

Now that space is allocated for the second image, it only has to be
stored to the memory section. This can be achieved in multiple ways. One
way is inside the console with the commands showed in
Listing \[lst:copy\_image\_to\_qspi\_uboot\].

      echo Load image from sd card (bootdev)
      load mmc ${sdbootdev} 0x1000000 goldenImage.bin
      echo Check that the image size does not exceed 0x1000000
      if itest ${filesize} > 0x1000000
      then
      echo IMAGE SIZE BIGGER THEN 0x1000000! EXITING...
      else
      echo Image size ok.
      echo Erase QSPI boot section 0x1000000 - 0x2000000
      sf probe
      sf erase 0x1000000 0x2000000
      echo Write image to QSPI
      sf write 0x1000000 0x1000000 ${filesize}
      echo Check back written image
      sf read 0x2000000 0x1000000 ${filesize}
      if cmp.b 0x1000000 0x2000000 ${filesize}
      then
      echo IMAGE SUCCESSFULLY WRITTEN! EXITING...
      else
      echo IMAGE NOT SUCCESSFULLY WRITTEN! EXITING...
      fi
      fi

The other way is by doing these steps in Linux, which is a little
simpler. Listing \[lst:copy\_image\_to\_qspi\_linux\] shows the
different commands used in a Linux shell.

      MAX_FILESIZE=16777216; # Max allowed filesize
      FILENAME="goldenImage.bin";
      FILESIZE=$(stat -c%s "\$FILENAME");

      # Check file size
      if [[ $FILESIZE -gt $MAX_FILESIZE ]]
      then
          echo "Image to big!! Leaving..."
      else
          flashcp -v "$FILENAME" /dev/mtd0;
      fi
      exit 1;

In both ways the image size is checked, before writing to the storage
medium. In cases where the image is bigger than the allocated space,
parts of the fallowing storage sections are getting overwritten. In our
case, the environment variables in u-boot.

Update Process Implementation {#imp.update}
-----------------------------

The update process varies from use case to use case and is dependable on
both the boot and the update source used. Generally, the update
procedure looks as showed in figure \[fig:update\_process\]. The
implemented update process is very rudimentary and serves only as a
proof of concept. As described in the previous section, is used as boot
storage. Thus, to update the image, the allocated section in the boot
storage has to be overwritten with the new image. This is achieved in
the same way described in previous section.

![General update
procedure[]{data-label="fig:update_process"}](update_process){width="250pt"}

In the first step has to be ensured, that only authenticated users can
initiate the update process. Thus, we implemented the update process in
Linux, which provides a user login system. In the next step we only
check the image to be in the expected size, to avoid unintentional
overwrites of the golden image, which in our case is stored directly
after the regular image. Additionally, security checks as for example
authentication checks can be performed in order to provide even more
security. After the new image is written the device has to be restarted,
to use the new image. In our case the restart command is not executed
automatically. Therefore, the user has to restart the device manually,
making sure, that in the following restart the device boots from the
correct boot storage and image. The start address of the image is seen
at the end of Listing \[lst:update\_out\] after *Reading Src*.

      Xilinx Zynq MP First Stage Boot Loader
      Release 2019.2   Apr  2 2020  -  15:48:40
      Reset Mode      :       System Reset
      Platform: Silicon (4.0), Cluster ID 0x80000000
      Running on A53-0 (64-bit) Processor, Device Name: XCZU5EV
      Initializing DDR ECC
      Address 0x0, Length 80000000, ECC initialized
      Processor Initialization Done
      ================= In Stage 2 ============
      QSPI 32 bit Boot Mode
      QSPI is in single flash connection
      QSPI is using 4 bit bus
      FlashID=0x1 0x2 0x20
      SPANSION 512M Bits
      Multiboot Reg : 0x0
      QSPI Reading Src 0x0, Dest FFFF1C40, Length EC0
      ...

Key Revocation {#imp.key_revocation}
--------------

To successfully replace keys and revoke the old ones two major steps are
required. First, the file has to be adjusted to use the new keys and a
new image has to be generated, which is then loaded to the device.
Second, the device has to be updated. Also there are some difference
whether the primary or secondary key is revoked. The primary key is
checked with a hash stored inside the , which in case of a revocation
needs to be “replaced”. (Because the can only be written once, the term
replace is not 100% correct). The secondary keys can be changed as many
times as needed, because they are checked with the primary key. In case
of a revocation, the secondary key ID is marked as invalid, which
prevents the use of old images with revoked IDs.
Section \[sec\_features.key\_revocation\] explains this feature in more
detail. In every case new keys should be generated. For more information
about hwo to generate new keys refer to Section \[imp.genrsa\_keys\].

### Primary Key Revocation

Primary key revocation was never tested, in this work, because it would
require to blow additional registers, which would limit the use of the
borrowed board. But to implement revocation is nearly the same
procedure, as for the initial implementation of the primary key.

The file adjustments are simple and consists only of changing the
attribute, to point to the new key. The procedure to write the registers
is explained in the application note Programming and
s [@xapp_programm_bbram_efuse] from Xilinx. Table \[tab:efuse\_reg\]
shows, that there are registers for two hashes (0 and 1) in the , as
well as a register for each hash two invalidate or revoke.

### Secondary Key Revocation

Secondary key revocation has only be tested with the dedicated
registers, but not with the fuses. The steps for identification via the
fuses and the fuses only differentiate in the procedure the ID and the
register are compared. The fuse is compared bit by bit
($FUSE_{SPK\_{ID}}=SPK\_{ID}$) and on a perfect match the key is valid.
The fuse represents an array in which the revoked IDs are marked with a
one, thus valid IDs are marked with a 0 ($USER\_{FUSE}[SPK\_{ID}]=0$).
Section \[sec\_features.key\_revocation\] gives a more detailed
explanation.

When revoking a secondary key the has to be changed a little further.
Beside changing the keysource with the attribute , the attribute and a
maybe need also to be changed. In order to maximaze the use of available
IDs to revoke read Section \[sec\_features.key\_revocation\].
Additionally the registers or need also to be updated. For information
on how to write the registers refer to application note Programming and
s [@xapp_programm_bbram_efuse] from Xilinx.

Tamper Monitoring {#imp.tamper_monitoring}
-----------------

For testing purposes, tamper reaction for toggling and temperature
change was implemented during the project. Only these two tamper events
could be tested. Reason was the lack of material at home, which was the
working place during the corona crisis. The implementation was entirely
made in , and then imported to the device via the .
Figure \[fig:tamper\_settings\] shows the settings made in tamper
registers. This settings control the reaction to different tamper events
and enable additional security measures, as for example erasing the .
Figure \[fig:sysmon\_settings\] shows the settings of the system
monitor. The system monitor is relevant if threshold values have to be
changed. In this Project the temperature values were adjusted, to
restrict device run-time between 35 and 85°C. In case of the temperature
restriction, some issues were encountered, which will be discussed in
Section \[res.tamper\_monitoring\].

![Tamper Response
settings[]{data-label="fig:tamper_settings"}](tamper_response){width="80.00000%"}

The tamper monitor and the system monitor unit are controlled by
registers. These register can be set in three different way. Because
this registers are security related, some ways may have restrictions or
disadvantages over others.

One way is with a . This is the way, which was tested. This is the
easiest way to enable and control the feature, due to an interface
provided inside . But the drawback is, that the configuration is only
taken over when the is loaded. Meaning, instead of being active after
the is loaded, the tamper monitoring unit with the desired settings is
active after the is loaded. Additionally we had problems with the tamper
events caused by the temperature thresholds. Instead of locking the
device and putting it to secure lockdown, the device waited until it
reached the needed temperature to proceed. More information about this
problem is given in Section \[res.tamper\_monitoring\].

![System Monitor
Settings[]{data-label="fig:sysmon_settings"}](system_management){width="80.00000%"}

An other way to configure the tamper and system monitor registers, are
via the or register drivers in Linux. These drivers provide an interface
to the device registers via the sysfs file structure. A small
description about the drivers are given on the Wiki page of
Xilinx [@xilinx_wiki_ams_register; @xilinx_wiki_csu_register] or in the
Software Developers Guide [@ug_zynq_us+_sdev]. This way of reading and
writing registers was mainly used for testing purposes. But most
relevant register are not accessible, due to security reasons. Because
Linux is running as an normal it has only restricted access to security
related registers, for example threshold values. To enable the access
from Linux to those register, the has to be authorized to handle the
secure world calls made in case of an access. The described solution in
the Software Development Guide [@ug_zynq_us+_sdev], did not work. The
access to the register remained restricted. In any way, doing the main
configuration of the tamper and system monitor is a security risk and
should not be considered.

The last way to set up the tamper and system monitoring unit, is by
writing the registers during start up. Looking to
figure \[fig:boot\_process\_tamper\] the tamper monitor gets active with
the desired state during the execution phase of the . Therefore, a
function needs to be created, which writes the desired values to the
registers. This function then has to be called during the initialization
process. Some registers can be set in the and file. Even though this is
the most time consuming and circumstantial way, it is one of the most
secure ways to do.

Crypto API Implementation {#imp.encryption}
-------------------------

This section describes how to implement an application, which uses the .
A more detailed description on how the works, can be found in
section \[sec.crypto\_api\]. The implementation consists of simple
examples, which show how to use the different parts of the . To compile
the examples we used a separate compiler for the aarch64 architecture.
The examples have been compiled as Linux applications with a makefile,
the full compiler name is . It can be downloaded from
[Arm](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads).
We compiled the applications separately, because it is much faster to
develop and compile single applications, than with petalinux.

### Examples

The is designed in a way, to work platform independent and present the
user a consistent interface. This requires the hardware manufacturer to
implement the drivers for the crypto hardware in a certain way. Thus, it
should not matter, on which platform the software runs.

As described in section \[sec.crypto\_api\_hw\], the has crypto hardware
from Arm and Xilinx implemented. Arm has implemented their hardware
drivers like other manufacturers. Therefore, the same code that uses the
on the Cortex A architecture, can be compiled for the x86 architecture.
The drivers from Xilinx differ from the usual implementation and are
therefore not platform independent.

#### SHA Examples

For are two examples available. One is for and can be found in the
appendix \[crypto-api-example-sha256\], the other for and can be found
in \[crypto-api-example-sha384\]. Table \[tab:overview\_sha\_examples\]
gives an overview of the examples.

[1]{}[|l|l|l|&gt;X|]{} Example & Hardware & Compatibility &
Verification\
& & compatible with other architectures &\
& Xilinx crypto hardware & only on &\

The example for uses the implemented by Arm and is compatible with other
architectures. The example for is for the Xilinx crypto hardware, and
thus not compatible to other architectures. In both exampled the data
which is hashed, is stored in the variable at the beginning of the code.
The resulting hash is printed out to the terminal.

To verify the output of the implementation, the command line tool can be
used to calculate hashes for . To verify the implementation, the tool
can be used. Examples to use these tools can be found in the appendix,
for \[crypto-api-example-sha256-test\] and for
\[crypto-api-example-sha-384-test\]. -&gt; Erst in results

#### AES Examples

As descibed in section \[sec\_features.aes\], has different modes.
Examples are available for the modes and .

### AES CBC Example

The example for can be found in the appendix
\[crypto-api-example-aes-cbc-256\]. It uses the hardware from the ans is
compatible to other architectures like x86.

##### Key and key Size

works with key sizes 128, 192 and 256 bit. The key size can be modified
with the define and the key can be set with the variable.

##### Data

The data to encrypt or decrypt can be set with the variable . The data
length can be set with the define and must be a multiple of 128 bit (16
byte).

##### Encryption/Decryption

To select if the data should encrypted or decrypted, the variable can be
set with or .

##### 

The can be set with the variable and must be 128 bit (16 byte) long.

### AES GCM Example

The first example is for the Xilinx crypto hardware implementation and
can be found here \[crypto-api-example-aes-gcm-256-xilinx\]. The other
example is for the usual implementation on other architectures and can
be found here \[crypto-api-example-aes-gcm-256\].

#### Example config

##### Key and key Size

works with key sizes 128, 192 and 256 bit. The key size can be modified
with the define and the key can be set with the variable.

##### Data

The data to encrypt or decrypt can be set with the variable . The data
length can be set with the define and must be a multiple of 128 bit (16
byte).

##### Encryption/Decryption

To select if the data should encrypted or decrypted, the variable can be
set with or .

##### 

The can be set with the variable and must be 128 bit (16 byte) long.

TrustZone Implementation {#imp.trustzone}
------------------------

- ATF - ist immer dabei - selber compilieren - packaging

- OP-TEE - create project - op-tee compilieren - packaging - ta
compilieren - signing - ta ausführen
