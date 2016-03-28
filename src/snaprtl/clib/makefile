#############################################################################
#
#  ========================================================================
#
#   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
#
#   This file may be distributed and/or modified under the terms of the
#   GNU General Public License version 2.0 as published by the Free
#   Software Foundation and appearing in the file LICENSE.GPL included
#   in the packaging of this file.
#
#   Licensees holding a valid Commercial License for this product from
#   SciTech Software, Inc. may use this file in accordance with the
#   Commercial License Agreement provided with the Software.
#
#   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
#   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   See http://www.scitechsoft.com/license/ for information about
#   the licensing options available and how to purchase a Commercial
#   License Agreement.
#
#   Contact license@scitechsoft.com if any conditions of this licensing
#   are not clear to you, or you have questions about licensing options.
#
#  ========================================================================
#
# Descripton:   Generic makefile for building the SciTech SNAP Binary
#               Portable Driver runtime library. This library is based on
#               the Open Watcom runtime library.
#
#############################################################################

#----------------------------------------------------------------------------
# Target dependent configuration stuff
#----------------------------------------------------------------------------

STOP_ON_WARN    := 1
NO_DEFAULT_LIBS := 1
UNSIGNED_CHAR   := 1
CFLAGS          += -D__SNAPDRV__ -I..\h -Ichar\h -Iheap\h -Imath\h -Imisc\h \
                   -Istartup\h -Istreamio\h -Istring\h -Itime\h

.IF $(USE_PPC_BE)
CFLAGS += -U__LINUX__
.ENDIF
.IF $(USE_X86_64)
CFLAGS += -U__LINUX__
.ENDIF
.IF $(USE_MIPS_LE)
CFLAGS += -U__LINUX__
.ENDIF

#----------------------------------------------------------------------------
# Since this runtime library *requires* the Open Watcom assembler, we have
# to override the assembler flags in here.
#----------------------------------------------------------------------------

.IF $(USE_X86)
USE_WASM        := 1
ASFLAGS         := -I..\h -ms-5r-e-fpi87-fp5-w4-zld-bt=SNAP -D__SNAPDRV__
.ELIF $(USE_PPC_BE)
.ENDIF

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

ALIAS_OBJS      = _icstrdu$O

CGSUPP_OBJS     = 7fu8386$O 7u8f386$O amodf386$O bufld386$O div386$O fdc386$O \
                  fdfs386$O fdi4386$O fdi8386$O fdmath$O fdmth386$O fdn386$O \
                  fprem386$O fsc386$O fsfd386$O fsi4386$O fsi8386$O fsmth386$O \
                  fsn386$O fstat386$O i4fd386$O i4fs386$O i8d$O i8fd386$O \
                  i8fs386$O i8m386$O i8rs386$O ldiv386$O __copy$O __stos$O

CHAR_OBJS       = isalnum$O isalpha$O isascii$O iscntrl$O isdigit$O isgraph$O \
                  islower$O isprint$O ispunct$O isspace$O istable$O isupper$O \
                  isxdigit$O tolower$O toupper$O

CONVERT_OBJS    = alphabet$O atoi$O atol$O atoll$O itoa$O lltoa$O ltoa$O \
                  strtol$O

ENVIRON_OBJS    = gtenvsnp$O isattsnp$O ptenvsnp$O

FILE_OBJS       = accsssnp$O removsnp$O renamsnp$O tmpnm$O

FPU_OBJS        = chipa32$O chipd32$O chipr32$O chipt32$O chipw32$O \
                  ini87386$O old87$O cntrl87$O cntrlfp$O

HANDLEIO_OBJS   = close$O eof$O filelen$O lseeksnp$O opensnp$O qreadsnp$O \
                  qwritsnp$O tellsnp$O writesnp$O _clsesnp$O

HEAP_OBJS       = amblksiz$O ambsptr$O calloc$O freect$O grownear$O heapen$O \
                  heapgrow$O mem$O memavl$O memmax$O ncalloc$O \
                  nexpand$O nfree$O nheapchk$O nheapmin$O nheapset$O \
                  nheapwal$O nmalloc$O nmemneed$O nmsize$O nrealloc$O \
                  sbrksnp$O

INTEL_OBJS      = chipvar$O i64ts386$O chipbug$O save8087$O

MATH_OBJS       = abs$O hugeval$O hvalptr$O labs$O max$O min$O rand$O

MEMORY_OBJS     = memccpy$O memchr$O memcmp$O memcpy$O memmove$O memset$O \
                  swab$O

MISC_OBJS       = locale$O localeco$O setlocal$O

PROCESS_OBJS    = signlsnp$O systmsnp$O _raise$O

SEARCH_OBJS     = bsearch$O lfind$O lsearch$O qsort$O

STARTUP_OBJS    = initrtns$O cvtbuf$O abort$O atexit$O drvinit$O errno$O \
                  exit$O fltused$O ioexit$O liballoc$O ljmphdl$O mainsnp$O \
                  onexit$O seterrno$O xmsgsnp$O

STARTUP_OBJS_X86 = crwd386$O getds$O seg386$O stjmp386$O stksnp$O \
                   _8087386$O 8087cw$O chk8087$O stack386$O segdefns$O \
                   cmodel$O enterdb$O drvimps$O

