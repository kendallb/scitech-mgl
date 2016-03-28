/* clibext.h:
   This file contains defines and prototypes of functions that are present
   in Watcom's CLIB but not in many other C libraries */

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#define O_BINARY 0
#define O_TEXT 0
#ifndef __SNAP__
#define stricmp strcasecmp
#define strcmpi strcasecmp
#define strnicmp strncasecmp
#endif
#define getch getchar
#define _vbprintf vsnprintf
#define __near
#define near
#define __based(x)
#define _snprintf snprintf
#define _vsnprintf vsnprintf
#define  __va_list  va_list
#define __Strtold(s,ld,endptr) ((*(double *)(ld))=strtod(s,endptr))
#define SOPEN_DEFINED
#define sopen(x,y,z) open((x),(y))
#define _fsopen(x,y,z) fopen(x,y)
#define _fmemcpy memcpy
#ifndef _MAX_PATH
#define _MAX_PATH (PATH_MAX+1)
#endif
#ifndef _MAX_PATH2
#define _MAX_PATH2 (_MAX_PATH+3)
#endif
#ifndef _MAX_DRIVE
#define _MAX_DRIVE   3
#endif
#ifndef _MAX_DIR
#define _MAX_DIR (PATH_MAX-3)
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME (PATH_MAX-3)
#endif
#ifndef _MAX_EXT
#define _MAX_EXT (PATH_MAX-3)
#endif
#ifndef SH_DENYWR
#define SH_DENYWR 0
#endif

#ifndef max 
#define max(x,y) (((x)>(y))?(x):(y))
#endif
#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif
#define _WCRTLINK
#define _WCI86FAR
#define _WCNEAR
#define __int64 long long

#define _HEAPOK 0
#define _heapchk(x) _HEAPOK
#define _expand(x,y) (NULL)

char *itoa( int value, char *buf, int radix );
char *utoa( unsigned int value, char *buf, int radix );
char *ltoa( long int value, char *buf, int radix );
char *ultoa( unsigned long int value, char *buf, int radix );
void _splitpath2( const char *inp, char *outp, char **drive,
                  char **dir, char **fn, char **ext );
void _splitpath( const char *path, char *drive,
                 char *dir, char *fname, char *ext );
void _makepath( char *path, const char *drive, const char *dir,
                const char *fname, const char *ext );
char *_fullpath( char *buf, const char *path, size_t size );
char *strlwr( char *string );
char *strupr( char *string );
char *strrev( char *string );
int memicmp(const void *, const void *, size_t);
long tell( int handle );
long filelength(int handle);
int eof( int fildes );
char *getcmd( char *buffer );
char *_cmdname( char *name );
void _searchenv( const char *name, const char *env_var, char *buf );

extern char **_argv;
