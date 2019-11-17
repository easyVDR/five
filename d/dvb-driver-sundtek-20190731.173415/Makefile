VERSION ?= 101107
DEB_HOST_ARCH ?= $(shell dpkg-architecture -qDEB_HOST_ARCH)
ifeq (i386,$(DEB_HOST_ARCH))
  ARCH := 32bit
else
  ifeq (amd64,$(DEB_HOST_ARCH))
    ARCH := 64bit
  else
    ifeq (armel,$(DEB_HOST_ARCH))
      ARCH := armsysv
    endif
  endif
endif

all: sundtek_installer_${VERSION}.sh
	@chmod +x sundtek_installer_${VERSION}.sh
	./sundtek_installer_${VERSION}.sh -e

install:
	@mkdir -p $(DESTDIR)
	tar xzmf ${ARCH}/installer.tar.gz -C $(DESTDIR)
	@rm -rf $(DESTDIR)/etc/udev/rules.d/80-mediasrv.rules
	chmod gou=sx $(DESTDIR)/opt/bin/mediasrv
	@mkdir -p $(DESTDIR)/usr/lib/pm-utils/sleep.d
	cp $(DESTDIR)/opt/lib/pm/10mediasrv $(DESTDIR)/usr/lib/pm-utils/sleep.d/
	# remove obsolete files
	@rm -f $(DESTDIR)/opt/bin/lirc.sh
	@mkdir -p $(DESTDIR)/usr/include
	@mv $(DESTDIR)/opt/include $(DESTDIR)/usr/include/sundtek
	@mkdir -p $(DESTDIR)/usr/lib/pkgconfig
	@mv $(DESTDIR)/opt/doc/libmedia.pc $(DESTDIR)/usr/lib/pkgconfig

clean:
	@rm -rf 32bit chkarmsysv installer.tar.gz* 32bit23 chkmips mips \
                64bit chkmipsel mipsel armoabi chkmipsel2 mipsel2 armsysv \
                chkopenwrtmipsr2 openwrtmipsr2 chk32bit chkppc32 ppc32 \
                chk32bit23 chkppc64 ppc64 chk64bit chkarmoabi dreambox \
		mipselbcm chkmipselbcm chkopenwrtarm4 openwrtarm4 chksh4 \
		sh4 c3 armsysvhf chkarmsysvhf openwrtmipsr3
		

sundtek_installer_${VERSION}.sh:
	@rm -f sundtek_installer_*.sh
	wget http://www.sundtek.de/media/sundtek_installer_${VERSION}.sh
	chmod +x sundtek_installer_${VERSION}.sh

