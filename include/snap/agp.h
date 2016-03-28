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
* Environment:  Any 32-bit protected mode environment
*
* Description:  Header file for the SciTech SNAP AGP GART device driver
*               Hardware Abstraction Layer.
*
****************************************************************************/

#ifndef __SNAP_AGP_H
#define __SNAP_AGP_H

#include "snap/common.h"
#include "clib/modloadr.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Signature for the graphics driver file */

#define AGP_SIGNATURE           "AGP"

/* Define the interface version */

#define AGP_VERSION             0x100

/****************************************************************************
REMARKS:
This enumeration defines the identifiers used to obtain the device context
function group pointer structures. As new features and capabilities are
added to the future versions of the specification, new identifiers will
be added to extract new function pointers from the drivers.

The AGP_GET_FIRST_OEM defines the first identifier for OEM extensions. OEM's
are free to added their own private functional extensions to the drivers
as desired. Note that OEM's must verify the presence of their OEM drivers
via the the OemVendorName string before attempting to use OEM extension
functions.

HEADER:
snap/agp.h

MEMBERS:
AGP_GET_RESERVED        - Reserved value
AGP_GET_DRIVERFUNCS     - Get AGP_driverFuncs structure
AGP_GET_FIRST_OEM       - ID of first OEM extension function
****************************************************************************/
typedef enum {
    AGP_GET_RESERVED,
    AGP_GET_DRIVERFUNCS,
    AGP_GET_FIRST_OEM               = 0x00010000
    } AGP_funcGroupsType;

/****************************************************************************
REMARKS:
This enumeration defines the available AGP capabilities supported by the
driver, and stored in the Capabilities field of the AGP_devCtx structure.

HEADER:
snap/agp.h

MEMBERS:
agpCanMapPhysical       - Indicates the driver can map physical memory
agpCanMapIntelDCACHE    - Indicates the driver can map Intel DCACHE memory
****************************************************************************/
typedef enum {
    agpCanMapPhysical       = 0x00000001,
    agpCanMapIntelDCACHE    = 0x00000002
    } AGP_capabilitiesFlags;

/****************************************************************************
REMARKS:
This enumeration defines the flags passed to the ReservePhysical function,
to define how the physical memory mapping should be handled.

The agpUncached type indicates that the memory should be allocated as
uncached memory.

The agpWriteCombine flag indicates that write combining should be enabled
for physical memory mapping. This is used for framebuffer write combing and
speeds up direct framebuffer writes to the memory.

The agpIntelDCACHE flag indicates that memory should come from the Intel
i81x Display Cache (or DCACHE) memory pool. This flag is specific to the
Intel i810 and i815 controllers, and should not be passed for any other
controller type.

HEADER:
snap/agp.h

MEMBERS:
agpUncached     - Indicates that the memory should be uncached
agpWriteCombine - Indicates that the memory should be write combined
agpIntelDCACHE  - Indicates that memory should come from the DCACHE pool
****************************************************************************/
typedef enum {
    agpUncached,
    agpWriteCombine,
    agpIntelDCACHE
    } AGP_physicalFlagsType;

/****************************************************************************
REMARKS:
This enumeration defines the available AGP aperture sizes that are
supported by the driver.

HEADER:
snap/agp.h

MEMBERS:
agpSize4MB      - Indicates the aperture size is 4 megabytes
agpSize8MB      - Indicates the aperture size is 8 megabytes
agpSize16MB     - Indicates the aperture size is 16 megabytes
agpSize32MB     - Indicates the aperture size is 32 megabytes
agpSize64MB     - Indicates the aperture size is 64 megabytes
agpSize128MB    - Indicates the aperture size is 128 megabytes
agpSize256MB    - Indicates the aperture size is 256 megabytes
agpSize512MB    - Indicates the aperture size is 512 megabytes
agpSize1GB      - Indicates the aperture size is 1 gigabyte
agpSize2GB      - Indicates the aperture size is 2 gigabytes
****************************************************************************/
typedef enum {
    agpSize4MB,
    agpSize8MB,
    agpSize16MB,
    agpSize32MB,
    agpSize64MB,
    agpSize128MB,
    agpSize256MB,
    agpSize512MB,
    agpSize1GB,
    agpSize2GB
    } AGP_sizeType;

/****************************************************************************
REMARKS:
Definitions for values stored in the CertifyFlags member of the
AGP_certifyChipInfo and structure. These flags define what certification
tests have been run on the included drivers.

The gaCertified value indicates that the driver has been certified to have
passed all the base certification tests, and is usually the only important
flag for most situations.

HEADER:
snap/agp.h

MEMBERS:
agpCertified    - Driver has passed all base certification tests
****************************************************************************/
typedef enum {
    agpCertified            = 0x01
    } AGP_CertifyFlagsType;

