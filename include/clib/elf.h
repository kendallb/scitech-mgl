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
* Description:  Internal header file describing the ELF (Executable and
*               Linkable Format) structures.
*
****************************************************************************/

#ifndef __ELF_H
#define __ELF_H

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Macro to round a value up to a 4Kb boundary */

#define ROUND_4K(s) (((unsigned long)(s) + 0xFFF) & ~0xFFF)

/* Values for the file type field in the ELF header */

#define ET_NONE     0       /* No file type         */
#define ET_REL      1       /* Relocatable file     */
#define ET_EXEC     2       /* Executable file      */
#define ET_DYN      3       /* Shared object file   */
#define ET_CORE     4       /* Core file            */
#define ET_LOPROC   0xff00  /* Processor specific   */
#define ET_HIPROC   0xffff  /* Processor specific   */

/* Values for the machine type field in the ELF header */

#define EM_NONE     0   /* No machine       */
#define EM_M32      1   /* AT&T WE 32100    */
#define EM_SPARC    2   /* Sun SPARC        */
#define EM_386      3   /* Intel 80386      */
#define EM_68K      4   /* Motorola 68000   */
#define EM_88K      5   /* Motorola 88000   */
#define EM_860      7   /* Intel 80860      */
#define EM_MIPS     8   /* MIPS RS3000      */
#define EM_PPC      20  /* PowerPC          */
#define EM_X86_64   62  /* AMD64            */

/* Values for the object file version field in the ELF header */

#define EV_NONE     0   /* Invalid version  */
#define EV_CURRENT  1   /* Current version  */

/* Offsets into the ELF identification array in the ELF header */

#define EI_MAG0     0   /* File identification  */
#define EI_MAG1     1   /* File identification  */
#define EI_MAG2     2   /* File identification  */
#define EI_MAG3     3   /* File identification  */
#define EI_CLASS    4   /* File class           */
#define EI_DATA     5   /* Data encoding        */
#define EI_VERSION  6   /* File version         */
#define EI_PAD      7   /* Start of padding     */
#define EI_NIDENT   16  /* Size of e_ident[]    */

/* Magic byte definitions */

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* ELF file class definitions */

#define ELFCLASSNONE    0   /* Invalid class    */
#define ELFCLASS32      1   /* 32-bit objects   */
#define ELFCLASS64      2   /* 64-bit objects   */

/* Data encoding definitions */

#define ELFDATANONE     0   /* Invalid data encoding    */
#define ELFDATA2LSB     1   /* Little endian data       */
#define ELFDATA2MSB     2   /* Big endian data          */

/* Special section indexes */

#define SHN_UNDEF       0       /* Undefined section            */
#define SHN_LORESERVE   0xff00  /* Reserved - lower bound       */
#define SHN_LOPROC      0xff00  /* CPU specific - lower bound   */
#define SHN_HIPROC      0xff1f  /* CPU specific - upper bound   */
#define SHN_ABS         0xfff1  /* Non-relocatable section      */
#define SHN_COMMON      0xfff2  /* Common section               */
#define SHN_HIRESERVE   0xffff  /* Reserved - upper bound       */

/* Values for section types in ELF section header */

#define SHT_NULL        0               /* Inactive                             */
#define SHT_PROGBITS    1               /* Meaning defined by program           */
#define SHT_SYMTAB      2               /* Symbol table                         */
#define SHT_STRTAB      3               /* String table                         */
#define SHT_RELA        4               /* Reloc entries with explicit addends  */
#define SHT_HASH        5               /* Symbol hash table                    */
#define SHT_DYNAMIC     6               /* Dynamic linking information          */
#define SHT_NOTE        7               /* Comment information                  */
#define SHT_NOBITS      8               /* Like PROGBITS but no space in file   */
#define SHT_REL         9               /* As RELA but no explicit addends      */
#define SHT_SHLIB       10              /* Reserved - non conforming            */
#define SHT_DYNSYM      11              /* Dynamic link symbol table            */
#define SHT_OS          0x60000001      /* Info to identify target OS           */
#define SHT_IMPORTS     0x60000002      /* Info on refs to external symbols     */
#define SHT_EXPORTS     0x60000003      /* Info on symbols exported by ordinal  */
#define SHT_RES         0x60000004      /* Read-only resource data.             */
#define SHT_PROGFRAGS   0x60001001      /* Similar to SHT_PROGBITS              */
#define SHT_IDMDLL      0x60001002      /* Symbol name demangling information   */
#define SHT_DEFLIB      0x60001003      /* Default static libraries             */
#define SHT_LOPROC      0x70000000      /* Processor specific - lower bound     */
#define SHT_HIPROC      0x7fffffff      /* Processor specific - upper bound     */
#define SHT_LOUSER      0x80000000      /* User defined sections - lower bound  */
#define SHT_HIUSER      0xffffffff      /* User defined sections - upper bound  */

