
SUBDIRS = . yard2flash addon

bin_PROGRAMS =  \
   yard2srvd    \
   yard2wakeup  \
   yard2config  \
   yard2record  \
   yard2lcdtest \
   lirctest

yard2srvd_SOURCES = \
   yard2srvd.c \
   util.c \
   inih/ini.c

yard2wakeup_SOURCES = \
   yard2wakeup.c

yard2config_SOURCES = \
   yard2config.c \
   util.c

yard2record_SOURCES = \
   yard2record.c \
   util.c

yard2lcdtest_SOURCES = \
   yard2lcdtest.c \
   util.c

lirctest_SOURCES = \
   lirctest.c \
   util.c

noinst_HEADERS = \
   Command_Defines.h \
   global_config.h \
   lirctest.h \
   util.h \
   yard2config.h \
   yard2lcdtest.h \
   yard2record.h \
   yard2srvd.h \
   yard2wakeup.h

EXTRA_DIST = \
   LCDproc \
   lirc \
   Readme.txt \
   _input_h_KeybIDs.txt \
   .gitignore \
   getVersion.sh \
   inih \
   yard2srvd.ini.def

BUILT_SOURCES = release.h

# FIXME: Add -Werror to CFLAGS; maybe some checks needs to be disabled.
AM_CFLAGS   = -Wall -Wno-pointer-sign
AM_CPPFLAGS = $(AM_CFLAGS)

# automatic re-configure, when someone changes the package version 
CONFIG_STATUS_DEPENDENCIES = $(top_srcdir)/ChangeLog 

if PROG_GIT
# got this idea from here:
# http://stackoverflow.com/a/12368262
GIT_VERSION = "$(shell git rev-parse 2> /dev/null; if [ $$? -eq 0 ]; then \
                 echo -n '-'; git describe --abbrev=6 --dirty --always; fi)"
else
# Next line will set current date/time; enabl eit for debug builds 
#GIT_VERSION = "-$(shell date +%F:%T)"
GIT_VERSION = ""
endif

PKG_VERSION = "$(shell head -1 $(top_srcdir)/ChangeLog | cut -d " " -f 1)"

release.h: $(top_srcdir)/ChangeLog
	echo "#define PKG_VERSION \"$(PKG_VERSION)\"" > release.h
	echo "#define GIT_VERSION \"$(GIT_VERSION)\"" >> release.h

.PHONY: clean-release_h
clean-release_h:
	rm -f release.h

# got this idea from here:
# http://stackoverflow.com/questions/3523534/good-ways-to-manage-a-changelog-using-git
#.PHONY: update-ChangeLog
#update-ChangeLog:
#	if test -d $(srcdir)/.git; then                         \
#		rm -f ChangeLog.tmp;                                \
#		$(srcdir)/build-aux/gitlog-to-changelog             \
#		  --format='%s%n%n%b%n' --no-cluster                \
#		  --strip-tab --strip-cherry-pick                   \
#		  -- $$(cat $(srcdir)/.last-cl-gen)..               \
#		> ChangeLog.tmp                                     \
#	  && git rev-list -n 1 HEAD > .last-cl-gen.tmp          \
#	  && (echo; cat $(srcdir)/ChangeLog) >> ChangeLog.tmp   \
#	  && mv -f ChangeLog.tmp $(srcdir)/ChangeLog            \
#	  && mv -f .last-cl-gen.tmp $(srcdir)/.last-cl-gen      \
#	  && rm -f ChangeLog.tmp;                               \
#	fi

#EXTRA_DIST += .last-cl-gen

clean-local: clean-release_h

