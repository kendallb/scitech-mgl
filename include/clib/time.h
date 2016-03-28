/*
 *  time.h      Time functions
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
#ifndef _TIME_H_INCLUDED
#define _TIME_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif

#if defined(_M_IX86)
  #pragma pack(1)
#else
  #pragma pack(8)
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED_
#ifdef __cplusplus
typedef long char wchar_t;
#else
typedef unsigned short wchar_t;
#endif
#endif

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
#define _SIZE_T_DEFINED_
 #if defined(__386__) || defined(__PPC__)
  typedef unsigned size_t;
 #else
  typedef unsigned long size_t;
 #endif
#endif

#ifndef NULL
 #if defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__) || defined(__AXP__) || defined(__PPC__)
  #define NULL   0
 #else
  #define NULL   0L
 #endif
#endif

#ifndef _TIME_T_DEFINED
#define _TIME_T_DEFINED
#define _TIME_T_DEFINED_
typedef unsigned long time_t; /* time value */
#endif


#ifndef _CLOCK_T_DEFINED
 #define _CLOCK_T_DEFINED
 typedef unsigned long  clock_t;
#endif

struct  tm {
        int  tm_sec;    /* seconds after the minute -- [0,61] */
        int  tm_min;    /* minutes after the hour   -- [0,59] */
        int  tm_hour;   /* hours after midnight     -- [0,23] */
        int  tm_mday;   /* day of the month         -- [1,31] */
        int  tm_mon;    /* months since January     -- [0,11] */
        int  tm_year;   /* years since 1900                   */
        int  tm_wday;   /* days since Sunday        -- [0,6]  */
        int  tm_yday;   /* days since January 1     -- [0,365]*/
        int  tm_isdst;  /* Daylight Savings Time flag */
};

_WCRTLINK extern char *asctime( const struct tm *__timeptr );
_WCRTLINK extern clock_t clock( void );
_WCRTLINK extern char *ctime( const time_t *__timer );
_WMRTLINK extern double difftime( time_t __t1, time_t __t0 );
_WCRTLINK extern struct tm *gmtime( const time_t *__timer );
_WCRTLINK extern struct tm *localtime( const time_t *__timer );
_WCRTLINK extern time_t mktime( struct tm *__timeptr );
_WCRTLINK extern size_t strftime( char *__s, size_t __maxsiz, const char *__fmt,
                                  const struct tm *__tp );
_WCRTLINK extern time_t time( time_t *__timer );

#ifndef __cplusplus
#define difftime(t1,t0) ((double)(t1)-(double)(t0))
#endif

#if !defined(NO_EXT_KEYS) /* extensions enabled */
_WCRTLINK extern char *_asctime( const struct tm *__timeptr, char *__buf );
_WCRTLINK extern char *_ctime( const time_t *__timer, char *__buf );
_WCRTLINK extern struct tm *_gmtime( const time_t *__timer, struct tm *__tmbuf );
_WCRTLINK extern struct tm *_localtime( const time_t *__timer,
                                        struct tm *__tmbuf );
_WCRTLINK extern char *_strdate( char *__buf );
_WCRTLINK extern char *_strtime( char *__buf );
#endif

_WCRTLINK extern void tzset( void );

#if defined(__FUNCTION_DATA_ACCESS)
 #define tzname (*__get_tzname_ptr())
#elif defined(__SW_BR) || defined(_RTDLL)
 #define tzname tzname_br
#endif
_WCRTLINK extern char   *tzname[2]; /*  time zone names */
#if defined(__FUNCTION_DATA_ACCESS)
 #define timezone (*__get_timezone_ptr())
 #define daylight (*__get_daylight_ptr())
#elif defined(__SW_BR) || defined(_RTDLL)
 #define timezone timezone_br
 #define daylight daylight_br
#endif
_WCRTLINK extern long   timezone;       /* # of seconds from GMT */
_WCRTLINK extern int    daylight;       /* d.s.t. indicator */

#pragma pack()
#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
