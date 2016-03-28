/*
 *  limits.h    Machine and OS limits
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
#ifndef _LIMITS_H_INCLUDED
#define _LIMITS_H_INCLUDED


#ifndef MB_CUR_MAX
    #define MB_CUR_MAX  2
#endif
#ifndef MB_LEN_MAX
    #define MB_LEN_MAX  2
#endif

/*
 *  ANSI required limits
 */
#define CHAR_BIT    8           /*  number of bits in a byte        */
#ifdef __CHAR_SIGNED__
 #define CHAR_MIN   (-128)      /*  minimum value of a char         */
 #define CHAR_MAX   127         /*  maximum value of a char         */
#else
 #define CHAR_MIN   0           /*  minimum value of a char         */
 #define CHAR_MAX   255         /*  maximum value of a char         */
#endif
#define SCHAR_MIN   (-128)      /*  minimum value of a signed char      */
#define SCHAR_MAX   127         /*  maximum value of a signed char      */
#define UCHAR_MAX   255U        /*  maximum value of an unsigned char   */

#define SHRT_MIN    (-32767-1)  /*  minimum value of a short int        */
#define SHRT_MAX    32767       /*  maximum value of a short int        */
#define USHRT_MAX   65535U      /*  maximum value of an unsigned short  */
#define LONG_MAX    2147483647L      /* maximum value of a long int      */
#define LONG_MIN    (-2147483647L-1) /* minimum value of a long int      */
#define ULONG_MAX   4294967295UL     /* maximum value of an unsigned long*/
#if defined(__386__) || defined(__AXP__) || defined(__PPC__) || defined(__x86_64__) || defined(__MIPS__)
 #define INT_MIN    (-2147483647-1) /*  minimum value of an int         */
 #define INT_MAX    2147483647      /*  maximum value of an int         */
 #define UINT_MAX   4294967295U     /*  maximum value of an unsigned int*/
#elif defined(__16BIT__)
 #define INT_MIN    (-32767-1)      /*  minimum value of an int         */
 #define INT_MAX    32767           /*  maximum value of an int         */
 #define UINT_MAX   65535U          /*  maximum value of an unsigned int*/
#else
 #error Unsupported platform!
#endif
#ifdef __WATCOM_INT64__
 #define LONGLONG_MIN    (-9223372036854775807I64-1)
                                    /*  minimum value of an __int64         */
 #define LONGLONG_MAX    9223372036854775807I64
                                    /*  maximum value of an __int64         */
 #define ULONGLONG_MAX   18446744073709551615UI64
                                    /*  maximum value of an unsigned __int64*/
 #define LLONG_MIN       (-9223372036854775807LL-1) /* for C99 */
 #define LLONG_MAX       9223372036854775807LL      /* for C99 */
 #define ULLONG_MAX      18446744073709551615ULL    /* for C99 */
#endif

#define _I8_MIN     SCHAR_MIN   /*  minimum value of a signed 8 bit type   */
#define _I8_MAX     SCHAR_MAX   /*  maximum value of a signed 8 bit type   */
#define _UI8_MAX    UCHAR_MAX   /*  maximum value of an unsigned 8 bit type*/

#define _I16_MIN    SHRT_MIN    /*  minimum value of a signed 16 bit type   */
#define _I16_MAX    SHRT_MAX    /*  maximum value of a signed 16 bit type   */
#define _UI16_MAX   USHRT_MAX   /*  maximum value of an unsigned 16 bit type*/

#define _I32_MIN    LONG_MIN    /*  minimum value of a signed 32 bit type   */
#define _I32_MAX    LONG_MAX    /*  maximum value of a signed 32 bit type   */
#define _UI32_MAX   ULONG_MAX   /*  maximum value of an unsigned 32 bit type*/

#ifdef __WATCOM_INT64__
 #define _I64_MIN   LONGLONG_MIN  /* minimum value of a signed 64 bit type   */
 #define _I64_MAX   LONGLONG_MAX  /* maximum value of a signed 64 bit type   */
 #define _UI64_MAX  ULONGLONG_MAX /* maximum value of an unsigned 64 bit type*/
#endif

#define TZNAME_MAX  128             /*  The maximum number of bytes         */
                                    /*  supported for the name of a time    */
                                    /*  zone (not of the TZ variable).      */


#endif
