#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.2 2004/09/18 00:09:20 austriancoder Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = dxr3

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      = g++
CXXFLAGS = -O2 -Wall -Woverloaded-virtual

### The directory environment:

##DVBDIR = ../../../../DVB
##VDRDIR = ../../..
##LIBDIR = ../../lib
##TMPDIR = /tmp
##FFMDIR = ../../../../ffmpeg
### LIBMP3DIR = /usr/lib

DVBDIR = /usr/src/linux
VDRDIR = /usr/include/vdr
LIBDIR = /usr/lib
TMPDIR = /tmp
FFMDIR = /usr/include/ffmpeg
EM8300 = /usr/include

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include -I$(FFMDIR) -I$(EM8300)
LIBS     = -L$(FFMDIR)/libavcodec -lavcodec -ljpeg
DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'
DEFINES += -DSOCKET_CHMOD=0660
DEFINES += -D_GNU_SOURCE

# where is the microcode for the dxr3 located?
export MICROCODE := /usr/share/misc/em8300.uc

EXTRA_CFLAGS := -DMICROCODE=\"$(MICROCODE)\"

### The object files (add further files here):

OBJS = $(PLUGIN).o dxr3multichannelaudio.o dxr3sysclock.o dxr3colormanager.o dxr3syncbuffer.o dxr3audiodecoder.o \
dxr3blackframe.o dxr3palettemanager.o dxr3nextpts.o dxr3pesframe.o dxr3demuxdevice.o dxr3configdata.o \
dxr3log.o dxr3ffmpeg.o dxr3interface_spu_encoder.o dxr3i18n.o \
dxr3interface.o dxr3device.o dxr3outputthread.o dxr3osd.o dxr3osd_subpicture.o dxr3spudecoder.o dxr3unixserversocket.o \
dxr3cpu.o dxr3memcpy.o dxr3dolbydigital.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -lz -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
