#
# Makefile for a Video Disk Recorder plugin
#
# $Id$


# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.

# (Default) DO NOT UNCOMMENT IT IN DEVELOPER-VERSIONS
HAVE_IMAGEMAGICK=1
IMAGEMAGICKDIR=/usr/include/ImageMagick

# Uncomment the following line, if you don't have libsndfile installed
#WITHOUT_LIBSNDFILE=1

# Uncomment the following line, if you don't have libvorbisfile installed
#WITHOUT_LIBVORBISFILE=1


BROKEN_PCM=1

# Uncomment the following line, if you want to include debug symbols
#DBG=1

# Uncomment if you want debug output on stdout
DEBUG=1

# internal use
# BUGHUNT
#BUGHUNT=1

# internal use
# DEBUG_COVER
#DEBUG_COVER=1

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
# IMPORTANT: the presence of this macro is important for the Make.config
# file. So it must be defined, even if it is not used here!
#
PLUGIN = music

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -g -O2 -Wall -Woverloaded-virtual -Wno-parentheses

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Make sure that necessary options are included:

-include $(VDRDIR)/Make.global

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config


DOXYGEN  = /usr/bin/doxygen
DOXYFILE = Doxyfile

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### Test whether VDR has locale support
VDRLOCALE = $(shell grep '^LOCALEDIR' $(VDRDIR)/Make.config)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)


### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef BROKEN_PCM
	DEFINES += -DBROKEN_PCM
endif

ifeq ($(shell test -f $(VDRDIR)/fontsym.h ; echo $$?),0)
	DEFINES += -DHAVE_BEAUTYPATCH
endif  

ifdef DBG
	CXXFLAGS += -g
endif

ifdef DEBUG
	DEFINES += -DDEBUG
else
	DEFINES += -DNO_DEBUG
endif

ifdef BUGHUNT
	DEFINES += -DBUGHUNT
endif

ifdef DEBUG_COVER
	DEFINES += -DDEBUG_COVER
endif


### The object files (add further files here):

OBJS     =  $(PLUGIN).o config.o vars.o bitmap.o \
	    commands.o options.o lyrics.o funct.o cover.o skin.o visual.o tracklist.o \
	    search.o mp3id3.o mp3id3tag.o rating.o menubrowse.o mp3control.o \
	    i18n.o icons.o data.o menu.o \
	    data-mp3.o setup-mp3.o player-mp3.o stream.o network.o \
	    decoder.o decoder-mp3.o decoder-mp3-stream.o decoder-snd.o \
	    decoder-ogg.o

LIBS     += -lasound -lmad -lid3tag -lz -lid3


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

ifndef WITHOUT_LIBSNDFILE
	LIBS    += -lsndfile
	DEFINES += -DHAVE_SNDFILE
endif

ifndef WITHOUT_LIBVORBISFILE
	LIBS    += -lvorbisfile -lvorbis
	DEFINES += -DHAVE_VORBISFILE
endif

### The main target:

all: libvdr-$(PLUGIN).so i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

### Dependencies:

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
