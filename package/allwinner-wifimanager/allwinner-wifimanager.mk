################################################################################
#
# allwinner-wifimanager
#
################################################################################

ALLWINNER_WIFIMANAGER_VERSION = 1.0
ALLWINNER_WIFIMANAGER_SITE = $(TOPDIR)/package/allwinner-wifimanager/src
ALLWINNER_WIFIMANAGER_SITE_METHOD = local
ALLWINNER_WIFIMANAGER_LICENSE = PROPRIETARY
ALLWINNER_WIFIMANAGER_INSTALL_STAGING = YES

define ALLWINNER_WIFIMANAGER_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define ALLWINNER_WIFIMANAGER_INSTALL_STAGING_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) install
endef

define ALLWINNER_WIFIMANAGER_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install
endef

$(eval $(generic-package))
