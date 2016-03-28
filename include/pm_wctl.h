/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Language:     ANSI C
* Environment:  Win32, OS/2
*
* Description:  Header file to define all the control codes for the DOS
*               and Win32 device driver API's for calling from ring 3
*               into the ring 0 device drivers.
*
****************************************************************************/

/* Version function used by all drivers */
PMHELP_CTL_CODE(GETVER                      ,0x0000),

/* Functions used by obsolete 16-bit DOS TSR */
PMHELP_CTL_CODE(RDREGB                      ,0x0003),
PMHELP_CTL_CODE(WRREGB                      ,0x0004),
PMHELP_CTL_CODE(RDREGW                      ,0x0005),
PMHELP_CTL_CODE(WRREGW                      ,0x0006),
PMHELP_CTL_CODE(RDREGL                      ,0x0008),
PMHELP_CTL_CODE(WRREGL                      ,0x0009),

/* Functions used by obsolete WinDirect */
PMHELP_CTL_CODE(MAPPHYS                     ,0x000F),
PMHELP_CTL_CODE(GETVESABUF                  ,0x0013),

/* Functions used by PM library */
PMHELP_CTL_CODE(INT86                       ,0x0015),
PMHELP_CTL_CODE(INT86X                      ,0x0016),
PMHELP_CTL_CODE(CALLREALMODE                ,0x0017),
PMHELP_CTL_CODE(ALLOCLOCKED                 ,0x0018),
PMHELP_CTL_CODE(FREELOCKED                  ,0x0019),
PMHELP_CTL_CODE(ENABLELFBCOMB               ,0x001A),
PMHELP_CTL_CODE(GETPHYSICALADDR             ,0x001B),
PMHELP_CTL_CODE(MALLOCSHARED                ,0x001D),
PMHELP_CTL_CODE(FREESHARED                  ,0x001F),
PMHELP_CTL_CODE(LOCKDATAPAGES               ,0x0020),
PMHELP_CTL_CODE(UNLOCKDATAPAGES             ,0x0021),
PMHELP_CTL_CODE(LOCKCODEPAGES               ,0x0022),
PMHELP_CTL_CODE(UNLOCKCODEPAGES             ,0x0023),
PMHELP_CTL_CODE(GETCALLGATE                 ,0x0024),
PMHELP_CTL_CODE(SETCNTPATH                  ,0x0025),
PMHELP_CTL_CODE(GETPDB                      ,0x0026),
PMHELP_CTL_CODE(FLUSHTLB                    ,0x0027),
PMHELP_CTL_CODE(GETPHYSICALADDRRANGE        ,0x0028),
PMHELP_CTL_CODE(ALLOCPAGE                   ,0x0029),
PMHELP_CTL_CODE(FREEPAGE                    ,0x002A),
PMHELP_CTL_CODE(ENABLERING3IOPL             ,0x002B),
PMHELP_CTL_CODE(DISABLERING3IOPL            ,0x002C),
PMHELP_CTL_CODE(GASETLOCALPATH              ,0x002D),
PMHELP_CTL_CODE(GAGETEXPORTS                ,0x002E),
PMHELP_CTL_CODE(GATHUNK                     ,0x002F),
PMHELP_CTL_CODE(SETSNAPPATH                 ,0x0030),
PMHELP_CTL_CODE(GALOADDRIVER                ,0x0031),
PMHELP_CTL_CODE(GAUNLOADDRIVER              ,0x0032),
PMHELP_CTL_CODE(ENABLEGLOBALMAPPING         ,0x0033),
PMHELP_CTL_CODE(GATHUNKPTRALIAS             ,0x0034),
PMHELP_CTL_CODE(GATHUNKMODEINFOALIAS        ,0x0035),
PMHELP_CTL_CODE(GATHUNKCERTIFYINFOALIAS     ,0x0036),
PMHELP_CTL_CODE(ISSDDACTIVE                 ,0x0037),
PMHELP_CTL_CODE(RUNNINGINAWINDOW            ,0x0038),
PMHELP_CTL_CODE(GAGETSHAREDINFO             ,0x0039),
PMHELP_CTL_CODE(ENABLEFILEFILTER            ,0x003A),
PMHELP_CTL_CODE(DISABLEFILEFILTER           ,0x003B),
PMHELP_CTL_CODE(REBUILDSDDMODELIST          ,0x003C),
PMHELP_CTL_CODE(READMSR                     ,0x003D),
PMHELP_CTL_CODE(WRITEMSR                    ,0x003E),
PMHELP_CTL_CODE(PCIBIOSDETECT               ,0x003F),
PMHELP_CTL_CODE(PCIBIOSSERVICE              ,0x0040),

