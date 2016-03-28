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
# Descripton:   Makefile for the OpenGL demos.
#
#############################################################################

.INCLUDE: "../common.mak"

# Names of all executable files built

EXEFILES        = accanti$E anti$E bezmesh$E checker$E depthcue$E       \
                  dof$E fog$E fogindex$E material$E mipmap$E nurbs$E    \
                  pickdp$E plane$E polys$E scclight$E scene$E sceneba$E \
                  scenef$E stencil$E stroke$E surface$E teaamb$E        \
                  texture$E
EXELIBS         = $(BASELIBS)

.IF $(WIN32_GUI)
RCFILE          = demo$R
.ELSE
RCFILE          =
.ENDIF

all: $(EXEFILES)

accanti$E: accanti$O $(RCFILE)
anti$E: anti$O $(RCFILE)
bezmesh$E: bezmesh$O $(RCFILE)
checker$E: checker$O $(RCFILE)
depthcue$E: depthcue$O $(RCFILE)
dof$E: dof$O $(RCFILE)
fog$E: fog$O $(RCFILE)
fogindex$E: fogindex$O $(RCFILE)
material$E: material$O $(RCFILE)
mipmap$E: mipmap$O $(RCFILE)
nurbs$E: nurbs$O $(RCFILE)
pickdp$E: pickdp$O $(RCFILE)
plane$E: plane$O $(RCFILE)
polys$E: polys$O $(RCFILE)
scclight$E: scclight$O $(RCFILE)
scene$E: scene$O $(RCFILE)
sceneba$E: sceneba$O $(RCFILE)
scenef$E: scenef$O $(RCFILE)
stencil$E: stencil$O $(RCFILE)
stroke$E: stroke$O $(RCFILE)
surface$E: surface$O $(RCFILE)
teaamb$E: teaamb$O $(RCFILE)
teapots$E: teapot$O $(RCFILE)
texture$E: texture$O $(RCFILE)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

