## Tools and Environment

This section describes the tools and the environment used for the
implementation. All features were tested on a Mercury XU5
(ME-XU5-5EV-2I-D12E) module mounted on a Mercury PE1 bard
(ME-PE1-300-W).
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
generation and hardware configurations were made in
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
2019.2. The official template from Enclustra was used, which can be
downloaded from their website . Further configuration and generating the
boot images were made with
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
2019.2. The applications were manually compiled, although they can be
directly integrated into the
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
process. Nevertheless, because it is often very time consuming, the
gnu-aarch64-none-linux-gcc compiler (9.2-2019.12) was used. The
operating system of the host computer was based on Ubuntu 18.04.

### <span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span> or <span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>?

<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
is the tool chain from Xilinx to create a Linux image for the
<span data-acronym-label="ultrascale+" data-acronym-form="singular+short">ultrascale+</span>.
It is built up on
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>,
which is an open source project, to create Linux distributions for
different platforms. Instead of using
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>,
it is possible to use
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>
to develop for the
<span data-acronym-label="ultrascale+" data-acronym-form="singular+short">ultrascale+</span>.

The advantage of
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>
is, it works for many platforms and therefore the same tools and
workflow can be used for different projects. Because
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>
is wider used, it has a better documentation and more examples and
guides are available than for
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>.
Further,
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
is based on
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>,
but some points are implemented unusuall and are a bit difficult to
understand.

But there is also a disadvantage. Altough Xilinx provides guides to work
with
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>,
it is not official supported by Xilinx. If any problem occours, even if
it is related to
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>,
Xilinx won’t provide any support for it. Xilinx maintains a forum for
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>,
where most questions are answered by Xilinx. Further the guides and
examples for
<span data-acronym-label="yocto" data-acronym-form="singular+short">yocto</span>
are not as good maintained as for
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
and are sometimes out of date.

### Getting started

This section describes how to create the first project for a Mercury
Board from Enclustra. A description of how to install and use Petalinux
can be found in the following user manuals and application notes from
Xilinx. The following Guides focuses on the specialities of the Mercury
Board.

  - PetaLinux Tools Documentation Reference Guide (UG1144) 

  - PetaLinux Tools Documentation Command Line Reference Guide (UG1157) 

##### Create <span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span> project

If further adjustments to the hardware have to be made, it makes sense
to create a
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
project as well. The user guide to the reference design from Enclustra 
describes how to create a
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
project.

  - Mercury XU5 SoC Module Reference Design for Mercury+ PE1 Base Board
    User Manual 

##### Export Hardware

After synthesizing the
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
project, the hardware information and the
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
have to be exported. To export the hardware access

in the menu bar. It is essential to check in the options window, in
order to use the export in
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>.

##### Create <span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span> Project

In this case, none of the template board support packages from Xilinx
will fit the chip, and an exported hardware will be later imported to
the project. Thus, create a project with the template.

##### Import hardware to the <span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span> project

This step is always necessary, after any changes in hardware. The new
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
has to be exported and imported to
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>.
During this work, no issues were detected with the import. Though, since
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
2019.2 Xilinx introduced a new file format for hardware description
files. The new files have the ending , instead of the older . However,
older files can still be imported. If in previous steps no hardware has
been exported, the exported hardware in the reference design from
Enclustra  can be imported. For the board used in this work the file can
be found in this path:

It is essential only to have one exported hardware file in the folder,
else
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
will complain. Import the hardware description file to
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>
with the following command.

##### Configure <span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span> Project

After importing the hardware description, the first configuring window
appears. It can be exited by double-pressing the key. The different
components are configured separately in
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>.
There are two ways to configure components. If available, a can be
invoked, as seen after the hardware import, by typing:

The different components with are:

  - 
  - 
  - (default)

Other components can be configured by creating a configuration file.
These components are:

  - 
  - —
    <span data-acronym-label="fsbl" data-acronym-form="singular+abbrv">fsbl</span>

  - 
##### Build <span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span> project

Likewise, to configuring, build individual components with the flag. To
build the whole project use:

##### Packaging the project

After building the project, all individual executables are generated
individually but are not bundled together in a boot-able image. To
generate a basic boot-able image use:

