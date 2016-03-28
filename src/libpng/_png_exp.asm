;****************************************************************************
;*
;* Language:    NASM
;* Environment: IBM PC 32 bit Protected Mode.
;*
;* Description: Module to implement the export stubs for all the PNG
;*              image library.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

BEGIN_IMPORTS_DEF   _PNG_exports
DECLARE_IMP png_set_sig_bytes,0
DECLARE_IMP __png_sig_cmp,0               ; Handled in C code!
DECLARE_IMP png_check_sig,0
DECLARE_IMP __png_create_read_struct,0    ; Handled in C code!
DECLARE_IMP __png_create_write_struct,0   ; Handled in C code!
DECLARE_IMP png_write_chunk,0
DECLARE_IMP png_write_chunk_start,0
DECLARE_IMP png_write_chunk_data,0
DECLARE_IMP png_write_chunk_end,0
DECLARE_IMP png_create_info_struct,0
DECLARE_IMP png_write_info,0
DECLARE_IMP png_read_info,0
DECLARE_IMP png_convert_from_struct_tm,0
DECLARE_IMP png_convert_from_time_t,0
DECLARE_IMP png_set_expand,0
DECLARE_IMP png_set_bgr,0
DECLARE_IMP png_set_gray_to_rgb,0
DECLARE_IMP png_build_grayscale_palette,0
DECLARE_IMP png_set_strip_alpha,0
DECLARE_IMP png_set_swap_alpha,0
DECLARE_IMP png_set_invert_alpha,0
DECLARE_IMP png_set_filler,0
DECLARE_IMP png_set_swap,0
DECLARE_IMP png_set_packing,0
DECLARE_IMP png_set_packswap,0
DECLARE_IMP png_set_shift,0
DECLARE_IMP png_set_interlace_handling,0
DECLARE_IMP png_set_invert_mono,0
DECLARE_IMP png_set_background,0
DECLARE_IMP png_set_strip_16,0
DECLARE_IMP png_set_dither,0
DECLARE_IMP png_set_gamma,0
DECLARE_IMP png_set_flush,0
DECLARE_IMP png_write_flush,0
DECLARE_IMP png_start_read_image,0
DECLARE_IMP png_read_update_info,0
DECLARE_IMP png_read_rows,0
DECLARE_IMP png_read_row,0
DECLARE_IMP png_read_image,0
DECLARE_IMP png_write_row,0
DECLARE_IMP png_write_rows,0
DECLARE_IMP png_write_image,0
DECLARE_IMP png_write_end,0
DECLARE_IMP png_read_end,0
DECLARE_IMP png_destroy_info_struct,0
DECLARE_IMP png_destroy_read_struct,0
DECLARE_IMP png_destroy_write_struct,0
DECLARE_IMP png_set_crc_action,0
DECLARE_IMP png_set_filter,0
DECLARE_IMP png_set_filter_heuristics,0
DECLARE_IMP png_set_compression_level,0
DECLARE_IMP png_set_compression_mem_level,0
DECLARE_IMP png_set_compression_strategy,0
DECLARE_IMP png_set_compression_window_bits,0
DECLARE_IMP png_set_compression_method,0
DECLARE_IMP png_init_io,0
DECLARE_IMP png_set_error_fn,0
DECLARE_IMP png_get_error_ptr,0
DECLARE_IMP png_set_write_fn,0
DECLARE_IMP png_set_read_fn,0
DECLARE_IMP png_get_io_ptr,0
DECLARE_IMP png_set_read_status_fn,0
DECLARE_IMP png_set_write_status_fn,0
DECLARE_IMP png_set_read_user_transform_fn,0
DECLARE_IMP png_set_write_user_transform_fn,0
DECLARE_IMP png_set_progressive_read_fn,0
DECLARE_IMP png_get_progressive_ptr,0
DECLARE_IMP png_process_data,0
DECLARE_IMP png_progressive_combine_row,0
DECLARE_IMP png_memcpy_check,0
DECLARE_IMP png_memset_check,0
DECLARE_IMP png_error,0
DECLARE_IMP png_chunk_error,0
DECLARE_IMP png_warning,0
DECLARE_IMP png_chunk_warning,0
DECLARE_IMP png_get_valid,0
DECLARE_IMP png_get_rowbytes,0
DECLARE_IMP png_get_channels,0
DECLARE_IMP png_get_image_width,0
DECLARE_IMP png_get_image_height,0
DECLARE_IMP png_get_bit_depth,0
DECLARE_IMP png_get_color_type,0
DECLARE_IMP png_get_filter_type,0
DECLARE_IMP png_get_interlace_type,0
DECLARE_IMP png_get_compression_type,0
DECLARE_IMP png_get_pixels_per_meter,0
DECLARE_IMP png_get_x_pixels_per_meter,0
DECLARE_IMP png_get_y_pixels_per_meter,0
DECLARE_IMP png_get_pixel_aspect_ratio,0
DECLARE_IMP png_get_x_offset_pixels,0
DECLARE_IMP png_get_y_offset_pixels,0
DECLARE_IMP png_get_x_offset_microns,0
DECLARE_IMP png_get_y_offset_microns,0
DECLARE_IMP png_get_signature,0
DECLARE_IMP png_get_bKGD,0
DECLARE_IMP png_set_bKGD,0
DECLARE_IMP png_get_cHRM,0
DECLARE_IMP png_set_cHRM,0
DECLARE_IMP png_get_gAMA,0
DECLARE_IMP png_set_gAMA,0
DECLARE_IMP png_get_hIST,0
DECLARE_IMP png_set_hIST,0
DECLARE_IMP png_get_IHDR,0
DECLARE_IMP png_set_IHDR,0
DECLARE_IMP png_get_oFFs,0
DECLARE_IMP png_set_oFFs,0
DECLARE_IMP png_get_pCAL,0
DECLARE_IMP png_set_pCAL,0
DECLARE_IMP png_get_pHYs,0
DECLARE_IMP png_set_pHYs,0
DECLARE_IMP png_get_PLTE,0
DECLARE_IMP png_set_PLTE,0
DECLARE_IMP png_get_sBIT,0
DECLARE_IMP png_set_sBIT,0
DECLARE_IMP png_get_sRGB,0
DECLARE_IMP png_set_sRGB,0
DECLARE_IMP png_set_sRGB_gAMA_and_cHRM,0
DECLARE_IMP png_get_text,0
DECLARE_IMP png_set_text,0
DECLARE_IMP png_get_tIME,0
DECLARE_IMP png_set_tIME,0
DECLARE_IMP png_get_tRNS,0
DECLARE_IMP png_set_tRNS,0
END_IMPORTS_DEF

        END