/* Values for section attribute flags in ELF section header */

#define SHF_WRITE       0x00000001      /* Section writable during execution    */
#define SHF_ALLOC       0x00000002      /* Section occupies space during exec.  */
#define SHF_EXECINSTR   0x00000004      /* Section contains code.               */
#define SHF_BEGIN       0x01000000      /* First of like-named sections         */
#define SHF_END         0x02000000      /* Same, end.                           */
#define SHF_MASKPROC    0xf0000000      /* Processor specific flags             */

/* Macros to access symbol type information */

#define ELF32_ST_BIND(i)        ((i)>>4)                /* Get "bind" subfield  */
#define ELF32_ST_TYPE(i)        ((i)&0xf)               /* Get type subfield    */
#define ELF32_ST_INFO(b,t)      (((b)<<4)+((t)&0xf))    /* Make a new st_info   */

#define ELF64_ST_BIND(i)        ((i)>>4)                /* Get "bind" subfield  */
#define ELF64_ST_TYPE(i)        ((i)&0xf)               /* Get type subfield    */
#define ELF64_ST_INFO(b,t)      (((b)<<4)+((t)&0xf))    /* Make a new st_info   */

/* Bind subfield of symbol information definition */

#define STB_LOCAL       0       /* Symbol has local binding                     */
#define STB_GLOBAL      1       /* Symbol has global binding                    */
#define STB_WEAK        2       /* Symbol has weak binding                      */
#define STB_ENTRY       12      /* Symbol is entry-point for the load module    */
#define STB_LOPROC      13      /* Processor specific semantics - lower bound   */
#define STB_HIPROC      15      /* Processor specific semantics - upper bound   */

/* Type subfield of symbol information definition */

#define STT_NOTYPE      0       /* Not specified                                */
#define STT_OBJECT      1       /* Symbol is a data object                      */
#define STT_FUNC        2       /* Symbol is a code symbol                      */
#define STT_SECTION     3       /* Symbol associated with a section             */
#define STT_FILE        4       /* Symbol gives name of the source file.        */
#define STT_IMPORT      11      /* Reference to a symbol in another module      */
#define STT_LOPROC      13      /* Processor specific semantics - lower bound   */
#define STT_HIPROC      15      /* Processor specific semantics - lower bound   */

/* Macros to access relocation information */

#define ELF32_R_SYM(i)    ((i)>>8)                      /* Get the symbol index */
#define ELF32_R_TYPE(i)   ((unsigned char)(i))          /* Get the symbol type  */
#define ELF32_R_INFO(s,t) (((s)<<8)+(unsigned char)(t)) /* Make a new r_info    */

#define ELF64_R_SYM(i)    ((i)>>32)                     /* Get the symbol index */
#define ELF64_R_TYPE(i)   ((i) & 0xffffffffL)           /* Get the symbol type  */
#define ELF64_R_INFO(s,t) (((s)<<32)+((t) & 0xffffffffL)/* Make a new r_info    */

/* Relocation type definitions for the 386 */

#define R_386_NONE              0
#define R_386_32                1
#define R_386_PC32              2
#define R_386_GOT32             3
#define R_386_PLT32             4
#define R_386_COPY              5
#define R_386_GLOB_DAT          6
#define R_386_JMP_SLOT          7
#define R_386_RELATIVE          8
#define R_386_GOTOFF            9
#define R_386_GOTPC             10

/* Relocation type definitions for the PowerPC */

#define R_PPC_NONE              0
#define R_PPC_ADDR32            1
#define R_PPC_ADDR24            2
#define R_PPC_ADDR16            3
#define R_PPC_ADDR16_LO         4
#define R_PPC_ADDR16_HI         5
#define R_PPC_ADDR16_HA         6
#define R_PPC_ADDR14            7
#define R_PPC_ADDR14_BRTAKEN    8
#define R_PPC_ADDR14_BRNTAKEN   9
#define R_PPC_REL24             10
#define R_PPC_REL14             11
#define R_PPC_REL14_BRTAKEN     12
#define R_PPC_REL14_BRNTAKEN    13
#define R_PPC_GOT16             14
#define R_PPC_GOT16_LO          15
#define R_PPC_GOT16_HI          16
#define R_PPC_GOT16_HA          17
#define R_PPC_PLTREL24          18
#define R_PPC_COPY              19
#define R_PPC_GLOB_DAT          20
#define R_PPC_JMP_SLOT          21
#define R_PPC_RELATIVE          22
#define R_PPC_LOCAL24PC         23
#define R_PPC_UADDR32           24
#define R_PPC_UADDR16           25
#define R_PPC_REL32             26
#define R_PPC_PLT32             27
#define R_PPC_PLTREL32          28
#define R_PPC_PLT16_LO          29
#define R_PPC_PLT16_HI          30
#define R_PPC_PLT16_HA          31
#define R_PPC_SDAREL16          32

