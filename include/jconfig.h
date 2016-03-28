/* jconfig.mgl -- jconfig.h for SciTech MGL supported compilers */

#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
/* #define void char */
/* #define const */
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H
#define HAVE_STDLIB_H
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Note that we override the default memory allocation in jmemmgl.c to
 * use the regular malloc and free functions, and we also override the
 * JFREAD and JFWRITE macros in JINCLUDE.H to use the MGL_fread and MGL_fwrite
 * functions.
 */

#include "pmapi.h"
#include <stdio.h>

#pragma pack(1)

typedef struct {
    ulong   dwSize;
    size_t  (PMAPIP MGL_fread)(void *ptr,size_t size,size_t n,FILE *f);
    size_t  (PMAPIP MGL_fwrite)(const void *ptr,size_t size,size_t n,FILE *f);
    } JPEG_imports;

#pragma pack()

#ifdef  __DRIVER__
extern JPEG_imports _VARAPI _JPEG_imports;
#define MGL_fread(ptr,size,n,f)     _JPEG_imports.MGL_fread(ptr,size,n,f)
#define MGL_fwrite(ptr,size,n,f)    _JPEG_imports.MGL_fwrite(ptr,size,n,f)
#else
size_t  PMAPI MGL_fread(void *ptr,size_t size,size_t n,FILE *f);
size_t  PMAPI MGL_fwrite(const void *ptr,size_t size,size_t n,FILE *f);
#endif

#define JFREAD(file,buf,sizeofbuf)  \
  ((size_t) MGL_fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
#define JFWRITE(file,buf,sizeofbuf)  \
  ((size_t) MGL_fwrite((const void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */

#ifdef JPEG_CJPEG_DJPEG

#define BMP_SUPPORTED       /* BMP image file format */
#define GIF_SUPPORTED       /* GIF image file format */
#define PPM_SUPPORTED       /* PBMPLUS PPM/PGM image file format */
#undef RLE_SUPPORTED        /* Utah RLE image file format */
#define TARGA_SUPPORTED     /* Targa image file format */

#define TWO_FILE_COMMANDLINE
#define USE_SETMODE     /* Needed to make one-file style work in Watcom */
#undef NEED_SIGNAL_CATCHER  /* Define this if you use jmemname.c */
#undef DONT_USE_B_MODE
#undef PROGRESS_REPORT      /* optional */

#endif /* JPEG_CJPEG_DJPEG */
