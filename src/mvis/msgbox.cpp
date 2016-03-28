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
* Description:  Implementation of the messageBox routine.
*
****************************************************************************/

#include "mvis/mvision.hpp"
#include "mvis/mprogram.hpp"
#include "mvis/msgbox.hpp"
#include "mvis/mdialog.hpp"
#include "mvis/mbutton.hpp"
#include "mvis/msttext.hpp"
#include "mvis/mstatic.hpp"
#include "mvis/mfontmgr.hpp"
#include "mvis/minputl.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


/*----------------------------- Implementation ----------------------------*/

static const char *buttonNames[] = {
    MVMsgBoxText::yesText,
    MVMsgBoxText::noText,
    MVMsgBoxText::okText,
    MVMsgBoxText::cancelText,
    };

static ulong commands[] = {
    cmYes,
    cmNo,
    cmOk,
    cmCancel
    };

static const char *titles[] = {
    "",
    MVMsgBoxText::warningText,
    MVMsgBoxText::errorText,
    MVMsgBoxText::informationText,
    MVMsgBoxText::confirmText,
    "","",""
    };

static MGLIcon **icons[] = {
    NULL,
    &MV_ICON_WARNING,
    &MV_ICON_ERROR,
    &MV_ICON_INFORMATION,
    &MV_ICON_CONFIRM,
    &MV_ICON_CONFIRM,       // Filler
    &MV_ICON_CONFIRM,       // Filler
    &MV_ICON_CONFIRM,       // Filler
    };

uint MV_messageBoxEx(MGLDevCtx dc,const char *msg,const char *title,
    uint options)
/****************************************************************************
*
* Function:     MV_messageBoxEx
* Parameters:   msg     - Message text to display in the box
*               title   - Tile for the window
*               options - Options flags defining the type of box to display
* Returns:      Command code representing the choice made.
*
* Description:  Pops up a standard message box, displays the message and
*               waits for the user to respond. Returns one of the
*               following codes:
*
*                   cmOK        - The OK button was hit
*                   cmCancel    - The Cancel button was hit
*                   cmYes       - The Yes button was hit
*                   cmNo        - The No button was hit
*
****************************************************************************/
{
    dc.useFont(MV_fontManager.useFont(fmSystemFont));
    int haveIcon = (options & 0x7) != mfNothing;
    int height = dc.textHeight();
    int buttonHeight = height + 12;
    int dlgwidth = (dc.sizex() / 2);
    int iconDiff = (MV_ICON_SIZE + _MV_iconOffset*2);
    int msgwidth = dlgwidth - (haveIcon ? iconDiff : 0);
    int maxwidth;
    char *m = MV_newStr(msg);

    // Compute the number of lines to display and compute the size of
    // the message box that will hold the text.
    int lines = MV_countLines(dc,msg,msgwidth,maxwidth,true);
    msgwidth = maxwidth;
    dlgwidth = msgwidth + (haveIcon ? iconDiff : 0);
    MVRect bounds(0,0,dlgwidth + 30,100);

    uint flags = wfNormalDialog & ~wfSystemMenu;
    if (!(options & mfCancelButton))
        flags &= ~wfClose;
    MVDialog *dialog = new MVDialog(dc,bounds,title,flags);
    dialog->setOption(ofCentered,true);

    // Now adjust the size of the dialog box to fit the text that will
    // be displayed in it, and compute the bounding box for the text.
    MVRect inner(dialog->getClientRect());
    int adjustTop = inner.top() - bounds.top();
    int adjustBottom = bounds.bottom() - inner.bottom();

    // Create the icon and insert it
    if (haveIcon) {
        MVStaticIcon *icon = new MVStaticIcon(dc,
            MVPoint(inner.left() + _MV_iconOffset,inner.top() + _MV_iconOffset),
            **icons[options & 0x7]);
        dialog->insert(icon);
        }

    // Create the text item and insert it
    inner.bottom() = inner.top() + MAX(40,height * lines);
    bounds.bottom() = inner.bottom() + adjustBottom + buttonHeight + 20;
    dialog->setBounds(bounds);
    if (haveIcon) {
        inner.left() += iconDiff;
        inner.right() = inner.left() + msgwidth;
        inner.moveTo(inner.left(),adjustTop+5);
        }
    else {
        inner.moveTo(0,adjustTop+5);
        inner.right() = msgwidth;
        }

    flags = sfWrap | ((options & mfCenterText) ? 0 : sfLeftJust);
    MVMultiLineText *text = new MVMultiLineText(dc,inner,m,flags);
    if (!haveIcon)
        text->setOption(ofCenterX,true);
    dialog->insert(text);

    // Create the buttons to be inserted into the box, and insert them.
    // Note that the Yes or Ok button is made the default.
    uint        mask = 0x0080;
    int         i,count,width = dc.textWidth(MVMsgBoxText::cancelText) + 16;
    MVButton    *buttons[4];

    for (i = 3,count = 0; i >= 0; i--,mask >>= 1) {
        if (options & mask) {
            buttons[count] = new MVButton(dc,MVRect(0,0,width,buttonHeight),
                buttonNames[i],commands[i],bfNormal);
            dialog->insert(buttons[count]);
            if (options & (mask << 4)) {
                buttons[count]->setDefault(true);
                dialog->select(buttons[count]);
                }
            count++;
            }
        }

    // Determine where to place each of the buttons that have been created,
    // and move them to the correct locations.
    int     x,sizex = (bounds.right()-bounds.left()-1+width) / (count+1);

    for (i = count-1, x = sizex - width/2; i >= 0; i--) {
        buttons[i]->moveTo(x - width/2,bounds.bottom() - buttonHeight - 10);
        x += sizex;
        }

    uint retVal = MVProgram::deskTop->execView(dialog);
    delete dialog;
    delete [] m;
    return retVal;
}

