manpages = *.1
inst_man = $(manpages)

.PHONY: all
all: $(manpages)
$(manpages): atsc_epg.1 av7110_loadkeys.1 azap.1 czap.1 dib3000-watch.1 \
dst_test.1 dvbdate.1 dvbnet.1 dvbscan.1 dvbtraffic.1 femon.1 gnutv.1 gotox.1 \
scan.1 szap.1 tzap.1 zap.1
include ../Make.rules

install::
	@echo installing manpages
	@mkdir -p $(DESTDIR)$(mandir)
	@install -m 644 *.1 $(DESTDIR)$(mandir)/