/* Relocation type definitions for the AMD64 */

#define R_X86_64_NONE           0
#define R_X86_64_64             1
#define R_X86_64_PC32           2
#define R_X86_64_GOT32          3
#define R_X86_64_PLT32          4
#define R_X86_64_COPY           5
#define R_X86_64_GLOB_DAT       6
#define R_X86_64_JUMP_SLOT      7
#define R_X86_64_RELATIVE       8
#define R_X86_64_GOTPCREL       9
#define R_X86_64_32             10
#define R_X86_64_32S            11
#define R_X86_64_16             12
#define R_X86_64_PC16           13
#define R_X86_64_8              14
#define R_X86_64_PC8            15

/* Relocation type definitions for MIPS32 */

#define R_MIPS_NONE             0
#define R_MIPS_16               1
#define R_MIPS_32               2
#define R_MIPS_REL32            3
#define R_MIPS_26               4
#define R_MIPS_HI16             5
#define R_MIPS_LO16             6
#define R_MIPS_GPREL16          7
#define R_MIPS_LITERAL          8
#define R_MIPS_GOT16            9
#define R_MIPS_PC16             10
#define R_MIPS_CALL16           11
#define R_MIPS_GPREL32          12
#define R_MIPS_GOTHI16          21
#define R_MIPS_GOTLO16          22
#define R_MIPS_CALLHI16         30
#define R_MIPS_CALLLO16         31

/* Values for the program type field in ELF program header */

#define PT_NULL         0               /* Unused segment                       */
#define PT_LOAD         1               /* Loadable segment                     */
#define PT_DYNAMIC      2               /* Contains dynamic linking information */
#define PT_INTERP       3               /* Reference to a program interpreter   */
#define PT_NOTE         4               /* Comments and auxiliary information   */
#define PT_SHLIB        5               /* Reserved - non conforming            */
#define PT_PHDR         6               /* Address of program header in memory  */
#define PT_OS           0x60000001      /* Target OS information                */
#define PT_RES          0x60000002      /* Read-only resource information       */
#define PT_LOPROC       0x70000000      /* Processor specific - lower bound     */
#define PT_HIPROC       0x7fffffff      /* Processor specific - upper bound     */

/* Values for the ELF segment flags */

#define PF_X            0x1             /* Segment has execute permissions      */
#define PF_W            0x2             /* Segment has write permissions        */
#define PF_R            0x4             /* Segment has read permissions         */
#define PF_S            0x01000000      /* Segment is shared                    */
#define PF_MASKPROC     0xf0000000      /* Processor-specific flag mask         */

/* Values for the ELF dynamic array tags */

#define DT_NULL         0
#define DT_NEEDED       1               /* Name of needed library        */
#define DT_PLTRELSZ     2               /* Size of reloc entries for PLT */
#define DT_PLTGOT       3               /* Address of PLT or GOT section */
#define DT_HASH         4               /* Address of hash table section */
#define DT_STRTAB       5               /* String table address          */
#define DT_SYMTAB       6               /* Symbol table address          */
#define DT_RELA         7               /* Address of .rela table        */
#define DT_RELASZ       8               /* Size of .rela table           */
#define DT_RELAENT      9               /* Size of single .rela entry    */
#define DT_STRSZ        10              /* Size of the string table      */
#define DT_SYMENT       11              /* Size of a symbol table entry  */
#define DT_SONAME       14              /* Shared object name            */
#define DT_RPATH        15              /* Search path string            */
#define DT_REL          17              /* Address of .rel table         */
#define DT_RELSZ        18              /* Size of .rel table            */
#define DT_RELENT       19              /* Size of single .rel entry     */
#define DT_PLTREL       20              /* Type of reloc entry for PLT   */
#define DT_DEBUG        21              /* For debugging information     */
#define DT_JMPREL       23              /* Reloc entries for PLT         */


/* ELF data types for 32-bit architectures */

typedef unsigned long   Elf32_Addr;
typedef unsigned short  Elf32_Half;
typedef unsigned long   Elf32_Off;
typedef signed long     Elf32_Sword;
typedef unsigned long   Elf32_Word;

/* ELF data types for 64-bit architectures */

typedef unsigned long   Elf64_Addr;
typedef unsigned long   Elf64_Off;
typedef unsigned short  Elf64_Half;
typedef unsigned int    Elf64_Word;
typedef signed int      Elf64_Sword;
typedef unsigned long   Elf64_Xword;
typedef signed long     Elf64_Sxword;

