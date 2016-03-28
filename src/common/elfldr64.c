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
* Description:  Module to implement a simple 64-bit ELF module loader library.
*               This library can be used to load 64-bit ELF modules under
*               any supported OS, provided the modules do not have any
*               imports in the import table.
*
****************************************************************************/

#include <string.h>
#include "clib/elf.h"
#include "clib/elfloadr.h"
#include "pmapi.h"
#include "clib/os/os.h"
#include "clib/os/init.h"
#ifdef __LINUX__
#include <sys/mman.h>
#include <errno.h>
#endif

/*--------------------------- Global variables ----------------------------*/

static int  result = ELF_ok;

/*------------------------- Implementation --------------------------------*/

#if defined(__PPC__)

/* Function to synchronize data and instruction caches on PowerPC; note
 * that it only affects one cache block.
 */
static __inline__ void ppc_sync_caches(size_t addr)
{
	__asm__ volatile (
		"dcbf 0,%0;" /* flush D-cache */
		"icbi 0,%0;" /* invalidate I-cache */
		"sync;"      /* synchronize */
		"isync;"
		: : "r"(addr) : "memory");
}

static void cache_sync_region(void *base, size_t size)
{
    size_t  offset, address;

    address = (size_t)base;
    for (offset = address; offset < address + size; offset += 16) {
        ppc_sync_caches(offset);
    }
}
#endif

#if defined(__WATCOMC__) && defined(__INTEL__) && defined(CHECKED)

#define WATCOM_DEBUG_SYMBOLS

/* Variable used to determine if the debugger is present */

extern char volatile __WD_Present;

/* Macro to enter the debugger and pass a message */

void EnterDebuggerWithMessage( const char __far * );
#pragma aux EnterDebuggerWithMessage parm caller [] = \
        "int 3" \
        "jmp short L1" \
        'W' 'V' 'I' 'D' 'E' 'O' \
        "L1:"

/* Inline assembler to get DS selector */

ushort GetCS(void);
#pragma aux GetCS parm caller [] = \
        "mov ax,cs"

/* Messages to load debug symbols */

#define DEBUGGER_LOADMODULE_COMMAND "!LOADMODULE "
#define DEBUGGER_LOADMODULE_FORMAT DEBUGGER_LOADMODULE_COMMAND "0x%4.4x:0x%8.8x,%s"

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
    sprintf(buf, DEBUGGER_LOADMODULE_FORMAT, GetCS(), offset, modname );
    if (__WD_Present)
        EnterDebuggerWithMessage(buf);
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
        EnterDebuggerWithMessage(buf);
}
#endif

/****************************************************************************
PARAMETERS:
f           - Handle to open file to read module from
startOffset - Offset to the start of the module within the file
filehdr     - Pointer to ELF header structure that will be read

RETURNS:
Handle to loaded ELF module, or NULL on failure.

REMARKS:
This function loads an ELF shared library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular ELF_loadLibrary except that it takes a handle to an
open file and an offset within that file for the module to load.
****************************************************************************/
static int ELF_readHeader(
    FILE *f,
    long startOffset,
    Elf64_Ehdr *elfhdr)
{
    /* Read the ELF header and check for valid header signature */
    result = ELF_invalidImage;
    fseek(f, startOffset, SEEK_SET);
    if (fread(elfhdr, 1, sizeof(*elfhdr), f) != sizeof(*elfhdr))
        return false;
    if ((elfhdr->e_ident[EI_MAG0] != ELFMAG0) ||
        (elfhdr->e_ident[EI_MAG1] != ELFMAG1) ||
        (elfhdr->e_ident[EI_MAG2] != ELFMAG2) ||
        (elfhdr->e_ident[EI_MAG3] != ELFMAG3))
        return false;

    /* Now check that the header is what we're expecting */
#if defined(__BIG_ENDIAN__)
    if (elfhdr->e_ident[EI_DATA] != ELFDATA2MSB)
        return false;
#else
    if (elfhdr->e_ident[EI_DATA] != ELFDATA2LSB)
        return false;
#endif
    if (elfhdr->e_ident[EI_CLASS] != ELFCLASS64)
        return false;
    if (elfhdr->e_ident[EI_VERSION] != EV_CURRENT)
        return false;
    if ((elfhdr->e_type != ET_DYN) && (elfhdr->e_type != ET_EXEC))
        return false;
#if defined(__X86_64__)
    if (elfhdr->e_machine != EM_X86_64)
        return false;
#elif defined(__PPC__)
    if (elfhdr->e_machine != EM_PPC)
        return false;
#else
    #error Unsupported platform!
#endif
    if (elfhdr->e_phoff == 0)
        return false;
    if (elfhdr->e_shoff == 0)
        return false;
    if (elfhdr->e_phentsize != sizeof(Elf64_Phdr))
        return false;
    if (elfhdr->e_shentsize != sizeof(Elf64_Shdr))
        return false;
    /* Success, so return true! */
    return true;
}

