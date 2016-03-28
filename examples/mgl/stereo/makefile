############################################################################
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
# Descripton:   Makefile for sample programs. Requires the SciTech
#               Software makefile definitions package to be installed.
#
#############################################################################

.INCLUDE: "../common.mak"

MGLSAMP_OBJ     = mglsamp$O cmdline$O
.IF $(WIN32_GUI)
MGLSAMP_OBJ     += mglsamp$R
.ENDIF
EXELIBS         = $(BASELIBS)

EXEFILES        = sterbmp$E stertest$E sterdemo$E

all: $(EXEFILES)

sterbmp$E:  sterbmp$O
stertest$E: stertest$O
sterdemo$E: sterdemo$O $(MGLSAMP_OBJ)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"


