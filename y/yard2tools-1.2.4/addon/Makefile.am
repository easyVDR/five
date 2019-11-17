
SUBDIRS = .

# install to /lib/udev/rules.d or PREFIX/lib/udev/ruels.d, when distcheck is done
pkgdatadir = $(shell echo '$(prefix)' | grep -q '_inst' && echo '$(prefix)')/lib/udev/rules.d
dist_pkgdata_DATA = \
   60-usb-yard2.rules

# install to /etc/vdr or PREFIX/etc/vdr, when distcheck is done
etcvdrdir = $(shell echo '$(prefix)' | grep -q '_inst' && echo '$(prefix)')/etc/vdr
dist_etcvdr_DATA = \
   vdr-addon-yard2wakeup.conf
   
# install to /etc/yard2 or PREFIX/etc/yard2, when distcheck is done
etcyard2dir = $(shell echo '$(prefix)' | grep -q '_inst' && echo '$(prefix)')/etc/yard2
dist_etcyard2_DATA = \
   yard2tools.cfg

# install to PREFIX/share/vdr/shutdown-hooks
shutdownhooksdir = $(prefix)/share/vdr/shutdown-hooks
dist_shutdownhooks_DATA = \
   S90.yard2-wakeup
