################################################################################
#
# wayland-protocols
#
################################################################################

WAYLAND_PROTOCOLS_VERSION = rel_imx_5.4.70_2.3.0
WAYLAND_PROTOCOLS_VERSION = 9cacf108d0ee5863c7a656da5d2271bc2396e43d
WAYLAND_PROTOCOLS_SITE = https://source.codeaurora.org/external/imx/wayland-protocols-imx
WAYLAND_PROTOCOLS_SITE_METHOD = git
WAYLAND_PROTOCOLS_LICENSE = MIT
WAYLAND_PROTOCOLS_LICENSE_FILES = COPYING
WAYLAND_PROTOCOLS_INSTALL_STAGING = YES
WAYLAND_PROTOCOLS_INSTALL_TARGET = NO


define WAYLAND_PROTOCOLS_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef
WAYLAND_PROTOCOLS_PRE_CONFIGURE_HOOKS += WAYLAND_PROTOCOLS_RUN_AUTOGEN

$(eval $(autotools-package))
