/*  DOS specific include file (32-bit) */

#ifndef _I_DOS_H_
#define _I_DOS_H_

#ifdef INCL_IO
#include <io.h>       /* for access() */
#include <direct.h>   /* for mkdir()  */
#endif

/* Symbolic constants for the access() function */

#define R_OK    4       /*  Test for read permission    */
#define W_OK    2       /*  Test for write permission   */
#define X_OK    1       /*  Test for execute permission */
#define F_OK    0       /*  Test for existence of file  */

#define MAXCHAR         ((char)0x7f)
#define MAXSHORT        ((short)0x7fff)

// Max pos 32-bit int.
#define MAXINT          ((int)0x7fffffff)
#define MAXLONG         ((long)0x7fffffff)
#define MINCHAR         ((char)0x80)
#define MINSHORT        ((short)0x8000)

// Max negative 32-bit integer.
#define MININT          ((int)0x80000000)
#define MINLONG         ((long)0x80000000)

#endif /* _I_DOS_H_ */
