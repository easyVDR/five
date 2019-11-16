#
# Makefile for VDRSeriesTimer.pl
#

.DELETE_ON_ERROR:

SCRIPT     = VDRSeriesTimer.pl

DESTDIR   ?=
CONFDIR    = /etc/eplists
PREFIX    ?= /usr

MODULEDIR ?= $(shell perl -e '$$prefix = "$(PREFIX)"; $$prefix =~ s|/$$||; foreach (@INC) { if ( m|$${prefix}/+lib| ) { print $$_."\n"; exit 0; } } print "/tmp"; exit 1;')

MANDIR     = $(PREFIX)/share/man
SCRIPTDIR  = $(PREFIX)/bin
LOCDIR     = $(PREFIX)/share/locale
DOCDIR     = $(PREFIX)/share/doc/eplists
TMPDIR     = /tmp

SOURCEDIR   = src/

VERSION = $(shell grep 'my $$Version *=' $(SOURCEDIR)$(SCRIPT) | awk '{ print $$4 }'  | sed -e 's/[";]//g')

ARCHIVE = $(SCRIPT)-$(VERSION)
PACKAGE = $(ARCHIVE)

# The main program:

# Internationalization (I18N):

PODIR     = po
LOCALEDIR = locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/eplists.mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/eplists.pot

FILES       = VDRSeriesTimer.pl Eplists.pm EplistsClient.pm VDREplistChecker.pl

SCRIPTFILES = VDRSeriesTimer.pl VDREplistChecker.pl svdrpsend-ng.pl
MODULEFILES = Eplists.pm EplistsClient.pm
CONFFILES   = etc/*.example

.PHONY: i18n
i18n: $(I18Npot) $(I18Nmsgs)

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(SOURCEDIR)*
	xgettext -C -L perl --from-code=UTF-8 -cTRANSLATORS --no-wrap -k -k__ --msgid-bugs-address='<eplists@constabel.net>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/eplists.mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@


install-i18n:
	@mkdir -p $(DESTDIR)$(LOCDIR)
	@(cd $(LOCALEDIR); cp -r --parents * $(DESTDIR)$(LOCDIR))

# Install the files:
test:	all
all:
	@echo "Done."


install: i18n install-script install-conf install-doc install-i18n installed_msg

# Install scripts:

install-script:
	@mkdir -p $(DESTDIR)$(SCRIPTDIR)
	@mkdir -p $(DESTDIR)$(MODULEDIR)
	@(cd $(SOURCEDIR) && cp --remove-destination $(SCRIPTFILES) $(DESTDIR)$(SCRIPTDIR)/ && cd $(DESTDIR)$(SCRIPTDIR) && chmod 755 $(SCRIPTFILES))
	@(cd $(SOURCEDIR) && cp --remove-destination $(MODULEFILES) $(DESTDIR)$(MODULEDIR)/)

# Configuration files:

install-conf:
	@mkdir -p $(DESTDIR)$(CONFDIR)
	@cp --remove-destination $(CONFFILES) $(DESTDIR)$(CONFDIR)/

# Documentation:

install-doc:
	@mkdir -p $(DESTDIR)$(DOCDIR)
	@mkdir -p $(DESTDIR)$(MANDIR)/man1
	@mkdir -p $(DESTDIR)$(MANDIR)/man5
	@cp --remove-destination -R COPYING HISTORY INSTALL UPGRADE *README* FAQ contrib html $(DESTDIR)$(DOCDIR)/
	@cp --remove-destination man/en/*.1.gz $(DESTDIR)$(MANDIR)/man1/

dist: i18n clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)  
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/*.txt
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/*.1
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/*.old
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/doc-src
	@-rm -rf $(TMPDIR)/$(ARCHIVE)/docsrc*
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

# Remove

uninstall: remove

remove:
	-rm -rf $(DESTDIR)$(DOCDIR)/
	-rm -f $(DESTDIR)$(MANDIR)/man1/VDRSeriesTimer.pl.1.gz $(DESTDIR)$(MANDIR)/man1/svdrpsend-ng.pl.1.gz
	-rm -f $(DESTDIR)$(SCRIPTDIR)/VDRSeriesTimer.pl
	-rm -f $(DESTDIR)$(SCRIPTDIR)/VDREplistChecker.pl
	-rm -f $(DESTDIR)$(SCRIPTDIR)/svdrpsend-ng.pl
	-rm -f $(DESTDIR)$(MODULEDIR)/Eplists*.pm
	-rm -f $(DESTDIR)$(CONFDIR)/*.example
	@-rmdir $(DESTDIR)$(CONFDIR)/

.PHONY: installed_msg
installed_msg:
	@printf "#\n# Installation of $(SCRIPT) v$(VERSION) finnished.\n#\n# Installed files in SCRIPTDIR: $(SCRIPTFILES)\n#\n# Installed files in MODULEDIR: $(MODULEFILES)\n#\n# PREFIX:    $(PREFIX)\n#\n# SCRIPTDIR: $(SCRIPTDIR)\n#\n# MODULEDIR: $(MODULEDIR)\n#\n"

# Housekeeping:

clean:
	@-find ./ -name "*~" -delete -o -name "*.tgz" -delete -o -name "*.tmp" -delete

CLEAN: clean