/****************************************************************************
DESCRIPTION:
Find the actual size of an ELF file image

HEADER:
clib/elfloadr.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file

RETURNS:
Size of the module file on disk, or -1 on error

REMARKS:
This function scans the headers for a Portable Binary module to determine
the length of the module file on disk.
****************************************************************************/
ulong ELFAPI ELF_getFileSize(
    FILE *f,
    ulong startOffset)
{
    Elf64_Ehdr      elfhdr;
    Elf64_Off       sh_size;

    /* Read the ELF header from disk */
    if (!ELF_readHeader(f, startOffset, &elfhdr))
        return -1;

    /* Assume that the section headers are at the end of the image (as they
     * should) and there is no extra data following them.
     */
    sh_size = elfhdr.e_shoff + elfhdr.e_shentsize * elfhdr.e_shnum;
    return sh_size;
}

#if defined(__X86_64__)

/****************************************************************************
PARAMETERS:
f           - Handle to open file to read module from
startOffset - Offset to the start of the module within the file
filehdr     - Pointer to ELF header structure that will be read
hMod        - Pointer to loaded ELF module data

RETURNS:
Handle to loaded ELF module, or NULL on failure.

REMARKS:
This function applies relocations to an ELF image loaded in memory. The
relocations are processor specific, this is the Intel x86 version.
****************************************************************************/
static int ELF_performRelocs(
    FILE *f,
    long startOffset,
    Elf64_Ehdr *elfhdr,
    Elf64_Shdr *sh_ptr,
    ELF_MODULE *hMod)
{
    Elf64_Shdr  *secthdr;
    Elf64_Rela  *reloca, *rela_ptr;
    Elf64_Sym   *symbol, *sym_table;
    ulong       reloc_size, sec_vaddr;
    int         i, j;

    /* Scan and process relocation sections */
    for (i = 0, secthdr = sh_ptr; i < elfhdr->e_shnum; i++, secthdr++) {
        switch (secthdr->sh_type) {
            case SHT_REL:
                break;
            case SHT_RELA:
                reloc_size = secthdr->sh_size;
                sec_vaddr  = sh_ptr[secthdr->sh_info].sh_addr;
                if (sh_ptr[secthdr->sh_link].sh_type == SHT_DYNSYM)
                    sym_table = (Elf64_Sym*)hMod->dynsym;
                else
                    sym_table = (Elf64_Sym*)hMod->symtab;

                rela_ptr = (Elf64_Rela*)PM_malloc(reloc_size);
                if (!rela_ptr)
                    return false;
                fseek(f, startOffset + secthdr->sh_offset, SEEK_SET);
                if (fread(rela_ptr, 1, secthdr->sh_size, f) != secthdr->sh_size) {
                    PM_free(rela_ptr);
                    return false;
                    }
                for (j = 0, reloca = rela_ptr; j < reloc_size / sizeof(Elf64_Rela); j++, reloca++) {
                    int     symtype, symidx;
                    ulong   mem_addr;
                    ulong   *mem_ptr64;
                    uint    *mem_ptr32;
                    ushort  *mem_ptr16;
                    ulong   A, B, P, S;

                    symtype  = ELF64_R_TYPE(reloca->r_info);
                    symidx   = ELF64_R_SYM(reloca->r_info);
                    mem_addr = (ulong)hMod->pbase + reloca->r_offset;
                    mem_ptr64 = (ulong*)mem_addr;
                    mem_ptr32 = (uint*)mem_addr;
                    mem_ptr16 = (ushort*)mem_addr;
                    symbol    = sym_table + symidx;
                    A = reloca->r_addend;
                    B = (ulong)hMod->pbase;
                    P = (ulong)hMod->pbase + reloca->r_offset;
                    /* Workaround for gcc weirdness */
                    if (ELF64_ST_TYPE(symbol->st_info) == STT_SECTION)
                        S = (ulong)hMod->pbase;
                    else
                        S = hMod->pbase - hMod->vaddr + symbol->st_value;

                    switch (symtype) {
                        case R_X86_64_64:
                            *mem_ptr64 = S + A;
                            break;
                        case R_X86_64_PC32:
                            *mem_ptr32 = S + A - P;
                            break;
                        case R_X86_64_GLOB_DAT:
                        case R_X86_64_JUMP_SLOT:
                            *mem_ptr64 = S;
                            break;
                        case R_X86_64_RELATIVE:
                            *mem_ptr64 = B + A;
                            break;
                        case R_X86_64_NONE:
                        case R_X86_64_COPY:
                            break;
                        default:
                            { __asm__ __volatile__ ("int $3"); }
                            break;
                        }
                    }
                PM_free(rela_ptr);
                break;
            default:
                break;
            }
        }
    return false;
}