uint MV_messageBoxEx(MGLDevCtx dc,uint options,const char *title,
    const char *fmt, ... )
/****************************************************************************
*
* Function:     MV_messageBoxEx
* Parameters:   options - Options flags defining the type of box to display
*               fmt     - Format string for message text to display
*               ...     - Optional printf() style arguments
* Returns:      Command code representing the choice made.
*
* Description:  Same as above, but performs formatting on the string
*               first. The maximum formatted message length is 255
*               characters.
*
****************************************************************************/
{
    char        msg[256];
    va_list     argptr;

    va_start(argptr,fmt);
    vsprintf(msg,fmt,argptr);
    return MV_messageBoxEx(dc,msg,title,options);
}

uint MV_messageBox(MGLDevCtx dc,const char *msg,uint options)
{ return MV_messageBoxEx(dc,msg,titles[options & 0x07],options); }

uint MV_messageBox(MGLDevCtx dc,uint options,const char *fmt, ... )
{
    char        msg[256];
    va_list     argptr;

    va_start(argptr,fmt);
    vsprintf(msg,fmt,argptr);
    return MV_messageBox(dc,msg,options);
}

uint MV_inputBox(MGLDevCtx dc,const char *title,const char *label,
    char *str,int limit)
/****************************************************************************
*
* Function:     MV_inputBox
* Parameters:   title   - Title for the input box
*               label   - Label above the input line
*               str     - Place to store the input data
*               limit   - Maximum number of characters to input
* Returns:      Command that caused the box to be closed.
*
* Description:  Pops up an input box for input, using the data already
*               in 'str' as the default text. Either cmOK or cmCancel will
*               be returned depending on which button was pressed.
*
****************************************************************************/
{
    MV_fontManager.useFont(fmSystemFont);
    int height = dc.textHeight();
    int buttonHeight = height + 12;
    int msgwidth = dc.sizex()/2;

    // Compute the number of lines to display and compute the size of
    // the message box that will hold the text.

    MVRect bounds(0,0,msgwidth+30,100);

    MVDialog *dialog = new MVDialog(dc,bounds,title);
    dialog->setOption(ofCentered,true);

    // Now adjust the size of the dialog box to fit the text that will
    // be displayed in it, and compute the bounding box for the text.

    MVRect inner(dialog->getClientRect());
    int adjustBottom = bounds.bottom() - inner.bottom();

    // Create and insert the label

    inner.top() += height;
    inner.bottom() = inner.top() + height;
    inner.left() = 15;
    inner.right() = 15 + dc.textWidth(label);
    MVLabel *l = new MVLabel(dc,inner,label,NULL);
    dialog->insert(l);

    // Create and insert the input line

    inner.top() = inner.bottom();
    inner.bottom() = inner.top() + height;
    inner.right() = msgwidth+15;
    MVInputLine *input = new MVInputLine(dc,inner,limit,str[0] == '\0' ? (const char*)NULL : str);
    dialog->insert(input);
    dialog->select(input);
    l->setAttached(input);

    // Adjust the bounds of the box to fit, including the buttons

    bounds.bottom() = inner.bottom() + adjustBottom + buttonHeight + 20;
    dialog->setBounds(bounds);

    // Create the buttons to be inserted into the box, and insert them.

    int width = dc.textWidth(MVMsgBoxText::cancelText) + 16;
    int sizex = (bounds.right()-bounds.left()-1+width) / 3;
    inner.top() = bounds.bottom() - buttonHeight - 10;
    inner.bottom() = inner.top() + buttonHeight;
    inner.left() = sizex - width;
    inner.right() =  inner.left() + width;
    MVButton *okBut = new MVButton(dc,inner,MVMsgBoxText::okText,
        cmOk,bfDefault);

    inner.left() += sizex;
    inner.right() =  inner.left() + width;
    MVButton *cancelBut = new MVButton(dc,inner,MVMsgBoxText::cancelText,
        cmCancel,bfNormal);

    dialog->insert(cancelBut);
    dialog->insert(okBut);

    uint retVal = MVProgram::deskTop->execView(dialog);
    strcpy(str,input->getText());
    delete dialog;
    return retVal;
}