/* Structure defining the 32-bit ELF header */

typedef struct {
    unsigned char   e_ident[EI_NIDENT];
    Elf32_Half      e_type;
    Elf32_Half      e_machine;
    Elf32_Word      e_version;
    Elf32_Addr      e_entry;
    Elf32_Off       e_phoff;
    Elf32_Off       e_shoff;
    Elf32_Word      e_flags;
    Elf32_Half      e_ehsize;
    Elf32_Half      e_phentsize;
    Elf32_Half      e_phnum;
    Elf32_Half      e_shentsize;
    Elf32_Half      e_shnum;
    Elf32_Half      e_shstrndx;
    } Elf32_Ehdr;

/* Structure defining the 64-bit ELF header */

typedef struct {
    unsigned char   e_ident[EI_NIDENT];
    Elf64_Half      e_type;
    Elf64_Half      e_machine;
    Elf64_Word      e_version;
    Elf64_Addr      e_entry;
    Elf64_Off       e_phoff;
    Elf64_Off       e_shoff;
    Elf64_Word      e_flags;
    Elf64_Half      e_ehsize;
    Elf64_Half      e_phentsize;
    Elf64_Half      e_phnum;
    Elf64_Half      e_shentsize;
    Elf64_Half      e_shnum;
    Elf64_Half      e_shstrndx;
    } Elf64_Ehdr;

/* Structure defining the 32-bit ELF section header */

typedef struct {
    Elf32_Word      sh_name;
    Elf32_Word      sh_type;
    Elf32_Word      sh_flags;
    Elf32_Addr      sh_addr;
    Elf32_Off       sh_offset;
    Elf32_Word      sh_size;
    Elf32_Word      sh_link;
    Elf32_Word      sh_info;
    Elf32_Word      sh_addralign;
    Elf32_Word      sh_entsize;
    } Elf32_Shdr;

/* Structure defining the 64-bit ELF section header */

typedef struct {
    Elf64_Word      sh_name;
    Elf64_Word      sh_type;
    Elf64_Xword     sh_flags;
    Elf64_Addr      sh_addr;
    Elf64_Off       sh_offset;
    Elf64_Xword     sh_size;
    Elf64_Word      sh_link;
    Elf64_Word      sh_info;
    Elf64_Xword     sh_addralign;
    Elf64_Xword     sh_entsize;
    } Elf64_Shdr;

/* Structure defining the 32-bit ELF symbol table entry */

typedef struct {
    Elf32_Word      st_name;
    Elf32_Addr      st_value;
    Elf32_Word      st_size;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf32_Half      st_shndx;
    } Elf32_Sym;

/* Structure defining the 64-bit ELF symbol table entry */

typedef struct {
    Elf64_Word      st_name;
    unsigned char   st_info;
    unsigned char   st_other;
    Elf64_Half      st_shndx;
    Elf64_Addr      st_value;
    Elf64_Xword     st_size;
    } Elf64_Sym;

/* Structure defining the 32-bit ELF relocation entry */

typedef struct {
    Elf32_Addr      r_offset;
    Elf32_Word      r_info;
    } Elf32_Rel;

/* Structure defining the 64-bit ELF relocation entry */

typedef struct {
    Elf64_Addr      r_offset;
    Elf64_Xword     r_info;
    } Elf64_Rel;

/* Structure defining the 32-bit ELF relocation entry with addend */

typedef struct {
    Elf32_Addr      r_offset;
    Elf32_Word      r_info;
    Elf32_Sword     r_addend;
    } Elf32_Rela;

/* Structure defining the 64-bit ELF relocation entry with addend */

typedef struct {
    Elf64_Addr      r_offset;
    Elf64_Xword     r_info;
    Elf64_Sxword    r_addend;
    } Elf64_Rela;

/* Structure defining the 32-bit ELF dynamic segment entry */

typedef struct {
    Elf32_Sword         d_tag;
    union {
        Elf32_Word      d_val;
        Elf32_Addr      d_ptr;
        } d_un;
    } Elf32_Dyn;

/* Structure defining the 32-bit ELF program header */

typedef struct {
    Elf32_Word  p_type;
    Elf32_Off   p_offset;
    Elf32_Addr  p_vaddr;
    Elf32_Addr  p_paddr;
    Elf32_Word  p_filesz;
    Elf32_Word  p_memsz;
    Elf32_Word  p_flags;
    Elf32_Word  p_align;
    } Elf32_Phdr;

/* Structure defining the 64-bit ELF program header */

typedef struct {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;
    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
    } Elf64_Phdr;

#pragma pack()

#endif  /* __ELF_H */
