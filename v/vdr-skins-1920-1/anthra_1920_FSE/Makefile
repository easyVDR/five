#
#	Makefile for anthra_HD for text2skin
#
#	based on text2skin Makefile.example by Thomas Günther <tom@toms-cafe.de>
#
#	Copyright (C) 2010 - 2011 Tomas Saxer <tsaxer@gmx.de>   
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


SKIN = anthra_1920_FSE
VERSION = 0.0.5

-include Make.config

MAINMENU_12 = 1

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

	@echo
	@echo "$(SKIN)-$(VERSION) is generated with following settings:"; \
	echo "----------------------------------------------------------"; \

	@if [ "$(HEIGHT68)" = "1" ]; then\
	  cat xml-height-68/*.xml > $(SKIN).skin;\
	  echo "- HEIGHT68";\
	else\
	  cat xml-height-58/*.xml > $(SKIN).skin;\
	  echo "- HEIGHT58";\
	fi
	
	@sed 's/VERSION/$(VERSION)/g' -i $(SKIN).skin;
	
	@if [ "$(VIDEOSIZE)" = "" ]; then\
	  sed -e '/--VIDEOSIZE/d' -i $(SKIN).skin;\
	  echo "- NO VIDEOSIZE";\
	else\
	  sed -e '/NOVIDEOSIZE/d' -i $(SKIN).skin;\
	  echo "- VIDEOSIZE";\
	fi
		
	@if [ "$(LOGOSLEFT)" = "1" -a -n "$(CHANNELLOGOSPATH)" ];then\
	  sed 's/CHANNELLOGOSPATH/$(CHANNELLOGOSPATH)/g;/LOGOSRIGHT/d;/NOLOGOS/d' -i $(SKIN).skin;\
	  echo "- LOGOSLEFT";\
	  echo "- CHANNELLOGOSPATH: "$(CHANNELLOGOSPATH)"";\
	fi
	
	@if [ "$(LOGOSLEFT)" = "1" -a -z "$(CHANNELLOGOSPATH)" ];then\
	  sed '/LOGOSRIGHT/d;/NOLOGOS/d;' -i $(SKIN).skin;\
	  echo "\033[93;41m  !!! ERROR !!! CHANNELLOGOS are activated,\
	  you have to set CHANNELLOGOSPATH   \033[0m";\
	fi
	
	@if [ "$(LOGOSRIGHT)" = "1" -a -n "$(CHANNELLOGOSPATH)" ];then\
	  sed 's/CHANNELLOGOSPATH/$(CHANNELLOGOSPATH)/g;/LOGOSLEFT/d;/NOLOGOS/d;' -i $(SKIN).skin;\
	  echo "- LOGOSRIGHT";\
	  echo "- CHANNELLOGOSPATH: "$(CHANNELLOGOSPATH)"";\
	fi	
	
	@if [ "$(LOGOSRIGHT)" = "1" -a -z "$(CHANNELLOGOSPATH)" ];then\
	  sed '/LOGOSLEFT/d;/NOLOGOS/d;' -i $(SKIN).skin;\
	  echo "\033[93;41m  !!! ERROR !!! CHANNELLOGOS are activated,\
	  you have to set CHANNELLOGOSPATH   \033[0m";\
	fi	
	
	@if [ "$(NOLOGOS)" = "1" ]; then\
	  sed -e '/LOGOSLEFT/d;/LOGOSRIGHT/d;' -i $(SKIN).skin;\
	  echo "- NOLOGOS"; \
	fi
		
	@if [ "$(SEPARATORLOGOS-LEFT)" = "1" -o  "$(SEPARATORLOGOS-RIGHT)" = "1" ];then\
	     if [ -z "$(SEPARATORLOGOSPATH)" ]; then\
	       echo "\033[93;41m  !!! ERROR !!! SEPARATORLOGOS are activated,\
	       you have to set   SEPARATORLOGOSPATH \033[0m";\
	     elif [ "$(SEPARATORLOGOS-LEFT)" = "1" ]; then\
	       sed -e '/SEPARATORLOGOS-RIGHT/d' -i $(SKIN).skin;\
	       sed 's/SEPARATORLOGOSPATH/$(SEPARATORLOGOSPATH)/g' -i $(SKIN).skin;\
	       echo "- SEPARATORLOGOS-LEFT";\
	       echo "- SEPARATORLOGOSPATH: "$(SEPARATORLOGOSPATH)"";\
	     else \
	       sed -e '/SEPARATORLOGOS-LEFT/d' -i $(SKIN).skin;\
	       sed 's/SEPARATORLOGOSPATH/$(SEPARATORLOGOSPATH)/g' -i $(SKIN).skin;\
	       echo "- SEPARATORLOGOS-RIGHT";\
	       echo "- SEPARATORLOGOSPATH: "$(SEPARATORLOGOSPATH)"";\
	      fi \
	fi

	@if [ "$(MENULOGOS)" = "" ]; then\
	  sed -e '/-MAINMENULOGOS/d;/SUBMENULOGOS/d' -i $(SKIN).skin;\
	  echo "- NO MENULOGOS";\
	else \
	  sed -e'/NOMAINMENULOGOS/d' -i $(SKIN).skin;\
	  echo "- MENULOGOS";\
	fi
	
	@if [ "$(EPGIMAGES)" = "" ];then\
	  sed -e '/EPG-IMAGES/d;/NOEPGIM/d;' -i $(SKIN).skin ;\
	  echo "- NO EPGIMAGES"; \
	fi
	 
	@if [ "$(EPGIMAGES)" = "1" -a -n "$(EPGIMAGESPATH)" ]; then\
	  sed 's/EPGIMAGESPATH/$(EPGIMAGESPATH)/g' -i $(SKIN).skin;\
	  echo "- EPGIMAGES";\
	  echo "- EPGIMAGESPATH: "$(EPGIMAGESPATH)"";\
	  if [ "$(EPGIMAGESJPG)" = "" ]; then\
	    echo "- EPGIMAGESFORMAT: png";\
	  fi \
	fi
	
	@if [ "$(EPGIMAGES)" = "1" -a -z "$(EPGIMAGESPATH)" ];then\
	  echo "\033[93;41m  !!! ERROR !!! EPGIMAGES are activated, you have to set EPGIMAGESPATH   \033[0m";\
	fi
	
	@if [ "$(EPGIMAGES)" = "1" -a "$(EPGIMAGESJPG)" = "1" ]; then\
	  sed 's/{PresentEventID}.png/{PresentEventID}.jpg/g' -i $(SKIN).skin;\
	  echo "- EPGIMAGESFORMAT: jpg";\
	fi
	
	@if [ "$(RECORDINGIMAGES)" = "1" ];then\
	  echo "- RECORDINGIMAGES";\
	  if [ "$(EPGIMAGESJPG)" = "1" ];then\
	    echo "- RECORDINGIMAGESFORMAT: jpg";\
	    sed 's/RECORDINGIMAGENAME.png/RECORDINGIMAGENAME.jpg/g' -i $(SKIN).skin;\
	  else \
	    echo "- RECORDINGIMAGESFORMAT: png";\
	  fi; \
	  sed 's/RECORDINGIMAGENAME/$(RECORDINGIMAGENAME)/g' -i $(SKIN).skin;\
	    echo "- RECORDINGIMAGENAME: $(RECORDINGIMAGENAME)";\
	else \
	  sed -e '/RECORDINGIMAGES/d' -i $(SKIN).skin ;\
	  echo "- NO RECORDINGIMAGES";\
	fi
	
	@if [ "$(TIMER-INFO_MAIN-MENU)" = "1" ];then\
	  echo "- TIMER-INFO_MAIN-MENU";\
	else\
	  sed -e '/TIMER-INFO_MAIN-MENU/d' -i $(SKIN).skin ;\
	  echo "- NO TIMER-INFO_MAIN-MENU";\
	fi
	
	@if [ "$(TIMER-INFO_MAIN-MENU)" = "1" -a "$(RUNNING_TIMER_BLINK)" = 1 ];then\
	  echo "- RUNNING_TIMER_BLINK";\
	else\
	  sed -e '/_BLINK/d' -i $(SKIN).skin ;\
	  echo "- NO RUNNING_TIMER_BLINK";\
	fi
	
	@if [ "$(SMALLMENUFONT)" = "1" ]; then\
	  sed -e '/DEFAULTMENUFONT/d;/LARGEMENUFONT/d;' -i $(SKIN).skin;\
	  echo "- SMALLMENUFONT";\
	fi
	
	@if [ "$(DEFAULTMENUFONT)" = "1" ]; then\
	  sed -e '/SMALLMENUFONT/d;/LARGEMENUFONT/d;' -i $(SKIN).skin;\
	  echo "- DEFAULTMENUFONT";\
	fi
	
	@if [ "$(LARGEMENUFONT)" = "1" ]; then\
	  sed -e '/SMALLMENUFONT/d;/DEFAULTMENUFONT/d;' -i $(SKIN).skin;\
	  echo "- LARGEMENUFONT";\
	fi
		
	@if [ "$(DIGIT_SIGNAL)" = "1" ]; then\
	  sed -e '/GRAPH_SIGNAL/d;' -i $(SKIN).skin;\
	  echo "- DIGIT_SIGNAL";\
	else \
	  sed -e '/DIGIT_SIGNAL/d;' -i $(SKIN).skin;\
	  echo "- GRAPH_SIGNAL";\
	fi
	
	@if [ "$(SCROLLBAR)" = "1" ]; then\
	  sed -e '/SCROLLARROWS/d' -i $(SKIN).skin ;\
	  echo "- SCROLLBAR";\
	fi
	
	@if [ "$(SCROLLARROWS)" = "1" ]; then\
	  sed -e '/SCROLLBAR/d' -i $(SKIN).skin ;\
	  echo "- SCROLLARROWS";\
	fi
	
	@if [ "$(yaVDRMENULOGOS_de)" = "1" ]; then\
	  sed '/yaVDR/r yaVDRlogoslocale/de.xml' -i $(SKIN).skin ;\
	  echo "- yaVDRMENULOGOS_de";\
	fi
	
	@if [ "$(yaVDRMENULOGOS_en)" = "1" ]; then\
	  sed '/yaVDR/r yaVDRlogoslocale/en.xml' -i $(SKIN).skin ;\
	  echo "- yaVDRMENULOGOS_en";\
	fi
	
	@if [ -x /usr/bin/xmlstarlet ]; then\
	  echo "------------------> Testing $(SKIN).skin with RELAX NG schema for text2skin";\
	  echo "\n";\
	  xmlstarlet val -e -r text2skin-1.1.rng $(SKIN).skin;\
	  echo "\n";\
	fi

install-i18n: i18n
	@mkdir -p $(DESTDIR)$(LOCDIR)
	@cp -r $(LOCALEDIR)/* $(DESTDIR)$(LOCDIR)
	
install: install-i18n 
	@mkdir -p $(DESTDIR)$(SKINDIR)
	@cp  $(SKIN).skin $(DESTDIR)$(SKINDIR)
	@cp  $(SKIN).colors $(DESTDIR)$(SKINDIR)
	@cp -r images $(DESTDIR)$(SKINDIR)
	@cp -r mainmenulogos $(DESTDIR)$(SKINDIR)	
	@cp -r submenulogos $(DESTDIR)$(SKINDIR)	
	@cp  COPYING $(DESTDIR)$(SKINDIR)
	@mkdir -p $(DESTDIR)$(THEMESDIR)
	@cp themes/*.theme $(DESTDIR)$(THEMESDIR)

clean:
	@-rm -rf $(LOCALEDIR) $(I18Npot) $(SKIN).skin *~

uninstall:
	@-rm -rf $(DESTDIR)$(SKINDIR)

