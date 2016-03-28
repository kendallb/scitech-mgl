/****************************************************************************
*
*                     SciTech SNAP Graphics Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Module to implement a simple Portable Binary DLL loader
*               library. This library can be used to load PE DLL's under
*               any Intel based OS, provided the DLL's do not have any
*               imports in the import table.
*
*               NOTE: This loader module expects the DLL's to be built with
*                     Watcom C++ and may produce unexpected results with
*                     DLL's linked by another compiler.
*
****************************************************************************/

#include "clib/peloader.h"
#include "pmapi.h"
#include "clib/os/os.h"
#include "clib/os/init.h"
#include "clib/pe.h"

/*--------------------------- Global variables ----------------------------*/

static int  result = PE_ok;

/*------------------------- Implementation --------------------------------*/

#if defined(__INTEL__) && defined(CHECKED)

#define WATCOM_DEBUG_SYMBOLS

#if defined(__WATCOMC__)

/* Variable used to determine if the debugger is present */

extern char volatile __WD_Present;

#else

/* Variable used to determine if the debugger is present */

char volatile __WD_Present = 0;

#endif

/* External assembler function to enter the debugger and pass a message */

extern void _ASMAPI _PE_enterDebuggerWithMessage( const char * );

/* External assembler to get CS selector */

extern ushort _ASMAPI _PE_getCS(void);

/* Messages to load debug symbols */

#define DEBUGGER_LOADMODULE_COMMAND "!LOADMODULE "
#define DEBUGGER_LOADMODULE_FORMAT DEBUGGER_LOADMODULE_COMMAND "0x%4.4x:0x%8.8lx,%s"

/* Messages to unload debug symbols */

#define DEBUGGER_UNLOADMODULE_COMMAND "!UNLOADMODULE "
#define DEBUGGER_UNLOADMODULE_FORMAT DEBUGGER_UNLOADMODULE_COMMAND "%s"

/****************************************************************************
DESCRIPTION:
Notify the Open Watcom debugger of module load events. WD will attempt
to load symbolic debugging information for the module much like it would for
OS loaded DLLs.
****************************************************************************/
static void NotifyWDLoad(
    char *modname,
    unsigned long offset)
{
    char buf[PM_MAX_PATH + sizeof(DEBUGGER_LOADMODULE_COMMAND) + 2+4+1+8+1+1];
    sprintf(buf, DEBUGGER_LOADMODULE_FORMAT, _PE_getCS(), offset, modname );
    if (__WD_Present)
        _PE_enterDebuggerWithMessage(buf);
}

/****************************************************************************
DESCRIPTION:
Notify the Open Watcom debugger of module unload events.
****************************************************************************/
static void NotifyWDUnload(
    char *modname)
{
    char buf[PM_MAX_PATH + sizeof(DEBUGGER_UNLOADMODULE_COMMAND) + 1];
    sprintf(buf, DEBUGGER_UNLOADMODULE_FORMAT, modname);
    if (__WD_Present)
        _PE_enterDebuggerWithMessage(buf);
}
#endif

/* Special functions needed to read into high memory on OS/2 */
#if defined(__OS2__) && !defined(__DRIVER__)

/* Read via intermediate buffer on the stack. NB: This function is
 * only required for reads that may go to high memory
 */
static size_t PE_fread( void *ptr, size_t size, size_t n, FILE *fp )
{
    char    buf[4096];
    size_t  len;
    size_t  res;
    size_t  read = 0;
    char    *p;

    len = size * n;
    p = ptr;
    while (len > sizeof(buf)) {
        res = fread(buf, 1, sizeof(buf), fp);
        read += res;
        if (res < sizeof(buf))
            return read;
        memcpy(p, buf, sizeof(buf));
        p   += sizeof(buf);
        len -= sizeof(buf);
        }
    if (len) {
        res = fread(buf, 1, len, fp);
        read += res;
        if (res < len)
            return read;
        memcpy(p, buf, len);
        }
    return read;
}

#else

/* Use regular fread() */
#define PE_fread    fread

