/*
 * jpeginit.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the code specific to initialising a SciTech MGL
 * Binary Portable DLL.
 */

#include "jpeglib.h"
#include "scitech.h"
#include "clib/os/imports.h"
#include <stdio.h>
#include <string.h>

/*------------------------- Global Variables ------------------------------*/

PM_imports _VARAPI      _PM_imports;
JPEG_imports _VARAPI    _JPEG_imports;

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
REMARKS:
Fatal error handler called when a non-imported function is called by the
driver. We leave this to a runtime error so that older applications and
shell drivers will work with newer bpd drivers provided no newer functions
are required by the driver itself. If they are, the application or shell
driver needs to be recompiled.
****************************************************************************/
static void _PM_fatalErrorHandler(void)
{
    _PM_imports.PM_fatalError("Unsupported PM_imports import function called! Please re-compile!\n");
}

/****************************************************************************
REMARKS:
Fatal error handler for non-imported JPEG_imports.
****************************************************************************/
static void _JPEG_fatalErrorHandler(void)
{
    _PM_imports.PM_fatalError("Unsupported JPEG_imports import function called! Please re-compile!\n");
}

/****************************************************************************
PARAMETERS:

RETURNS:
Pointer to exported function list

REMARKS:
This function initialises the JPEG image library and returns the list of
loader library exported functions.
{secret}
****************************************************************************/
JPEG_exports * _CEXPORT JPEG_initLibrary(
    PM_imports *pmImp,
    JPEG_imports *jpegImp)
{
    static JPEG_exports _JPEG_exports = {
        sizeof(JPEG_exports),
        jpeg_std_error,
        jpeg_CreateCompress,
        jpeg_CreateDecompress,
        jpeg_destroy_compress,
        jpeg_destroy_decompress,
        jpeg_stdio_dest,
        jpeg_stdio_src,
        jpeg_set_defaults,
        jpeg_set_colorspace,
        jpeg_default_colorspace,
        jpeg_set_quality,
        jpeg_set_linear_quality,
        jpeg_add_quant_table,
        jpeg_quality_scaling,
        jpeg_simple_progression,
        jpeg_suppress_tables,
        jpeg_alloc_quant_table,
        jpeg_alloc_huff_table,
        jpeg_start_compress,
        jpeg_write_scanlines,
        jpeg_finish_compress,
        jpeg_write_raw_data,
        jpeg_write_marker,
        jpeg_write_tables,
        jpeg_read_header,
        jpeg_start_decompress,
        jpeg_read_scanlines,
        jpeg_finish_decompress,
        jpeg_read_raw_data,
        jpeg_has_multiple_scans,
        jpeg_start_output,
        jpeg_finish_output,
        jpeg_input_complete,
        jpeg_new_colormap,
        jpeg_consume_input,
        jpeg_calc_output_dimensions,
        jpeg_set_marker_processor,
        jpeg_read_coefficients,
        jpeg_write_coefficients,
        jpeg_copy_critical_parameters,
        jpeg_abort_compress,
        jpeg_abort_decompress,
        jpeg_abort,
        jpeg_destroy,
        jpeg_resync_to_restart,
        };
    int     i,max;
    ulong   *p;

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility.
     */
    max = sizeof(_PM_imports)/sizeof(JPEG_initLibrary_t);
    for (i = 0,p = (ulong*)&_PM_imports; i < max; i++)
        *p++ = (ulong)_PM_fatalErrorHandler;
    max = sizeof(_JPEG_imports)/sizeof(JPEG_initLibrary_t);
    for (i = 0,p = (ulong*)&_JPEG_imports; i < max; i++)
        *p++ = (ulong)_JPEG_fatalErrorHandler;

    /* Now copy all our imported functions */
    memcpy(&_PM_imports,pmImp,MIN(sizeof(_PM_imports),pmImp->dwSize));
    memcpy(&_JPEG_imports,jpegImp,MIN(sizeof(_JPEG_imports),jpegImp->dwSize));
    return &_JPEG_exports;
}
