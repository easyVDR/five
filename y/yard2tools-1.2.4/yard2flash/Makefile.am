
SUBDIRS = .

# we don't want to find usb.h in the yard2flash directory. It's for the Windows built.
AUTOMAKE_OPTIONS = nostdinc
DEFAULT_INCLUDES =

bin_PROGRAMS =  \
   yard2flash
   
yard2flash_SOURCES = \
   main.c \
   fsusb.c \
   rjlhex.c \
   memimg.c

# FIXME: getline.h and usb.h not required any longer
noinst_HEADERS = \
   bootload.h \
   fsusb.h \
   memimg.h \
   rjlhex.h \
   getline.h \
   usb.h

#FIXME: getline.c not required any longer
EXTRA_DIST = \
   README \
   COPYING \
   getline.c

# FIXME: Add -Werror to CFLAGS
AM_CFLAGS   = -Wall -Wno-unused-variable -Wno-unused-result -Wno-uninitialized -D_GNU_SOURCE
AM_CPPFLAGS = $(AM_CFLAGS)
