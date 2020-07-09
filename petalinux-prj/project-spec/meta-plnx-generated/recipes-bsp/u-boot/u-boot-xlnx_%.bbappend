
SRC_URI += "file://config.cfg file://platform-auto.h"
FILESEXTRAPATHS_prepend := "${THISDIR}/configs:"
do_configure_append () {
	install ${WORKDIR}/platform-auto.h ${S}/include/configs/
	install ${WORKDIR}/platform-top.h ${S}/include/configs/
}
