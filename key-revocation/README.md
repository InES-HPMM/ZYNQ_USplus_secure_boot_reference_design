# Key Revocation

## key-revocation folder structure

| Folder/File name | Description |
|------------------|-------------|
| README.md | This file. |

## Implementation

Two major steps are required to revoke keys. First, the `.bif` file has to be adjusted
to use the new keys, and a new image has to be generated, which is then loaded
to the device. Second, the device eFuse has to be updated. Also, there are some
differences, whether the primary or secondary key is revoked. The primary key is
checked bit by bit with a hash stored inside the eFuse. Thus, in case of
revocation, the hash needs to be “replaced”. (Because the eFuse can only be
written once, the term replace is not 100% correct). The secondary keys can be
changed as many times as needed because there is no direct connection between
eFuse and secondary key authentication. In case of a revocation, the secondary
key ID is marked as invalid, which prevents the use of old images with revoked
IDs. For more information about how to generate new rsa keys refer to
to the README in the petalinux-prj folder.

### Primary Key Revocation

Primary key revocation was never tested, in this work, because it would require
to blow additional eFuse registers, which would limit the use of the borrowed
board. Nevertheless, to implement revocation is nearly the same procedure, as
for the initial implementation of the primary key.

The `.bif` file adjustments are simple and consists only of changing the
`pskfile` attribute, to
point to the new key. The procedure to write the eFuse registers is explained in
the [application note Programming BBRAM and eFuses
(XAPP1283)](https://www.xilinx.com/support/documentation/application_notes/xapp1283-internalprogramming-bbram-efuses.pdf)
from Xilinx. In the eFuse are registers for two PPK hashes (PPK0 and PPK1) in
the eFuse registers, as well as a register for each hash two invalidate or
revoke.

### Secondary Key Revocation

Secondary key revocation has only been tested with the dedicated eFuse
registers, but not with the `USER` fuses. The steps for SPK identification via
the `USER` fuses and the `SPK_ID` fuses only differentiate in the procedure the
ID and the eFuse register are compared. The `SPK_ID` fuse is compared bit by bit
(\(FUSE_{SPK\_{ID}}=SPK\_{ID}\)) the ID in the boot header, and on a perfect
match, the key is valid. The `USER` fuse represents an array in which the
revoked IDs are marked with a one. Thus, valid IDs are marked with a 0
(\(USER\_{FUSE}[SPK\_{ID}]=0\)).

When revoking a secondary key the `.bif` file has to be changed a little
further. Beside changing the keysource with the attribute `sskfile` , the ID
with the attribute `spk_id` and the ID source with `spk_select` needs also to be
changed. To get the maximum amount of revocations, the bits have to be flipped
to ones separately (eFuse registers can only be burnt from a zero to a one).
This leads to the following `SPK_IDs`: `0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f,
0x7f, 0xff, 0x1f, ...`. Additionally the eFuse registers `SPK_ID` or `USER_0...7` need also to
be updated.
