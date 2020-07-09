DESCRIPTION = "PETALINUX image definition for Xilinx boards"
LICENSE = "MIT"

require recipes-core/images/petalinux-image-common.inc 

inherit extrausers 
COMMON_FEATURES = "\
		ssh-server-dropbear \
		hwcodecs \
		"
IMAGE_LINGUAS = " "

IMAGE_INSTALL = "\
		kernel-modules \
		haveged \
		mtd-utils \
		canutils \
		openssh-sftp-server \
		pciutils \
		run-postinsts \
		udev-extraconf \
		packagegroup-core-boot \
		packagegroup-core-ssh-dropbear \
		tcf-agent \
		watchdog-init \
		bridge-utils \
		hellopm \
		"
EXTRA_USERS_PARAMS ?= "usermod -P root root;"
