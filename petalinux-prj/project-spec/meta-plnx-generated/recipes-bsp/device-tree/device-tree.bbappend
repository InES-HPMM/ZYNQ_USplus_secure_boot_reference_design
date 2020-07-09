
XSCTH_WS = "${TOPDIR}/../components/plnx_workspace/device-tree"
export PETALINUX
YAML_CONSOLE_DEVICE_CONFIG_forcevariable = "psu_uart_0"
DT_PADDING_SIZE = "0x1000"
DEVICETREE_FLAGS += ""
YAML_MAIN_MEMORY_CONFIG_forcevariable = "PSU_DDR_0"
KERNEL_DTS_INCLUDE = "${STAGING_KERNEL_DIR}/include"
FILESEXTRAPATHS_append := ":${sysconf}"
sysconf = "${TOPDIR}/../project-spec/configs"
SRC_URI_append ="\
    file://config\
"
do_configure_append () {
	script="${PETALINUX}/etc/hsm/scripts/petalinux_hsm_bridge.tcl"
	data=${PETALINUX}/etc/hsm/data/
	eval xsct -sdx -nodisp ${script} -c ${WORKDIR}/config \
	-hdf ${DT_FILES_PATH}/hardware_description.${HDF_EXT} -repo ${S}\
	-data ${data} -sw ${DT_FILES_PATH} -o ${DT_FILES_PATH} -a "soc_mapping"
}
