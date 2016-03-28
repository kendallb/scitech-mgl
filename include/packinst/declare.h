/******************************************************/
/*                                                    */
/* declare.h: declaration-symbols (UCHAR, ULONG, ...) */
/*                                                    */
/******************************************************/

#ifndef __declare_h
#define __declare_h

#ifndef OS2_H_INCLUDED
#ifdef AMIGA

#include <exec/types.h>

#else /* AMIGA */

typedef unsigned short USHORT;
typedef short          SHORT ;
typedef unsigned short UWORD ;
typedef short          WORD  ;
typedef unsigned long  ULONG ;
typedef long           LONG  ;

#endif  /* !AMIGA */

typedef unsigned char  UCHAR ;
typedef char           CHAR  ;
typedef unsigned       UINT  ;
typedef int            INT   ;
#else
typedef unsigned short UWORD ;
#endif


#endif /* __declare_h */

