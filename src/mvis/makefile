#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2.0 as published by the Free
#   Software Foundation and appearing in the file LICENSE.GPL included
#   in the packaging of this file.
#
#   Licensees holding a valid Commercial License for this product from
#   SciTech Software, Inc. may use this file in accordance with the
#   Commercial License Agreement provided with the Software.
#
#   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
#   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   See http://www.scitechsoft.com/license/ for information about
#   the licensing options available and how to purchase a Commercial
#   License Agreement.
#
#   Contact license@scitechsoft.com if any conditions of this licensing
#   are not clear to you, or you have questions about licensing options.
#
#  ========================================================================
#
# Descripton:   Generic makefile for the MegaVision Application Frameworks.
#               Requires the SciTech Software makefile definitions package
#               to be installed.
#
#               Specifying CHECKS on the command line will enable a
#               debugging version of the MVis that does extra runtime
#               checking.
#
#############################################################################

# Comment the following if you dont use pre-compiled headers

.IF $(NO_PCH)
.ELSE
CFLAGS          += -DPC_HEADERS
.ENDIF

# Include definitions for the target system

.IMPORT .IGNORE : MGLDOS MGLWIN MGLPM MGLX

.IF $(USE_WIN32)
CFLAGS          += -DMGLWIN
.ENDIF

# Name of library

.IF $(STKCALL)
LIBFILE         = $(LP)smvis$L
.ELSE
LIBFILE         = $(LP)mvis$L
.ENDIF

LIBCLEAN        = *.dll *.lib *.a

# All the object modules in the library

OBJECTS =   mvision$O                                                   \
            hotkey$O mbackgnd$O mbitbtn$O mbutton$O mcheckbx$O          \
            mdesktop$O mdialog$O message$O mevent$O mfiledlg$O          \
            mfontmgr$O mframe$O mgroup$O minputl$O misc$O mlabel$O      \
            mlist$O mmenu$O mmenubar$O mmenuite$O mmltext$O mmodesel$O  \
            mprogram$O mradio$O mrendera$O mscrollb$O msgbox$O          \
            mstatic$O msttext$O mtbbtn$O mtitleb$O mview$O mvtext$O     \
            mwindow$O safetyp$O validvw$O viewstk$O

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

