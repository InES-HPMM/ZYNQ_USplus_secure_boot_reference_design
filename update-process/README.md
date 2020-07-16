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

## Key Revocation

Two major steps are required to revoke keys. First, the file has to be adjusted
to use the new keys, and a new image has to be generated, which is then loaded
to the device. Second, the device eFuse has to be updated. Also, there are some
differences, whether the primary or secondary key is revoked. The primary key is
checked bit by bit with a hash stored inside the eFuse. Thus, in case of
revocation, the hash needs to be “replaced”. (Because the eFuse can only be
written once, the term replace is not 100% correct). The secondary keys can be
changed as many times as needed because there is no direct connection between
eFuse and secondary key authentication. In case of a revocation, the secondary
key ID is marked as invalid, which prevents the use of old images with revoked
IDs. Section [\[sec\_features.key\_revocation\]](#sec_features.key_revocation)
explains this feature in more detail. In every case, new keys should be
generated. For more information about how to generate new rsa keys refer to
Section [1.2.1.1](#imp.genrsa_keys).

### Primary Key Revocation

Primary key revocation was never tested, in this work, because it would require
to blow additional eFuse registers, which would limit the use of the borrowed
board. Nevertheless, to implement revocation is nearly the same procedure, as
for the initial implementation of the primary key.

The file adjustments are simple and consists only of changing the attribute, to
point to the new key. The procedure to write the eFuse registers is explained in
the application note Programming BBRAM and eFuses  from Xilinx.
Table [\[tab:efuse\_reg\]](#tab:efuse_reg) shows, that there are registers for
two PPK hashes (PPK0 and PPK1) in the eFuse registers, as well as a register for
each hash two invalidate or revoke.

### Secondary Key Revocation

Secondary key revocation has only be tested with the dedicated eFuse registers,
but not with the fuses. The steps for SPK identification via the fuses and the
fuses only differentiate in the procedure the ID and the eFuse register are
compared. The fuse is compared bit by bit (\(FUSE_{SPK\_{ID}}=SPK\_{ID}\)) the
ID in the boot header, and on a perfect match, the key is valid. The fuse
represents an array in which the revoked IDs are marked with a one. Thus, valid
IDs are marked with a 0 (\(USER\_{FUSE}[SPK\_{ID}]=0\)).
Section [\[sec\_features.key\_revocation\]](#sec_features.key_revocation) gives
a more detailed explanation.

When revoking a secondary key the has to be changed a little further. Beside
changing the keysource with the attribute , the ID with the attribute , the ID
source with needs also to be changed. In order to maximize the use of available
IDs read
Section [\[sec\_features.key\_revocation\]](#sec_features.key_revocation).
Additionally the eFuse registers or need also to be updated. For information on
how to write the eFuse registers refer to application note Programming BBRAM and
eFuses  from Xilinx.
