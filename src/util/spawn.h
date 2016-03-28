/****************************************************************************
*
*                   SciTech SNAP Graphics Graphics Architecture
*
*               Copyright (C) 1991-2002 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code contains proprietary technology      |
*  |owned by SciTech Software, Inc., located at 505 Wall Street,        |
*  |Chico, CA 95928 USA (http://www.scitechsoft.com).                   |
*  |                                                                    |
*  |The contents of this file are subject to the SciTech SNAP Graphics  |
*  |License; you may *not* use this file or related software except in  |
*  |compliance with the License. You may obtain a copy of the License   |
*  |at http://www.scitechsoft.com/snap-license.txt                      |
*  |                                                                    |
*  |Software distributed under the License is distributed on an         |
*  |"AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or      |
*  |implied. See the License for the specific language governing        |
*  |rights and limitations under the License.                           |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  32-bit console
*
* Description:  Implementation of the spawn() functions for Linux systems.
*
****************************************************************************/

#ifndef __SPAWN_H
#define __SPAWN_H

/*--------------------- Macros and type definitions -----------------------*/

#define P_WAIT          0
#define P_NOWAIT        1

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {                        /* Use "C" linkage when in C++ mode */
#endif

int spawnvp(int mode,char *path,char *argv[]);
int spawnlp(int mode,char *path, char *arg, ...);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SPAWN_H */

