/*
 *  stdlib.h    Standard Library functions
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
#ifndef _STDLIB_H_INCLUDED
#define _STDLIB_H_INCLUDED

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
#ifndef __GNUC__
typedef long char wchar_t;
#endif
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

#ifndef MB_CUR_MAX
    #define MB_CUR_MAX  2
#endif
#ifndef MB_LEN_MAX
    #define MB_LEN_MAX  2
#endif

#ifndef _MAX_PATH
 #if defined(__OS2__) || defined(__NT__)
  #define _MAX_PATH   260 /* maximum length of full pathname */
 #else
  #define _MAX_PATH   144 /* maximum length of full pathname */
 #endif
#endif

#define RAND_MAX        32767u
#define EXIT_SUCCESS    0
#define EXIT_FAILURE    0xff

typedef struct  {
        int     quot;
        int     rem;
} div_t;

typedef struct  {
        long    quot;
        long    rem;
} ldiv_t;

_WCRTLINK extern void    abort( void );
_WCIRTLINK extern int    abs( int __j );
          extern int     atexit( register void ( *__func )( void ) );
_WMRTLINK extern double  atof( const char *__nptr );
_WCRTLINK extern int     atoi( const char *__nptr );
_WCRTLINK extern long int atol( const char *__nptr );
_WCRTLINK extern void   *bsearch( const void *__key, const void *__base,
                                  size_t __nmemb, size_t __size,
                                  int (*__compar)(const void *__pkey,
                                                  const void *__pbase) );
_WCRTLINK extern void   *calloc( size_t __n, size_t __size );
_WCIRTLINK extern div_t  div( int __numer, int __denom );
_WCRTLINK extern void    exit( int __status );
_WCRTLINK extern void    free( void *__ptr );
_WCRTLINK extern char   *getenv( const char *__name );
_WCIRTLINK extern long int labs( long int __j );
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
_WCIRTLINK
#else
_WCRTLINK
#endif
extern ldiv_t ldiv( long int __numer, long int __denom );
_WCRTLINK extern void   *malloc( size_t __size );
_WCRTLINK extern void    qsort( void *__base, size_t __nmemb, size_t __size,
                                int (*__compar)( const void *, const void * ) );
_WCRTLINK extern int     rand( void );
_WCRTLINK extern void   *realloc( void *__ptr, size_t __size );
_WCRTLINK extern void    srand( unsigned int __seed );
_WMRTLINK extern double  strtod( const char *__nptr, char **__endptr );
_WCRTLINK extern long int strtol( const char *__nptr, char **__endptr,
                                 int __base );
_WCRTLINK extern unsigned long strtoul( const char *__nptr, char **__endptr,
                                       int __base );
_WCRTLINK extern int     system( const char *__string );

#if defined(__INLINE_FUNCTIONS__)
 #pragma intrinsic(abs,div,labs)
 #if defined(__386__) || defined(__AXP__) || defined(__PPC__)
  #pragma intrinsic(ldiv)
 #endif
#endif

#ifndef __cplusplus
#define atof(p)  strtod(p,(char **)NULL)
#endif


#if !defined(NO_EXT_KEYS) /* extensions enabled */

_WMRTLINK extern char   *ecvt( double __val, int __ndig, int *__dec,
                               int *__sign );
_WMRTLINK extern char   *_ecvt( double __val, int __ndig, int *__dec,
                               int *__sign );
_WMRTLINK extern char   *fcvt( double __val, int __ndig, int *__dec,
                               int *__sign );
_WMRTLINK extern char   *_fcvt( double __val, int __ndig, int *__dec,
                                int *__sign );
_WCRTLINK extern char   *_fullpath( char *__buf, const char *__path,
                                    size_t __size );
