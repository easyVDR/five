## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

view_picturedocdir = ${prefix}/doc/view_picture
view_picturedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(view_picturedoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(view_picturedocdir)
