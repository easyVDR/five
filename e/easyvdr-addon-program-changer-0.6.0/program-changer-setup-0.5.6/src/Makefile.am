## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = program_changer_setup.ui


AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(PROGRAM_CHANGER_SETUP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = program_changer_setup

program_changer_setup_SOURCES = \
	program-changer-setup.c \
	hotkey.c \
	hotkey.h \
	make_lirc_conf_files.c \
	make_lirc_conf_files.h \
	message_and_error_log.c \
	message_and_error_log.h \
	parameter.h \
	program-changer-setup.h \
	tools.c \
	tools.h	

program_changer_setup_CFLAGS = -lpthread 


program_changer_setup_LDFLAGS = \
	-Wl,--export-dynamic

program_changer_setup_LDADD = $(PROGRAM_CHANGER_SETUP_LIBS)




EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(pkgdatadir)

