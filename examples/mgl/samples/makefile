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

EXEFILES        = bitblt$E cliprgn$E ellipses$E lines$E mgldemo$E mouse$E 	\
                  mousedb$E pageflip$E palette$E pixelfmt$E polys$E rects$E     \
                  regions$E sprites$E alpha$E blend$E stretch$E textdemo$E viewport$E \
                  loadbmp$E loadpcx$E loadjpeg$E loadpng$E truetype$E \
                  copypage$E refresh$E flip$E

all: $(EXEFILES)

bitblt$E:   bitblt$O $(MGLSAMP_OBJ)
cliprgn$E:  cliprgn$O $(MGLSAMP_OBJ)
ellipses$E: ellipses$O $(MGLSAMP_OBJ)
lines$E:    lines$O $(MGLSAMP_OBJ)
mgldemo$E:  mgldemo$O $(MGLSAMP_OBJ)
mouse$E:    mouse$O $(MGLSAMP_OBJ)
mousedb$E:  mousedb$O $(MGLSAMP_OBJ)
pageflip$E: pageflip$O $(MGLSAMP_OBJ)
palette$E:  palette$O $(MGLSAMP_OBJ)
pixelfmt$E: pixelfmt$O $(MGLSAMP_OBJ)
polys$E:    polys$O $(MGLSAMP_OBJ)
rects$E:    rects$O $(MGLSAMP_OBJ)
regions$E:  regions$O $(MGLSAMP_OBJ)
sprites$E:  sprites$O $(MGLSAMP_OBJ)
alpha$E:    alpha$O $(MGLSAMP_OBJ)
stretch$E:  stretch$O $(MGLSAMP_OBJ)
textdemo$E: textdemo$O $(MGLSAMP_OBJ)
viewport$E: viewport$O $(MGLSAMP_OBJ)
loadbmp$E:  loadbmp$O $(MGLSAMP_OBJ)
loadpcx$E:  loadpcx$O $(MGLSAMP_OBJ)
loadjpeg$E: loadjpeg$O $(MGLSAMP_OBJ)
loadpng$E:  loadpng$O $(MGLSAMP_OBJ)
truetype$E: truetype$O $(MGLSAMP_OBJ)
copypage$E: copypage$O $(MGLSAMP_OBJ)
refresh$E:  refresh$O $(MGLSAMP_OBJ)
flip$E:     flip$O $(MGLSAMP_OBJ)
blend$E:    blend$O $(MGLSAMP_OBJ)
arcs$E:     arcs$O $(MGLSAMP_OBJ)
test$E:     test$O

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

