#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.2 2005/01/18 08:46:02 boonkerz Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#

## =============================
## for magick++ support do this:
## default Imlib2 
## =============================
HAVE_IMAGEMAGICK=1
IMAGEMAGICKDIR=/usr/include/ImageMagick

## =============================
## enable debug
## =============================
##(symbols) :
#DBG=1
## Imageloader- /-processing
#DEBUG_COVER=1
#( console output):
#DEBUG_WEATHER=1
DEBUG_WEATHER_SYSTEM=1
#DEBUG_WEATHER_CONFIG=1
DEBUG_WEATHER_OSD=1
#DEBUG_WEATHER_IMAGES=1


BUGHUNT=1


PLUGIN = weatherhd

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -g -O3 -Wall -Woverloaded-virtual -Wno-parentheses

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Make sure that necessary options are included:

-include $(VDRDIR)/Make.global

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

APIVERSION = $(shell grep 'define APIVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### Test whether VDR has locale support
VDRLOCALE = $(shell grep '^LOCALEDIR' $(VDRDIR)/Make.config)

### The name of the distribution archive:
ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

# for older versions
#ifdef HAVE_MAGICK
#	LIBS += -lMagick -lMagick++
#	DEFINES += -DHAVE_MAGICK
#else
#	LIBS += -lImlib2
#	DEFINES += -DHAVE_IMLIB2
#endif

ifdef HAVE_IMAGEMAGICK
	INCLUDES += -I$(IMAGEMAGICKDIR)
	LIBS += $(shell Magick++-config --libs)
	DEFINES += -DHAVE_IMAGEMAGICK
else
	INCLUDES += -I$(IMLIB)/src
	LIBS += $(shell imlib2-config --libs)
	DEFINES += -DHAVE_IMLIB2
endif

ifdef   DEBUG_WEATHER
	DEFINES += -DDEBUG_WEATHER
endif

ifdef   DEBUG_WEATHER_SYSTEM
	DEFINES += -DDEBUG_WEATHER_SYSTEM
endif

ifdef   DEBUG_WEATHER_CONFIG
	DEFINES += -DDEBUG_WEATHER_CONFIG
endif

ifdef   DEBUG_WEATHER_OSD
	DEFINES += -DDEBUG_WEATHER_OSD
endif

ifdef   DEBUG_COVER
	DEFINES += -DDEBUG_COVER
endif

ifdef   DEBUG_WEATHER_IMAGES
	DEFINES += -DDEBUG_WEATHER_IMAGES
endif

ifdef   BUGHUNT
	DEFINES += -DBUGHUNT
endif


INCLUDES += -I$(VDRDIR)/include 

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

OBJS =  $(PLUGIN).o bitmap.o imagecache.o\
	xml.o parsing.o skin.o OsdWeather.o setup.o stringfunct.o vars.o

#ifdef DBG
#	CXXFLAGS += -g
#endif

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:
MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Make.config
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(subst i18n.c,,$(OBJS:%.o=%.c)) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<gda@dachsweb.de>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs)

### Targets:

.PHONY: all dist clean

all: libvdr-$(PLUGIN).so i18n

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