#else
    #error "Target platform not yet supported"
#endif

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary module into memory from an open file

HEADER:
clib/elfloadr.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file
size        - Place to store the size of the driver loaded
shared      - True to load module into shared memory

RETURNS:
Handle to loaded ELF module, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular PE_loadLibrary except that it take a handle to an
open file and an offset within that file for the DLL to load.

SEE ALSO:
ELF_loadLibrary, ELF_getProcAddress, ELF_freeLibrary
****************************************************************************/
ELF_MODULE * ELFAPI ELF_loadLibraryExt(
    FILE *f,
    ulong startOffset,
    ulong *size,
    ibool shared)
{
    Elf64_Ehdr      elfhdr;
    Elf64_Phdr      *proghdr, *ph_ptr = NULL;
    Elf64_Shdr      *secthdr, *sh_ptr = NULL;
    InitLibC_t      InitLibC;
    ELF_MODULE      *hMod = NULL;
    ulong           image_base = 0xFFFFFFFF;
    ulong           image_top  = 0;
    ulong           image_size = 0;
    ulong           tmp_size;
    uchar           *image_ptr;
    int             i;
    int             have_strtab = -1, have_dynstr = -1;

    /* Read the ELF header from disk */
    if (!ELF_readHeader(f, startOffset, &elfhdr))
        return NULL;

    /* Read program and section headers into memory */
    ph_ptr = PM_malloc(elfhdr.e_phentsize * elfhdr.e_phnum);
    sh_ptr = PM_malloc(elfhdr.e_shentsize * elfhdr.e_shnum);
    if (!ph_ptr || !sh_ptr)
        goto Error;
    fseek(f, startOffset + elfhdr.e_phoff, SEEK_SET);
    tmp_size = elfhdr.e_phentsize * elfhdr.e_phnum;
    if (fread(ph_ptr, 1, tmp_size, f) != tmp_size)
        goto Error;
    fseek(f, startOffset + elfhdr.e_shoff, SEEK_SET);
    tmp_size = elfhdr.e_shentsize * elfhdr.e_shnum;
    if (fread(sh_ptr, 1, tmp_size, f) != tmp_size)
        goto Error;

    /* Scan segments to figure out image base address and how much memory
     * we need to load the image.
     */
    for (i = 0, proghdr = ph_ptr; i < elfhdr.e_phnum; i++, proghdr++) {
        if (proghdr->p_type == PT_LOAD) {
            if (proghdr->p_vaddr < image_base)
                image_base = proghdr->p_vaddr;
            if (proghdr->p_vaddr + proghdr->p_memsz > image_top)
                image_top = ROUND_4K(proghdr->p_vaddr + proghdr->p_memsz);
            }
        }
    image_base &= ~0xFFF;   /* Assuming 4K page size */
    image_size = image_top - image_base;

    /* Allocate memory for image */
    *size = sizeof(ELF_MODULE) + image_size + 4096;
    if (shared)
        hMod = PM_mallocShared(*size);
    else
        hMod = PM_malloc(*size);
    if (!hMod) {
        result = ELF_outOfMemory;
        goto Error;
        }

    /* Setup pointers into the loaded executable image */
    image_ptr = (uchar*)ROUND_4K((ulong)hMod + sizeof(ELF_MODULE));
    hMod->vaddr = (uchar*)image_base;
    hMod->pbase = (uchar*)image_ptr;
    hMod->symtab = NULL;
    hMod->dynsym = NULL;
    hMod->strtab = NULL;
    hMod->dynstr = NULL;
    hMod->numsym = 0;
    hMod->numdsym = 0;
    hMod->shared = shared;
    hMod->modname = NULL;

    /* Load segments into memory */
    for (i = 0, proghdr = ph_ptr; i < elfhdr.e_phnum; i++, proghdr++) {
        if (proghdr->p_type == PT_LOAD) {
            uchar *loadptr;

            fseek(f, startOffset + proghdr->p_offset, SEEK_SET);
            loadptr = image_ptr + (proghdr->p_vaddr - image_base);
            if (fread(loadptr, 1, proghdr->p_filesz, f) != proghdr->p_filesz)
                goto Error;
            }
        }

    /* Load symbol table sections into memory, we'll need those later */
    for (i = 0, secthdr = sh_ptr; i < elfhdr.e_shnum; i++, secthdr++) {
        switch (secthdr->sh_type) {
            case SHT_SYMTAB:
                tmp_size = secthdr->sh_size;
                if (shared)
                    hMod->symtab = PM_mallocShared(tmp_size);
                else
                    hMod->symtab = PM_malloc(tmp_size);
                if (!hMod->symtab) {
                    result = ELF_outOfMemory;
                    goto Error;
                    }
                fseek(f, startOffset + secthdr->sh_offset, SEEK_SET);
                if (fread(hMod->symtab, 1, tmp_size, f) != tmp_size)
                    goto Error;
                hMod->numsym = tmp_size / sizeof(Elf64_Sym);
                have_strtab  = secthdr->sh_link;
                break;
            case SHT_DYNSYM:
                tmp_size = secthdr->sh_size;
                if (shared)
                    hMod->dynsym = PM_mallocShared(tmp_size);
                else
                    hMod->dynsym = PM_malloc(tmp_size);
                if (!hMod->dynsym) {
                    result = ELF_outOfMemory;
                    goto Error;
                    }
                fseek(f, startOffset + secthdr->sh_offset, SEEK_SET);
                if (fread(hMod->dynsym, 1, tmp_size, f) != tmp_size)
                    goto Error;
                hMod->numdsym = tmp_size / sizeof(Elf64_Sym);
                have_dynstr   = secthdr->sh_link;
                break;
            default:
                break;
            }
        }

        /* Now load string tables associated with the symbol tables; those
         * will be needed to figure out symbol names.
         */
        if (have_strtab != -1) {
            secthdr = sh_ptr + have_strtab;
            if (shared)
                hMod->strtab = PM_mallocShared(secthdr->sh_size);
            else
                hMod->strtab = PM_malloc(secthdr->sh_size);
            if (!hMod->strtab) {
                result = ELF_outOfMemory;
                goto Error;
                }
            fseek(f, startOffset + secthdr->sh_offset, SEEK_SET);
            if (fread(hMod->strtab, 1, secthdr->sh_size, f) != secthdr->sh_size)
                goto Error;
            }
        if (have_dynstr != -1) {
            secthdr = sh_ptr + have_dynstr;
            if (shared)
                hMod->dynstr = PM_mallocShared(secthdr->sh_size);
            else
                hMod->dynstr = PM_malloc(secthdr->sh_size);
            if (!hMod->dynstr) {
                result = ELF_outOfMemory;
                goto Error;
                }
            fseek(f, startOffset + secthdr->sh_offset, SEEK_SET);
            if (fread(hMod->dynstr, 1, secthdr->sh_size, f) != secthdr->sh_size)
                goto Error;
            }

    /* Relocate the image */
    ELF_performRelocs(f, startOffset, &elfhdr, sh_ptr, hMod);

#ifdef __PPC__
    /* After relocating the image, we need to make sure that I-cache will see
     * every change that is now potentially only kept in D-cache; on PowerPC,
     * the I-cache and D-cache do not maintain coherency.
     */
    cache_sync_region(hMod->pbase, image_size);
#endif
    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    if (!PM_memProtect((void *)image_ptr, image_size,
        PM_MPROT_READ | PM_MPROT_WRITE | PM_MPROT_EXEC))
        goto Error;

    /* Initialise the C runtime library for the loaded DLL */
    result = ELF_unableToInitLibC;
    if ((InitLibC = (InitLibC_t)ELF_getProcAddress(hMod,"InitLibC")) == NULL)
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
    result = ELF_ok;
    PM_free(ph_ptr);
    PM_free(sh_ptr);
    return hMod;

Error:
    if (shared)
        PM_freeShared(hMod);
    else
        PM_free(hMod);
    PM_free(ph_ptr);
    PM_free(sh_ptr);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary module into memory

HEADER:
clib/elfloadr.h

PARAMETERS:
szDLLName   - Name of the ELF module to load
shared      - True to load module into shared memory

RETURNS:
Handle to loaded ELF module, or NULL on failure.

REMARKS:
This function loads a Portable Binary module from disk, relocates
the code and returns a handle to the loaded module. This function
will only work on modules that do not have any imports, since we don't
resolve import dependencies in this function.

SEE ALSO:
ELF_getProcAddress, ELF_freeLibrary
****************************************************************************/
ELF_MODULE * ELFAPI ELF_loadLibrary(
    const char *szDLLName,
    ibool shared)
{
    ELF_MODULE  *hMod;
    FILE        *f;
    ulong       size;

    /* Attempt to open the file on disk */
    if (shared < 0)
        shared = 0;
    if ((f = fopen(szDLLName,"rb")) == NULL) {
        result = ELF_fileNotFound;
        return NULL;
        }

    hMod = ELF_loadLibraryExt(f,0,&size,shared);
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
                strcpy(modname, szDLLName + 2);
            else
                strcpy(modname, szDLLName);
            hMod->modname = modname;
            // The 0x100 offset is just hacked in!
            NotifyWDLoad(hMod->modname, (ulong)hMod->pbase + 0x100);
            }
        }
