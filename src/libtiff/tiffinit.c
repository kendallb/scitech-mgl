/****************************************************************************
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  This file contains the code specific to initialising the
*               SciTech MGL Binary Portable DLL.
*
****************************************************************************/

#include "tiffiop.h"
#include <stdio.h>
#include <string.h>

/*------------------------- Global Variables ------------------------------*/

PM_imports _VARAPI      _PM_imports;
TIFF_imports _VARAPI    _TIFF_imports;

#if 0
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
Fatal error handler for non-imported TIFF_imports.
****************************************************************************/
static void _TIFF_fatalErrorHandler(void)
{
    _PM_imports.PM_fatalError("Unsupported TIFF_imports import function called! Please re-compile!\n");
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
TIFF_exports * _CEXPORT TIFF_initLibrary(
    PM_imports *pmImp,
    TIFF_imports *tiffImp)
{
    static TIFF_exports _TIFF_exports = {
        sizeof(TIFF_exports),
        TIFFGetVersion,
        TIFFFindCODEC,
        TIFFRegisterCODEC,
        TIFFUnRegisterCODEC,
        TIFFClose,
        TIFFFlush,
        TIFFFlushData,
        TIFFGetField,
        TIFFVGetField,
        TIFFGetFieldDefaulted,
        TIFFVGetFieldDefaulted,
        TIFFReadDirectory,
        TIFFScanlineSize,
        TIFFRasterScanlineSize,
        TIFFStripSize,
        TIFFVStripSize,
        TIFFTileRowSize,
        TIFFTileSize,
        TIFFVTileSize,
        TIFFDefaultStripSize,
        TIFFDefaultTileSize,
        TIFFFileno,
        TIFFGetMode,
        TIFFIsTiled,
        TIFFIsByteSwapped,
        TIFFIsUpSampled,
        TIFFIsMSB2LSB,
        TIFFCurrentRow,
        TIFFCurrentDirectory,
        TIFFCurrentDirOffset,
        TIFFCurrentStrip,
        TIFFCurrentTile,
        TIFFReadBufferSetup,
        TIFFWriteBufferSetup,
        TIFFLastDirectory,
        TIFFSetDirectory,
        TIFFSetSubDirectory,
        TIFFUnlinkDirectory,
        TIFFSetField,
        TIFFVSetField,
        TIFFWriteDirectory,
        TIFFPrintDirectory,
        TIFFReadScanline,
        TIFFWriteScanline,
        TIFFReadRGBAImage,
        TIFFRGBAImageOK,
        TIFFRGBAImageBegin,
        TIFFRGBAImageGet,
        TIFFRGBAImageEnd,
        TIFFOpen,
        TIFFFdOpen,
        TIFFClientOpen,
        TIFFFileName,
        TIFFError,
        TIFFWarning,
        TIFFSetErrorHandler,
        TIFFSetWarningHandler,
        TIFFSetTagExtender,
        TIFFComputeTile,
        TIFFCheckTile,
        TIFFNumberOfTiles,
        TIFFReadTile,
        TIFFWriteTile,
        TIFFComputeStrip,
        TIFFNumberOfStrips,
        TIFFReadEncodedStrip,
        TIFFReadRawStrip,
        TIFFReadEncodedTile,
        TIFFReadRawTile,
        TIFFWriteEncodedStrip,
        TIFFWriteRawStrip,
        TIFFWriteEncodedTile,
        TIFFWriteRawTile,
        TIFFSetWriteOffset,
        TIFFSwabShort,
        TIFFSwabLong,
        TIFFSwabDouble,
        TIFFSwabArrayOfShort,
        TIFFSwabArrayOfLong,
        TIFFSwabArrayOfDouble,
        TIFFReverseBits,
        TIFFGetBitRevTable,
        };
    int     i,max;
    ulong   *p;

    /* Initialize all default imports to point to fatal error handler
     * for upwards compatibility.
     */
    max = sizeof(_PM_imports)/sizeof(TIFF_initLibrary_t);
    for (i = 0,p = (ulong*)&_PM_imports; i < max; i++)
        *p++ = (ulong)_PM_fatalErrorHandler;
    max = sizeof(_TIFF_imports)/sizeof(TIFF_initLibrary_t);
    for (i = 0,p = (ulong*)&_TIFF_imports; i < max; i++)
        *p++ = (ulong)_TIFF_fatalErrorHandler;

    /* Now copy all our imported functions */
    memcpy(&_PM_imports,pmImp,MIN(sizeof(_PM_imports),pmImp->dwSize));
    memcpy(&_TIFF_imports,tiffImp,MIN(sizeof(_TIFF_imports),tiffImp->dwSize));
    return &_TIFF_exports;
}
#endif
