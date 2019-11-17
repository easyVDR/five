#
#	Makefile for anthra_1024 for text2skin
#
#	based on text2skin Makefile.example by Thomas Günther <tom@toms-cafe.de>
#
#	Copyright (C) 2010 Tomas Saxer <tsaxer@gmx.de>   
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


SKIN = anthra_1024
VERSION = 0.2

-include Make.config

### The main target:

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

$(SKIN).skin:

	@for FILES in $(wildcard xml/*.xml); do cat "$${FILES}" >> $(SKIN).skin; done

	
	@echo
	@echo "The skinfile is generated with the following settings:"; \
	echo "-------------------------------------------------------"; \
	
	@sed 's/VERSION/$(VERSION)/g' -i $(SKIN).skin;
		
	@if [ "$(LOGOSLEFT)" = "1" -a -n "$(CHANNELLOGOSPATH)" ];then\
	  sed 's/CHANNELLOGOSPATH/$(CHANNELLOGOSPATH)/g' -i $(SKIN).skin;\
	  sed -e '/NOLOGOS/d' -i $(SKIN).skin;\
	  echo "- LOGOSLEFT";\
	  echo "- CHANNELLOGOSPATH: "$(CHANNELLOGOSPATH)"";\
	fi
	
	@if [ "$(LOGOSLEFT)" = "1" -a -z "$(CHANNELLOGOSPATH)" ];then\
	  echo "\033[93;41m  !!! ERROR !!! CHANNELLOGOS are activated,\
	  you have to set CHANNELLOGOSPATH   \033[0m";\
	fi
		
	@if [ "$(NOLOGOS)" = "1" ]; then\
	  sed -e '/LOGOSLEFT/d;/LOGOSRIGHT/d;' -i $(SKIN).skin;\
	  echo "- NOLOGOS"; \
	fi
	
	@if [ "$(MENULOGOS)" = "" ]; then\
	  sed -e '/MENULOGOS/d' -i $(SKIN).skin;\
	  echo "- NOMENULOGOS";\
	else \
	  echo "- MENULOGOS";\
	fi
		
	@if [ "$(EPGIMAGES)" = "" ];then\
	  sed -e '/EPGIMAGES/d' -i $(SKIN).skin ;\
	  echo "- NOEPGIMAGES";\
	fi
	 
	@if [ "$(EPGIMAGES)" = "1" -a -n "$(EPGIMAGESPATH)" ]; then\
	  sed 's/EPGIMAGESPATH/$(EPGIMAGESPATH)/g' -i $(SKIN).skin;\
	  echo "- EPGIMAGES";\
	  echo "- EPGIMAGESPATH: "$(EPGIMAGESPATH)"";\
	  if [ "$(EPGIMAGESJPG)" = "" ]; then\
	  echo "- EPGIMAGESFORMAT: png";\
	  fi;\
	fi
	
	@if [ "$(TIMER-INFO_MAIN-MENU)" = "1" ];then\
	  echo "- TIMER-INFO_MAIN-MENU";\
	else\
	  sed -e '/TIMER-INFO_MAIN-MENU/d' -i $(SKIN).skin ;\
	  echo "- NO_TIMER-INFO_MAIN-MENU";\
	fi
	
	@if [ "$(EPGIMAGES)" = "1" -a -z "$(EPGIMAGESPATH)" ];then\
	  echo "\033[93;41m  !!! ERROR !!! EPGIMAGES are activated, you have to set EPGIMAGESPATH   \033[0m";\
	fi
	
	@if [ "$(EPGIMAGES)" = "1" -a "$(EPGIMAGESJPG)" = "1" ]; then\
	  sed 's/{PresentEventID}.png/{PresentEventID}.jpg/g' -i $(SKIN).skin;\
	  echo "- EPGIMAGESFORMAT: jpg";\
	fi
	
	@if [ "$(NOMARQUEE)" = "1" ]; then\
	  sed -e '/marquee/d;' -i $(SKIN).skin;\
	  echo "- NOMARQUEE";\
	else\
	  sed -e '/NOMARQUEE/d;' -i $(SKIN).skin;\
	  echo "- MARQUEE";\
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