/****************************************************************************
REMARKS:
This enumeration defines the flags passed to the TranslateAddress function,
to define what kind of an address should be translated and how.

The agpMapAGPOffToPhysAddr requests a translation of offset into the AGP
aperture to a physical address as seen by the CPU.

The agpMapAGPOffToLogAddr requests a translation of physical address in
CPU address space to an offset in the AGP aperture.

HEADER:
snap/agp.h

MEMBERS:
agpMapAGPOffToPhysAddr  - Translate AGP offset to physical address
agpMapPhysAddrToAGPOff  - Translate physical address to AGP offset
****************************************************************************/
typedef enum {
    agpMapAGPOffToPhysAddr  = 0x0001,
    agpMapPhysAddrToAGPOff  = 0x0002
    } AGP_TranslateFlagsType;

/****************************************************************************
REMARKS:
Structure returned by GetGlobalOptions, which contains configuration
information about the options effective for all installed AGP drivers.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/agp.h

MEMBERS:
dwSize                  - Set to size of structure in bytes
bAllowNonCertified      - Allow uncertified drivers to load
wCertifiedVersion       - Version of certify program to allow
bEnableFastWrite        - True to enable AGP FastWrite
maxAGPRate              - Maximum AGP data rate to support
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint8         bAllowNonCertified;
    N_uint16        wCertifiedVersion;
    N_uint8         bEnableFastWrite;
    N_uint8         maxAGPRate;
    } AGP_globalOptions;

/****************************************************************************
REMARKS:
Structure pointed to be the AGP_certifyInfo structure, which contains
certification information about the specific chipsets in the device driver.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/agp.h

MEMBERS:
dwSize              - Set to size of structure in bytes
ChipsetName         - Name of graphics chipset name
CertifyVersion      - Version of certification program used
CertifiedDate       - Date that the card was certified
CertifyFlags        - Flags for certification information
****************************************************************************/
typedef struct {
    N_uint32    dwSize;
    char        ChipsetName[30];
    N_uint16    CertifyVersion;
    char        CertifiedDate[19];
    N_uint8     CertifyFlags;
    } AGP_certifyChipInfo;

/****************************************************************************
REMARKS:
Structure returned by GetCertifyInfo, which contains configuration
information about the certification status of the drivers.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/agp.h

MEMBERS:
dwSize              - Set to size of structure in bytes
Signature           - Signature to identify certification information
BuildDate           - String representation of the build date for driver
MaxCertifiedCards   - Maximum number of certified chipsets in driver
NumCertifiedCards   - Number of certified chipsets
CertifiedCards      - List of all certified cards in the driver
****************************************************************************/
typedef struct {
    N_uint32            dwSize;
    char                Signature[20];
    char                BuildDate[30];
    char                MaxCertifiedChips;
    char                NumCertifiedChips;
    AGP_certifyChipInfo *CertifiedCards;
    } AGP_certifyInfo;

/****************************************************************************
REMARKS:
Structure returned by GetConfigInfo, which contains configuration
information about the installed graphics hardware.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/agp.h

MEMBERS:
dwSize              - Set to size of structure in bytes
ManufacturerName    - Name of graphics chipset manufacturer
ChipsetName         - Name of graphics chipset name
VersionInfo         - String representation of version and build for driver
BuildDate           - String representation of the build date for driver
Certified           - True if the installed device is certified
CertifiedDate       - Date when the device was certified
CertifyVersion      - Version of certification program used
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    char            ManufacturerName[80];
    char            ChipsetName[80];
    char            VersionInfo[80];
    char            BuildDate[80];
    char            Certified;
    char            CertifiedDate[20];
    N_uint16        CertifyVersion;
    } AGP_configInfo;

/****************************************************************************
REMARKS:
Main AGP device context structure. This structure consists of a header
block that contains configuration information about the AGP controller,
as well as detection information and runtime state information.

The Signature member is filled with the null terminated string 'GRAPHICS\0'
by the driver implementation. This can be used to verify that the file loaded
really is an graphics device driver.

The Version member is a BCD value which specifies what revision level of the
graphics specification is implemented in the driver. The high byte specifies
the major version number and the low byte specifies the minor version number.
For example, the BCD value for version 1.0 is 0x100 and the BCD value for
version 2.2 would be 0x202.

The DriverRev member specifies the driver revision level, and is used by the
driver configuration software to determine which version was used to generate
the driver file.

The OemVendorName member contains the name of the vendor that developed the
device driver implementation, and can be up to 80 characters in length.

The OemCopyright member contains a copyright string for the vendor that
developed the device driver implementation and may be up to 80 characters
in length.

The PageSize member defines the logical page size as used by the AGP
driver. For Intel based systems this is always 4096 bytes.

The AllocationPageLimit member defines the limit on the amount of AGP
memory that can be allocated, in units of pages. This limit is usually
set in the BIOS, although some AGP bridges do allow this to be re-programmed
on the fly.

Note:   None of the function pointers in this structure are listed here
        for brevity. All the functions are documented in the function
        documentation section for each individual function. Please refer
        to this list, or the official snap/agp.h header file for more
        information.

HEADER:
snap/agp.h

MEMBERS:
Signature           - 'AGP\0' 20 byte signature
Version             - Driver Interface Version
DriverRev           - Driver revision number
OemVendorName       - Vendor Name string
OemCopyright        - Vendor Copyright string
Capabilities        - Capabilities of the driver
MaxApertureSize     - Maximum AGP aperture size (one of AGP_sizeType)
IOBase              - Base address for I/O mapped registers (relocateable)
MMIOBase            - Base addresses of memory mapped I/O regions (up to 4)
MMIOLen             - Length of memory mapped I/O regions in bytes
IOMemMaps           - Pointer to mapped I/O memory regions
****************************************************************************/
typedef struct {
    /*------------------------------------------------------------------*/
    /* Device driver header block                                       */
    /*------------------------------------------------------------------*/
    char            Signature[20];
    N_uint32        Version;
    N_uint32        DriverRev;
    char            OemVendorName[80];
    char            OemCopyright[80];
    N_uint32        Capabilities;
    N_int32         MaxApertureSize;
    N_uint32        IOBase;
    N_uint32        MMIOBase[4];
    N_uint32        MMIOLen[4];
    N_uint32        res1[31];

    /*------------------------------------------------------------------*/
    /* Near pointers mapped by loader for driver                        */
    /*------------------------------------------------------------------*/
    void            _FAR_ *IOMemMaps[4];

    /*------------------------------------------------------------------*/
    /* Driver initialization functions                                  */
    /*------------------------------------------------------------------*/
    N_int32         (NAPIP InitDriver)(void);
    ibool           (NAPIP QueryFunctions)(N_uint32 id,void _FAR_ *funcs);
    void            (NAPIP UnloadDriver)(void);
    } AGP_devCtx;

