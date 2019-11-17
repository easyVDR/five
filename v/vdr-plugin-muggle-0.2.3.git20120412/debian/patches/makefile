Index: vdr-plugin-muggle-0.2.3.git20120412/Makefile
===================================================================
--- vdr-plugin-muggle-0.2.3.git20120412.orig/Makefile	2011-11-20 23:08:58.000000000 +0100
+++ vdr-plugin-muggle-0.2.3.git20120412/Makefile	2012-04-12 23:17:41.635911482 +0200
@@ -47,9 +47,9 @@
 VDRDIR ?= ../../..
 LIBDIR ?= ../../lib
 TMPDIR ?= /tmp
-BINDIR ?= /usr/local/bin
+#BINDIR ?= /usr/bin
 
-MUSICDIR ?= /mnt/music
+MUSICDIR ?= /media/easyvdr01/mp3
 
 ### Allow user defined options to overwrite defaults:
 
@@ -177,7 +177,7 @@
 ### Internationalization (I18N):
 
 PODIR     = po
-LOCALEDIR = $(VDRDIR)/locale
+LOCALEDIR = locale
 I18Npo    = $(wildcard $(PODIR)/*.po)
 I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
 I18Npot   = $(PODIR)/$(PLUGIN).pot
@@ -211,15 +211,12 @@
 	$(CXX) $(CXXFLAGS) $^ $(MILIBS) $(SQLLIBS) -o $@
 
 install: all
-	@cp --remove-destination $(LIBDIR)/libvdr-muggle*.so.* \
-		$(DESTDIR)$(PLUGINLIBDIR)
-	@chmod 644 $(DESTDIR)$(PLUGINLIBDIR)/libvdr-muggle*.so*
-	@cp mugglei $(DESTDIR)$(BINDIR)/
-	@cp scripts/muggle-image-convert $(DESTDIR)$(BINDIR)/
+#	@cp mugglei $(DESTDIR)$(BINDIR)/
+#	@cp scripts/muggle-image-convert $(DESTDIR)$(BINDIR)/
 
 dist: $(I18Npo) distclean mg_tables.h
 	@-rm -rf $(TMPDIR)/$(ARCHIVE)
-	@mkdir $(TMPDIR)/$(ARCHIVE)
+#	@mkdir $(TMPDIR)/$(ARCHIVE)
 	@cp -a * $(TMPDIR)/$(ARCHIVE)
 	@tar czf $(PACKAGE).tgz --exclude=.svn -C $(TMPDIR) $(ARCHIVE)
 	@-rm -rf $(TMPDIR)/$(ARCHIVE)
