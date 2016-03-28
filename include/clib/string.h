/*
 *  string.h    String functions
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
#ifndef _STRING_H_INCLUDED
#define _STRING_H_INCLUDED

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

#ifndef _NLSCMPERROR
 #include <limits.h>
 #define _NLSCMPERROR INT_MAX
#endif

_WCIRTLINK extern void *memchr( const void *__s, int __c, size_t __n );
_WCIRTLINK extern int memcmp( const void *__s1, const void *__s2, size_t __n );
_WCIRTLINK extern void *memcpy( void *__s1, const void *__s2, size_t __n );
_WCRTLINK extern void *memmove( void *__s1, const void *__s2, size_t __n );
#if defined(M_I86)
_WCIRTLINK
#else
_WCRTLINK
#endif
extern void *memset( void *__s, int __c, size_t __n );
_WCIRTLINK extern char *strcat( char *__s1, const char *__s2 );
_WCIRTLINK extern char *strchr( const char *__s, int __c );
#if defined(M_I86)
_WCIRTLINK
#else
_WCRTLINK
#endif
extern int strcmp( const char *__s1, const char *__s2 );
_WCRTLINK extern int strcoll( const char *__s1, const char *__s2 );
_WCRTLINK extern int _stricoll( const char *__s1, const char *__s2 );
_WCRTLINK extern int _strncoll( const char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern int _strnicoll( const char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern size_t strxfrm( char *__s1, const char *__s2, size_t __n );
_WCIRTLINK extern char *strcpy( char *__s1, const char *__s2 );
_WCRTLINK extern size_t strcspn( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strerror( int __errnum );
_WCIRTLINK extern size_t strlen( const char *__s );
_WCRTLINK extern char *strncat( char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern int strncmp( const char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern char *strncpy( char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern char *strpbrk( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strrchr( const char *__s, int __c );
_WCRTLINK extern size_t strspn( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strspnp( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strstr( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strtok( char *__s1, const char *__s2 );

#if !defined(NO_EXT_KEYS) /* extensions enabled */

_WCRTLINK extern int bcmp( const void *__s1, const void *__s2, size_t __n );
_WCRTLINK extern void bcopy( const void *__s1, void *__s2, size_t __n );
_WCRTLINK extern void bzero( void *__s, size_t __n );
_WCRTLINK extern void *memccpy( void *__s1, const void *__s2, int __c, size_t __n );
_WCRTLINK extern int memicmp( const void *__s1, const void *__s2, size_t __n );
_WCRTLINK extern int strcmpi( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strdup( const char *__string );
_WCRTLINK extern int stricmp( const char *__s1, const char *__s2 );
_WCRTLINK extern char *strlwr( char *__string );
_WCRTLINK extern int strnicmp( const char *__s1, const char *__s2, size_t __n );
_WCRTLINK extern char *strnset( char *__string, int __c, size_t __len );
_WCRTLINK extern char *strrev( char *__string );
_WCRTLINK extern char *strset( char *__string, int __c );
_WCRTLINK extern char *strupr( char *__string );

#endif  /* EXTENSIONS */

#ifdef  __INLINE_FUNCTIONS__
 #pragma intrinsic(memchr,memcmp,memcpy,strcat,strcpy,strlen,strchr)
 #if defined(M_I86)
  #pragma intrinsic(memset,strcmp)
 #endif
#endif  /* __INLINE_FUNCTIONS__ */

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
