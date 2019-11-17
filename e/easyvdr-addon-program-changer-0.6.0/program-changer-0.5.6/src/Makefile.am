## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = program_changer.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(PROGRAM_CHANGER_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = program_changer


program_changer_SOURCES =  \
	error.c \
	hotkey.c \
	mouse.c \
	pconfig.c \
	program-changer.c \
	remote.c \
	tools.c \
	error.h \
	hotkey.h \
	mouse.h \
	pconfig.h \
	program-changer.h \
	remote.h \
	tools.h

program_changer_CFLAGS = -pthread 

program_changer_LDFLAGS = \
	-Wl,--export-dynamic

program_changer_LDADD = $(PROGRAM_CHANGER_LIBS)




EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

