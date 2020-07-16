# Update Process

## update_process folder structure

| Folder/File name | Description |
|------------------|-------------|
| README.md | This file |

## Implementation

The update process varies from use case to use case and is dependable on both
the boot and the update source used. The update process has to be
initiated by an authorized source. Then the new image is read from the external
source and checked. If the check was successful, the image can be written to the
boot storage and the device can be rebooted. Else, the update process has to be
stopped. The implemented update process is very rudimentary and serves only as a
proof of concept. As described in the previous section, QSPI is used as the
primary boot storage. Thus, to update the image, the allocated section in the
boot storage has to be overwritten with the new image. To overwrite the image,
we used the same approach as described in the previous section.

In the first step has to be ensured that only authenticated users can initiate
the update process. Thus, we implemented the update process in Linux, which
provides a user login system. In the next step, we check the image to be in the
expected size, to avoid unintentional overwrites of the golden image, which in
our case is stored directly after the regular image. Additionally, security
checks, for example, authentication checks can be performed in order to provide
even more security. After the new image is written, the device has to be
restarted, to use the new image. In our case, the restart command is not
automatically executed. Therefore, the user has to restart the device manually,
making sure, that in the following restart the device boots from the correct
boot storage and image. The start address of the image is seen at the end of the
following listing after *QSPI Reading Src* on Line 16.

```
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
```