#endif
    return hMod;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary module into memory

HEADER:
clib/elfloadr.h

PARAMETERS:
szDLLName   - Name of the ELF module to load
shared      - True to load module into shared memory

RETURNS:
Handle to loaded ELF module, or NULL on failure.

REMARKS:
This function is the same as the regular PE_loadLibrary function, except
that it looks for the drivers in the MGL_ROOT/drivers directory or a
/drivers directory relative to the current directory.

SEE ALSO:
ELF_loadLibrary, ELF_getProcAddress, ELF_freeLibrary
****************************************************************************/
ELF_MODULE * ELFAPI ELF_loadLibraryMGL(
    const char *szDLLName,
    ibool shared)
{
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__)
    ELF_MODULE   *hMod;
#endif
    char        path[256] = "";

    /* We look in the 'drivers' directory, optionally under the MGL_ROOT
     * environment variable directory.
     */
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__)
    if (getenv("MGL_ROOT")) {
        strcpy(path,getenv("MGL_ROOT"));
        PM_backslash(path);
        }
    strcat(path,"drivers");
    PM_backslash(path);
    strcat(path,szDLLName);
    if ((hMod = ELF_loadLibrary(path,shared)) != NULL)
        return hMod;
#endif
    strcpy(path,"drivers");
    PM_backslash(path);
    strcat(path,szDLLName);
    return ELF_loadLibrary(path,shared);
}

