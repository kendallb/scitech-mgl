/****************************************************************************
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  This file contains the code specific to initialising the
*               SciTech MGL Binary Portable DLL.
*
****************************************************************************/

#include "png.h"
#include <stdio.h>
#include <string.h>

#include "scitech.h"
#include "clib/os/imports.h"

/*------------------------- Global Variables ------------------------------*/

PM_imports _VARAPI  _PM_imports;
PNG_imports _VARAPI _PNG_imports;
FILE                *png_err_stream;

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
Fatal error handler for non-imported PNG_imports.
****************************************************************************/
static void _PNG_fatalErrorHandler(void)
{
    _PM_imports.PM_fatalError("Unsupported PNG_imports import function called! Please re-compile!\n");
}

/****************************************************************************
PARAMETERS:

RETURNS:
Pointer to exported function list

REMARKS:
This function initialises the PNG image library and returns the list of
loader library exported functions.
{secret}
****************************************************************************/
PNG_exports * _CEXPORT PNG_initLibrary(
    PM_imports *pmImp,
    PNG_imports *jpegImp)
{
    static PNG_exports _PNG_exports = {
        sizeof(PNG_exports),
        png_set_sig_bytes,
        png_sig_cmp,
        png_check_sig,
        png_create_read_struct,
        png_create_write_struct,
        png_write_chunk,
        png_write_chunk_start,
        png_write_chunk_data,
        png_write_chunk_end,
        png_create_info_struct,
        png_write_info,
        png_read_info,
        png_convert_from_struct_tm,
        png_convert_from_time_t,
        png_set_expand,
        png_set_bgr,
        png_set_gray_to_rgb,
        png_build_grayscale_palette,
        png_set_strip_alpha,
        png_set_swap_alpha,
        png_set_invert_alpha,
        png_set_filler,
        png_set_swap,
        png_set_packing,
        png_set_packswap,
        png_set_shift,
        png_set_interlace_handling,
        png_set_invert_mono,
        png_set_background,
        png_set_strip_16,
        png_set_dither,
        png_set_gamma,
        png_set_flush,
        png_write_flush,
        png_start_read_image,
        png_read_update_info,
        png_read_rows,
        png_read_row,
        png_read_image,
        png_write_row,
        png_write_rows,
        png_write_image,
        png_write_end,
        png_read_end,
        png_destroy_info_struct,
        png_destroy_read_struct,
        png_destroy_write_struct,
        png_set_crc_action,
        png_set_filter,
        png_set_filter_heuristics,
        png_set_compression_level,
        png_set_compression_mem_level,
        png_set_compression_strategy,
        png_set_compression_window_bits,
        png_set_compression_method,
        png_init_io,
        png_set_error_fn,
        png_get_error_ptr,
        png_set_write_fn,
        png_set_read_fn,
        png_get_io_ptr,
        png_set_read_status_fn,
        png_set_write_status_fn,
        png_set_read_user_transform_fn,
        png_set_write_user_transform_fn,
        png_set_progressive_read_fn,
        png_get_progressive_ptr,
        png_process_data,
        png_progressive_combine_row,
        png_memcpy_check,
        png_memset_check,
        png_error,
        png_chunk_error,
        png_warning,
        png_chunk_warning,
        png_get_valid,
        png_get_rowbytes,
        png_get_channels,
        png_get_image_width,
        png_get_image_height,
        png_get_bit_depth,
        png_get_color_type,
        png_get_filter_type,
        png_get_interlace_type,
        png_get_compression_type,
        png_get_pixels_per_meter,
        png_get_x_pixels_per_meter,
        png_get_y_pixels_per_meter,
        png_get_pixel_aspect_ratio,
        png_get_x_offset_pixels,
        png_get_y_offset_pixels,
        png_get_x_offset_microns,
        png_get_y_offset_microns,
        png_get_signature,
        png_get_bKGD,
        png_set_bKGD,
        png_get_cHRM,
        png_set_cHRM,
        png_get_gAMA,
        png_set_gAMA,
        png_get_hIST,
        png_set_hIST,
        png_get_IHDR,
        png_set_IHDR,
        png_get_oFFs,
        png_set_oFFs,
        png_get_pCAL,
        png_set_pCAL,
        png_get_pHYs,
        png_set_pHYs,
        png_get_PLTE,
        png_set_PLTE,
        png_get_sBIT,
        png_set_sBIT,
        png_get_sRGB,
        png_set_sRGB,
        png_set_sRGB_gAMA_and_cHRM,
        png_get_text,
        png_set_text,
        png_get_tIME,
        png_set_tIME,
        png_get_tRNS,
        png_set_tRNS,
        };
    int     i,max;
    ulong   *p;

    // Initialize all default imports to point to fatal error handler
    // for upwards compatibility.
    max = sizeof(_PM_imports)/sizeof(PNG_initLibrary_t);
    for (i = 0,p = (ulong*)&_PM_imports; i < max; i++)
        *p++ = (ulong)_PM_fatalErrorHandler;
    max = sizeof(_PNG_imports)/sizeof(PNG_initLibrary_t);
    for (i = 0,p = (ulong*)&_PNG_imports; i < max; i++)
        *p++ = (ulong)_PNG_fatalErrorHandler;

    // Now copy all our imported functions
    memcpy(&_PM_imports,pmImp,MIN(sizeof(_PM_imports),pmImp->dwSize));
    memcpy(&_PNG_imports,jpegImp,MIN(sizeof(_PNG_imports),jpegImp->dwSize));

#ifdef PNG_DEBUG
    png_err_stream = fopen("pngbpd.log", "w");
    setbuf(png_err_stream, NULL);
    fflush(png_err_stream);
    fprintf(png_err_stream, "Logging initialized.\n");
#endif
    return &_PNG_exports;
}
