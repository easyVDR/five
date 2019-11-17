#
#    Simple Makefile for anthra_SD for text2skin
#
#    Tomas Saxer <tsaxer@gmx.de>   
#


##### to comment an option just place a '#' at the beginning of the line
##### to uncomment an option please remove the leading '#'
##### !!! Take care to uncomment only ONE option in each section !!!

##### CHANNELLOGOS ######
#NOLOGOS = 1
LOGOSLEFT = 1

##### NOMARQUEE ######
#NOMARQUEE = 1

### The directory environment:

DESTDIR ?= 
PREFIX  ?= /usr
CONFDIR  = $(if $(subst /usr,,$(PREFIX)), $(PREFIX))/vdr/lib/vdr
SKINDIR  = $(CONFDIR)/plugins/text2skin/$(SKIN)
THEMESDIR  = $(CONFDIR)/themes
LOCDIR   = $(PREFIX)/share/locale

###### Don't edit below this line, if you don't know what you are doing ;) ######


SKIN = anthra_SD

all: $(SKIN).skin i18n

### Internationalization (I18N):
PODIR     = po
LOCALEDIR = locale

I18Npo    = $(notdir $(wildcard $(PODIR)/*.po))
I18Npot   = $(PODIR)/$(SKIN).pot

$(I18Npot): $(SKIN).skin
	@cat $^ | sed -e "s/('/(\"/g;s/')/\")/g" | grep -o "trans([^)]*)" | \
	xgettext -C --no-wrap --no-location -k -ktrans \
	         --msgid-bugs-address='<$(MAIL)>' -o $@ -

%.po: $(I18Npot)
	@msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(LOCALEDIR)/%/LC_MESSAGES/vdr-text2skin-$(SKIN).mo: $(PODIR)/%.po
	@mkdir -p $(dir $@)
	@msgfmt -c -o $@ $<

i18n: $(I18Npo:%.po=$(LOCALEDIR)/%/LC_MESSAGES/vdr-text2skin-$(SKIN).mo)

### Targets:

anthra_SD.skin:

	@cp $(SKIN).template $(SKIN).skin
	
	@echo
	@echo "The skinfile is generated with the following settings:"; \
	echo "-------------------------------------------------------"; \
	
	@if [ "$(LOGOSLEFT)" = "1" ]; then\
	  sed -e '/LOGOSRIGHT/d;/NOLOGOS/d;' -i $(SKIN).skin;\
	  echo "LOGOSLEFT"; \
	fi
		
	@if [ "$(NOLOGOS)" = "1" ]; then\
	  sed -e '/LOGOSLEFT/d;/LOGOSRIGHT/d;' -i $(SKIN).skin;\
	  echo "NOLOGOS"; \
	fi
	
	@if [ "$(NOMARQUEE)" = "1" ]; then\
	  sed -e '/marquee/d;' -i $(SKIN).skin;\
	  echo "NOMARQUEE"; \
	else \
	  sed -e '/NOMARQUEE/d;' -i $(SKIN).skin;\
	  echo "MARQUEE"; \
	fi
	

install-i18n: i18n
	@mkdir -p $(DESTDIR)$(LOCDIR)
	@cp -r $(LOCALEDIR)/* $(DESTDIR)$(LOCDIR)
	
install: install-i18n 
	@mkdir -p $(DESTDIR)$(SKINDIR)
	@cp  $(SKIN).skin $(DESTDIR)$(SKINDIR)
	@cp  $(SKIN).colors $(DESTDIR)$(SKINDIR)
	@cp -r images $(DESTDIR)$(SKINDIR)
	@cp  COPYING $(DESTDIR)$(SKINDIR)
	@mkdir -p $(DESTDIR)$(THEMESDIR)
	@cp themes/*.theme $(DESTDIR)$(THEMESDIR)

clean:
	@-rm -rf $(LOCALEDIR) $(I18Npot) $(SKIN).skin *~

uninstall:
	@-rm  $(DESTDIR)$(SKINDIR)/COPYING
	@-rm  $(DESTDIR)$(SKINDIR)/$(SKIN).skin
	@-rm  $(DESTDIR)$(SKINDIR)/$(SKIN).colors
	@-rmdir  $(DESTDIR)$(SKINDIR)
	
	
	 

 
