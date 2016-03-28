/****************************************************************************
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  This file contains the code specific to initialising the
*               SciTech MGL Binary Portable DLL.
*
****************************************************************************/

#include "freetype.h"
#include <stdio.h>
#include <string.h>

#include "scitech.h"
#include "clib/os/imports.h"

/*------------------------- Global Variables ------------------------------*/

/*-------------------------- Implementation -------------------------------*/

/****************************************************************************
PARAMETERS:

RETURNS:
Pointer to exported function list

REMARKS:
This function initialises the PNG image library and returns the list of
loader library exported functions.
{secret}
****************************************************************************/
TT_exports * _CEXPORT TT_initLibrary(void)
{
    static TT_exports _TT_exports = {
        sizeof(TT_exports),
        TT_Init_FreeType,
        TT_Done_FreeType,
        TT_Set_Raster_Gray_Palette,
        TT_Open_Face,
        TT_Open_Collection,
        TT_Get_Face_Properties,
        TT_Set_Face_Pointer,
        TT_Get_Face_Pointer,
        TT_Flush_Face,
        TT_Get_Face_Metrics,
        TT_Close_Face,
        TT_Get_Font_Data,
        TT_New_Instance,
        TT_Set_Instance_Resolutions,
        TT_Set_Instance_CharSize,
        TT_Set_Instance_CharSizes,
        TT_Set_Instance_PixelSizes,
        TT_Set_Instance_Transform_Flags,
        TT_Get_Instance_Metrics,
        TT_Set_Instance_Pointer,
        TT_Get_Instance_Pointer,
        TT_Done_Instance,
        TT_New_Glyph,
        TT_Done_Glyph,
        TT_Load_Glyph,
        TT_Get_Glyph_Outline,
        TT_Get_Glyph_Metrics,
        TT_Get_Glyph_Big_Metrics,
        TT_Get_Glyph_Bitmap,
        TT_Get_Glyph_Pixmap,
        TT_New_Outline,
        TT_Done_Outline,
        TT_Copy_Outline,
        TT_Get_Outline_Bitmap,
        TT_Get_Outline_Pixmap,
        TT_Get_Outline_BBox,
        TT_Transform_Outline,
        TT_Translate_Outline,
        TT_Transform_Vector,
        TT_MulDiv,
        TT_MulFix,
        TT_Get_CharMap_Count,
        TT_Get_CharMap_ID,
        TT_Get_CharMap,
        TT_Char_Index,
        TT_Get_Name_Count,
        TT_Get_Name_ID,
        TT_Get_Name_String
        };

    return &_TT_exports;
}
