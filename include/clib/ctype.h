/*
 *  ctype.h     Character Handling
 *
 *                          Open Watcom Project
 *
 *    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
 *
 *  ========================================================================
 *
 *    This file contains Original Code and/or Modifications of Original
 *    Code as defined in and that are subject to the Sybase Open Watcom
 *    Public License version 1.0 (the 'License'). You may not use this file
 *    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
 *    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
 *    provided with the Original Code and Modifications, and is also
 *    available at www.sybase.com/developer/opensource.
 *
 *    The Original Code and all software distributed under the License are
 *    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 *    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
 *    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
 *    NON-INFRINGEMENT. Please see the License for the specific language
 *    governing rights and limitations under the License.
 *
 *  ========================================================================
 */
#ifndef _CTYPE_H_INCLUDED
#define _CTYPE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED_
#ifdef __cplusplus
#ifndef __GNUC__
typedef long char wchar_t;
#endif
#else
typedef unsigned short wchar_t;
#endif
#endif

#ifndef _WCTYPE_T_DEFINED
#define _WCTYPE_T_DEFINED
#define _WCTYPE_T_DEFINED_
typedef wchar_t wint_t;
typedef wchar_t wctype_t;
#endif

#define _LOWER  0x80
#define _UPPER  0x40
#define _DIGIT  0x20
#define _XDIGT  0x10
#define _PRINT  0x08
#define _PUNCT  0x04
#define _SPACE  0x02
#define _CNTRL  0x01

_WCRTLINK extern int    isalnum(int);
_WCRTLINK extern int    isalpha(int);
_WCRTLINK extern int    iscntrl(int);
_WCRTLINK extern int    isdigit(int);
_WCRTLINK extern int    isgraph(int);
_WCRTLINK extern int    islower(int);
_WCRTLINK extern int    isprint(int);
_WCRTLINK extern int    ispunct(int);
_WCRTLINK extern int    isspace(int);
_WCRTLINK extern int    isupper(int);
_WCRTLINK extern int    isxdigit(int);
_WCRTLINK extern int    tolower(int);
_WCRTLINK extern int    toupper(int);

#if !defined(NO_EXT_KEYS) /* extensions enabled */
 _WCRTLINK extern int   isascii(int);
 _WCRTLINK extern int   __isascii(int);
 #if !defined(__FUNCTION_DATA_ACCESS)
  #define isascii(__c) ((unsigned)(__c) <= 0x7f)
  #define __isascii(__c) ((unsigned)(__c) <= 0x7f)
 #endif
#endif

#if (defined(__SW_BR) || defined(_RTDLL))
 #define _IsTable _IsTable_br
#endif
_WCRTLINK extern const char _WCDATA _IsTable[257];

#if !defined(__FUNCTION_DATA_ACCESS)
#define isalnum(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & (_LOWER|_UPPER|_DIGIT))
#define isalpha(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & (_LOWER|_UPPER))
#define iscntrl(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _CNTRL)
#define isdigit(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _DIGIT)
#define isgraph(__c) (((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & (_PRINT|_SPACE))==_PRINT)
#define islower(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _LOWER)
#define isprint(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _PRINT)
#define ispunct(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _PUNCT)
#define isspace(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _SPACE)
#define isupper(__c)  ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _UPPER)
#define isxdigit(__c) ((unsigned char)(_IsTable[((unsigned char)(__c))+1]) & _XDIGT)
#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
