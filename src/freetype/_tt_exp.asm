;****************************************************************************
;*
;*                    SciTech SNAP Graphics Architecture
;*
;*               Copyright (C) 1997-20- SciTech Software, Inc.
;*                            All rights reserved.
;*
;*  ======================================================================
;*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
;*  |                                                                    |
;*  |This copyrighted computer code is a proprietary trade secret of     |
;*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
;*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
;*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
;*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
;*  |written authorization from SciTech to possess or use this code, you |
;*  |may be subject to civil and/or criminal penalties.                  |
;*  |                                                                    |
;*  |If you received this code in error or you would like to report      |
;*  |improper use, please immediately contact SciTech Software, Inc. at  |
;*  |530-894-8400.                                                       |
;*  |                                                                    |
;*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
;*  ======================================================================
;*
;* Language:    NASM
;* Environment: IBM PC 32 bit Protected Mode.
;*
;* Description: Module to implement the export stubs for all the X86
;*				emulator library.
;*
;****************************************************************************

include "scitech.mac"           ; Memory model macros

BEGIN_IMPORTS_DEF	_TT_exports
DECLARE_IMP ___TT_Init_FreeType,0	; Handled in C code
DECLARE_IMP ___TT_Done_FreeType,0	; Handled in C code
DECLARE_IMP TT_Set_Raster_Gray_Palette,0
DECLARE_IMP TT_Open_Face,0
DECLARE_IMP TT_Open_Collection,0
DECLARE_IMP TT_Get_Face_Properties,0
DECLARE_IMP TT_Set_Face_Pointer,0
DECLARE_IMP TT_Get_Face_Pointer,0
DECLARE_IMP TT_Flush_Face,0
DECLARE_IMP TT_Get_Face_Metrics,0
DECLARE_IMP TT_Close_Face,0
DECLARE_IMP TT_Get_Font_Data,0
DECLARE_IMP TT_New_Instance,0
DECLARE_IMP TT_Set_Instance_Resolutions,0
DECLARE_IMP TT_Set_Instance_CharSize,0
DECLARE_IMP TT_Set_Instance_CharSizes,0
DECLARE_IMP TT_Set_Instance_PixelSizes,0
DECLARE_IMP TT_Set_Instance_Transform_Flags,0
DECLARE_IMP TT_Get_Instance_Metrics,0
DECLARE_IMP TT_Set_Instance_Pointer,0
DECLARE_IMP TT_Get_Instance_Pointer,0
DECLARE_IMP TT_Done_Instance,0
DECLARE_IMP TT_New_Glyph,0
DECLARE_IMP TT_Done_Glyph,0
DECLARE_IMP TT_Load_Glyph,0
DECLARE_IMP TT_Get_Glyph_Outline,0
DECLARE_IMP TT_Get_Glyph_Metrics,0
DECLARE_IMP TT_Get_Glyph_Big_Metrics,0
DECLARE_IMP TT_Get_Glyph_Bitmap,0
DECLARE_IMP TT_Get_Glyph_Pixmap,0
DECLARE_IMP TT_New_Outline,0
DECLARE_IMP TT_Done_Outline,0
DECLARE_IMP TT_Copy_Outline,0
DECLARE_IMP TT_Get_Outline_Bitmap,0
DECLARE_IMP TT_Get_Outline_Pixmap,0
DECLARE_IMP TT_Get_Outline_BBox,0
DECLARE_IMP TT_Transform_Outline,0
DECLARE_IMP TT_Translate_Outline,0
DECLARE_IMP TT_Transform_Vector,0
DECLARE_IMP TT_MulDiv,0
DECLARE_IMP TT_MulFix,0
DECLARE_IMP TT_Get_CharMap_Count,0
DECLARE_IMP TT_Get_CharMap_ID,0
DECLARE_IMP TT_Get_CharMap,0
DECLARE_IMP TT_Char_Index,0
DECLARE_IMP TT_Get_Name_Count,0
DECLARE_IMP TT_Get_Name_ID,0
DECLARE_IMP TT_Get_Name_String,0
END_IMPORTS_DEF

		END
