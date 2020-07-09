
do_configure[depends] += "kern-tools-native:do_populate_sysroot"
SRC_URI += "file://plnx_kernel.cfg"
FILESEXTRAPATHS_prepend := "${THISDIR}/configs:"
RDEPENDS_${KERNEL_PACKAGE_NAME}-base = ""
do_deploy_append () {
	install -m 0644 ${D}/boot/System.map-${KERNEL_VERSION} ${DEPLOYDIR}/System.map.linux
}
