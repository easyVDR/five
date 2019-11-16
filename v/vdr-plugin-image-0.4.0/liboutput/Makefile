#
# Makefile for Image plugin to VDR
#
# (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
#
# This code is distributed under the terms and conditions of the
# GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
#
# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.
#
# $Id$
#

VDRDIR = ../../../..

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -O2 -Wall -Woverloaded-virtual
PKG-CONFIG ?= pkg-config

-include $(VDRDIR)/Make.config

### The directory environment:


INCLUDES += -I$(VDRDIR)/include -I.
ifdef FFMDIR
INCLUDES += -I$(FFMDIR)
DEFINES += -DFFMDIR
ifndef WITHOUT_SWSCALER
  DEFINES += -DHAVE_SWSCALER
  INCLUDES += -I$(FFMDIR)
endif
else

ifndef WITHOUT_SWSCALER
  DEFINES += -DHAVE_SWSCALER
  PKG-INCLUDES += libswscale
endif

  PKG-INCLUDES += libavcodec
endif

DEFINES  += -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS

ifdef PKG-INCLUDES
INCLUDES += $(shell $(PKG-CONFIG) --cflags $(PKG-INCLUDES))
endif


### The object files (add further files here):

OBJS = encode.o stillimage.o stillimage-player.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: liboutput.a

liboutput.a : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
#	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@ 

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.a *.so *.tgz core* *~
