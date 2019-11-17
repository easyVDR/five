#
# Makefile for a Video Disk Recorder plugin
#
# $Id$


# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.


#HAVE_HD_OSD=1

# Default Imlib2, otherwise uncomment, !! GraphTFT user should uncomment this !!
#HAVE_MAGICK=1
#MAGICKDIR=/usr/include/ImageMagick

# Uncomment the following line, if you don't have libsndfile installed
#WITHOUT_LIBSNDFILE=1

# Uncomment the following line, if you don't have libvorbisfile installed
#WITHOUT_LIBVORBISFILE=1


#BROKEN_PCM=1

# Uncomment the following line, if you want to include debug symbols
DBG=1

# Uncomment if you want debug output on stdout
DEBUG=1

# internal use
#DEBUG_important=1

# Uncomment if you want to use TrueType fonts.
HAVE_FREETYPE=1

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
# IMPORTANT: the presence of this macro is important for the Make.config
# file. So it must be defined, even if it is not used here!
#
PLUGIN = music-sd

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -g -O2 -Wall -Woverloaded-virtual -Wno-parentheses

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config


DOXYGEN  = /usr/bin/doxygen
DOXYFILE = Doxyfile

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)


### Test whether VDR has locale support
VDRLOCALE = $(shell grep 'I18N_DEFAULT_LOCALE' $(VDRDIR)/i18n.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)


### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

ifdef HAVE_OSDMOD
	DEFINES += -DHAVE_OSDMOD
endif

ifdef HAVE_HD_OSD
	DEFINES += -DHAVE_HD_OSD
endif

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

ifdef DEBUG_important
	DEFINES += -DDEBUG_important
endif

### The object files (add further files here):

OBJS     =  $(PLUGIN).o config.o vars.o bitmap.o imagecache.o quantize.o \
	    commands.o options.o lyrics.o stringfunct.o cover.o skin.o visual.o tracklist.o \
	    search.o mp3id3.o mp3id3tag.o rating.o menubrowse.o mp3control.o \
	    i18n.o icons.o data.o menu.o \
	    data-mp3.o setup-mp3.o player-mp3.o stream.o network.o \
	    decoder.o decoder-mp3.o decoder-mp3-stream.o decoder-snd.o \
	    decoder-ogg.o

LIBS     += -lmad -lid3tag


# for older versions
#ifdef HAVE_MAGICK
#	LIBS += -lMagick -lMagick++
#	DEFINES += -DHAVE_MAGICK
#else
#	LIBS += -lImlib2
#	DEFINES += -DHAVE_IMLIB2
#endif

ifdef HAVE_MAGICK
	INCLUDES += -I$(MAGICKDIR)
	LIBS += $(shell Magick++-config --libs)
	DEFINES += -DHAVE_MAGICK
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
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(subst i18n.c,,$(OBJS:%.o=%.c)) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Ndirs  = $(notdir $(foreach file, $(I18Npo), $(basename $(file))))
I18Npot   = $(PODIR)/$(PLUGIN).pot
I18Nvdrmo = vdr-$(PLUGIN).mo
ifeq ($(strip $(APIVERSION)),1.5.7)
  I18Nvdrmo = $(PLUGIN).mo
endif

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(subst i18n.c,,$(wildcard *.c))
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<djoimania@web.de>' -o $@ $(subst i18n.c,,$(wildcard *.c))

$(I18Npo): $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<

ifneq ($(strip $(VDRLOCALE)),)
### do gettext based i18n stuff

i18n: $(I18Nmo)
	@mkdir -p $(LOCALEDIR)
	for i in $(I18Ndirs); do\
	    mkdir -p $(LOCALEDIR)/$$i/LC_MESSAGES;\
	    cp $(PODIR)/$$i.mo $(LOCALEDIR)/$$i/LC_MESSAGES/$(I18Nvdrmo);\
	    done

i18n.c: i18n-template.c
	@cp i18n-template.c i18n.c

else ### do i18n.c based i18n stuff

i18n: $(I18Nmo)
	@### nothing to do

#i18n compatibility generator:
i18n.c: i18n-template.c buildtools/po2i18n.pl $(I18Npo)
	buildtools/po2i18n.pl < i18n-template.c > i18n.c

endif

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean i18n
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz --exclude CVS --exclude '.#*' --exclude '*.bak' --exclude '*.mo' --exclude '*.pot' -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

srcdoc:
	@cp $(DOXYFILE) $(DOXYFILE).tmp
	@echo PROJECT_NUMBER = $(VERSION) >> $(DOXYFILE).tmp
	$(DOXYGEN) $(DOXYFILE).tmp
	@rm $(DOXYFILE).tmp

clean:
	@-rm -f $(OBJS) $(DEPFILE) i18n.c *.so *.tgz core* *~ $(PODIR)/*.mo $(PODIR)/*.pot