/****************************************************************************
DESCRIPTION:
Gets a function address from a Portable Binary module

HEADER:
clib/elfloadr.h

PARAMETERS:
hModule     - Handle to a loaded ELF module
szProcName  - Name of the function to get the address of

RETURNS:
Pointer to the function, or NULL on failure.

REMARKS:
This function searches for the named, exported function in a loaded ELF
module, and returns the address of the function. If the function is
not found in the module, this function return NULL.

SEE ALSO:
ELF_loadLibrary, ELF_freeLibrary
****************************************************************************/
void * ELFAPI ELF_getProcAddress(
    ELF_MODULE *hModule,
    const char *szProcName)
{
    Elf64_Sym       *symbol;
    int             i;
    ulong           funcOffset = 0;

    if (!hModule)
        return NULL;

    /* Walk through the symbol table(s) and see if we can find the name */
    if (hModule->symtab) {
        symbol = (Elf64_Sym*)hModule->symtab;
        for (i = 0; i < hModule->numsym; i++, symbol++) {
            if (ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL) {
                if (!strcmp(szProcName, hModule->strtab + symbol->st_name)) {
                    funcOffset = symbol->st_value;
                    break;
                    }
                }
            }
        }

    if (!funcOffset && hModule->dynsym) {
        symbol = (Elf64_Sym*)hModule->dynsym;
        for (i = 0; i < hModule->numdsym; i++, symbol++) {
            if (ELF64_ST_BIND(symbol->st_info) == STB_GLOBAL) {
                if (!strcmp(szProcName, hModule->dynstr + symbol->st_name)) {
                    funcOffset = symbol->st_value;
                    break;
                    }
                }
            }
        }

    if (!funcOffset)
        return NULL;
    return (void*)(hModule->pbase + funcOffset - hModule->vaddr);
}

