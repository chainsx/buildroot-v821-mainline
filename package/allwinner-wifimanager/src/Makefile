ALL_CPPFLAGS += -D_GNU_SOURCE=1
ALL_CPPFLAGS += -DOS_NET_XRLINK_OS=1
ALL_CPPFLAGS += -DSUPPORT_STA_MODE
ALL_CPPFLAGS += -Iinclude
ALL_CPPFLAGS += -Iinclude/os
ALL_CPPFLAGS += -Iinclude/os/linux
ALL_CPPFLAGS += -Iinclude/os/xrlink
ALL_CPPFLAGS += -Icommon
ALL_CPPFLAGS += -Icommon/os
ALL_CPPFLAGS += -Icommon/os/linux

ALL_CFLAGS += $(CFLAGS) -fPIC -Wall
LDLIBS += -lpthread

LIB_SRCS += src/wifimg.c
LIB_SRCS += src/wmg_common.c
LIB_SRCS += src/wmg_sta.c
LIB_SRCS += src/expand_cmd.c
LIB_SRCS += src/wmg_get_config.c
LIB_SRCS += src/wifi_log.c
LIB_SRCS += src/os/xrlink/msg_netlink.c
LIB_SRCS += src/os/xrlink/xrlink_common.c
LIB_SRCS += src/os/xrlink/xrlink_get_config.c
LIB_SRCS += src/os/xrlink/xrlink_sta.c
LIB_SRCS += common/api_action.c
LIB_SRCS += common/os/linux/mutex.c
LIB_SRCS += common/os/linux/queue.c
LIB_SRCS += common/os/linux/ring_buff.c
LIB_SRCS += common/os/linux/sem.c
LIB_SRCS += common/os/linux/sync_notify.c
LIB_SRCS += common/os/linux/thread.c

TOOLS += wifi_scan
TOOLS += wifi_sta

all: libwifimg-v2.0.so $(TOOLS)

libwifimg-v2.0.so: $(LIB_SRCS)
	$(CC) $(ALL_CPPFLAGS) $(ALL_CFLAGS) -shared -Wl,-soname,libwifimg-v2.0.so \
		$^ -o $@ $(LDLIBS)

wifi_scan: demo/wifi_scan.c libwifimg-v2.0.so
	$(CC) $(ALL_CPPFLAGS) $(ALL_CFLAGS) $< -L. -lwifimg-v2.0 \
		-Wl,-rpath,/usr/lib -o $@ $(LDLIBS)

wifi_sta: demo/wifi_sta.c libwifimg-v2.0.so
	$(CC) $(ALL_CPPFLAGS) $(ALL_CFLAGS) $< -L. -lwifimg-v2.0 \
		-Wl,-rpath,/usr/lib -o $@ $(LDLIBS)

install: all
	install -D -m 0755 libwifimg-v2.0.so \
		$(DESTDIR)/usr/lib/libwifimg-v2.0.so
	install -D -m 0755 wifi_sta $(DESTDIR)/usr/bin/wifi_sta
	install -D -m 0755 wifi_scan $(DESTDIR)/usr/bin/wifi_scan
	install -D -m 0644 files/wifimg.config \
		$(DESTDIR)/etc/wifi/wifimg.config
	mkdir -p $(DESTDIR)/usr/include/allwinner-wifimanager
	mkdir -p $(DESTDIR)/usr/include/allwinner-wifimanager/common
	mkdir -p $(DESTDIR)/usr/include/allwinner-wifimanager/common/os
	cp -a include/. $(DESTDIR)/usr/include/allwinner-wifimanager/
	cp -a common/*.h \
		$(DESTDIR)/usr/include/allwinner-wifimanager/common/
	cp -a common/os/*.h \
		$(DESTDIR)/usr/include/allwinner-wifimanager/common/os/

clean:
	rm -f libwifimg-v2.0.so $(TOOLS)

.PHONY: all install clean
