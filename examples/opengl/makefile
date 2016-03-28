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
# Descripton:   Global makefile to build SciTech MGL OpenGL sample programs.
#
#############################################################################

# defines for where the atlantis programs end up

OGLDOS_DIR  = c:\scitech\demos\ogldos
OGLWIN_DIR  = c:\scitech\demos\oglwin

# Targets to build DOS sample programs

.PHONY mkdos:
	@cdit atlantis dmake cleanexe
	@cdit atlantis build wc11-d32 -u OPT=1 USE_PMODEW=1
	@$(CP) atlantis\atlantis.exe $(OGLDOS_DIR)
	@cdit demos dmake cleanexe
	@cdit demos build wc11-d32 -u OPT=1 USE_PMODEW=1
	@$(CP) demos\*.exe $(OGLDOS_DIR)
	@cdit ideas dmake cleanexe
	@cdit ideas build wc11-d32 -u OPT=1 USE_PMODEW=1
	@$(CP) ideas\ideas.exe $(OGLDOS_DIR)

# Targets to build Windows sample programs

.PHONY mkwin:
	@cdit atlantis dmake cleanexe
	@cdit atlantis build bc50-w32 -u OPT=1
	@$(CP) atlantis\atlantis.exe $(OGLWIN_DIR)
	@cdit demos dmake cleanexe
	@cdit demos build bc50-w32 -u OPT=1
	@$(CP) demos\*.exe $(OGLWIN_DIR)
	@cdit ideas dmake cleanexe
	@cdit ideas build bc50-w32 -u OPT=1
	@$(CP) ideas\ideas.exe $(OGLWIN_DIR)
	@cdit skyfly dmake cleanexe
	@cdit skyfly build bc50-w32 -u OPT=1
	@$(CP) skyfly\skyfly.exe $(OGLWIN_DIR)