/****************************************************************************
DESCRIPTION:
Frees a loaded Portable Binary module

HEADER:
clib/elfloadr.h

PARAMETERS:
hModule     - Handle to a loaded ELF module to free

REMARKS:
This function frees a loaded ELF module from memory.

SEE ALSO:
ELF_getProcAddress, ELF_loadLibrary
****************************************************************************/
void ELFAPI ELF_freeLibrary(
    ELF_MODULE *hModule)
{
    TerminateLibC_t TerminateLibC;

    if (hModule) {
        /* Run the C runtime library exit code on module unload */
        if ((TerminateLibC = (TerminateLibC_t)ELF_getProcAddress(hModule,"TerminateLibC")) != NULL)
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
        if (hModule->shared) {
            if (hModule->symtab) {
                PM_freeShared(hModule->symtab);
                PM_freeShared(hModule->strtab);
                }
            if (hModule->dynsym) {
                PM_freeShared(hModule->dynsym);
                PM_freeShared(hModule->dynstr);
                }
            PM_freeShared(hModule);
            }
        else {
            if (hModule->symtab) {
                PM_free(hModule->symtab);
                PM_free(hModule->strtab);
                }
            if (hModule->dynsym) {
                PM_free(hModule->dynsym);
                PM_free(hModule->dynstr);
                }
            PM_free(hModule);
            }
        }
}

/****************************************************************************
DESCRIPTION:
Returns the error code for the last operation

HEADER:
clib/elfloadr.h

RETURNS:
Error code for the last operation.

SEE ALSO:
ELF_getProcAddress, ELF_loadLibrary
****************************************************************************/
int ELFAPI ELF_getError(void)
{
    return result;
}

