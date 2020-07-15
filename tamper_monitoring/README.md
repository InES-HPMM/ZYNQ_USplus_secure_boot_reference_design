## Tamper Monitoring

Only three tamper events could have been implemented for testing. Those
were the tamper events for
<span data-acronym-label="jtag" data-acronym-form="singular+abbrv">jtag</span>
toggling, for temperature change and the manual tamper event trigger.
Only these three tamper events could be tested due to the lack of
material at home, which was the working place during the corona crisis.
The implementation was entirely made in
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>
and then imported to the device via the
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>.
Figure [1.2](#fig:tamper_settings) shows the settings made in the tamper
registers. This settings control the reaction to different tamper events
and enable additional security measures as erasing the
<span data-acronym-label="bbram" data-acronym-form="singular+abbrv">bbram</span>.
Figure [1.3](#fig:sysmon_settings) shows the settings of the system
monitor. The system monitor is relevant if threshold values have to be
changed. In this project, the temperature values were adjusted to
restrict device run-time between 35 and 85°C. In the case of the
temperature restriction, we encountered some issues, which will be
discussed in
Section [\[res.tamper\_monitoring\]](#res.tamper_monitoring).

![Tamper Response settings](tamper_response)

Registers control the tamper monitor and the system monitor unit. These
register can be set in three different way. Because these registers are
security-related, some ways may have restrictions or disadvantages over
others.

One way is by loading the settings with a
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
to the device. This is the easiest way to enable and control the
feature, due to an interface provided inside
<span data-acronym-label="vivado" data-acronym-form="singular+short">vivado</span>.
However, the drawback is, that the configuration is only taken over when
the
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
is loaded. Meaning, instead of being active after the
<span data-acronym-label="fsbl" data-acronym-form="singular+abbrv">fsbl</span>
is loaded, the tamper monitoring unit with the desired settings is
active after the
<span data-acronym-label="bitstream" data-acronym-form="singular+short">bitstream</span>
is loaded. Additionally, we had problems with the tamper events caused
by temperature thresholds. Instead of locking the device and putting it
to secure lockdown, the device waited until it reached the needed
temperature to proceed. More information about this problem is given in
Section [\[res.tamper\_monitoring\]](#res.tamper_monitoring).

![System Monitor Settings](system_management)

Another way to configure the tamper and system monitor registers are via
the
<span data-acronym-label="ams" data-acronym-form="singular+abbrv">ams</span>
or
<span data-acronym-label="csu" data-acronym-form="singular+abbrv">csu</span>
register drivers in Linux. These drivers provide an interface to the
device registers via the sysfs file structure. A small description about
the drivers are given on the Wiki page of Xilinx  or in the Software
Developers Guide . This way of reading and writing registers was mainly
used for testing purposes. However, most relevant registers are not
accessible due to security reasons. Because Linux is running as a normal
<span data-acronym-label="os" data-acronym-form="singular+abbrv">os</span>
it has only restricted access to security-related registers, for
example, threshold values. To access these register with Linux, the
<span data-acronym-label="pmufw" data-acronym-form="singular+abbrv">pmufw</span>
has to be authorized. In case of access, Linux makes a
<span data-acronym-label="smc" data-acronym-form="singular+abbrv">smc</span>
which is handled by the
<span data-acronym-label="pmufw" data-acronym-form="singular+abbrv">pmufw</span>.
However, the described solution in the Software Development Guide  did
not work. The access to the register remained restricted. In any way,
doing the main configuration of the tamper and system monitor in Linux
is a security risk and should not be considered.

The last way to set up the tamper and system monitoring unit is by
writing the registers during startup. Looking to
figure [\[fig:boot\_process\_tamper\]](#fig:boot_process_tamper), the
tamper monitor gets active with the desired state during the execution
phase of the
<span data-acronym-label="fsbl" data-acronym-form="singular+abbrv">fsbl</span>.
Therefore, a function needs to be created, which writes the desired
values to the registers. This function then has to be called during the
initialization process. Some registers can be set in the and file. Even
though this is the most time consuming and circumstantial way, it is one
of the most secure ways to do.