#endif
/****************************************************************************
PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular PE_loadLibrary except that it take a handle to an
open file and an offset within that file for the DLL to load.
****************************************************************************/
static int PE_readHeader(
    FILE *f,
    long startOffset,
    FILE_HDR *filehdr,
    OPTIONAL_HDR *opthdr)
{
    EXE_HDR exehdr;
    ulong   offset,signature;

    /* Read the EXE header and check for valid header signature */
    result = PE_invalidDLLImage;
    fseek(f, startOffset, SEEK_SET);
    if (fread(&exehdr, 1, sizeof(exehdr), f) != sizeof(exehdr))
        return false;
    if (exehdr.signature != 0x5A4D)
        return false;

    /* Now seek to the start of the PE header defined at offset 0x3C
     * in the MS-DOS EXE header, and read the signature and check it.
     */
    fseek(f, startOffset+0x3C, SEEK_SET);
    if (fread(&offset, 1, sizeof(offset), f) != sizeof(offset))
        return false;
    fseek(f, startOffset+offset, SEEK_SET);
    if (fread(&signature, 1, sizeof(signature), f) != sizeof(signature))
        return false;
    if (signature != 0x00004550)
        return false;

    /* Now read the PE file header and check that it is correct */
    if (fread(filehdr, 1, sizeof(*filehdr), f) != sizeof(*filehdr))
        return false;
    if (filehdr->Machine != IMAGE_FILE_MACHINE_I386)
        return false;
    if (!(filehdr->Characteristics & IMAGE_FILE_32BIT_MACHINE))
        return false;
    if (!(filehdr->Characteristics & IMAGE_FILE_DLL))
        return false;
    if (fread(opthdr, 1, sizeof(*opthdr), f) != sizeof(*opthdr))
        return false;
    if (opthdr->Magic != 0x10B)
        return false;

    /* Success, so return true! */
    return true;
}

