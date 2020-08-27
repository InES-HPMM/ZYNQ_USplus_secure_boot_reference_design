# Write eFuse or BBRAM

The main tool to write the eFuse and the BBRAM is with the xilskey library
provided by Xilinx. Xilinx already provides an example which only has to be
complemented with the user-specific data. To use the example the description in
the [application note Programming BBRAM and eFuses
(XAPP1283)](https://www.xilinx.com/support/documentation/application_notes/xapp1283-internalprogramming-bbram-efuses.pdf)
can be followed. While the BBRAM can be written a limitless amount, the bits in
the eFuse can only be set once from 0 to 1.

## eFuse Registers

The following table provides an overview about the most important eFuse
registers. A complete list is available in the [Zynq Ultrascale+ MPSoC+ Devices
Register Reference
(UG1087)](https://www.xilinx.com/html_docs/registers/ug1087/ug1087-zynq-ultrascale-registers.html).

| Name        | Size (bits)  | Description                                                                                      |
| :---------- | :----------: | :----------------------------------------------------------------------------------------------- |
| RSA\_EN     | 15           | Enforces to boot with authentication. Enables secure boot.                                       |
| PPK0\_WRLK  | 1            | Locks writing to the PPK0 eFuse.                                                                 |
| PPK0\_INVLD | 2            | Revokes the PPK0.                                                                                |
| PPK0\_0…11  | 12x32        | Hash of PPK0                                                                                     |
| PPK1\_WRLK  | 1            | Locks writing to the PPK1 eFuse.                                                                 |
| PPK1\_INVLD | 2            | Revokes the PPK1.                                                                                |
| PPK1\_0…11  | 12x32        | Hash of PPK1, in case of key revocation                                                          |
| SPK\_ID     | 32           | SPK ID to disable images with old secondary keys                                                 |
| USER\_0…7   | 8x32         | Eight user fuses. Can be used for enhanced key revocation or user specific features.             |
| ENC\_ONLY   | 1            | Enforces encrypted boot partitions. Only the eFuse key can be used for AES. Enables secure boot. |
| AES\_RDLK   | 1            | Disables the CRC check for the AES key.                                                          |
| AES\_WRLK   | 1            | Locks writing the AES eFuse.                                                                     |
| AES\_KEY    | 256          | eFuse register for the AES key.                                                                  |
| BBRAM\_DIS  | 1            | Disables the BBRAM key.                                                                          |
| JTAG\_DIS   | 1            | Disables the jtag controller.                                                                    |
| DFT\_DIS    | 1            | Disable dft.                                                                                     |
| SEC\_LOCK   | 1            | Disable reboot into jtag mode after secure lockdown.                                             |
