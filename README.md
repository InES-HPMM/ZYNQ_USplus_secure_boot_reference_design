# Secure Boot reference design for Xilinx Zynq Ultrascale+

This reference design for the Mercury+ PE1 board with the SoC Module Mercury Xu5
was created during a bachelor thesis at the ZHAW School of Engineering. It
should give a head start designing a secure and bootable image for your device.
This reference design only contains descriptions to the implementation of the
various features. For information on how to develop a secure product, and a more
indept description on the separate features consider reading the bachelor thesis
documentation.

The complete documentation of the bachelor thesis can be found in the
[Documentation](BA20_rosn_02_Secure_Boot_final.pdf).

## Repository structure

| Folder/File name | Description |
|------------------|-------------|
| crypto-api | Contains the description and according examples to work with the Linux Crypto API. |
| petalinux-prj | Contains a PetaLinux project and the description to generate an encrypted and authenticated boot image. |
| program-efuse-bbram | Contains the description how to program the eFuse and BBRAM. |
| tamper-monitoring | Contains the description how to implement the tamper monitoring unit. Additionally, it contains also files used for testing. |
| trustzone | Contains the description for TrustZone implementation. How to implement OP-TEE on the zcu102 Evaluation Kit from Xilinx is also described. |
| update-process | Contains a description how an update procedure may look like. |
| BA20_rosn_02_Secure_Boot_final.pdf | Final bachelor thesis documentation. |
| README.md | This README file. |

## Secure Boot

The basis for secure code execution is the root of trust. Therefore, each
executed software, up to the operating system has to be trustworthy. The
`petalinux-prj` directory contains a PetaLinux project, which can be built with
just a few steps. Start here if you want to build your own secure image. Later
you can expand this image with various other features.

## Tamper Monitoring

The tamper monitoring can detect various tamper event sources. Besides, it works
also with internal and external sensors, to react on suspicious events. This
section describes how to implement the tamper monitoring unit on the Zynq
Ultrascale+. It describes three different ways to configure the tamper and
system monitoring unit. Additionally, some files are provided to test the
implementation.

## Programming the eFuse and BBRAM

The eFuse and BBRAM are the secure storage in the Zynq Ultrascale+. The BBRAM is
a battery backed storage, which is non-permanent. It can contain the AES device
key. The eFuse is a permanent register, where important security settings are
made. Besides, it can also contain the AES device key as well as the RSA key
hash for verification.

## Update Process

Updating firmware and software is crucial to maintain security during the
life-cycle of a device. In this directory, some advises are given on how to
implement an update procedure.

## Crypto API

The Zynq Ultrascale+ has cryptography hardware for different algorithms with key
handling integrated in the hardware. The chapter [Crypto API](crypto-api)
describes how the hardware of the Zynq Ultrascale+ can be used in Linux with the
Crypto API.

## TrustZone

The Zynq Ultrascale+ supports the ARM TrustZone. The chapter
[TrustZone](trustzone) describes the concept and shows the implementation of
OP-TEE on the Xilinx zcu102 development board.

## Tools and Environment

This section describes the tools and the environment used for the
implementation. All features were tested on a Mercury XU5 (ME-XU5-5EV-2I-D12E)
module mounted on a Mercury PE1 board (ME-PE1-300-W). To generate the bitstream
and configure the hardware Vivado 2019.2 was used. The official template from
Enclustra was used, which can be downloaded from their website . Further
configuration and generating the boot images were made with PetaLinux 2019.2.
The applications were manually compiled, although they can be directly
integrated into the PetaLinux process. Nevertheless, because it is often very
time consuming, the gnu-aarch64-none-linux-gcc compiler (9.2-2019.12) was used.
The operating system of the host computer was based on Ubuntu 18.04.