/****************************************************************************
DESCRIPTION:
Find the actual size of a PE file image

HEADER:
clib/peloader.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file

RETURNS:
Size of the DLL file on disk, or -1 on error

REMARKS:
This function scans the headers for a Portable Binary DLL to determine the
length of the DLL file on disk.
****************************************************************************/
ulong PEAPI PE_getFileSize(
    FILE *f,
    ulong startOffset)
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    ulong           size;
    int             i;

    /* Read the PE file headers from disk */
    if (!PE_readHeader(f,startOffset,&filehdr,&opthdr))
        return 0xFFFFFFFF;

    /* Scan all the section headers summing up the total size */
    size = opthdr.SizeOfHeaders;
    for (i = 0; i < filehdr.NumberOfSections; i++) {
        if (fread(&secthdr, 1, sizeof(secthdr), f) != sizeof(secthdr))
            return 0xFFFFFFFF;
        if (!(secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
            size += secthdr.SizeOfRawData;
        }
    return size;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory from an open file

HEADER:
clib/peloader.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file
size        - Place to store the size of the driver loaded
shared      - True to load module into shared memory

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular PE_loadLibrary except that it take a handle to an
open file and an offset within that file for the DLL to load.

SEE ALSO:
PE_loadLibrary, PE_getProcAddress, PE_freeLibrary
****************************************************************************/
PE_MODULE * PEAPI PE_loadLibraryExt(
    FILE *f,
    ulong startOffset,
    ulong *size,
    ibool shared)
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    ulong           offset,pageOffset;
    ulong           text_raw_off,text_base,text_size,text_end;
    ulong           data_raw_off,data_base,data_size,data_virt_size,data_end;
    ulong           bss_raw_off,bss_base,bss_size,bss_end;
    ulong           import_raw_off,import_base,import_size,import_end;
    ulong           export_raw_off,export_base,export_size,export_end;
    ulong           reloc_raw_off,reloc_base= 0,reloc_size;
    ulong           image_base = 0,image_size,image_end;
    uchar           *image_ptr;
    int             i,delta,numFixups;
    ushort          relocType,*fixup;
    PE_MODULE       *hMod = NULL;
    void            *reloc = NULL;
    BASE_RELOCATION *baseReloc;
    InitLibC_t      InitLibC;

    /* Read the PE file headers from disk */
    if (!PE_readHeader(f,startOffset,&filehdr,&opthdr))
        return NULL;

    /* Scan all the section headers and find the necessary sections */
    text_raw_off = text_base = text_size = text_end = 0;
    data_raw_off = data_base = data_size = data_virt_size = data_end = 0;
    bss_raw_off = bss_base = bss_size = bss_end = 0;
    import_raw_off = import_base = import_size = import_end = 0;
    export_raw_off = export_base = export_size = export_end = 0;
    reloc_raw_off = reloc_size = 0;
    for (i = 0; i < filehdr.NumberOfSections; i++) {
        if (fread(&secthdr, 1, sizeof(secthdr), f) != sizeof(secthdr))
            goto Error;
        if (i == 0)
            image_base = secthdr.VirtualAddress;
        if (strcmp(secthdr.Name, ".edata") == 0 || strcmp(secthdr.Name, ".rdata") == 0) {
            /* Exports section */
            export_raw_off = secthdr.PointerToRawData;
            export_base = secthdr.VirtualAddress;
            export_size = secthdr.SizeOfRawData;
            export_end = export_base + export_size;
            }
        else if (strcmp(secthdr.Name, ".idata") == 0) {
            /* Imports section */
            import_raw_off = secthdr.PointerToRawData;
            import_base = secthdr.VirtualAddress;
            import_size = secthdr.SizeOfRawData;
            import_end = import_base + import_size;
            }
        else if (strcmp(secthdr.Name, ".reloc") == 0) {
            /* Relocations section */
            reloc_raw_off = secthdr.PointerToRawData;
            reloc_base = secthdr.VirtualAddress;
            reloc_size = secthdr.SizeOfRawData;
            }
        else if (!text_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_CODE) {
            /* Code section */
            text_raw_off = secthdr.PointerToRawData;
            text_base = secthdr.VirtualAddress;
            text_size = secthdr.SizeOfRawData;
            text_end = text_base + text_size;
            }
        else if (!data_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            /* Data section */
            data_raw_off = secthdr.PointerToRawData;
            data_base = secthdr.VirtualAddress;
            data_size = secthdr.SizeOfRawData;
            data_virt_size = secthdr.VirtualSize;
            data_end = data_base + data_size;
            }
        else if (!bss_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            /* BSS data section */
            bss_raw_off = secthdr.PointerToRawData;
            bss_base = secthdr.VirtualAddress;
            bss_size = secthdr.SizeOfRawData;
            bss_end = bss_base + bss_size;
            }
        }

    /* Check to make sure that we have all the sections we need */
    if (!text_raw_off || !data_raw_off || !export_raw_off || !reloc_raw_off) {
        result = PE_invalidDLLImage;
        goto Error;
        }

    /* Make sure the .reloc section is after everything else we load! */
    image_end = MAX(MAX(MAX(MAX(bss_end,data_end),text_end),import_end),export_end);
    if (reloc_base <= image_end) {
        result = PE_unknownImageFormat;
        goto Error;
        }

    /* Find the size of the image sections to load and allocate memory for
     * them. We only load section data up to the .reloc section, and then
     * ignore everything else after that (eventually we might need to handle
     * the .rsrc section separately).
     */
    image_size = image_end - image_base;
    *size = sizeof(PE_MODULE) + image_size + 4096;
    if (shared)
        hMod = PM_mallocShared(*size);
    else
        hMod = PM_malloc(*size);
    reloc = PM_malloc(reloc_size);
    if (!hMod || !reloc) {
        result = PE_outOfMemory;
        goto Error;
        }

    /* Setup all the pointers into our loaded executeable image */
    image_ptr = (uchar*)ROUND_4K((ulong)hMod + sizeof(PE_MODULE));
    hMod->pbase = image_ptr;
    hMod->ptext = image_ptr + (text_base - image_base);
    hMod->pdata = image_ptr + (data_base - image_base);
    if (bss_base)
        hMod->pbss = image_ptr + (bss_base - image_base);
    else
        hMod->pbss = NULL;
    if (import_base)
        hMod->pimport = image_ptr + (import_base - image_base);
    else
        hMod->pimport = NULL;
    hMod->pexport = image_ptr + (export_base - image_base);
    hMod->textBase = text_base;
    hMod->dataBase = data_base;
    hMod->bssBase = bss_base;
    hMod->importBase = import_base;
    hMod->exportBase = export_base;
    hMod->exportDir = opthdr.DataDirectory[0].RelVirtualAddress - export_base;
    hMod->shared = shared;
    hMod->modname = NULL;

    /* Now read the section images from disk */
    result = PE_invalidDLLImage;
    fseek(f, startOffset+text_raw_off, SEEK_SET);
    if (PE_fread(hMod->ptext, 1, text_size, f) != text_size)
        goto Error;
    fseek(f, startOffset+data_raw_off, SEEK_SET);
    if (data_virt_size) {
        /* Some linkers will put uninitalised data at the end
         * of the primary data section, so we first must clear
         * the data section to zeros for the entire length of
         * VirtualSize, which can be longer than the size on disk.
         * Note also that some linkers set this value to zero, so
         * we ignore this value in that case (those linkers also
         * have a seperate BSS section).
         */
        memset(hMod->pdata, 0, data_virt_size);
        }
    if (PE_fread(hMod->pdata, 1, data_size, f) != data_size)
        goto Error;
    if (import_base) {
        fseek(f, startOffset+import_raw_off, SEEK_SET);
        if (PE_fread(hMod->pimport, 1, import_size, f) != import_size)
            goto Error;
        }
    fseek(f, startOffset+export_raw_off, SEEK_SET);
    if (PE_fread(hMod->pexport, 1, export_size, f) != export_size)
        goto Error;
    fseek(f, startOffset+reloc_raw_off, SEEK_SET);
    if (PE_fread(reloc, 1, reloc_size, f) != reloc_size)
        goto Error;

    /* Make sure the BSS section is cleared to zero if it exists */
    if (hMod->pbss)
        memset(hMod->pbss, 0, bss_size);

    /* Now perform relocations on all sections in the image */
    delta = (ulong)image_ptr - opthdr.ImageBase - image_base;
    baseReloc = (BASE_RELOCATION*)reloc;
    for (;;) {
        /* Check for termination condition */
        if (!baseReloc->PageRVA || !baseReloc->BlockSize)
            break;

        /* Do fixups */
        numFixups = (baseReloc->BlockSize - sizeof(BASE_RELOCATION)) / sizeof(ushort);
        fixup = (ushort*)(baseReloc + 1);
        pageOffset = baseReloc->PageRVA - image_base;
        for (i = 0; i < numFixups; i++) {
            relocType = *fixup >> 12;
            if (relocType) {
                offset = pageOffset + (*fixup & 0x0FFF);
                *(ulong*)(image_ptr + offset) += delta;
                }
            fixup++;
            }

        /* Move to next relocation block */
        baseReloc = (BASE_RELOCATION*)((ulong)baseReloc + baseReloc->BlockSize);
        }

    /* Initialise the C runtime library for the loaded DLL */
    result = PE_unableToInitLibC;
    if ((InitLibC = (InitLibC_t)PE_getProcAddress(hMod,"_InitLibC")) == NULL)
        goto Error;

    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    if (!PM_memProtect((void *)image_ptr, image_size,
        PM_MPROT_READ | PM_MPROT_WRITE | PM_MPROT_EXEC))
        goto Error;

    /* Due to the fact that the C runtime library init code will do
     * some floating point init code, we need to make sure we save/restore
     * the floating point state across this call for environments where it
     * is unsafe to do floating point without this (Windows 9x VxD's and
     * Windows NT/2K/XP device drivers).
     */
    PM_saveFPUState();

    /* Call the C library init code */
#ifdef __DRIVER__
    if (!InitLibC(&___imports,PM_getOSType()))
        goto Error;
#else
    if (!InitLibC(shared ? &___imports_shared : &___imports,PM_getOSType()))
        goto Error;
#endif

    /* Restore the floating point state */
    PM_restoreFPUState();

    /* Save the size in the module handler */
    hMod->size = *size;

    /* Clean up, close the file and return the loaded module handle */
    PM_free(reloc);
    result = PE_ok;
    return hMod;

Error:
    if (shared)
        PM_freeShared(hMod);
    else
        PM_free(hMod);
    PM_free(reloc);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory

HEADER:
clib/peloader.h

PARAMETERS:
szDLLName   - Name of the PE DLL library to load
shared      - True to load module into shared memory

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function
will only work on DLL's that do not have any imports, since we don't
resolve pimport dependencies in this function.

SEE ALSO:
PE_getProcAddress, PE_freeLibrary
****************************************************************************/
PE_MODULE * PEAPI PE_loadLibrary(
    const char *szDLLName,
    ibool shared)
{
    PE_MODULE   *hMod;
    FILE        *f;
    ulong       size;

    /* Attempt to open the file on disk */
    if (shared < 0)
        shared = 0;
    if ((f = fopen(szDLLName,"rb")) == NULL) {
        result = PE_fileNotFound;
        return NULL;
        }
    hMod = PE_loadLibraryExt(f,0,&size,shared);
    fclose(f);

    /* Notify the Watcom Debugger of module load and let it load symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
    if (hMod) {
        ulong   size;
        char    *modname;

        /* Store the file name in the hMod structure; this must be the real
         * file name where the debugger will try to load symbolic info from
         */
        size = strlen(szDLLName) + 1;
        if (shared)
            modname = PM_mallocShared(size);
        else
            modname = PM_malloc(size);
        if (modname) {
            if (szDLLName[1] == ':')
                strcpy(modname, szDLLName+2);
            else
                strcpy(modname, szDLLName);
            hMod->modname = modname;
            NotifyWDLoad(hMod->modname, (ulong)hMod->pbase);
            }
        }
#endif
    return hMod;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory

HEADER:
clib/peloader.h

PARAMETERS:
szDLLName   - Name of the PE DLL library to load
shared      - True to load module into shared memory

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function is the same as the regular PE_loadLibrary function, except
that it looks for the drivers in the MGL_ROOT/drivers directory or a
/drivers directory relative to the current directory.

SEE ALSO:
PE_loadLibraryMGL, PE_getProcAddress, PE_freeLibrary
****************************************************************************/
PE_MODULE * PEAPI PE_loadLibraryMGL(
    const char *szDLLName,
    ibool shared)
{
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__) && !defined(__WINCE__)
    PE_MODULE   *hMod;
#endif
    char        path[256] = "";

    /* We look in the 'drivers' directory, optionally under the MGL_ROOT
     * environment variable directory.
     */
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__) && !defined(__WINCE__)
    if (getenv("MGL_ROOT")) {
        strcpy(path,getenv("MGL_ROOT"));
        PM_backslash(path);
        }
    strcat(path,"drivers");
    PM_backslash(path);
    strcat(path,szDLLName);
    if ((hMod = PE_loadLibrary(path,shared)) != NULL)
        return hMod;
#endif
    strcpy(path,"drivers");
    PM_backslash(path);
    strcat(path,szDLLName);
    return PE_loadLibrary(path,shared);
}

