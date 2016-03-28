/****************************************************************************
*
*                     SciTech SNAP Audio Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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
* Environment:  Any 32-bit protected mode environment
*
* Description:  WAV file manipulation module.
*
****************************************************************************/

#ifndef AAWAVE_H
#define AAWAVE_H

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

typedef struct {
    void        *buffer;
    N_uint32    length;
    N_uint32    rate;
    N_uint16    format;
    N_uint16    channel;
    N_uint16    bit;
    } AA_waveFile;

/*------------------------- Function Prototypes ---------------------------*/

AA_waveFile     *WAV_load(const char *filename);
void            WAV_unload(AA_waveFile *wave);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif

#endif  /* AAWAVE_H */