With this image, the device will not boot securely. To generate an image
for a secure boot, read the following section.

## Secure Boot

The implementation of secure boot focuses mainly on packaging and
settings in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
registers. As explained in
Section [\[sec\_features.boot\_flow\]](#sec_features.boot_flow) two
implementations of secure boot are possible. By setting the bit in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
the “encryption only” secure boot process is activated. This boot mode
is limiting, because this mode only uses
<span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>
for authentication and confidentiality. Additionally, the device can
only use the
<span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>
master or device key stored in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
register. Therefore, the other secure boot mode “root of trust” has been
implemented. To activate the “root of trust” mode one has to write the
bits in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>.
For testing purposes, Xilinx provides a feature to work with this secure
boot mode, before writing the bits. In this mode, everything works as
expected, with authentication, but the authentication of the
<span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>
never happens. Instead, the bootloader skips this step and goes right to
the authentication process. Therefore, neither secure boot nor key
revocation works, because the checks between
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
and the boot header are skipped. Nevertheless, the chip is not limited
to authenticated images. It can run any images, as long as those secure
boot bits are not set.

Too boot securely, the content of the boot-able image and the security
features of each partition have to be defined. Instead of packaging the
image only using the command above, which is very limited, an additional
file is introduced. This file is a description file of the boot image.
It defines:

  - The containing partitions

  - Where the partition is executed

  - If the partitions are encrypted, what key to use

  - If the partitions are authenticated, what key to use

  - Where and when partitions are loaded

A complete list and description of all definitions is available in the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
User Guide .
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
is the tool behind the file. It is automatically installed when
installing
<span data-acronym-label="petalinux" data-acronym-form="singular+short">petalinux</span>.
In case the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
tool has to be used separately, it can be installed using the
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
Design Suite Web installer .

### Authentication

The key and signature paths have to be included in order to generate
images with authentication. Thus, in the file has to be declared, which
key is used for which partition. Because
<span data-acronym-label="rsa" data-acronym-form="singular+short">rsa</span>
is an asymmetric authentication method with public and private keys,
there are multiple possibilities to do so. The more straightforward way,
which we implemented, was by providing the secret keys. The other
possibility is by using only public keys. This possibility can be used
to protect the secret keys.
Listing [\[lst:public\_key\_bif\]](#lst:public_key_bif) shows the
settings when only public keys are used.

``` objectivec
image : {
    /* Key revocation features */
    [auth_params] ppk_select=0; spk_select=spk-efuse; spk_id=0x00000000

    /* Define primary public key file */
    [ppkfile]primarypublickey.pem

    /* Define secondary public key file for the boot header*/
    [spkfile]secondarypublickey1.pem

    /* Define the signature file for the secondary public key */
    [spksignature]spk_signature.sig

    /* Define the signature file for the boot header and fsbl as they are together authenticated */
    [bhsignature]bh_signature.sig

    /* Define the signature file for the header table */
    [headersignature] header_signature.sig

    /* Enabling authentication and define signature file for individual images */
    [
    authentication    = rsa,
    spkfile           = secondarypublickey2.pem,
    presign           = signature2.sig,
    /* (Optional) Select a different spk_id and spk_id source */
    spk_select        = <spk-efuse/user-efuse>,
    spk_id            = 0x00000000
    ] image.bin
  }
```

This way is more complicated than with secret keys because additionally,
a signature has to be provided. However, it is a more secure way because
the secret keys remain unrevealed. To generate the signatures and key
files, use a toolkit like
<span data-acronym-label="openssl" data-acronym-form="singular+short">openssl</span>.
The generated keys have to have a key length of 4096 bits. A good
systematic description is available in the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
documentation  in the description about creating images using a
<span data-acronym-label="hsm" data-acronym-form="singular+abbrv">hsm</span>
module, starting on page 73. The key revocation settings will be
discussed in Section [1.5](#imp.key_revocation).

If the secret keys are available during the image creation, the file
looks a bit simpler. Instead of defining public keys and signatures,
only the secret keys have to be defined.
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
automatically generates signatures and public keys in the process.

``` objectivec
image : {
    /* Key revocation features */
    [auth_params] ppk_select=0; spk_select=spk-efuse; spk_id=0x00000000

    /* Define primary secret key */
    [pskfile] primarysecretkey.pem

    /* Define secondary secret key for the boot header*/
    [sskfile] secondarysecretkey1.pem

    /* Enabling authentication and define a secondary secret key for individual images */
    [
    authentication    = rsa,
    sskfile           = secondarysecretkey2.pem,
    /* (Optional) Select a different spk_id and spk_id source */
    spk_select        = <spk-efuse/user-efuse>,
    spk_id            = 0x00000000
    ] image.bin
  }
```

An example of a full file can be found in
Listing [\[lst:full\_bif\]](#lst:full_bif) in
Appendix [\[appen.bif\_example\]](#appen.bif_example). In this example,
for each partition, a different key has been used. The additional
parameter

has been set, to test authentication and secure boot without blowing the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>.
With this parameter set, authentication will be enabled. However, the
<span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>
will not be checked with the hash stored in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>,
as well as the s. If neither the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
nor this parameter is set, nothing of the boot image will be
authenticated.

##### Important

The boot header and the
<span data-acronym-label="fsbl" data-acronym-form="singular+abbrv">fsbl</span>
need to have the same key defined. They usually are authenticated
separately, but we noticed during testing, that the authentication
throws an error, if two separate keys are defined.

#### Generate Keys for <span data-acronym-label="rsa" data-acronym-form="singular+short">rsa</span>

In both cases, at some point, keys have to be generated. Xilinx provides
a way of using
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>.

With this way,
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
will generate secret
<span data-acronym-label="rsa" data-acronym-form="singular+short">rsa</span>
keys in the defined location. The location must exist, else
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
will throw a segmentation fault. This command is only usable if a
primary and one secondary key is used. In cases, which more secondary
keys are used,
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
will not generate the other keys. Thus, the second approach with
<span data-acronym-label="openssl" data-acronym-form="singular+short">openssl</span>
is easier.

Important is to generate a key with the key length of 4096 bits.

#### Generate <span data-acronym-label="ppk" data-acronym-form="singular+full">ppk</span> Hash

To be able to write the hash from the
<span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>
to the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>,
it first has to be generated. Thus, the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
tool also provides a feature to generate the hash automatically.

<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
stores the hash in the defined file. In this example .

### Encryption

As
<span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>
is a symmetrical method, it is much easier to implement. However, it has
the disadvantage that the key files have to be available, for the
generation of the image. The operational key method reduces the amount,
in which the device key is used. While with the normal method, all the
images would be decrypted using the device key, with the operational key
method, only a small encrypted part of the boot header is decrypted with
the device key. All following partitions are decrypted using their own
key, saved in the previous partition. To further reduce the risk of a
key leakage, the rolling key method can also be implemented.
Listing [\[lst:operational\_rolling\_key\_bif\]](#lst:operational_rolling_key_bif)
shows a basic file, implementing encryption with the operational key
method and the rolling key method for the file. An example of a complete
file can be found in Listing [\[lst:full\_bif\]](#lst:full_bif) in
Appendix [\[appen.bif\_example\]](#appen.bif_example). More information
about file attributes can be found in the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
User Guide .

``` objectivec
image : {
    /* Define source of device key */
    [keysrc_encryption] bbram_red_key

    /* Further options */
    [fsbl_config] opt_key

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
```

#### Generate Keys for <span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>

To generate keys for
<span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>
nothing has to be especially done. When the
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
command runs, it automatically generates new keys, if it does not find
the defined ones. Only the part name has to be specified with the
option, in addition to the usual options. The part name can be anything.
Listing [\[lst:aes\_key\_file\]](#lst:aes_key_file) shows that
<span data-acronym-label="bootgen" data-acronym-form="singular+short">bootgen</span>
writes the part name into the key file. The key files may vary. This key
file has been generated with the file from the
appendix [\[appen.bif\_example\]](#appen.bif_example).

    Device       <partname>;
    
      Key 0        50C0F949817F0A00DD3A66117599936D7A14BAB349BB546E1CBBAD32F69278D3;
      IV 0         A2E411F57D6045FE506B994F;
    
      Key 1        83283F2FCA0F1A2643A8D62811CBB2DB6184C330CF9E926818FA098556ECBBD5;
      IV 1         C28E78DAE84F9BF415FD142D;
    
      Key Opt      96EEBD73F10EB683E8F4028D609AB86AA90E8D19920AB55BF507878C6DEE3E86;

The first key is the device key, which has to be loaded, to the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
or
<span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>.
This key should be the same across all generated key files. Also, the
according
<span data-acronym-label="iv" data-acronym-form="singular+abbrv">iv</span>
should be the same. The second key is the individual key for each
partition. This key and the
<span data-acronym-label="iv" data-acronym-form="singular+abbrv">iv</span>
are different from partition to partition. The last key is the
operational key. This key and
<span data-acronym-label="iv" data-acronym-form="singular+abbrv">iv</span>
are also identical on all key files.

### Write <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span> or <span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>

The main possibility to write the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
and the
<span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>
is through the xilskey library provided by Xilinx . Xilinx already
provides an example which only has to be complemented with the
user-specific data. To use the example the description in the
application note Programming
<span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>
and
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>s 
can be followed. While the
<span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>
can be written a limitless amount, the bits in the
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
can only be set once from 0 to 1.

#### <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span> Registers

Table [\[tab:efuse\_reg\]](#tab:efuse_reg) provides an overview about
the most important
<span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>
registers. A complete list is available in the
<span data-acronym-label="ultrascale+" data-acronym-form="singular+short">ultrascale+</span>
Devices Register Reference .

|  |
| :---------- | :------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|             |                |                                                                                                                                                                                                                                                      |
| Name        |  Size (bits)   | Description                                                                                                                                                                                                                                          |
| RSA\_EN     |       15       | Enforces to boot with authentication. Enables secure boot.                                                                                                                                                                                           |
| PPK0\_WRLK  |       1        | Locks writing to the <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>0 <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>.                                                                 |
| PPK0\_INVLD |       2        | Revokes the <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>0.                                                                                                                                                           |
| PPK0\_0…11  | 12\(\times\)32 | Hash of <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>0                                                                                                                                                                |
| PPK1\_WRLK  |       1        | Locks writing to the <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>1 <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>.                                                                 |
| PPK1\_INVLD |       2        | Revokes the <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>1.                                                                                                                                                           |
| PPK1\_0…11  | 12\(\times\)32 | Hash of <span data-acronym-label="ppk" data-acronym-form="singular+abbrv">ppk</span>1, in case of key revocation                                                                                                                                     |
| SPK\_ID     |       32       | <span data-acronym-label="spk" data-acronym-form="singular+abbrv">spk</span> ID to disable images with old secondary keys                                                                                                                            |
| USER\_0…7   | 8\(\times\)32  | Eight user fuses. Can be used for enhanced key revocation or user specific features.                                                                                                                                                                 |
| ENC\_ONLY   |       1        | Enforces encrypted boot partitions. Only the <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span> key can be used for <span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span>. Enables secure boot. |
| AES\_RDLK   |       1        | Disables the <span data-acronym-label="crc" data-acronym-form="singular+abbrv">crc</span> check for the <span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span> key.                                                            |
| AES\_WRLK   |       1        | Locks writing the <span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span> <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span>.                                                                     |
| AES\_KEY    |      256       | <span data-acronym-label="efuse" data-acronym-form="singular+abbrv">efuse</span> register for the <span data-acronym-label="aes" data-acronym-form="singular+abbrv">aes</span> key.                                                                  |
| BBRAM\_DIS  |       1        | Disables the <span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span> key.                                                                                                                                                   |
| JTAG\_DIS   |       1        | Disables the <span data-acronym-label="jtag" data-acronym-form="singular+abbrv">jtag</span> controller.                                                                                                                                              |
| DFT\_DIS    |       1        | Disable <span data-acronym-label="dft" data-acronym-form="singular+full">dft</span>.                                                                                                                                                                 |
| SEC\_LOCK   |       1        | Disable reboot into <span data-acronym-label="jtag" data-acronym-form="singular+abbrv">jtag</span> mode after secure lockdown.                                                                                                                       |
|             |                |                                                                                                                                                                                                                                                      |