/****************************************************************************
DESCRIPTION:
Gets a function address from a Portable Binary DLL

HEADER:
clib/peloader.h

PARAMETERS:
hModule     - Handle to a loaded PE DLL library
szProcName  - Name of the function to get the address of

RETURNS:
Pointer to the function, or NULL on failure.

REMARKS:
This function searches for the named, exported function in a loaded PE
DLL library, and returns the address of the function. If the function is
not found in the library, this function return NULL.

SEE ALSO:
PE_loadLibrary, PE_freeLibrary
****************************************************************************/
void * PEAPI PE_getProcAddress(
    PE_MODULE *hModule,
    const char *szProcName)
{
    uint                i;
    EXPORT_DIRECTORY    *exports;
    ulong               funcOffset;
    ulong               *AddressTable;
    ulong               *NameTable;
    ushort              *OrdinalTable;
    char                *name;

    /* Find the address of the pexport tables from the pexport section */
    if (!hModule)
        return NULL;
    exports = (EXPORT_DIRECTORY*)(hModule->pexport + hModule->exportDir);
    AddressTable = (ulong*)(hModule->pexport + exports->AddressTableRVA - hModule->exportBase);
    NameTable = (ulong*)(hModule->pexport + exports->NameTableRVA - hModule->exportBase);
    OrdinalTable = (ushort*)(hModule->pexport + exports->OrdinalTableRVA - hModule->exportBase);

    /* Search the pexport name table to find the function name */
    for (i = 0; i < exports->NumberOfNamePointers; i++) {
        name = (char*)(hModule->pexport + NameTable[i] - hModule->exportBase);
        if (strcmp(name,szProcName) == 0)
            break;
        }
    if (i == exports->NumberOfNamePointers)
        return NULL;
    funcOffset = AddressTable[OrdinalTable[i]];
    if (!funcOffset)
        return NULL;
    return (void*)(hModule->ptext + funcOffset - hModule->textBase);
}

