## Process this file with automake to produce Makefile.in

## Created by Anjuta



AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(VIEW_PICTURE_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = view_picture

view_picture_SOURCES = \
	view-picture.c

view_picture_LDFLAGS = \
	-Wl,--export-dynamic

view_picture_LDADD = $(VIEW_PICTURE_LIBS)

