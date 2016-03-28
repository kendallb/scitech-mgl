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
# Descripton:   Makefile for command line utilities for SciTech makefiles.
#
#############################################################################

.IF $(UNIX_HOST)
all: loc$E makedep$E trimlib$E rclink$E k_echo$E k_rm$E k_cp$E k_md$E \
     k_cc$E k_tee$E
SPAWN   = spawn$O
.ELSE
all: dj_ld$E expndinc$E loc$E makedef$E makedep$E makedepx$E \
     trimlib$E k_echo$E k_rm$E k_cp$E rclink$E bclink$E k_md$E k_cc$E \
     comstrip$E k_tee$E
.ENDIF

dj_ld$E:    dj_ld$O
expndinc$E: expndinc$O
loc$E:      loc$O
makedef$E:  makedef$O
makedep$E:  makedep$O cmdline$O
makedepx$E: makedepx$O cmdline$O
rclink$E:   rclink$O $(SPAWN)
bclink$E:   bclink$O
trimlib$E:  trimlib$O
tolower$E:  tolower$O cmdline$O
k_echo$E:   k_echo$O
k_rm$E:     k_rm$O
k_cp$E:     k_cp$O
k_md$E:     k_md$O
k_tee$E:    k_tee$O
k_cc$E:     k_cc$O cmdline$O
p4_cvs$E:   p4_cvs$O
p4_upd$E:   p4_upd$O
cvs_date$E: cvs_date$O
bin2c$E:    bin2c$O
ntset$E:    ntset$O
comstrip$E: comstrip$O
rtftable$E: rtftable$O

DEPEND_OBJ = dj_ld$O expndinc$O loc$O makedef$O makedep$O cmdline$O \
             makedep$O rclink$O bclink$O trimlib$O tolower$O \
             k_echo$O k_rm$O k_cp$O k_md$O

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