/****************************************************************************
DESCRIPTION:
Frees a loaded Portable Binary DLL

HEADER:
clib/peloader.h

PARAMETERS:
hModule     - Handle to a loaded PE DLL library to free

REMARKS:
This function frees a loaded PE DLL library from memory.

SEE ALSO:
PE_getProcAddress, PE_loadLibrary
****************************************************************************/
void PEAPI PE_freeLibrary(
    PE_MODULE *hModule)
{
    TerminateLibC_t TerminateLibC;

    if (hModule) {
        /* Run the C runtime library exit code on module unload */
        if ((TerminateLibC = (TerminateLibC_t)PE_getProcAddress(hModule,"_TerminateLibC")) != NULL)
            TerminateLibC();

        /* Notify the Watcom Debugger of module load and let it remove symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
        if (hModule->modname) {
            NotifyWDUnload(hModule->modname);
            if (hModule->shared)
                PM_freeShared(hModule->modname);
            else
                PM_free(hModule->modname);
            }
#endif
        if (hModule->shared)
            PM_freeShared(hModule);
        else
            PM_free(hModule);
        }
}

/****************************************************************************
DESCRIPTION:
Returns the error code for the last operation

HEADER:
clib/peloader.h

RETURNS:
Error code for the last operation.

SEE ALSO:
PE_getProcAddress, PE_loadLibrary
****************************************************************************/
int PEAPI PE_getError(void)
{
    return result;
}

