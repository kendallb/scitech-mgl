/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#include <sys/types.h>
#include <sys/stat.h>

#define USE_EF_UT_TIME          /* Enable use of "UT" extra field time info */

/* Enable use of optimized x86 assembler version of longest_match() for
   MSDOS, WIN32 and OS2 per default.  */
#if !defined(NO_ASM) && !defined(ASMV)
#  define ASMV
#endif

/* Get asm routines to link properly without using "__cdecl": */
#ifdef __WATCOMC__
#  ifdef __386__
#    ifdef ASMV
#      pragma aux match_init    "_*" parm caller [] modify []
#      pragma aux longest_match "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif
#    if defined(ASM_CRC) && !defined(USE_ZLIB)
#      pragma aux crc32         "_*" parm caller [] value [eax] modify [eax]
#      pragma aux get_crc_table "_*" parm caller [] value [eax] \
                                      modify [eax ecx edx]
#    endif /* ASM_CRC && !USE_ZLIB */
#  endif /* __386__ */
#endif /* __WATCOMC__ */