_WMRTLINK extern char   *gcvt( double __val, int __ndig, char *__buf );
_WMRTLINK extern char   *_gcvt( double __val, int __ndig, char *__buf );
_WCRTLINK extern char   *itoa( int __value, char *__buf, int __radix );
_WCRTLINK extern char   *_itoa( int __value, char *__buf, int __radix );
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
_WCIRTLINK
#else
_WCRTLINK
#endif
extern unsigned long _lrotl( unsigned long __value, unsigned int __shift );
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
_WCIRTLINK
#else
_WCRTLINK
#endif
extern unsigned long _lrotr( unsigned long __value, unsigned int __shift );
_WCRTLINK extern char   *ltoa( long int __value, char *__buf, int __radix );
_WCRTLINK extern char   *_ltoa( long int __value, char *__buf, int __radix );
_WCRTLINK extern void    _makepath( char *__path, const char *__drive,
                                    const char *__dir, const char *__fname,
                                    const char *__ext );
_WCIRTLINK extern unsigned int _rotl( unsigned int __value, unsigned int __shift );
_WCIRTLINK extern unsigned int _rotr( unsigned int __value, unsigned int __shift );

_WCRTLINK extern int     putenv( const char *__string );
_WCRTLINK extern void    _searchenv( const char *__name, const char *__env_var,
                                     char *__buf );
_WCRTLINK extern void    _splitpath2( const char *__inp, char *__outp,
                                      char **__drive, char **__dir,
                                      char **__fn, char **__ext );
_WCRTLINK extern void    _splitpath( const char *__path, char *__drive,
                                     char *__dir, char *__fname, char *__ext );
_WCRTLINK extern void    swab( char *__src, char *__dest, int __num );
_WCRTLINK extern char   *ultoa( unsigned long int __value, char *__buf,
                                int __radix );
_WCRTLINK extern char   *_ultoa( unsigned long int __value, char *__buf,
                                int __radix );
_WCRTLINK extern char   *utoa( unsigned int __value, char *__buf, int __radix );
_WCRTLINK extern char   *_utoa( unsigned int __value, char *__buf, int __radix );

#if defined(__INLINE_FUNCTIONS__)
 #pragma intrinsic(_rotl,_rotr)
 #if defined(__386__) || defined(__AXP__) || defined(__PPC__)
  #pragma intrinsic(_lrotl,_lrotr)
 #endif
#endif

/* min and max macros */
#if !defined(__max)
#define __max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(max) && !defined(__cplusplus)
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#if !defined(__min)
#define __min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#if !defined(min) && !defined(__cplusplus)
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

/*
 * The following sizes are the maximum sizes of buffers used by the _fullpath()
 * _makepath() and _splitpath() functions.  They include space for the '\0'
 * terminator.
 */

#define _MAX_PATH2 (_MAX_PATH+3) /* maximum size of output buffer
                                    for _splitpath2() */

#if defined(__FUNCTION_DATA_ACCESS)
 #define environ (*__get_environ_ptr())
 #define _wenviron (*__get_wenviron_ptr())
 #define _fileinfo (*__get_fileinfo_ptr())
#elif defined(__SW_BR) || defined(_RTDLL)
 #define environ   environ_br
 #define _wenviron _wenviron_br
 #define _fileinfo _fileinfo_br
#endif
_WCRTLINK extern char **_WCNEAR environ;        /*  pointer to environment table */
#if defined(__NT__)
 _WCRTLINK extern int _fileinfo;        /* for inheriting POSIX handles */
#endif

#ifndef errno
_WCRTLINK extern int  (*__get_errno_ptr( void ));
#define errno (*__get_errno_ptr())
#endif
_WCRTLINK extern int errno;
#define _doserrno (*__get_doserrno_ptr())
_WCRTLINK extern int                _doserrno;  /* DOS system error code value */
#if !defined(__NETWARE__)
#define sys_errlist _sys_errlist
#define sys_nerr _sys_nerr
#endif
#if defined(__FUNCTION_DATA_ACCESS)
 #define _psp                (*__get_psp_ptr())
 #define _osmode             (*__get_osmode_ptr())
 #define _fmode              (*__get_fmode_ptr())
 #define _sys_errlist        (*__get_sys_errlist_ptr())
 #define _sys_nerr           (*__get_sys_nerr_ptr())
 #define __minreal           (*__get_minreal_ptr())
 #define __win_alloc_flags   (*__get_win_alloc_flags_ptr())
 #define __win_realloc_flags (*__get_win_realloc_flags_ptr())
