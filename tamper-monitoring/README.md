# Tamper Monitoring

## tamper_monitoring folder structure

| Folder/File name | Description                                               |
|------------------|-----------------------------------------------------------|
| testing          | Contains different programs to read and write registers   |

## Implementation

Only three tamper events have been implemented for testing. Those were the
tamper events for JTAG toggling, for temperature change and the manual tamper
event trigger. Only these three tamper events could be tested due to the lack of
material at home, which was the working place during the corona crisis. The
implementation was entirely made in Vivado and then imported to the device via
the bitstream. We worked with the Vivado reference design from Enclustra. There,
th tamper monitoring settings can be accessed by configuring the main Zynq
Ultrascale+ MPSoC module. In the configurations menu the advanced mode has to be
activated to get to the settings through the advanced configuration page. This
settings control the reaction to different tamper events and enable additional
security measures as erasing the BBRAM.

The system monitor has to be configured through the system management wizard.
The adjustments can be made in the On-Chip Sensor Selection tab. The system
monitor is relevant if threshold values have to be changed. In this project, the
temperature values were adjusted to restrict device run-time between 35 and
85°C. In the case of the temperature restriction, we encountered some issues,
which will are discussed in the result section of the bachelor thesis.

Registers control the tamper monitor and the system monitor unit. These register
can be set in three different way. Because these registers are security-related,
some ways may have restrictions or disadvantages over others.

One way is by loading the settings with a bitstream to the device. This is the
easiest way to enable and control the feature, due to an interface provided
inside Vivado. However, the drawback is, that the configuration is only taken
over when the bitstream is loaded. Meaning, instead of being active after the
FSBL is loaded, the tamper monitoring unit with the desired settings is active
after the bitstream is loaded. Additionally, we had problems with the tamper
events caused by temperature thresholds. Instead of locking the device and
putting it to secure lockdown, the device waited until it reached the needed
temperature to proceed. More information about this problem is given in the result
section of the bachelor thesis.

Another way to configure the tamper and system monitor registers are via the AMS
or CSU register drivers in Linux. These drivers provide an interface to the
device registers via the sysfs file structure. A small description about the
drivers are given on the [Wiki page of
Xilinx](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842163/Zynq+UltraScale+MPSoC+AMS) 
or in the [Software Developers Guide
(UG1137)](https://www.xilinx.com/support/documentation/user_guides/ug1137-zynq-ultrascale-mpsoc-swdev.pdf) on
page 148. This way of reading and writing registers was mainly used for testing
purposes. However, most relevant registers are not accessible due to security
reasons. Because Linux is running as a normal OS it has only restricted access
to security-related registers, for example, threshold values. To access these
register with Linux, the PMUFW has to be authorized. In case of access, Linux
makes a SMC which is handled by the PMUFW. However, the described solution in
the Software Development Guide  did not work. The access to the register
remained restricted. In any way, doing the main configuration of the tamper and
system monitor in Linux is a security risk and should not be considered.

The last way to set up the tamper and system monitoring unit is by writing the
registers during startup. The tamper monitor gets active with the desired state
during the execution phase of the FSBL. Therefore, a function needs to be
created, which writes the desired values to the registers. This function then
has to be called during the initialization process. Some registers can be set in
the `psu_init.c` and `psu_init.h` file. Even though this is the most time
consuming and circumstantial way, it is one of the most secure ways to do.

## Testing

The folder testing contains to files. The file `mec.c` can be used to write or read any
memory section. However, most of them are not accessible this way. The other
file `read_temp.c` is a program to read out and print the chip temperature every
second. Both can be used for testing purposes.
