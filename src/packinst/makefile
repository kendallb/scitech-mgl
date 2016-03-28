#############################################################################
#
#                   Copyright (C) 1995 SciTech Software
#                           All rights reserved.
#
# Descripton:   Makefile for the SDD/2 installer components.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

CFLAGS          += -DSDDINST -I$(SCITECH)\include\packinst -I$(SCITECH)\include\unzip
PACKINST_OBJ    = packinst$O unzip$O ioapi$O

#----------------------------------------------------------------------------
# Define the targets to build the test programs and analysis tools
#----------------------------------------------------------------------------

all: packinst$E

packinst$E: $(PACKINST_OBJ)

EXELIBS        += $(LL)zlib$(LE)

# Define the list of object files to create dependency information for

DEPEND_SRC      := ../zlib/unzip
DEPEND_OBJ      = $(PACKINST_OBJ)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