#elif defined(__SW_BR) || defined(_RTDLL)
 #define _psp                _psp_br
 #define _osmode             _osmode_br
 #define _fmode              _fmode_br
 #define _sys_errlist        _sys_errlist_br
 #define _sys_nerr           _sys_nerr_br
 #define __minreal           __minreal_br
 #define __win_alloc_flags   __win_alloc_flags_br
 #define __win_realloc_flags __win_realloc_flags_br
#endif
_WCRTLINK extern unsigned _WCDATA    _psp;      /* Program Segment Prefix */
#define DOS_MODE 0                              /* Real Address Mode */
#define OS2_MODE 1                              /* Protected Address Mode */
_WCRTLINK extern unsigned char _WCNEAR _osmode; /* DOS_MODE or OS2_MODE */
_WCRTLINK extern int _WCNEAR        _fmode;     /* default file translation mode */
_WCRTLINK extern char *             _sys_errlist[];/* strerror error message table */
_WCRTLINK extern int _WCNEAR        _sys_nerr;  /* # of entries in _sys_errlist array */
_WCRTLINK extern unsigned _WCDATA   __minreal;  /* DOS4GW var for WLINK MINREAL option*/
_WCRTLINK extern unsigned long _WCDATA __win_alloc_flags; /* Windows allocation flags */
_WCRTLINK extern unsigned long _WCDATA __win_realloc_flags;/* Windows reallocation flags */
#if defined(__FUNCTION_DATA_ACCESS)
 #define _amblksiz (*__get_amblksiz_ptr())
 #define _osmajor (*__get_osmajor_ptr())
 #define _osminor (*__get_osminor_ptr())
#elif defined(__SW_BR) || defined(_RTDLL)
 #define _amblksiz _amblksiz_br
 #define _osmajor _osmajor_br
 #define _osminor _osminor_br
#endif
_WCRTLINK extern unsigned _WCNEAR      _amblksiz;   /*  mallocs done in multiples of    */
_WCRTLINK extern unsigned char _WCNEAR _osmajor;    /*  O/S major version # */
_WCRTLINK extern unsigned char _WCNEAR _osminor;    /*  O/S minor version # */
#if defined(__NT__)
 #if defined(__FUNCTION_DATA_ACCESS)
  #define _osbuild  (*__get_osbuild_ptr())
  #define _osver    (*__get_osver_ptr())
  #define _winmajor (*__get_winmajor_ptr())
  #define _winminor (*__get_winminor_ptr())
  #define _winver   (*__get_winver_ptr())
 #elif defined(__SW_BR) || defined(_RTDLL)
  #define _osbuild  _osbuild_br
  #define _osver    _osver_br
  #define _winmajor _winmajor_br
  #define _winminor _winminor_br
  #define _winver   _winver_br
 #endif
 _WCRTLINK extern unsigned short _WCDATA _osbuild;  /*  O/S build revision  */
 _WCRTLINK extern unsigned int _WCDATA _osver;      /*  O/S build revision  */
 _WCRTLINK extern unsigned int _WCDATA _winmajor;   /*  O/S major version # */
 _WCRTLINK extern unsigned int _WCDATA _winminor;   /*  O/S minor version # */
 _WCRTLINK extern unsigned int _WCDATA _winver;     /*  O/S version #       */
#endif
extern  int       __argc;       /* number of cmd line args */
extern  char    **__argv;       /* vector of cmd line args */


_WCRTLINK extern void _WCNEAR *__brk( size_t __new_brk_value );
_WCRTLINK extern void _WCNEAR *sbrk( size_t __increment );

 typedef void (*onexit_t)();
_WCRTLINK extern onexit_t onexit(onexit_t __func);

#endif

#pragma pack()
#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif
