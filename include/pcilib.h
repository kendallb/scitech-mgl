/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Description:  Header file for interface routines to the PCI bus.
*
****************************************************************************/

#ifndef __PCILIB_H
#define __PCILIB_H

#include "scitech.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

#undef  PCI_MAX_BUS
#define PCI_MAX_BUS         16
#define PCI_MAX_DEVICE      32
#define PCI_MAX_FUNCTION    8
#define NUM_PCI_REG         (sizeof(PCIDeviceInfo) / 4) - 10

#define PCIAPI  _ASMAPI     /* 'C' calling conventions always   */

/****************************************************************************
REMARKS:
Defines for the PCIDeviceInfo.HeaderType field. The names of the flags are
self explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_deviceType                  = 0x00,
    PCI_bridgeType                  = 0x01,
    PCI_cardBusBridgeType           = 0x02,
    PCI_multiFunctionType           = 0x80
    } PCIHeaderTypeFlags;

/****************************************************************************
REMARKS:
Defines for the PCIDeviceInfo.Command field, which control which features
of the device are enabled or disabled. A 1 in the field means that the
feature is enabled, 0 means it is disabled. The names of the flags are
self explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_enableIOSpace               = 0x0001,
    PCI_enableMemorySpace           = 0x0002,
    PCI_enableBusMaster             = 0x0004,
    PCI_enableSpecialCycles         = 0x0008,
    PCI_enableWriteAndInvalidate    = 0x0010,
    PCI_enableVGACompatiblePalette  = 0x0020,
    PCI_enableParity                = 0x0040,
    PCI_enableWaitCycle             = 0x0080,
    PCI_enableSerr                  = 0x0100,
    PCI_enableFastBackToBack        = 0x0200
    } PCICommandFlags;

/****************************************************************************
REMARKS:
Defines for the PCIDeviceInfo.Status field, which control which features
of the device are supported. A 1 in the field means that the feature is
supported, 0 means it is not supported. The names of the flags are self
explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_statusCapabilitiesList      = 0x0010,
    PCI_status66MhzCapable          = 0x0020,
    PCI_statusUDFSupported          = 0x0040,
    PCI_statusFastBackToBack        = 0x0080,
    PCI_statusDataParityDetected    = 0x0100,
    PCI_statusDevSel                = 0x0600,
    PCI_statusSignaledTargetAbort   = 0x0800,
    PCI_statusRecievedTargetAbort   = 0x1000,
    PCI_statusRecievedMasterAbort   = 0x2000,
    PCI_statusSignaledSystemError   = 0x4000,
    PCI_statusDetectedParityError   = 0x8000
    } PCIStatusFlags;

/****************************************************************************
REMARKS:
Defines for the known PCI class device types and sub class device types.
The names of the types are self explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_BRIDGE_CLASS            = 0x06,
    PCI_HOST_BRIDGE_SUBCLASS    = 0x00,
    PCI_ISA_BRIDGE_SUBCLASS     = 0x01,
    PCI_EISA_BRIDGE_SUBCLASS    = 0x02,
    PCI_MCA_BRIDGE_SUBCLASS     = 0x03,
    PCI_PCI_BRIDGE_SUBCLASS     = 0x04,
    PCI_PCMCIA_BRIDGE_SUBCLASS  = 0x05,
    PCI_NUBUS_BRIDGE_SUBCLASS   = 0x06,
    PCI_CARDBUS_BRIDGE_SUBCLASS = 0x07,
    PCI_EARLY_VGA_CLASS         = 0x00,
    PCI_EARLY_VGA_SUBCLASS      = 0x01,
    PCI_DISPLAY_CLASS           = 0x03,
    PCI_DISPLAY_VGA_SUBCLASS    = 0x00,
    PCI_DISPLAY_XGA_SUBCLASS    = 0x01,
    PCI_DISPLAY_OTHER_SUBCLASS  = 0x80,
    PCI_MM_CLASS                = 0x04,
    PCI_AUDIO_SUBCLASS          = 0x01,
    PCI_HD_AUDIO_SUBCLASS       = 0x03,
    PCI_OTHER_SUBCLASS          = 0x80
    } PCIClassTypes;

/****************************************************************************
REMARKS:
Defines for the PCI capability IDs, which define what extended PCI
capabilities a device supports. The names of the flags are self explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_capsPowerManagement         = 0x01,
    PCI_capsAGP                     = 0x02,
    PCI_capsMSI                     = 0x05,
    PCI_capsPCIe                    = 0x10
    } PCICapsType;

/****************************************************************************
REMARKS:
Function codes to pass to PCI_accessReg. The names of the flags are self
explanatory.

HEADER:
pcilib.h
****************************************************************************/
typedef enum {
    PCI_READ_BYTE               = 0,
    PCI_READ_WORD               = 1,
    PCI_READ_DWORD              = 2,
    PCI_WRITE_BYTE              = 3,
    PCI_WRITE_WORD              = 4,
    PCI_WRITE_DWORD             = 5
    } PCIAccessRegFlags;

/****************************************************************************
REMARKS:
Structure defining a PCI slot identifier.

NOTE:   We define all bitfield's as uint's, specifically so that the IBM
        Visual Age C++ compiler does not complain. We need them to be
        32-bits wide, and this is the width of an unsigned integer, but
        we can't use a ulong to make this explicit or we get errors.

HEADER:
pcilib.h

MEMBERS:
Zero        - Always set to zero
Register    - Field containing the PCI register index
Function    - Field containing the PCI function index
Device      - Field containing the PCI device index
Bus         - Field containing the PCI bus index
Reserved    - Reserved (always 0)
Enable      - Enable bit to enable this slot
i           - Union field to access as a 32-bit integer
****************************************************************************/
typedef union {
    struct {
        u32     Zero:2;
        u32     Register:6;
        u32     Function:3;
        u32     Device:5;
        u32     Bus:8;
        u32     Reserved:7;
        u32     Enable:1;
        } p;
    u32     i;
    } PCIslot;

/****************************************************************************
REMARKS:
Structure defining the regular (type 0) PCI configuration register layout.
We use this in the PCIDeviceInfo union so we can describe all types of PCI
configuration spaces with a single structure.

NOTE:   The PCI base address length values are not actually in the PCI
        configuration space, but are calculated when the configuration
        space is enumerated as they are useful values to know.

HEADER:
pcilib.h

MEMBERS:
BaseAddress10       - Base address register (BAR) 10h
BaseAddress14       - Base address register (BAR) 14h
BaseAddress18       - Base address register (BAR) 18h
BaseAddress1C       - Base address register (BAR) 1Ch
BaseAddress20       - Base address register (BAR) 20h
BaseAddress24       - Base address register (BAR) 24h
CardbusCISPointer   - Pointer to CardBus Information Structure in config space
SubSystemVendorID   - Sub System Vendor ID for this device type
SubSystemID         - Sub System ID for this device type
ROMBaseAddress      - Base address for ROM on device (if any)
CapabilitiesPointer - Pointer to PCI capabilities list
reserved1           - Reserved: not used for this device type
reserved2           - Reserved: not used for this device type
reserved3           - Reserved: not used for this device type
reserved4           - Reserved: not used for this device type
InterruptLine       - Interrupt line assigned to this device
InterruptPin        - Interrupt pin assigned to this device
MinimumGrant        - Minimum interrupt grant assigned to this device
MaximumLatency      - Maximum interrupt latency assigned to this device
BaseAddress10Len    - Length of BAR 10 (calculated value)
BaseAddress14Len    - Length of BAR 14 (calculated value)
BaseAddress18Len    - Length of BAR 18 (calculated value)
BaseAddress1CLen    - Length of BAR 1C (calculated value)
BaseAddress20Len    - Length of BAR 20 (calculated value)
BaseAddress24Len    - Length of BAR 24 (calculated value)
ROMBaseAddressLen   - Length of ROM (calculated value)
****************************************************************************/
typedef struct {
    u32     BaseAddress10;
    u32     BaseAddress14;
    u32     BaseAddress18;
    u32     BaseAddress1C;
    u32     BaseAddress20;
    u32     BaseAddress24;
    u32     CardbusCISPointer;
    u16     SubSystemVendorID;
    u16     SubSystemID;
    u32     ROMBaseAddress;
    u8      CapabilitiesPointer;
    u8      reserved1;
    u8      reserved2;
    u8      reserved3;
    u32     reserved4;
    u8      InterruptLine;
    u8      InterruptPin;
    u8      MinimumGrant;
    u8      MaximumLatency;
    u32     BaseAddress10Len;
    u32     BaseAddress14Len;
    u32     BaseAddress18Len;
    u32     BaseAddress1CLen;
    u32     BaseAddress20Len;
    u32     BaseAddress24Len;
    u32     ROMBaseAddressLen;
    } PCIType0Info;

/****************************************************************************
REMARKS:
Structure defining PCI to PCI bridge (type 1) PCI configuration register
layout. We use this in the PCIDeviceInfo union so we can describe all types
of PCI configuration spaces with a single structure.

HEADER:
pcilib.h

MEMBERS:
BaseAddress10           - Base address register (BAR) 10h
BaseAddress14           - Base address register (BAR) 14h
PrimaryBusNumber        - Primary bus number this bridge lives on
SecondaryBusNumber      - Secondary bus number this bridge controls
SubordinateBusNumber    - Subordinate bus number for this bridge
SecondaryLatencyTimer   - Secondary latency timer
IOBase                  - I/O base address for bridge control registers
IOLimit                 - I/O limit for bridge control registers
SecondaryStatus         - Secondary status
MemoryBase              - Memory mapped base address for bridge control registers
MemoryLimit             - Memory mapped limit for bridge control registers
PrefetchableMemoryBase  - Base of pre-fetchable memory on bus
PrefetchableMemoryLimit - Length of pre-fetchable memory on bus
PrefetchableBaseHi      - High portion of prefetchable base value
PrefetchableLimitHi     - High portion of prefetchable limit value
IOBaseHi                - High value of I/O base address
IOLimitHi               - High value of I/O limit
CapabilitiesPointer     - Pointer to PCI bridge capabilities list
reserved1               - Reserved: not used for this device type
reserved2               - Reserved: not used for this device type
reserved3               - Reserved: not used for this device type
ROMBaseAddress          - Address if ROM for bridge (if any)
InterruptLine           - Interrupt line assigned to this device
InterruptPin            - Interrupt pin assigned to this device
BridgeControl           - Bridge control register
****************************************************************************/
typedef struct {
    u32     BaseAddress10;
    u32     BaseAddress14;
    u8      PrimaryBusNumber;
    u8      SecondaryBusNumber;
    u8      SubordinateBusNumber;
    u8      SecondaryLatencyTimer;
    u8      IOBase;
    u8      IOLimit;
    u16     SecondaryStatus;
    u16     MemoryBase;
    u16     MemoryLimit;
    u16     PrefetchableMemoryBase;
    u16     PrefetchableMemoryLimit;
    u32     PrefetchableBaseHi;
    u32     PrefetchableLimitHi;
    u16     IOBaseHi;
    u16     IOLimitHi;
    u8      CapabilitiesPointer;
    u8      reserved1;
    u8      reserved2;
    u8      reserved3;
    u32     ROMBaseAddress;
    u8      InterruptLine;
    u8      InterruptPin;
    u16     BridgeControl;
    } PCIType1Info;

/****************************************************************************
REMARKS:
Structure defining PCI to CardBus bridge (type 2) PCI configuration register
layout. We use this in the PCIDeviceInfo union so we can describe all types
of PCI configuration spaces with a single structure.

HEADER:
pcilib.h

MEMBERS:
SocketRegistersBaseAddress  - Base address for control registers
CapabilitiesPointer         - CardBus bridge capabilities pointer
reserved1                   - Reserved: not used for this device type
SecondaryStatus             - Secondary status
PrimaryBus                  - Primary bus number bridge is connected to
SecondaryBus                - Secondary bus bridge controls
SubordinateBus              - Subordinate bus for bridge
SecondaryLatency            - Secondary latency
Range                       - Array of four base/limit ranges
InterruptLine               - Interrupt line assigned to this device
InterruptPin                - Interrupt pin assigned to this device
BridgeControl               - Bridge control register
****************************************************************************/
typedef struct {
    u32     SocketRegistersBaseAddress;
    u8      CapabilitiesPointer;
    u8      reserved1;
    u16     SecondaryStatus;
    u8      PrimaryBus;
    u8      SecondaryBus;
    u8      SubordinateBus;
    u8      SecondaryLatency;
    struct  {
        u32     Base;
        u32     Limit;
        } Range[4];
    u8      InterruptLine;
    u8      InterruptPin;
    u16     BridgeControl;
    } PCIType2Info;

/****************************************************************************
REMARKS:
Structure defining the PCI configuration space information for a single PCI
device on the PCI bus. We enumerate all this information for all PCI devices
on the bus.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
pcilib.h

MEMBERS:
dwSize          - Set to size of structure in bytes
slot            - PCI slot identifier for this device
mech1           - True if we enumerated this bus using PCI access mechanism 1
VendorID        - Unique PCI device Vendor ID value
DeviceID        - Unique PCI device Device ID value
Command         - Device command register used to control the device (PCICommandFlags)
Status          - Device status register flags (PCIStatusFlags)
RevID           - Device revision ID value
Interface       - Device interface type value
SubClass        - Device Sub Class field
BaseClass       - Device Base Class field
CacheLineSize   - Cache line size for the device
LatencyTimer    - Latency timer value
HeaderType      - Header type field, defining type of info (PCIHeaderTypeFlags)
BIST            - BIST value
type0           - Union to access PCI type 0 specific information
type1           - Union to access PCI type 1 specific information
type2           - Union to access PCI type 2 specific information
****************************************************************************/
typedef struct {
    u32                 dwSize;
    PCIslot             slot;
    u32                 mech1;
    u16                 VendorID;
    u16                 DeviceID;
    u16                 Command;
    u16                 Status;
    u8                  RevID;
    u8                  Interface;
    u8                  SubClass;
    u8                  BaseClass;
    u8                  CacheLineSize;
    u8                  LatencyTimer;
    u8                  HeaderType;
    u8                  BIST;
    union {
        PCIType0Info    type0;
        PCIType1Info    type1;
        PCIType2Info    type2;
        } u;
    } PCIDeviceInfo;

/****************************************************************************
REMARKS:
PCI Capability header structure. All PCI capabilities have this header
field to describe the capability type.

HEADER:
pcilib.h

MEMBERS:
capsID  - Used to identify the type of the structure (PCICapsType).
next    - Next is the offset in PCI configuration space (0x40-0xFC) of the
          next capability structure in the list, or 0x00 if there are no
          more entries.
****************************************************************************/
typedef struct {
    u8      capsID;
    u8      next;
    } PCICapsHeader;

/****************************************************************************
REMARKS:
Structure defining the PCI AGP status register contents

HEADER:
pcilib.h

MEMBERS:
rate                        - Supported AGP rate
agp3mode                    - True if AGP 3.0 mode active
fastWrite                   - AGP FastWrite supported
fourGB                      - 4GB addressing supported
noHostTrans                 - Disable GART translations (target only, 3.0)
rsvd1                       - Reserved; not used
sideBandAddressing          - Side band addressing supported (required for 3.0)
calCycle                    - Required period for core logic bus cycle (3.0)
arqsz                       - Log2 of optimium async request in bytes (3.0)
rsvd2                       - Reserved; not used
requestQueueDepthMaximum    - Maximum request queue depth (target only)
****************************************************************************/
typedef struct {
#ifdef __BIG_ENDIAN__
    u32     requestQueueDepthMaximum:8;
    u32     rsvd2:8;
    u32     arqsz:3;
    u32     calCycle:3;
    u32     sideBandAddressing:1;
    u32     rsvd1:2;
    u32     noHostTrans:1;
    u32     fourGB:1;
    u32     fastWrite:1;
    u32     agp3mode:1;
    u32     rate:3;
#else
    u32     rate:3;
    u32     agp3mode:1;
    u32     fastWrite:1;
    u32     fourGB:1;
    u32     noHostTrans:1;
    u32     rsvd1:2;
    u32     sideBandAddressing:1;
    u32     calCycle:3;
    u32     arqsz:3;
    u32     rsvd2:8;
    u32     requestQueueDepthMaximum:8;
#endif
    } PCIAGPStatus;

