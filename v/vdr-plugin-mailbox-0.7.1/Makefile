#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile 426 2014-10-26 10:34:20Z alex $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = mailbox

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' AxPluginMailBox.cpp | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow user defined options to overwrite defaults:

-include $(PLGCFG)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

# ---------------------------------------------------------------------
#      c-client / IMAP header- and library- files
# ---------------------------------------------------------------------
#
#  Gentoo
#  ------
#  emerge c-client (tested with c-client-2007e-r2)

#IMAPINCDIR ?= /usr/include/imap
#IMAPLFLAGS ?= -lc-client

#
#  Debian/Ubuntu
#  -------------
#  install the packages 'libc-client2007e-dev'
#  and set the following variables
#
IMAPINCDIR ?= /usr/include/c-client
IMAPLFLAGS ?= -lc-client
#
#  c-client installation from source
#  ---------------------------------
# enable and modify the following lines, if you installed
# uw-imapd yourself or if you want to link the
# c-client-library statically to the plugin
#
# IMAPDIR    ?= /usr/local/src/imap/c-client
# IMAPINCDIR ?= $(IMAPDIR)
# IMAPLFLAGS ?= $(IMAPDIR)/c-client.a -lssl -lpam


# ---------------------------------------------------------------------
#      (there should be) no need to change anything below
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
#      use "external" AxLib
# ---------------------------------------------------------------------
ifdef AXBASEDIR

AXINCDIR   ?= $(AXBASEDIR)/include
AXCXXFLAGS ?= -I$(AXINCDIR) -DHAVE_AXLIB=1
AXLIBS     ?= -lAxTools -lAxMail -lAxVdr
AXLIBDIR   ?= $(AXBASEDIR)/lib/linux
AXOBJS      =
LDFLAGS    ?= -L$(AXLIBDIR) $(AXLIBS)

# ---------------------------------------------------------------------
#      use "internal" AxLib
# ---------------------------------------------------------------------

else

AXCXXFLAGS += -I./AxLib/include

AXTOOLSDIR  = ./AxLib/src/Tools
AXOBJS     +=   $(AXTOOLSDIR)/String.o \
                $(AXTOOLSDIR)/Globals.o \

AXMAILDIR   = ./AxLib/src/Mail
AXOBJS     +=   $(AXMAILDIR)/CallBackDispatcher.o \
                $(AXMAILDIR)/CallBackProcessor.o \
                $(AXMAILDIR)/Internal.o \
                $(AXMAILDIR)/LogHandlerCBP.o \
                $(AXMAILDIR)/LogHandler.o \
                $(AXMAILDIR)/MailBoxCBP.o \
                $(AXMAILDIR)/MailBox.o \
                $(AXMAILDIR)/MailBoxInternal.o \
                $(AXMAILDIR)/MailBoxMgr.o \
                $(AXMAILDIR)/MailBoxSetup.o \
                $(AXMAILDIR)/Mail.o \
                $(AXMAILDIR)/MailFolder.o \
                $(AXMAILDIR)/MailPart.o \
                $(AXMAILDIR)/MsgBody.o \
                $(AXMAILDIR)/MsgCache.o \
                $(AXMAILDIR)/Request.o \
                $(AXMAILDIR)/Tools.o \

AXVDRDIR    = ./AxLib/src/Vdr
AXOBJS     +=   $(AXVDRDIR)/Action.o \
                $(AXVDRDIR)/MenuCode.o \
                $(AXVDRDIR)/MenuText.o \
                $(AXVDRDIR)/Thread.o \
                $(AXVDRDIR)/Tools.o \

endif

# ---------------------------------------------------------------------
#
# CXXFLAGS:
# -fno-operator-names   is needed for c-client.h
# -O2                   is needed to avoid "undefined symbol" in
#                       Ax::VDR::OsdMenuBase<cOsdMenu>::...
#
CXXFLAGS += -fno-operator-names -DHAVE_VDR=1
DEFINES  += -D_GNU_SOURCE
INCLUDES += $(AXCXXFLAGS) -I$(IMAPINCDIR)

# ---------------------------------------------------------------------

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -D__STL_CONFIG_H

### The object files (add further files here):

OBJS = AxVdrTools.o \
       AxPluginSetup.o AxPluginMailBox.o AxMailWrapper.o \
       AxMenuMailViewBase.o AxMenuMailBoxView.o \
       AxMenuMailListView.o AxMenuMailView.o  AxMenuConfigPlugin.o \
       AxMenuConfigCommon.o AxMenuConfigMailBox.o AxMailChecker.o \
       AxMenuFolders.o AxMenuLogView.o \
       $(AXOBJS)


### The main target:

all: $(SOFILE) i18n

### Implicit rules:

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.cpp) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.cpp)
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
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@ $(IMAPLFLAGS)

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install: install-lib install-i18n

dist:
	-rm -rf $(TMPDIR)/$(ARCHIVE)
	mkdir $(TMPDIR)/$(ARCHIVE)
	svn export --force svn://svnserver/vdr-plugin-mailbox/trunk $(TMPDIR)/$(ARCHIVE)/
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/include
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Mail
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Tools
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Vdr
	mkdir  $(TMPDIR)/$(ARCHIVE)/AxLib/src
	#
	svn export --force svn://svnserver/AxLib/trunk/src/AxMail/include/Ax/Mail      $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Mail
	svn export --force svn://svnserver/AxLib/trunk/src/AxTools/include/Ax/Tools    $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Tools
	svn export --force svn://svnserver/AxLib/trunk/src/AxVdr/include/Ax/Vdr        $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Vdr
	#
	#
	svn export --force svn://svnserver/AxLib/trunk/src/AxMail/src                  $(TMPDIR)/$(ARCHIVE)/AxLib/src/Mail
	svn export --force svn://svnserver/AxLib/trunk/src/AxTools/src                 $(TMPDIR)/$(ARCHIVE)/AxLib/src/Tools
	svn export --force svn://svnserver/AxLib/trunk/src/AxVdr/src                   $(TMPDIR)/$(ARCHIVE)/AxLib/src/Vdr
	#
	# create translation file
	#
	$(TMPDIR)/$(ARCHIVE)/po2i18n.pl < $(TMPDIR)/$(ARCHIVE)/i18n.cpp-template > $(TMPDIR)/$(ARCHIVE)/i18n.cpp
	#
	# remove unused files
	#
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Vdr/Shape
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/src/Vdr/Shape
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Vdr/Net
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/src/Vdr/Shape
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/src/Vdr/Net
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLib/src/Vdr/NetClient
	rm     $(TMPDIR)/$(ARCHIVE)/po2i18n.pl
	rm     $(TMPDIR)/$(ARCHIVE)/mailbox-plugin.*
	find $(TMPDIR)/$(ARCHIVE)/AxLib -name "*.pro"            -exec rm {} \;
	find $(TMPDIR)/$(ARCHIVE)/AxLib -name "Exception*"       -exec rm {} \;
	find $(TMPDIR)/$(ARCHIVE)/AxLib -name "InstanceCounter*" -exec rm {} \;
	find $(TMPDIR)/$(ARCHIVE)/AxLib -name "CMakeLists.txt"   -exec rm {} \;
	#
	# copy some dummy files
	#
	cp -v $(TMPDIR)/$(ARCHIVE)/AxLibDummy/include/Ax/Tools/*.h $(TMPDIR)/$(ARCHIVE)/AxLib/include/Ax/Tools/
	rm -rf $(TMPDIR)/$(ARCHIVE)/AxLibDummy
	#
	# pack the sources
	#
	tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	-rm -rf $(TMPDIR)/$(ARCHIVE)
	echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
