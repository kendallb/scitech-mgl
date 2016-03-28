/* $Header: /usr/people/sam/tiff/libtiff/RCS/tif_win32.c,v 1.3 1996/01/10 19:33:18 sam Rel $ */

/*
 * Copyright (c) 1988-1996 Sam Leffler
 * Copyright (c) 1991-1996 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * TIFF Library Scitech BPD-specific Routines.
 */
#include <stdio.h>
#include <stdlib.h>
#include "tiffiop.h"

static tsize_t
_tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return (fread(buf,1,size,(FILE*)fd));
}

static tsize_t
_tiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
    return (fwrite(buf,1,size,(FILE*)fd));
}

static toff_t
_tiffSeekProc(thandle_t fd, toff_t off, int whence)
{
    return (fseek((FILE*)fd, (off_t) off, whence));
}

static int
_tiffCloseProc(thandle_t fd)
{
    return (fclose((FILE*)fd));
}

static toff_t
_tiffSizeProc(thandle_t fd)
{
    toff_t size,oldpos = ftell((FILE*)fd);
    fseek((FILE*)fd,0,SEEK_END);
    size = ftell((FILE*)fd);
    fseek((FILE*)fd,oldpos,SEEK_SET);
    return size;
}

static int
_tiffMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
    return (0);
}

static void
_tiffUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{
}

/*
 * Open a TIFF file descriptor for read/writing.
 */
TIFF*
TIFFFdOpen(FILE *fd, const char* name, const char* mode)
{
    TIFF* tif;

    tif = TIFFClientOpen(name, mode,
        (void*)fd,
        _tiffReadProc, _tiffWriteProc, _tiffSeekProc, _tiffCloseProc,
        _tiffSizeProc, _tiffMapProc, _tiffUnmapProc);
    if (tif)
        tif->tif_fd = fd;
    return (tif);
}

/*
 * Open a TIFF file for read/writing.
 */
TIFF*
TIFFOpen(const char* name, const char* mode)
{
    static const char module[] = "TIFFOpen";
    int m, fd;

    m = _TIFFgetMode(mode, module);
    if (m == -1)
        return ((TIFF*)0);
    fd = open(name, m|O_BINARY, 0666);
    if (fd < 0) {
        TIFFError(module, "%s: Cannot open", name);
        return ((TIFF*)0);
    }
    return (TIFFFdOpen(fd, name, mode));
}

tdata_t
_TIFFmalloc(tsize_t s)
{
    return (PM_malloc((size_t) s));
}

void
_TIFFfree(tdata_t p)
{
    PM_free(p);
}

tdata_t
_TIFFrealloc(tdata_t p, tsize_t s)
{
    return (PM_realloc(p, (size_t) s));
}

void
_TIFFmemset(tdata_t p, int v, tsize_t c)
{
    memset(p, v, (size_t) c);
}

void
_TIFFmemcpy(tdata_t d, const tdata_t s, tsize_t c)
{
    memcpy(d, s, (size_t) c);
}

int
_TIFFmemcmp(const tdata_t p1, const tdata_t p2, tsize_t c)
{
    return (memcmp(p1, p2, (size_t) c));
}

static void
msdosWarningHandler(const char* module, const char* fmt, va_list ap)
{
    // TODO: This needs to go to a log file or something!
#if 0
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
#endif
}
TIFFErrorHandler _TIFFwarningHandler = msdosWarningHandler;

static void
msdosErrorHandler(const char* module, const char* fmt, va_list ap)
{
    // TODO: This needs to go to a log file or something!
#if 0
    if (module != NULL)
        fprintf(stderr, "%s: ", module);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
#endif
}
TIFFErrorHandler _TIFFerrorHandler = msdosErrorHandler;
#endif
