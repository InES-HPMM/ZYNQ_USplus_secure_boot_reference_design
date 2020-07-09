
SRC_URI_append ="\
    file://config\
"
sysconf = "${TOPDIR}/../project-spec/configs"
FILESEXTRAPATHS_append := ":${sysconf}"
ATF_CONSOLE_zynqmp = "cadence"
EXTRA_OEMAKE_append = " ${extra_settings} PRELOADED_BL33_BASE=${atf_bl33_load}"
extra_settings = ""
atf_bl33_load = "0x8000000"
