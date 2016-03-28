/****************************************************************************
*
*                    MegaVision Application Framework
*
*      A C++ GUI Toolkit for the SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     C++ 3.0
* Environment:  Any
*
* Description:  Member functions for the MVMenuItem class.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mmenu.hpp"

/*----------------------------- Implementation ----------------------------*/

MVMenuItem::MVMenuItem(const char *name,ulong command,
    const MVHotKey& hotKey,uint helpCtx,const char *hotKeyText)
    : command(command), hotKey(hotKey), helpCtx(helpCtx)
/****************************************************************************
*
* Function:     MVMenuItem::MVMenuItem
* Parameters:   name        - Name to display for the menu item
*               command     - Command to post when activated
*               hotKey      - Altenate hot key value for menu item
*               helpCtx     - Help context number for the item
*               hotKeyText  - Text to display for HotKey value
*
* Description:  Constructor for a MVMenuItem.
*
****************************************************************************/
{
    this->name = MV_newHotStr(name,hotChar,hotIndex);
    this->hotKeyText = MV_newStr(hotKeyText);
    disabled = false;       // Needs to be set on globally disabled commands!
}

MVMenuItem::MVMenuItem(const char *name,MVMenu *subMenu,uint helpCtx)
    : hotKey(kbNoKey,0), helpCtx(helpCtx), subMenu(subMenu)
/****************************************************************************
*
* Function:     MVMenuItem::MVMenuItem
* Parameters:   name    - Name to display for menu item
*               subMenu - Pointer to the submenu definition for the menu
*               helpCtx - Help context number for the item
*
* Description:  Constructor for a MVMenuItem. Create a submenu definition.
*
****************************************************************************/
{
    this->name = MV_newHotStr(name,hotChar,hotIndex);
    command = 0;
    disabled = false;
}

MVMenuItem::MVMenuItem(const MVMenuItem& c)
  :  command(c.command), disabled(c.disabled), hotKey(c.hotKey), helpCtx(c.helpCtx),
     hot1(c.hot1), hot2(c.hot2), bounds(c.bounds)
{
    name = MV_newStr(c.name);
    if (c.isSubMenu())
        subMenu = c.subMenu;
    else
        hotKeyText = MV_newStr(c.hotKeyText);
}

MVMenuItem::~MVMenuItem()
/****************************************************************************
*
* Function:     MVMenuItem::~MVMenuItem
*
* Description:  Destructor for the MVMenuItem class.
*
****************************************************************************/
{
    delete [] name;
    if (isSubMenu())
        delete subMenu;
    else
        delete [] hotKeyText;
}