STARTUP_OBJS_PPC = enterdb$O crwdppc$O drvimps$O

STARTUP_OBJS_AMD64 = enterdb$O crwda64$O drvimps$O

STARTUP_OBJS_MIPS = enterdb$O crwda64$O drvimps$O

STREAMIO_OBJS   = allocfp$O chktty$O clearerr$O comtflag$O fclose$O fdopen$O \
                  feof$O ferror$O freefp$O fflush$O fgetc$O fgetchar$O fgetpos$O \
                  fgets$O fileno$O flush$O flushall$O fopen$O fprintf$O fprtf$O \
                  fputc$O fputchar$O fputs$O fread$O fscanf$O fseek$O fsetpos$O \
                  ftell$O fwrite$O getc$O getchar$O gets$O getw$O initfile$O \
                  ioalloc$O iob$O iobaddr$O iobptr$O noefgfmt$O perror$O printf$O \
                  prtf$O putc$O putchar$O puts$O putw$O rewind$O scanf$O scnf$O \
                  setbuf$O setefg$O setvbuf$O ungetc$O vfprintf$O vprintf$O

STRING_OBJS     = bits$O snprintf$O sprintf$O sscanf$O strcat$O strchr$O \
                  strcmp$O strcoll$O strcpy$O strcspn$O strdup$O strerror$O \
                  stricmp$O strlen$O strlwr$O strncat$O strncmp$O strncpy$O \
                  strnicmp$O strnset$O strpbrk$O strrchr$O strspn$O \
                  strspnp$O strstr$O strtok$O strupr$O strxfrm$O vsprintf$O

TIME_OBJS       = asctime$O clocksnp$O gmtime$O localtim$O mktime$O \
                  strftime$O timesnp$O timeutil$O tzset$O

#----------------------------------------------------------------------------
# Name of library and object files required to build it
#----------------------------------------------------------------------------

.IF $(USE_X86)
OBJECTS         = $(ALIAS_OBJS) $(CGSUPP_OBJS) $(CHAR_OBJS) $(CONVERT_OBJS) \
                  $(ENVIRON_OBJS) $(FILE_OBJS) $(FPU_OBJS) $(HANDLEIO_OBJS) \
                  $(HEAP_OBJS) $(INTEL_OBJS) $(MATH_OBJS) $(MEMORY_OBJS) \
                  $(MISC_OBJS) $(PROCESS_OBJS) $(SEARCH_OBJS) $(STARTUP_OBJS) \
                  $(STREAMIO_OBJS) $(STRING_OBJS) $(TIME_OBJS) \
                  $(STARTUP_OBJS_X86)
.ENDIF

.IF $(USE_PPC_BE)
OBJECTS         = $(ALIAS_OBJS_PPC) $(CHAR_OBJS) $(CONVERT_OBJS) \
                  $(ENVIRON_OBJS) $(FILE_OBJS) $(HANDLEIO_OBJS) \
                  $(HEAP_OBJS) $(PPC_OBJS) $(MATH_OBJS) $(MEMORY_OBJS) \
                  $(MISC_OBJS) $(PROCESS_OBJS) $(SEARCH_OBJS) \
                  $(STARTUP_OBJS) $(STREAMIO_OBJS) $(STRING_OBJS) \
                  $(TIME_OBJS) $(STARTUP_OBJS_PPC)
.ENDIF

.IF $(USE_X86_64)
OBJECTS         = $(ALIAS_OBJS_AMD64) $(CHAR_OBJS) $(CONVERT_OBJS) \
                  $(ENVIRON_OBJS) $(FILE_OBJS) $(HANDLEIO_OBJS) \
                  $(HEAP_OBJS) $(PPC_OBJS) $(MATH_OBJS) $(MEMORY_OBJS) \
                  $(MISC_OBJS) $(PROCESS_OBJS) $(SEARCH_OBJS) \
                  $(STARTUP_OBJS) $(STREAMIO_OBJS) $(STRING_OBJS) \
                  $(TIME_OBJS) $(STARTUP_OBJS_AMD64)
.ENDIF

.IF $(USE_MIPS_LE)
OBJECTS         = $(ALIAS_OBJS_MIPS) $(CHAR_OBJS) $(CONVERT_OBJS) \
                  $(ENVIRON_OBJS) $(FILE_OBJS) $(HANDLEIO_OBJS) \
                  $(HEAP_OBJS) $(PPC_OBJS) $(MATH_OBJS) $(MEMORY_OBJS) \
                  $(MISC_OBJS) $(PROCESS_OBJS) $(SEARCH_OBJS) \
                  $(STARTUP_OBJS) $(STREAMIO_OBJS) $(STRING_OBJS) \
                  $(TIME_OBJS) $(STARTUP_OBJS_MIPS)
.ENDIF


LIBFILE         = $(LP)clib$L
LIBCLEAN        = *.lib *.a

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS)
DEPEND_INC      = ../h;char/h;heap/h;math/h;misc/h;startup/h;streamio/h;string/h;time/h
DEPEND_SRC      = alias/a;cgsupp/a;char/c;convert/c;environ/c;file/c;fpu/a;fpu/c;handleio/c;heap/c;intel/a;intel/c;math/c;memory/c;misc/c;process/c;search/c;startup/a;startup/c;streamio/c;string/c;time/c

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
