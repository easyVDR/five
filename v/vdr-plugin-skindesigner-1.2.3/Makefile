#
# Makefile for a Video Disk Recorder plugin
#
# $Id$ Makefile 1.0 2014/07/24 louis Exp $

# Config
CONFIG := #-DDOPROFILE			        # enable profiling code
CONFIG += #-DDO_NOT_USE_SKININSTALLER	# disable installer in skindesigner setup

# The official name of this plugin.
PLUGIN = skindesigner

### The version number of this plugin (taken from the main source file):
VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --silence-errors --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --silence-errors --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG  = $(call PKGCFG,plgcfg)
VDRCONFDIR = $(call PKGCFG,configdir)
PLGRESDIR = $(call PKGCFG,resdir)/plugins/$(PLUGIN)
TMPDIR ?= /tmp
SKINDESIGNER_SCRIPTDIR ?= $(LIBDIR)/$(PLUGIN)/scripts

### The compiler options:
export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)
CXXFLAGS += -std=c++11
### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The version number of VDR's plugin API:
APIVERSION = $(call PKGCFG,apiversion)

### The name of the distribution archive:
ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:
SOFILE = libvdr-$(PLUGIN).so

### Includes and Defines and Dependencies:
DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"' $(CONFIG)
DEFINES += $(shell xml2-config --cflags)
DEFINES += -DSCRIPTFOLDER='"$(SKINDESIGNER_SCRIPTDIR)"'
DEFINES += -D__STL_CONFIG_H

INCLUDES += $(shell pkg-config --cflags freetype2 fontconfig)
INCLUDES += $(shell pkg-config --cflags librsvg-2.0 cairo-png) -ljpeg

LIBS += $(shell pkg-config --libs librsvg-2.0 cairo-png) -ljpeg
LIBS += $(shell xml2-config --libs)
LIBS += $(shell pkg-config --libs libcurl)

### The object files:
OBJS = $(PLUGIN).o \
       config.o \
       setup.o \
       designer.o \
       displaychannel.o \
       displaymenu.o \
       displaymessage.o \
       displayreplay.o \
       displaytracks.o \
       displayvolume.o \
       extensions/cairoimage.o \
       extensions/curlfuncs.o \
       extensions/fontmanager.o \
       extensions/globaltimers.o \
       extensions/imagecache.o \
       extensions/helpers.o \
       extensions/imageloader.o \
       extensions/libxmlwrapper.o \
       extensions/pluginmanager.o \
       extensions/recfolderinfo.o \
       extensions/scrapmanager.o \
       extensions/skinsetup.o \
       extensions/skinrepo.o \
       extensions/extrecinfo.o \
       coreengine/animation.o \
       coreengine/attribute.o \
       coreengine/attributes.o \
       coreengine/functions.o \
       coreengine/complextypes.o \
       coreengine/globals.o \
       coreengine/gridelement.o \
       coreengine/osdwrapper.o \
       coreengine/view.o \
       coreengine/viewdisplaychannel.o \
       coreengine/viewdisplaymenu.o \
       coreengine/viewdisplaymessage.o \
       coreengine/viewdisplayreplay.o \
       coreengine/viewdisplaytracks.o \
       coreengine/viewdisplayvolume.o \
       coreengine/viewdisplayplugin.o \
       coreengine/viewelement.o \
       coreengine/viewelementplugin.o \
       coreengine/viewelementscommon.o \
       coreengine/viewelementsdisplaychannel.o \
       coreengine/viewelementsdisplaymenu.o \
       coreengine/viewelementsdisplayreplay.o \
       coreengine/viewelementsdisplaytracks.o \
       coreengine/viewgrid.o \
       coreengine/viewlist.o \
       coreengine/viewdetail.o \
       coreengine/listelements.o \
       coreengine/area.o \
       coreengine/xmlparser.o

### The main target:

all: subprojects $(SOFILE) i18n

### subprojects:

.PHONY: libskindesignerapi subprojects

libskindesignerapi:
	$(MAKE) -C libskindesignerapi

subprojects: libskindesignerapi

install-subprojects:
	$(MAKE) -C libskindesignerapi install

clean-subprojects:
	$(MAKE) -C libskindesignerapi clean

### get version of subprojects in specific targets when subprojects have been built

%.o: SUB_DEFINES = -DLIBSKINDESIGNERAPIVERSION='"$(shell pkg-config --modversion libskindesignerapi/libskindesignerapi.pc)"'
$(SOFILE): SUB_LIBS = libskindesignerapi/libskindesignerapi.so.$(shell pkg-config --modversion libskindesignerapi/libskindesignerapi.pc)

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -std=c++11 -c $(DEFINES) $(SUB_DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

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
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

$(SOFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) -std=c++11  $(LDFLAGS) -shared $(OBJS) $(LIBS) $(SUB_LIBS) -o $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install-themes:
	mkdir -p $(DESTDIR)$(VDRCONFDIR)/themes
	cp themes/* $(DESTDIR)$(VDRCONFDIR)/themes

install-skins:
	mkdir -p $(DESTDIR)$(PLGRESDIR)/skins
	cp -r skins/* $(DESTDIR)$(PLGRESDIR)/skins
	mkdir -p $(DESTDIR)$(PLGRESDIR)/dtd
	cp -r dtd/* $(DESTDIR)$(PLGRESDIR)/dtd

install-scripts:
	mkdir -p $(DESTDIR)$(SKINDESIGNER_SCRIPTDIR)
	cp -r scripts/* $(DESTDIR)$(SKINDESIGNER_SCRIPTDIR)

install: install-subprojects install-lib install-i18n install-themes install-skins install-scripts

dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude .git* --exclude *.o --exclude *.rej --exclude *.orig -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean: clean-subprojects
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz *~
