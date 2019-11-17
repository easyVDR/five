## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

make_remote_mappingdocdir = ${prefix}/doc/make_remote_mapping
make_remote_mappingdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(make_remote_mappingdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(make_remote_mappingdocdir)