/****************************************************************************
REMARKS:
Structure defining the PCI AGP command register contents

HEADER:
pcilib.h

MEMBERS:
rate                - Enable AGP rate
rsvd1               - Reserved; not used
fastWriteEnable     - Enable AGP FastWrite
fourGBEnable        - Enable 4GB addressing
rsvd2               - Reserved; not used
AGPEnable           - Enable AGP bus
SBAEnable           - Enable side band addressing
calCycle            - Programmed core logic bus cycle (target only, 3.0)
arqsz               - Log2 of optimium async request in bytes (master only, 3.0)
rsvd3               - Reserved; not used
requestQueueDepth   - Request queue depth (master only)
****************************************************************************/
typedef struct {
#ifdef __BIG_ENDIAN__
    u32     requestQueueDepth:8;
    u32     rsvd3:8;
    u32     arqsz:3;
    u32     calCycle:3;
    u32     SBAEnable:1;
    u32     AGPEnable:1;
    u32     rsvd2:2;
    u32     fourGBEnable:1;
    u32     fastWriteEnable:1;
    u32     rsvd1:1;
    u32     rate:3;
#else
    u32     rate:3;
    u32     rsvd1:1;
    u32     fastWriteEnable:1;
    u32     fourGBEnable:1;
    u32     rsvd2:2;
    u32     AGPEnable:1;
    u32     SBAEnable:1;
    u32     calCycle:3;
    u32     arqsz:3;
    u32     rsvd3:8;
    u32     requestQueueDepth:8;
#endif
    } PCIAGPCommand;

/****************************************************************************
REMARKS:
AGP Capability structure

HEADER:
pcilib.h

MEMBERS:
h           - PCI capabilities header block
majMin      - Major/minor number
AGPStatus   - AGP status field
AGPCommand  - AGP command field
****************************************************************************/
typedef struct {
    PCICapsHeader   h;
    u16             majMin;
    PCIAGPStatus    AGPStatus;
    PCIAGPCommand   AGPCommand;
    } PCIAGPCapability;

/****************************************************************************
REMARKS:
Structure for obtaining the PCI IRQ routing information

HEADER:
pcilib.h

{secret} Not really implemented yet...
****************************************************************************/
typedef struct {
    u8      bus;
    u8      device;
    u8      linkA;
    u16     mapA;
    u8      linkB;
    u16     mapB;
    u8      linkC;
    u16     mapC;
    u8      linkD;
    u16     mapD;
    u8      slot;
    u8      reserved;
    } PCIRouteInfo;

/****************************************************************************
REMARKS:
Structure for obtaining the PCI IRQ routing information

HEADER:
pcilib.h

{secret} Not really implemented yet...
****************************************************************************/
typedef struct {
    u16             BufferSize;
    PCIRouteInfo    *DataBuffer;
    } PCIRoutingOptionsBuffer;

/* Macros to detect specific classes of devices */

#define PCI_IS_3DLABS(pci)  ((pci)->VendorID == 0x3D3D || (pci)->VendorID == 0x104C)

#define PCI_IS_OTHER_DISPLAY_CLASS(pci) \
   (((pci)->BaseClass == PCI_DISPLAY_CLASS && (pci)->SubClass == PCI_DISPLAY_OTHER_SUBCLASS) \
    && !PCI_IS_3DLABS(pci))

