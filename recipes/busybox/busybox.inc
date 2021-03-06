# -*- mode:python; -*-
DESCRIPTION = "BusyBox: The Swiss Army Knife of Embedded Linux"
HOMEPAGE = "http://www.busybox.net"
LICENSE = "GPL-2.0+"

COMPATIBLE_HOST_ARCHS = ".*linux"

RDEPENDS_SYSVINIT:USE_busybox_init = ""
RDEPENDS_RUNIT:USE_busybox_runit = ""
inherit c make sysvinit runit crontab

DEFAULT_PREFERENCE = "1"

SRC_URI = "http://www.busybox.net/downloads/busybox-${PV}.tar.bz2"
SRC_URI_FIXES = "http://busybox.net/downloads/fixes-${PV}/"

SRC_URI += "\
	file://defconfig \
	file://groups \
	file://inittab \
	file://rcS \
	file://rc \
	file://rcS-default \
	file://busybox-mdev \
	file://mdev.conf \
	file://busybox-klogd \
	file://busybox-syslogd \
	file://busybox-hwclock \
	file://busybox_hwclock.crontab \
	file://busybox-crond \
	file://busybox-watchdog \
	file://busybox-watchdog.runit \
	file://busybox-udhcpd \
	file://busybox-inetd \
	file://inetd.conf \
	file://busybox-ntpd \
	file://busybox-httpd \
	file://default.script \
	file://busybox-ifplugd \
	file://busybox-ifplugd.action \
	file://busybox-ifplugd.conf \
"

PACKAGES:<USE_busybox_udhcpc = "${PN}-udhcpc-scripts "

RECIPE_FLAGS += "busybox_init_shutdown_umount"
DEFAULT_USE_busybox_init_shutdown_umount = "1"
SRC_URI:>USE_busybox_init_shutdown_umount = " file://init-shutdown-umount.patch"

LIB_DEPENDS = "libc libm libcrypt"
DEPENDS = "${LIB_DEPENDS}"

export EXTRA_CFLAGS = "${CFLAGS}"
export EXTRA_LDFLAGS = "${LDFLAGS}"

require busybox-configure.inc

EXTRA_OEMAKE += "V=1 ARCH=${TARGET_ARCH} CROSS_COMPILE=${TARGET_PREFIX}"
EXTRA_OEMAKE += "CROSS=${HOST_PREFIX}"

do_compile() {
	unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
	do_compile_make
}

EXTRA_OEMAKE_COMPILE:TARGET_BINFMT_flat = "busybox_unstripped"
do_compile[postfuncs] += "${do_compile_binfmt_fixup}"
do_compile_binfmt_fixup:TARGET_BINFMT_flat = "do_compile_binfmt_flat_fixup"
do_compile_binfmt_flat_fixup() {
	mv busybox_unstripped busybox
	mv busybox_unstripped.gdb busybox_unstripped
}

require busybox-install.inc

PACKAGES:<USE_busybox_udhcpd = "${PN}-udhcpd-config "
PROVIDES_${PN}-udhcpd-config = " udhcpd-config"
FILES_${PN}-udhcpd-config = "${sysconfdir}/udhcpd.conf "
RDEPENDS_${PN}:>USE_busybox_udhcpd += " udhcpd-config"

RDEPENDS_${PN}:>USE_busybox_init = " ${PN}-sysinit"
PACKAGES:<USE_busybox_init = "${PN}-sysinit "
FILES_${PN}-sysinit:USE_busybox_init = "${sysconfdir}/init.d/rcS \
	${sysconfdir}/default/rcS"

FILES_${PN}:>USE_busybox_runit = " ${runitservicedir}"
FILES_${PN}:>USE_busybox_syslogd = " ${localstatedir}/log"
FILES_${PN}:>USE_busybox_httpd = " /srv/www"
FILES_${PN}:>USE_busybox_tftpd = " ${USE_busybox_tftpd_dir}"
FILES_${PN}:>USE_busybox_ftpd = " ${USE_busybox_ftpd_dir}"

FILES_${PN}-udhcpc-scripts = "${datadir}/udhcpc ${sysconfdir}/udhcpc.d"

RDEPENDS_${PN} = "netbase base-passwd base-files ${LIB_DEPENDS}"
RDEPENDS_${PN}:>USE_busybox_udhcpc += " ${PN}-udhcpc-scripts"
DEPENDS_${PN} += "libc"

BUSYBOX_PROVIDES += "util/awk"
PROVIDES_${PN} = "${BUSYBOX_PROVIDES}"
