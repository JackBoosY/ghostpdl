#    Copyright (C) 1997, 1998 Aladdin Enterprises.  All rights reserved.
#    Unauthorized use, copying, and/or distribution prohibited.

# pcl_sgi.mak
# Top-level makefile for PCL5* on Silicon Graphics Irix 6.x platforms.

# Directories
GLSRCDIR    = ../gs535
GLGENDIR    = $(GLSRCDIR)
GLOBJDIR    = $(GLSRCDIR)

PLSRCDIR    = ../pl
PLGENDIR    = $(PLSRCDIR)
PLOBJDIR    = $(PLSRCDIR)

PCLSRCDIR   = ../pcl_new
PCLGENDIR   = $(PCLSRCDIR)
PCLOBJDIR   = $(PCLSRCDIR)
GENDIR      = $(PCLSRCDIR)

COMMONDIR   = ../common

# Define the name of this makefile.
MAKEFILE    = $(PCLSRCDIR)/pcl_sgi.mak

# Language and configuration.  These are actually platform-independent,
# but we define them here just to keep all parameters in one place.
CONFIG      = 5
TARGET_DEVS = $(PCLOBJDIR)/pcl5c.dev $(PCLOBJDIR)/hpgl2c.dev
TARGET_XE   = pcl5
MAIN_OBJ    = $(PLOBJDIR)/plmain.$(OBJ) $(PCLOBJDIR)/pcimpl.$(OBJ)
TOP_OBJ    = $(PCLOBJDIR)/pctop.$(OBJ)

# Assorted definitions.  Some of these should probably be factored out....
#
# -woff turns off specific warnings:
#
#     1174 - unused function parameter
#
#     1209 - constant value for control expression
#
#     1506 - loss of precision on implicit rounding
#
CDEFS       = -DSYSV
XCFLAGS     =
SGICFLAGS   = -mips4 -n32 -fullwarn -woff 1506 -woff 1174 -woff 1209
CFLAGS      = -g $(CDEFS) $(SGICFLAGS) $(XCFLAGS)
LDFLAGS     = -n32 -mips4 $(XLDFLAGS)
EXTRALIBS   =
XINCLUDE    = -I/usr/include
XLIBDIRS    =
XLIBDIR     =
XLIBS       = Xt SM ICE X11

CCLD        = cc

DEVICE_DEVS = $(DD)x11mono.dev   \
              $(DD)x11.dev       \
              $(DD)x11alpha.dev  \
              $(DD)x11cmyk.dev   \
              $(DD)ljet4.dev     \
              $(DD)paintjet.dev  \
              $(DD)cljet.dev     \
              $(DD)pbmraw.dev

# Generic makefile
include $(COMMONDIR)/sgi_top.mak

# Subsystems
include $(PLSRCDIR)/pl.mak
include $(PCLSRCDIR)/pcl.mak

# Main program.
include $(PCLSRCDIR)/pcl_top.mak
