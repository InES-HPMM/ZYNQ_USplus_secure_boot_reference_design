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

