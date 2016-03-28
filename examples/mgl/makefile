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
# Descripton:   Global makefile to build SciTech MGL sample programs.
#
#############################################################################

# defines for where the demo programs end up

MGLDOS_DIR  = c:\scitech\demos\mgldos
MGLWIN_DIR  = c:\scitech\demos\mglwin

# Targets to build DOS sample programs

.PHONY mkdos:
	@cdit demo dmake cleanexe
	@cdit demo build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) demo\demo.exe $(MGLDOS_DIR)
	@cdit demo3d dmake cleanexe
	@cdit demo3d build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) demo3d\demo3d.exe $(MGLDOS_DIR)
	@cdit foxbear dmake cleanexe
	@cdit foxbear build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) foxbear\foxbear.exe $(MGLDOS_DIR)
	@cdit smacker dmake cleanexe
	@cdit smacker build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) smacker\play.exe $(MGLDOS_DIR)
	@cdit mgldog dmake cleanexe
	@cdit mgldog build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) mgldog\mgldog.exe $(MGLDOS_DIR)
	@cdit showbmp dmake cleanexe
	@cdit showbmp build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) showbmp\showbmp.exe $(MGLDOS_DIR)
	@cdit samples dmake cleanexe
	@cdit samples build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) samples\*.exe $(MGLDOS_DIR)
	@cdit gm dmake cleanexe
	@cdit gm build wc11-d32 -u OPT=1 USE_CAUSEWAY=1
	@$(CP) gm\*.exe $(MGLDOS_DIR)

# Targets to build Windows sample programs

.PHONY mkwin:
	@cdit demo dmake cleanexe
	@cdit demo build bc50-w32 -u OPT=1
	@$(CP) demo\demo.exe $(MGLWIN_DIR)
	@cdit demo3d dmake cleanexe
	@cdit demo3d build bc50-w32 -u OPT=1
	@$(CP) demo3d\demo3d.exe $(MGLWIN_DIR)
	@cdit foxbear dmake cleanexe
	@cdit foxbear build bc50-w32 -u OPT=1
	@$(CP) foxbear\foxbear.exe $(MGLWIN_DIR)
	@cdit smacker dmake cleanexe
	@cdit smacker build bc50-w32 -u OPT=1
	@$(CP) smacker\play.exe $(MGLWIN_DIR)
	@cdit mgldog dmake cleanexe
	@cdit mgldog build bc50-w32 -u OPT=1
	@$(CP) mgldog\mgldog.exe $(MGLWIN_DIR)
	@cdit showbmp dmake cleanexe
	@cdit showbmp build bc50-w32 -u OPT=1
	@$(CP) showbmp\showbmp.exe $(MGLWIN_DIR)
	@cdit wmgldog dmake cleanexe
	@cdit wmgldog build bc50-w32 -u OPT=1
	@$(CP) wmgldog\wmgldog.exe $(MGLWIN_DIR)
	@cdit wshowbmp dmake cleanexe
	@cdit wshowbmp build bc50-w32 -u OPT=1
	@$(CP) wshowbmp\wshowbmp.exe $(MGLWIN_DIR)
	@cdit samples dmake cleanexe
	@cdit samples build bc50-w32 -u OPT=1
	@$(CP) samples\*.exe $(MGLWIN_DIR)
	@cdit gm dmake cleanexe
	@cdit gm build bc50-w32 -u OPT=1
	@$(CP) gm\*.exe $(MGLWIN_DIR)
