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
# Descripton:   Generic makefile for building the SciTech SNAP Binary
#               Portable Driver C++ complex math library. This library is
#               based on the Open Watcom complex math library.
#
#############################################################################

#----------------------------------------------------------------------------
# Target dependent configuration stuff
#----------------------------------------------------------------------------

STOP_ON_WARN    := 1
NO_DEFAULT_LIBS := 1
UNSIGNED_CHAR   := 1
USE_RTTI        := 1
CFLAGS          += -D__SNAPDRV__ -D__OBSCURE_STREAM_INTERNALS -DNDEBUG
# -Iiostream\h -Iruntime\h -Istring\h -Ih -I..\h

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

COMPLEX_OBJS    = abs$O acos$O acosh$O arg$O asin$O asinh$O atan$O atanh$O \
                  cos$O cosh$O dicomcom$O direlcom$O divide$O exp$O log$O \
                  log10$O multiply$O opinput$O opoutput$O polar$O powcbcp$O \
                  powcbdp$O powccbip$O powdbcp$O sin$O sinh$O sqrt$O tan$O \
                  tanh.obj

#----------------------------------------------------------------------------
# Name of library and object files required to build it
#----------------------------------------------------------------------------

OBJECTS         = $(COMPLEX_OBJS)
LIBFILE         = $(LP)complex$L
LIBCLEAN        = *.lib *.a

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
#DEPEND_INC      = iostream/h;runtime/h;string/h;../h;h
#DEPEND_SRC      = contain/cpp;iostream/cpp;runtime/a;runtime/cpp;string/cpp

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

