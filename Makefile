#
# Makefile for a Video Disk Recorder plugin
#
# $Id: Makefile,v 1.1.2.28 2008/12/29 21:14:18 scop Exp $

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = dxr3

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'static const char VERSION\[\] *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      = g++
CXXFLAGS = -O2 -fPIC -Wall -Wextra -Woverloaded-virtual

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

# Usually something like -I/path/to/ffmpeg, should work as is if FFmpeg was
# installed properly and pkg-config is available.
FFMPEG_INC = $(shell pkg-config --cflags-only-I libavcodec)

# Usually something like -L/path/to/ffmpeg/libavcodec -lavcodec, should work
# as is if FFmpeg was installed properly and pkg-config is available.
FFMPEG_LIBS = $(shell pkg-config --libs libavcodec)

ALSA_INC = $(shell pkg-config --cflags-only-I alsa)
ALSA_LIBS = $(shell pkg-config --libs alsa)

# Usually something like -I/path/to/em8300/include,should work as is (empty)
# if em8300 headers were installed properly.
EM8300_INC = 

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = $(shell echo vdr-$(ARCHIVE) | sed -e 's/git$$/git'`date +%Y%m%d`/)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include $(FFMPEG_INC) $(EM8300_INC) $(ALSA_INC)
LIBS      = $(FFMPEG_LIBS) $(ALSA_LIBS)
DEFINES  += -DPLUGIN_NAME_I18N='"$(PLUGIN)"'
DEFINES  += -D_GNU_SOURCE

# where is the microcode for the dxr3 located?  em8300 driver version 0.15.2
# and later installs it by default to /lib/firmware/em8300.bin, and earlier
# versions to /usr/share/misc/em8300.uc.
DEFINES += -DMICROCODE=\"/lib/firmware/em8300.bin\"

### The object files (add further files here):

OBJS = $(PLUGIN).o dxr3multichannelaudio.o \
	dxr3syncbuffer.o dxr3audiodecoder.o dxr3blackframe.o dxr3audio.o \
	dxr3pesframe.o dxr3demuxdevice.o dxr3configdata.o \
	dxr3interface.o dxr3device.o \
	dxr3output.o dxr3output-video.o dxr3output-audio.o dxr3osd.o dxr3spudecoder.o \
        dxr3audio-oss.o dxr3audio-alsa.o spuencoder.o spuregion.o scaler.o

### Default target:

all: libvdr-$(PLUGIN).so i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmsgs  = $(addprefix $(LOCALEDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.h *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='dxr3plugin-users@lists.sourceforge.net' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

.PHONY: i18n
i18n: $(I18Nmsgs)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) \
	   --owner=root --group=root --exclude .gitignore $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so* *.tgz core* *~ \#*

#indent:
#	emacs -batch --eval '(dolist (file command-line-args-left) (progn (find-file file) (c-indent-region (point-min) (point-max)) (save-buffer)))' *.h *.c
