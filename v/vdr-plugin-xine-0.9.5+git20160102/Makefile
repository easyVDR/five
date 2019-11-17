#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = xine

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

# where to create fifos (xine expects them at /tmp/vdr-xine)
VDR_XINE_FIFO_DIR ?= /tmp/vdr-xine

# can be used to detect inefficient OSD drawing
# 0 - do not verify whether the dirty area of a bitmap is really dirty
# 1 - verify that bitmap is really dirty and print a message on console when it is not
# 2 - additionally fail with a core dump
VDR_XINE_VERIFY_BITMAP_DIRTY ?= 0

# enable to fully support yaepg plugin
VDR_XINE_SET_VIDEO_WINDOW = 1

# where are these utilities for image grabbing? (default: anywhere on your PATH)
#VDR_XINE_Y4MSCALER = /usr/bin/y4mscaler
#VDR_XINE_Y4MTOPPM  = /usr/bin/y4mtoppm
#VDR_XINE_PNMTOJPEG = /usr/bin/pnmtojpeg

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Includes and Defines (add further entries here):

INCLUDES += $(shell pkg-config --cflags libxine)

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

DEFINES += -DFIFO_DIR=\"$(VDR_XINE_FIFO_DIR)\"

DEFINES += -DVERIFY_BITMAP_DIRTY=$(VDR_XINE_VERIFY_BITMAP_DIRTY)

ifdef VDR_XINE_SET_VIDEO_WINDOW
DEFINES += -DSET_VIDEO_WINDOW
endif

ifdef VDR_XINE_Y4MSCALER
DEFINES += -DY4MSCALER=\"$(VDR_XINE_Y4MSCALER)\"
endif

ifdef VDR_XINE_Y4MTOPPM
DEFINES += -DY4MTOPPM=\"$(VDR_XINE_Y4MTOPPM)\"
endif

ifdef VDR_XINE_PNMTOJPEG
DEFINES += -DPNMTOJPEG=\"$(VDR_XINE_PNMTOJPEG)\"
endif

### The object files (add further files here):

OBJS = $(PLUGIN).o xineDevice.o xineLib.o xineOsd.o xineSettings.o xineSetupPage.o xineRemote.o xineExternal.o vdr172remux.o vdr172h264parser.o

### The main target:

all: $(SOFILE) i18n xineplayer

### Implicit rules:

%.o: %.c Makefile
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) xineplayer.c > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='Reinhard Nissl <rnissl@gmx.de>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

$(SOFILE): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@

xineplayer: xineplayer.o Makefile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) xineplayer.o -o $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install: install-lib install-i18n

dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
	@-rm -f xineplayer xineplayer.o
