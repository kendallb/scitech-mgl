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
# Descripton:   Generic makefile for the Techniques Class Library. Requires
#               the SciTech Software makefile definitions package to be
#               installed.
#
#############################################################################

# Name of library and object files required to build it

.IF $(STKCALL)
LIBFILE         = $(LP)stech$L
.ELSE
LIBFILE         = $(LP)tech$L
.ENDIF

LIBCLEAN        = *.lib *.a

OBJECTS         = dlist$O list$O hashtab$O error$O str$O          \
                  dynstr$O substr$O memmgr$O hash_c$O slist$O

# Sample test programs

hashtst$E: hashtst$O $(LIBFILE)
tststr$E: tststr$O $(LIBFILE)

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

