INST_LIB_PATH:=usr/lib
INST_BIN_PATH:=bin
PKG_NAME:=rsserial
PKG_RELEASE:=1.01
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)/src
PKG_BUILD_ROOT_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
define Package/$(PKG_NAME)/description
	core
endef
define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=$(PKG_NAME) -- 802.15.4 control interface
	DEPENDS:=+jansson +libpthread +librt +libstdcpp +libopenssl +kapparock-init-files
	Maintainer:=Yuming Liang
endef

define Build/Prepare
	#mkdir -p $(PKG_BUILD_DIR)
	#$(CP) -r ./src/* $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_ROOT_DIR)
	$(CP) -r * $(PKG_BUILD_ROOT_DIR)/   
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_BIN) ./files/rsserial-watch.init $(1)/etc/init.d/rsserial-watch
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) ./files/flashTI.sh $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rsserial-watch $(1)/bin/
	$(INSTALL_DIR) $(1)/$(INST_LIB_PATH)/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/tmp
	$(CP) ./files/*.json $(1)/usr/lib/$(PKG_NAME)
	$(CP) ./files/*.pem $(1)/usr/lib/$(PKG_NAME)
	$(CP) ./files/widget_registry_mgr $(1)/usr/lib/$(PKG_NAME)
	$(CP) ./files/temp_files/widget_registry.json $(1)/tmp/
	$(INSTALL_DIR) $(1)/usr/lib/kappaio
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/usr/lib/kappaio
	$(INSTALL_DIR) $(1)/etc/udev/rules.d
	$(CP) ./files/10-kappaio.rules $(1)/etc/udev/rules.d
	#ln -s /tmp $(1)/rstmp
endef

define Build/Compile
  $(call Build/Compile/Default,processor_family=$(_processor_family_) BOARD=$(BOARD))
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/kappaio
	$(INSTALL_DIR) $(1)/usr/include/kappaio
	$(CP) $(PKG_BUILD_DIR)/lib/* $(1)/usr/lib/kappaio/
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/kappaio/
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/kappaio/
endef
define Package/$(PKG_NAME)/postinst
#!/bin/sh
# check if we are on real system
#if [ -z "$${IPKG_INSTROOT}" ]; then
if [ ! -h /rstmp ]; then 
	ln -s /tmp /rstmp
fi
if [ ! -f /usr/lib/rsserial/widget_registry.json ]; then
	cp /tmp/widget_registry.json /usr/lib/rsserial/
	echo "widget_registry.json does not exist, use default"
else
	echo "widget_registry.json exist, do nothing"
fi
update-rc.d rsserial defaults 99 91
update-rc.d rsserial-watch defaults 99 91
echo /usr/lib/kappaio > /etc/ld.so.conf.d/kappaio.conf
ldconfig
echo "Enabling rc.d symlink..."
service rsserial-watch restart
#fi
exit 0
endef

define Package/$(PKG_NAME)/UploadAndInstall
ifeq ($(OPENWRT_BUILD),1)
compile: $(STAGING_DIR_ROOT)/stamp/.$(PKG_NAME)_installed
	$(SCP) $$(PACKAGE_DIR)/$$(PKG_NAME)_$$(VERSION)_$$(ARCH_PACKAGES).ipk $(1):/tmp
	$(SSH) $(1) opkg install --force-overwrite /tmp/$(PKG_NAME)_$$(VERSION)_$$(ARCH_PACKAGES).ipk
	$(SSH) $(1) rm /tmp/$$(PKG_NAME)_$$(VERSION)_$$(ARCH_PACKAGES).ipk
	$(SSH) $(1) rm /tmp/widget_registry.json
endif
ifeq ($(RASPBERRYPI_BUILD),1)
compile:$(STAMP_INSTALLED)
	@echo "---------------------------------------------------"
	@echo "**************** RASPBERRYPI_BUILD ****************"
	@echo "---------------------------------------------------"
	$(SCP) $$(PACKAGE_DIR)/$$(PACKAGE_BIN_DPKG) $(1):/tmp
	$(SSH) $(1) dpkg -i /tmp/$$(PACKAGE_BIN_DPKG)
endif
endef
UPLOAD_PATH:=$(or $(PKG_DST), $($(PKG_NAME)_DST))
$(if $(UPLOAD_PATH), $(eval $(call Package/$(PKG_NAME)/UploadAndInstall, $(UPLOAD_PATH))))

$(eval $(call BuildPackage,$(PKG_NAME)))