#define PCI_IS_SECONDARY_DISPLAY_ID(pci) \
    ((pci)->slot.p.Function > 0 && PCI_IS_OTHER_DISPLAY_CLASS(pci))

#define PCI_IS_DISPLAY_CLASS_CANDIDATE(pci) \
   (((pci)->BaseClass == PCI_DISPLAY_CLASS && (pci)->SubClass == PCI_DISPLAY_VGA_SUBCLASS) \
 || ((pci)->BaseClass == PCI_DISPLAY_CLASS && (pci)->SubClass == PCI_DISPLAY_XGA_SUBCLASS) \
 || ((pci)->BaseClass == PCI_EARLY_VGA_CLASS && (pci)->SubClass == PCI_EARLY_VGA_SUBCLASS) \
 || ((pci)->BaseClass == PCI_DISPLAY_CLASS && (pci)->SubClass == PCI_DISPLAY_OTHER_SUBCLASS))

#define PCI_IS_DISPLAY_CLASS(pci) \
    (PCI_IS_DISPLAY_CLASS_CANDIDATE(pci) && !PCI_IS_SECONDARY_DISPLAY_ID(pci))

/* Macros to read/write PCI registers. These assume a global PCI array
 * of device information.
 */

#define PCI_readPCIRegB(index,device)   \
    PCI_accessReg(index,0,0,&PCI[DeviceIndex[device]])

#define PCI_readPCIRegW(index,device)   \
    PCI_accessReg(index,0,1,&PCI[DeviceIndex[device]])

#define PCI_readPCIRegL(index,device)   \
    PCI_accessReg(index,0,2,&PCI[DeviceIndex[device]])

#define PCI_writePCIRegB(index,value,device)    \
    PCI_accessReg(index,value,3,&PCI[DeviceIndex[device]])

#define PCI_writePCIRegW(index,value,device)    \
    PCI_accessReg(index,value,4,&PCI[DeviceIndex[device]])

#define PCI_writePCIRegL(index,value,device)    \
    PCI_accessReg(index,value,5,&PCI[DeviceIndex[device]])

#pragma pack()

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {                        /* Use "C" linkage when in C++ mode */
#endif

/* Function to determine the number of PCI devices in the system */

int     PCIAPI PCI_getNumDevices(void);

/* Function to enumerate all device on the PCI bus */

int     PCIAPI PCI_enumerate(PCIDeviceInfo info[]);

/* Function to access PCI configuration registers */

u32     PCIAPI PCI_accessReg(int index,u32 value,int func,PCIDeviceInfo *info);

/* Function to read a block of PCI configuration space registers */

void    PCIAPI PCI_readRegBlock(PCIDeviceInfo *info,int index,void *dst,int count);

/* Function to write a block of PCI configuration space registers */

void    PCIAPI PCI_writeRegBlock(PCIDeviceInfo *info,int index,void *src,int count);

/* Functions used to access PCI BIOS directly */
/* {secret} */
int     PCIAPI  PCIBIOS_detect(uchar *hwType,uchar *lastBus);
/* {secret} */
u32     PCIAPI  PCIBIOS_readDWORD(int index,u32 slot);
/* {secret} */
u32     PCIAPI _PCIBIOS_service(u32 r_eax,u32 r_ebx,u32 r_edi,u32 r_ecx);

/* Functions not used at the moment (not fully tested) */
/* {secret} */
u32     PCIAPI PCI_getBIOSEntry(void);
/* {secret} */
int     PCIAPI PCI_getIRQRoutingOptions(int numDevices,PCIRouteInfo *buffer);
/* {secret} */
ibool   PCIAPI PCI_setHardwareIRQ(PCIDeviceInfo *info,uint intPin,uint IRQ);
/* {secret} */
void    PCIAPI PCI_generateSpecialCyle(uint bus,u32 specialCycleData);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __PCILIB_H */