/****************************************************************************
REMARKS:
Main device driver AGP bridge functions, used to reserve, commit, release
and free both physical and virtual address mappings.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    void        (NAPIP GetConfigInfo)(AGP_configInfo *info);
    void        (NAPIP GetCertifyInfo)(AGP_certifyInfo *info);
    N_int32     (NAPIP GetApertureSize)(void);
    N_physAddr  (NAPIP GetApertureBase)(void);
    N_int32     (NAPIP ReservePhysical)(N_uint32 numPages,N_int32 type,void **physContext,N_physAddr *physAddr);
    N_int32     (NAPIP ReleasePhysical)(void *physContext);
    N_int32     (NAPIP CommitPhysical)(void *physContext,N_uint32 numPages,N_uint32 startOffset,N_physAddr *physAddr);
    N_int32     (NAPIP FreePhysical)(void *physContext,N_uint32 numPages,N_uint32 startOffset);
    N_int32     (NAPIP TranslateAddress)(void *physContext,N_physAddr *physAddr,N_uintptr *offset,N_uint32 flags);
    } AGP_driverFuncs;

/****************************************************************************
REMARKS:
Structure defining all the SNAP AGP GART API functions as exported from
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    int             (NAPIP AGP_status)(void);
    const char *    (NAPIP AGP_errorMsg)(N_int32 status);
    AGP_devCtx *    (NAPIP AGP_loadDriver)(N_int32 deviceIndex);
    void            (NAPIP AGP_unloadDriver)(AGP_devCtx *dc);
    void            (NAPIP AGP_getGlobalOptions)(AGP_globalOptions *options);
    void            (NAPIP AGP_setGlobalOptions)(AGP_globalOptions *options);
    void            (NAPIP AGP_saveGlobalOptions)(AGP_globalOptions *options);
    } AGP_exports;

/****************************************************************************
REMARKS:
Structure defining all the SNAP GART API functions as imported into
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
#ifndef __INTEL__
    uchar           (NAPIP outpb)(ulong port,uchar val);
    ushort          (NAPIP outpw)(ulong port,ushort val);
    ulong           (NAPIP outpd)(ulong port,ulong val);
    uchar           (NAPIP inpb)(ulong port);
    ushort          (NAPIP inpw)(ulong port);
    ulong           (NAPIP inpd)(ulong port);
#endif
    } AGP_imports;

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef AGP_exports * (NAPIP AGP_initLibrary_t)(const char *path,const char *bpdname,PM_imports *pmImp,N_imports *nImp,AGP_imports *gaImp);

#pragma pack()

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Error handling functions for SNAP AGP drivers */

int             NAPI AGP_status(void);
const char *    NAPI AGP_errorMsg(N_int32 status);

/* Utility Functions to load an AGP driver and initialise it */

AGP_devCtx *    NAPI AGP_loadDriver(N_int32 deviceIndex);
void            NAPI AGP_unloadDriver(AGP_devCtx *dc);

/* Utility functions to manipulate global options effective for all devices */

void            NAPI AGP_getGlobalOptions(AGP_globalOptions *options);
void            NAPI AGP_setGlobalOptions(AGP_globalOptions *options);
void            NAPI AGP_saveGlobalOptions(AGP_globalOptions *options);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_AGP_H */

