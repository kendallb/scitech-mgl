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
* Description:  Module for interfacing to the PCI bus and configuration
*               space registers.
*
****************************************************************************/

#include "pmapi.h"
#include "pcilib.h"
#if !defined(__WIN32_VXD__) && !defined(__NT_DRIVER__)
#include <string.h>
#endif
#ifdef __WINDOWS32__
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "pm_help.h"
#endif
#ifdef __LINUX__
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/types.h>
#include <errno.h>
#endif

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Length of the memory mapping for the PCI BIOS */

#define BIOS_LIMIT          (128 * 1024L - 1)

/* Macros for accessing the PCI BIOS functions from 32-bit protected mode */

#define BIOS32_SIGNATURE    (((u32)'_' << 0) + ((u32)'3' << 8) + ((u32)'2' << 16) + ((u32)'_' << 24))
#define PCI_SIGNATURE       (((u32)'P' << 0) + ((u32)'C' << 8) + ((u32)'I' << 16) + ((u32)' ' << 24))
#define PCI_SERVICE         (((u32)'$' << 0) + ((u32)'P' << 8) + ((u32)'C' << 16) + ((u32)'I' << 24))
#define PCI_BIOS_PRESENT    0xB101
#define FIND_PCI_DEVICE     0xB102
#define FIND_PCI_CLASS      0xB103
#define GENERATE_SPECIAL    0xB106
#define READ_CONFIG_BYTE    0xB108
#define READ_CONFIG_WORD    0xB109
#define READ_CONFIG_DWORD   0xB10A
#define WRITE_CONFIG_BYTE   0xB10B
#define WRITE_CONFIG_WORD   0xB10C
#define WRITE_CONFIG_DWORD  0xB10D
#define GET_IRQ_ROUTING_OPT 0xB10E
#define SET_PCI_IRQ         0xB10F

/* This is the standard structure used to identify the entry point to the
 * BIOS32 Service Directory, as documented in PCI 2.1 BIOS Specicition.
 * {secret}
 */
typedef union {
    struct {
        u32   signature;        /* _32_                                 */
        u32   entry;            /* 32 bit physical address              */
        uchar revision;         /* Revision level, 0                    */
        uchar length;           /* Length in paragraphs should be 01    */
        uchar checksum;         /* All bytes must add up to zero        */
        uchar reserved[5];      /* Must be zero                         */
        } fields;
    char chars[16];
    } PCI_bios32;

/* Structure for a far pointer to call the PCI BIOS services with
 * {secret}
 */
typedef struct {
    u32     address;
    ushort  segment;
    } PCIBIOS_entry;

/* Macros to copy a structure that includes dwSize members */

#define COPY_STRUCTURE(d,s) memcpy(d,s,MIN((s)->dwSize,(d)->dwSize))

#pragma pack()

/*--------------------------- Global variables ----------------------------*/

#ifdef __INTEL__
#ifdef __WINDOWS32__
extern HANDLE           _PM_hDevice;        /* Handle to Win32 driver   */
#else
static uchar            *BIOSImage = NULL;  /* BIOS image mapping       */
static int              PCIBIOSVersion = -1;/* PCI BIOS version         */
static ulong            PCIPhysEntry = 0;   /* Physical address         */
#endif
PCIBIOS_entry _VARAPI   _PM_PCIEntry = {0}; /* PCI services entry point */
#endif

/*----------------------------- Implementation ----------------------------*/

#ifdef __INTEL__

/* External assembler helper functions */

/* {secret} */
uchar   PCIAPI _BIOS32_service(ulong service,ulong function,ulong *physBase,ulong *length,ulong *serviceOffset);
/* {secret} */
ushort  PCIAPI _PCIBIOS_isPresent(ulong i_eax,ulong *o_edx,ushort *o_ax,uchar *o_cl);
/* {secret} */
int     PCIAPI _PCIBIOS_getRouting(PCIRoutingOptionsBuffer *buf);
/* {secret} */
ibool   PCIAPI _PCIBIOS_setIRQ(int busDev,int intPin,int IRQ);
/* {secret} */
ulong   PCIAPI _PCIBIOS_specialCycle(int bus,ulong data);
/* {secret} */
ushort  PCIAPI _PCI_getCS(void);

/****************************************************************************
Returns the physical address of the PCI BIOS entry point. This is useful
for external code that needs to call the PCI BIOS directly for features
not supported by the PCI library.
{secret}
****************************************************************************/
u32 PCIAPI PCI_getBIOSEntry(void)
{
#ifdef __WINDOWS32__
    return 0;
#else
    return PCIPhysEntry;
#endif
}

/****************************************************************************
PARAMETERS:
hwType  - Place to store the PCI hardware access mechanism flags
lastBus - Place to store the index of the last PCI bus in the system

RETURNS:
Version number of the PCI BIOS found.

REMARKS:
This function determines if the PCI BIOS is present in the system, and if
so returns the information returned by the PCI BIOS detect function.
****************************************************************************/
int PCIAPI PCIBIOS_detect(
    uchar *hwType,
    uchar *lastBus)
{
#if defined(__WINDOWS32__)
    /* Pass on to ring-0 service if running at ring-3. */
    DWORD   inBuf[2];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    /* Pass in pointers to parameters in Win32 user space */
    inBuf[0] = (DWORD)&hwType;
    inBuf[1] = (DWORD)&lastBus;
    if (DeviceIoControl(_PM_hDevice, PMHELP_PCIBIOSDETECT32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
#else
    ulong   signature;
    ushort  stat,version;

#ifndef __16BIT__
    uchar           *BIOSEnd;
    PCI_bios32      *BIOSDir;
    ulong           physBase,length,offset;

    /* Bail if we have already detected no BIOS is present */
    if (PCIBIOSVersion == 0)
        return 0;

    /* First scan the memory from 0xE0000 to 0xFFFFF looking for the
     * BIOS32 service directory, so we can determine if we can call it
     * from 32-bit protected mode.
     */
    if (PCIBIOSVersion == -1) {
        PCIBIOSVersion = 0;
        BIOSImage = PM_mapPhysicalAddr(0xE0000,BIOS_LIMIT,false);
        if (!BIOSImage)
            return 0;
        BIOSEnd = BIOSImage + 0x20000;
        for (BIOSDir = (PCI_bios32*)BIOSImage; BIOSDir < (PCI_bios32*)BIOSEnd; BIOSDir++) {
            uchar   sum;
            int     i,length;

            if (BIOSDir->fields.signature != BIOS32_SIGNATURE)
                continue;
            length = BIOSDir->fields.length * 16;
            if (!length)
                continue;
            for (sum = i = 0; i < length; i++)
                sum += BIOSDir->chars[i];
            if (sum != 0)
                continue;
            _PM_PCIEntry.address = (ulong)BIOSImage + (BIOSDir->fields.entry - 0xE0000);
            _PM_PCIEntry.segment = _PCI_getCS();
            break;
            }

        /* If we found the BIOS32 directory, call it to get the address of the
         * PCI services.
         */
        if (_PM_PCIEntry.address == 0)
            return 0;
        if (_BIOS32_service(PCI_SERVICE,0,&physBase,&length,&offset) != 0)
            return 0;
        PCIPhysEntry = physBase + offset;
        _PM_PCIEntry.address = (ulong)BIOSImage + (PCIPhysEntry - 0xE0000);
        _PM_PCIEntry.segment = _PCI_getCS();
        }
#endif
    /* We found the BIOS entry, so now do the version check */
    version = _PCIBIOS_isPresent(PCI_BIOS_PRESENT,&signature,&stat,lastBus);
    if (version > 0 && ((stat >> 8) == 0) && signature == PCI_SIGNATURE) {
        *hwType = stat & 0xFF;
        return PCIBIOSVersion = version;
        }
    return 0;
#endif
}

#ifdef __WINDOWS32__
/****************************************************************************
REMARKS:
This functions reads/writes configuration data via the PCI BIOS. For Win32
we have to call into the ring 0 driver to make this work.
****************************************************************************/
static N_uint32 PCIAPI _PCIBIOS_service(
    N_uint32  r_eax,
    N_uint32  r_ebx,
    N_uint32  r_edi,
    N_uint32  r_ecx)
{
    DWORD   inBuf[4];   /* Buffer to send data to VxD       */
    DWORD   outBuf[1];  /* Buffer to receive data from VxD  */
    DWORD   count;      /* Count of bytes returned from VxD */

    inBuf[0] = r_eax;
    inBuf[1] = r_ebx;
    inBuf[2] = r_edi;
    inBuf[3] = r_ecx;
    if (DeviceIoControl(_PM_hDevice, PMHELP_PCIBIOSSERVICE32, inBuf, sizeof(inBuf),
            outBuf, sizeof(outBuf), &count, NULL))
        return outBuf[0];
    return 0;
}
#endif

/****************************************************************************
PARAMETERS:
numDevices  - Number of devices to query info for

RETURNS:
0 on success, -1 on error, number of devices to enumerate if numDevices = 0

REMARKS:
This function reads the PCI routing information. If you pass a value of
0 for numDevices, this function will return with the number of devices
needed in the routing buffer that will be filled in by the BIOS.
{secret}
****************************************************************************/
ibool PCIAPI PCI_getIRQRoutingOptions(
    int numDevices,
    PCIRouteInfo *buffer)
{
#ifndef __WINDOWS32__
    PCIRoutingOptionsBuffer buf;
    int                     ret;

    if (PCIPhysEntry) {
        buf.BufferSize = numDevices * sizeof(PCIRouteInfo);
        buf.DataBuffer = buffer;
        if ((ret = _PCIBIOS_getRouting(&buf)) == 0x89)
            return buf.BufferSize / sizeof(PCIRouteInfo);
        if (ret != 0)
            return -1;
        return 0;
        }
#endif
    /* We currently only support this via the PCI BIOS functions */
    return -1;
}

/****************************************************************************
PARAMETERS:
info    - PCI device information for the specified device
intPin  - Value to store in the PCI InterruptPin register
IRQ     - New ISA IRQ to map the PCI interrupt to (0-15)

RETURNS:
True on success, or false if this function failed.

REMARKS:
This function changes the PCI IRQ routing for the specified device to the
desired PCI interrupt and the desired ISA bus compatible IRQ. This function
may not be supported by the PCI BIOS, in which case this function will
fail.
{secret}
****************************************************************************/
ibool PCIAPI PCI_setHardwareIRQ(
    PCIDeviceInfo *info,
    uint intPin,
    uint IRQ)
{
#ifndef __WINDOWS32__
    if (PCIPhysEntry) {
        if (_PCIBIOS_setIRQ(info->slot.i >> 8,intPin,IRQ)) {
            info->u.type0.InterruptPin = intPin;
            info->u.type0.InterruptLine = IRQ;
            return true;
            }
        return false;
        }
#endif
    /* We currently only support this via the PCI BIOS functions */
    return false;
}

/****************************************************************************
PARAMETERS:
bus                 - Bus number to generate the special cycle for
specialCycleData    - Data to send for the special cyle

REMARKS:
This function generates a special cycle on the specified bus using with
the specified data.
{secret}
****************************************************************************/
void PCIAPI PCI_generateSpecialCyle(
    uint bus,
    u32 specialCycleData)
{
#ifndef __WINDOWS32__
    if (PCIPhysEntry)
        _PCIBIOS_specialCycle(bus,specialCycleData);
    /* We currently only support this via the PCI BIOS functions */
#endif
}

/****************************************************************************
PARAMETERS:
pci         - Pointer to the new device to check against
info        - Array of PCI devices to check against
numFound    - Number of devices in array to check against

RETURNS:
True if the device is a duplicate, false if not.

REMARKS:
This function goes through the list of all devices preceeding the newly
found device in the info structure, and checks that the device is not a
duplicate of a previous device. Some devices incorrectly enumerate
themselves at different function addresses so we check here to exclude
those cases.
****************************************************************************/
static ibool CheckDuplicate(
    PCIDeviceInfo *pci,
    PCIDeviceInfo *info,
    int numFound)
{
    /* Ignore devices with a vendor ID of 0 */
    if (pci->VendorID == 0)
        return true;

    /* Scan all existing devices to find duplicates */
    while (numFound--) {
        /* Ignore devices with the same device ID on the same bus with
         * the same device numbers
         */
        if (pci->slot.p.Bus == info->slot.p.Bus &&
            pci->slot.p.Device == info->slot.p.Device &&
            pci->DeviceID == info->DeviceID)
            return true;

        /* Ignore devices on different busses that have identical vendor
         * and device ID's, yet have identical resources allocated to them. It
         * it not possible for two devices to have the same memory and I/O
         * regions assigned to them.
         */
        if (pci->VendorID == info->VendorID && pci->DeviceID == info->DeviceID) {
            if ((pci->HeaderType & (~PCI_multiFunctionType)) == PCI_deviceType) {
                if (memcmp((void*)&pci->u.type0.BaseAddress10,(void*)&info->u.type0.BaseAddress10,
                    (&pci->u.type0.BaseAddress24 - &pci->u.type0.BaseAddress10)) == 0)
                    return true;
                }
            }
        info++;
        }
    return false;
}

/****************************************************************************
PARAMETERS:
info        - Array of PCIDeviceInfo structures to fill in
maxDevices  - Maximum number of of devices to enumerate into array

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate all available devices on the PCI bus into an array
of configuration information blocks.
****************************************************************************/
static int PCI_enumerateMech1(
    PCIDeviceInfo info[])
{
    int             bus,device,function,i,numFound = 0;
    ulong           *lp,tmp;
    PCIslot         slot = {{0,0,0,0,0,0,1}};
    PCIDeviceInfo   pci,prev = {0};

    /* Try PCI access mechanism 1 */
    PM_outpb(0xCFB,0x01);
    tmp = PM_inpd(0xCF8);
    PM_outpd(0xCF8,slot.i);
    if ((PM_inpd(0xCF8) == slot.i) && (PM_inpd(0xCFC) != 0xFFFFFFFFUL)) {
        /* PCI access mechanism 1 - the preferred mechanism */
        for (bus = 0; bus < PCI_MAX_BUS; bus++) {
            slot.p.Bus = bus;
            for (device = 0; device < PCI_MAX_DEVICE; device++) {
                slot.p.Device = device;
                for (function = 0; function < PCI_MAX_FUNCTION; function++) {
                    slot.p.Function = function;
                    slot.p.Register = 0;
                    PM_outpd(0xCF8,slot.i);
                    if (PM_inpd(0xCFC) != 0xFFFFFFFFUL) {
                        memset(&pci,0,sizeof(pci));
                        pci.dwSize = sizeof(pci);
                        pci.mech1 = 1;
                        pci.slot = slot;
                        lp = (ulong*)&(pci.VendorID);
                        for (i = 0; i < NUM_PCI_REG; i++, lp++) {
                            slot.p.Register = i;
                            PM_outpd(0xCF8,slot.i);
                            *lp = PM_inpd(0xCFC);
                            }
                        if (!CheckDuplicate(&pci,info ? info : &prev,info ? numFound : 1)) {
                            if (info)
                                COPY_STRUCTURE(&info[numFound],&pci);
                            ++numFound;
                            }
                        prev = pci;
                        }
                    }
                }
            }

        /* Disable PCI config cycle on exit */
        PM_outpd(0xCF8,0);
        return numFound;
        }
    PM_outpd(0xCF8,tmp);

    /* No hardware access mechanism 1 found */
    return 0;
}

/****************************************************************************
PARAMETERS:
info        - Array of PCIDeviceInfo structures to fill in
maxDevices  - Maximum number of of devices to enumerate into array

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate all available devices on the PCI bus into an array
of configuration information blocks.
****************************************************************************/
static int PCI_enumerateMech2(
    PCIDeviceInfo info[])
{
    int             bus,device,function,i,numFound = 0;
    ushort          deviceIO;
    ulong           *lp;
    PCIslot         slot = {{0,0,0,0,0,0,1}};
    PCIDeviceInfo   pci,prev = {0};

    /* Try PCI access mechanism 2 */
    PM_outpb(0xCFB,0x00);
    PM_outpb(0xCF8,0x00);
    PM_outpb(0xCFA,0x00);
    if (PM_inpb(0xCF8) == 0x00 && PM_inpb(0xCFB) == 0x00) {
        /* PCI access mechanism 2 - the older mechanism for legacy busses */
        for (bus = 0; bus < 2; bus++) {
            slot.p.Bus = bus;
            PM_outpb(0xCFA,(uchar)bus);
            for (device = 0; device < 16; device++) {
                slot.p.Device = device;
                deviceIO = 0xC000 + (device << 8);
                for (function = 0; function < 8; function++) {
                    slot.p.Function = function;
                    slot.p.Register = 0;
                    PM_outpb(0xCF8,(uchar)((function << 1) | 0x10));
                    if (PM_inpd(deviceIO) != 0xFFFFFFFFUL) {
                        memset(&pci,0,sizeof(pci));
                        pci.dwSize = sizeof(pci);
                        pci.mech1 = 0;
                        pci.slot = slot;
                        lp = (ulong*)&(pci.VendorID);
                        for (i = 0; i < NUM_PCI_REG; i++, lp++) {
                            slot.p.Register = i;
                            *lp = PM_inpd(deviceIO + (i << 2));
                            }
                        if (!CheckDuplicate(&pci,info ? info : &prev,info ? numFound : 1)) {
                            if (info)
                                COPY_STRUCTURE(&info[numFound],&pci);
                            ++numFound;
                            }
                        prev = pci;
                        }
                    }
                }
            }

        /* Disable PCI config cycle on exit */
        PM_outpb(0xCF8,0);
        return numFound;
        }

    /* No hardware access mechanism 2 found */
    return 0;
}

/****************************************************************************
REMARKS:
This functions reads a configuration dword via the PCI BIOS.
****************************************************************************/
u32 PCIAPI PCIBIOS_readDWORD(
    int index,
    u32 slot)
{
    return _PCIBIOS_service(READ_CONFIG_DWORD,slot >> 8,index,0);
}

/****************************************************************************
PARAMETERS:
info        - Array of PCIDeviceInfo structures to fill in
maxDevices  - Maximum number of of devices to enumerate into array

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate all available devices on the PCI bus into an array
of configuration information blocks.
****************************************************************************/
static int PCI_enumerateBIOS(
    PCIDeviceInfo info[])
{
    uchar           hwType,lastBus;
    int             bus,device,function,i,numFound = 0;
    ulong           *lp;
    PCIslot         slot = {{0,0,0,0,0,0,1}};
    PCIDeviceInfo   pci,prev = {0};

    if (PCIBIOS_detect(&hwType,&lastBus)) {
        /* PCI BIOS access - the ultimate fallback */
        for (bus = 0; bus <= lastBus; bus++) {
            slot.p.Bus = bus;
            for (device = 0; device < PCI_MAX_DEVICE; device++) {
                slot.p.Device = device;
                for (function = 0; function < PCI_MAX_FUNCTION; function++) {
                    slot.p.Function = function;
                    if (PCIBIOS_readDWORD(0,slot.i) != 0xFFFFFFFFUL) {
                        memset(&pci,0,sizeof(pci));
                        pci.dwSize = sizeof(pci);
                        pci.mech1 = 2;
                        pci.slot = slot;
                        lp = (ulong*)&(pci.VendorID);
                        for (i = 0; i < NUM_PCI_REG; i++, lp++)
                            *lp = PCIBIOS_readDWORD(i << 2,slot.i);
                        if (!CheckDuplicate(&pci,info ? info : &prev,info ? numFound : 1)) {
                            if (info)
                                COPY_STRUCTURE(&info[numFound],&pci);
                            ++numFound;
                            }
                        prev = pci;
                        }
                    }
                }
            }
        }

    /* Return number of devices found */
    return numFound;
}

#else /* !__INTEL__ */

/*
    Linux PCI specifics
        Under Linux we access the PCI bus through the kernel
        using /proc/bus/pci and friends
        Note that we do open and close on each PCI operation
*/

#define PROC_BUS_PCI    "/proc/bus/pci"

/*
 * The PCI interface treats multi-function devices as independent
 * devices.  The slot/function address of each device is encoded
 * in a single byte as follows:
 *
 *  7:3 = slot
 *  2:0 = function
 */
#define PCI_SLOT(devfn)         (((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)         ((devfn) & 0x07)

/****************************************************************************
PARAMETERS:
info        - Array of PCIDeviceInfo structures to fill in
maxDevices  - Maximum number of of devices to enumerate into array

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate all available devices on the PCI bus into an array
of configuration information blocks. We do this using the Linnux proc/pci
interface.
****************************************************************************/
static int PCI_enumerateLinux(
    PCIDeviceInfo info[])
{
    int             nb=0;
    char            dev[sizeof(PROC_BUS_PCI)+30];
    FILE            *fp;
    char            buff[512];
    PCIDeviceInfo   pci;
    int             numFound=0;
    PCIslot         slot = {{0,0,0,0,0,0,1}};

    /* Works only if the kernel has exported /proc/bus/pci */
    if (access(PROC_BUS_PCI, R_OK))
        return 0;

    /* Open /proc/bus/pci/devices */
    sprintf(dev, "%s/devices", PROC_BUS_PCI);
    if ((fp = fopen(dev, "r")) == NULL)
        return 0;

    /* Parse the list of devices on the PCI bus */
    while (fgets(buff, sizeof(buff)-1, fp)) {
        unsigned int    dfn, vend, cnt;
        int             irq;
        int             base_addr[6];       /* Base addresses */
        int             rom_base_addr;      /* Expansion ROM base address */
        int             size[6];            /* Region sizes */
        int             rom_size;           /* Expansion ROM size */
        u16             bus;                /* Higher byte can select host bridges */
        u8              dev, func;          /* Device and function */

        cnt = sscanf(buff,
            "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            &dfn,
            &vend,
            &irq,
            &base_addr[0],
            &base_addr[1],
            &base_addr[2],
            &base_addr[3],
            &base_addr[4],
            &base_addr[5],
            &rom_base_addr,
            &size[0],
            &size[1],
            &size[2],
            &size[3],
            &size[4],
            &size[5],
            &rom_size);
        if (cnt != 9 && cnt != 10 && cnt != 17) {
            fclose(fp);
            return 0;
            }
        bus  = dfn >> 8U;
        dev  = PCI_SLOT(dfn & 0xff);
        func = PCI_FUNC(dfn & 0xff);
        nb++;

        /* Copy information if not just counting the number of devices */
        if (info) {
            char    buf[sizeof(PROC_BUS_PCI)+60];
            int     n;
            int     fd;

            slot.p.Bus      = bus;
            slot.p.Device   = dev;
            slot.p.Function = func;
            sprintf(buf, "%s/%02x/%02x.%d", PROC_BUS_PCI, bus, dev, func);
            fd = open(buf, O_RDONLY);
            if (fd == -1) {
                fclose(fp);
                return 0;
                }

            memset(&pci,0,sizeof(pci));
            pci.dwSize = sizeof(pci);
            pci.mech1 = 3;
            pci.slot = slot;
            n = read(fd, &pci.VendorID, sizeof(u32) * NUM_PCI_REG);
            close(fd);
            if (n != sizeof(u32) * NUM_PCI_REG) {
                fclose(fp);
                return 0;
                }
#ifdef __BIG_ENDIAN__
            /* For Big Endian machiens we need to swizzle the entire configuration
             * space for all the registers, but we need to do this on a per structure
             * element basis as we need to know if each item is a BYTE, WORD or DWORD
             * before we can properly swizzle it. Hence we have to handle each PCI
             * device type separately.
             */
            PM_convLE_16(pci.VendorID);
            PM_convLE_16(pci.DeviceID);
            PM_convLE_16(pci.Command);
            PM_convLE_16(pci.Status);
            if ((pci.HeaderType & (~PCI_multiFunctionType)) == PCI_deviceType) {
                PM_convLE_32(pci.u.type0.BaseAddress10);
                PM_convLE_32(pci.u.type0.BaseAddress14);
                PM_convLE_32(pci.u.type0.BaseAddress18);
                PM_convLE_32(pci.u.type0.BaseAddress1C);
                PM_convLE_32(pci.u.type0.BaseAddress20);
                PM_convLE_32(pci.u.type0.BaseAddress24);
                PM_convLE_32(pci.u.type0.CardbusCISPointer);
                PM_convLE_16(pci.u.type0.SubSystemVendorID);
                PM_convLE_16(pci.u.type0.SubSystemID);
                PM_convLE_32(pci.u.type0.ROMBaseAddress);
                }
            else if ((pci.HeaderType & (~PCI_multiFunctionType)) == PCI_bridgeType) {
                PM_convLE_32(pci.u.type1.BaseAddress10);
                PM_convLE_32(pci.u.type1.BaseAddress14);
                PM_convLE_16(pci.u.type1.SecondaryStatus);
                PM_convLE_16(pci.u.type1.MemoryBase);
                PM_convLE_16(pci.u.type1.MemoryLimit);
                PM_convLE_16(pci.u.type1.PrefetchableMemoryBase);
                PM_convLE_16(pci.u.type1.PrefetchableMemoryLimit);
                PM_convLE_32(pci.u.type1.PrefetchableBaseHi);
                PM_convLE_32(pci.u.type1.PrefetchableLimitHi);
                PM_convLE_16(pci.u.type1.IOBaseHi);
                PM_convLE_16(pci.u.type1.IOLimitHi);
                PM_convLE_32(pci.u.type1.ROMBaseAddress);
                PM_convLE_16(pci.u.type1.BridgeControl);
                }
            else if ((pci.HeaderType & (~PCI_multiFunctionType)) == PCI_cardBusBridgeType) {
                int i;
                PM_convLE_32(pci.u.type2.SocketRegistersBaseAddress);
                PM_convLE_16(pci.u.type2.SecondaryStatus);
                PM_convLE_16(pci.u.type2.BridgeControl);
                for (i = 0; i < 4; i++) {
                    PM_convLE_32(pci.u.type2.Range[i].Base);
                    PM_convLE_32(pci.u.type2.Range[i].Limit);
                    }
                }
            else {
                PM_fatalError("Unknown PCI device type!!");
                return 0;
                }
#endif
            COPY_STRUCTURE(&info[numFound],&pci);
            ++numFound;
            }
        }
    fclose(fp);
    return nb;
}

/****************************************************************************
DESCRIPTION:
Function to read/write values to PCI congfiguration space registers

PARAMETERS:
index   - Index of the register to access
value   - Value to write to the register for write access
func    - Function to implement (PCIAccessRegFlags)
info    - PCI device information block for device to access

RETURNS:
The value read from the register for read operations

REMARKS:
This function can be used to read or write, BYTE, WORD and DWORD values
to and from PCI configuration space registers. This version works using the
Linux /proc/bus/pci interface.
****************************************************************************/
static u32 PCI_accessRegLinux(
    int index,
    u32 value,
    int func,
    PCIDeviceInfo *info)
{
    char    buf[sizeof(PROC_BUS_PCI)+50];
    u8      v8;
    u16     v16;
    u32     v32;
    int     fd, sz=0, status=0;

    /* Open /proc/bus/pci/devices/#bus/#device.#func */
    sprintf(buf, "%s/%02x/%02x.%d",
        PROC_BUS_PCI,
        info->slot.p.Bus,
        info->slot.p.Device,
        info->slot.p.Function );
    fd = open(buf,
        (func == PCI_READ_BYTE || func == PCI_READ_WORD || func == PCI_READ_DWORD) ? O_RDONLY : O_RDWR);
    if (fd == -1 ) {
        char msg[200];
        snprintf(msg, sizeof(msg), "Unable to open %s - errno = %d", buf, errno);
        PM_fatalError(msg);
        }

    /* Read or Write the data */
    if (lseek(fd, index, SEEK_SET) != index) {
        char msg[200];
        snprintf(msg, sizeof(msg), "Unable to lseek %s - errno = %d", buf, errno);
        PM_fatalError(msg);
        }
    switch (func) {
        case PCI_READ_BYTE:
            status = read(fd, &v8, sz = sizeof(u8));
            break;
        case PCI_READ_WORD:
            status = read(fd, &v16, sz = sizeof(u16));
            PM_convLE_16(v16);
            break;
        case PCI_READ_DWORD:
            status = read(fd, &v32, sz = sizeof(u32));
            PM_convLE_32(v32);
            break;
        case PCI_WRITE_BYTE:
            v8 = value;
            status = write(fd, &v8, sz = sizeof(u8));
            break;
        case PCI_WRITE_WORD:
            v16 = PM_getLE_16(value);
            status = write(fd, &v16, sz = sizeof(u16));
            break;
        case PCI_WRITE_DWORD:
            v32 = PM_getLE_32(value);
            status = write(fd, &v32, sz = sizeof(u32));
            break;
        }
    if (status != sz) {
        char msg[200];
        snprintf(msg, sizeof(msg), "Unable to read/write %s - errno = %d", buf, errno);
        PM_fatalError(msg);
        }
    close(fd);

    /* Return the value if this was a read operation */
    switch (func) {
        case PCI_READ_BYTE:
            return v8;
        case PCI_READ_WORD:
            return v16;
        case PCI_READ_DWORD:
            return v32;
        }
    return 0;
}

/****************************************************************************
PARAMETERS:
info    - PCI device information for the specified device
intPin  - Value to store in the PCI InterruptPin register
IRQ     - New ISA IRQ to map the PCI interrupt to (0-15)

RETURNS:
Always false on non-intel platforms.
{secret}
****************************************************************************/
ibool PCIAPI PCI_setHardwareIRQ(
    PCIDeviceInfo *info,
    uint intPin,
    uint IRQ)
{
    return false;
}

/****************************************************************************
PARAMETERS:
bus                 - Bus number to generate the special cycle for
specialCycleData    - Data to send for the special cyle

REMARKS:
Does nothing; this is just a place-holder to allow pm to link.
{secret}
****************************************************************************/
void PCIAPI PCI_generateSpecialCyle(
    uint bus,
    u32 specialCycleData)
{
}

/****************************************************************************
Returns the physical address of the PCI BIOS entry point. This is useful
for external code that needs to call the PCI BIOS directly for features
not supported by the PCI library.

This just returns 0 for non-intel platforms.
{secret}
****************************************************************************/
u32 PCIAPI PCI_getBIOSEntry(void)
{
    return 0;
}

#endif /* !__INTEL__ */

/****************************************************************************
DESCRIPTION:
Enumerates all devices on the PCI bus

HEADER:
pcilib.h

PARAMETERS:
info        - Array of PCIDeviceInfo structures to fill in
maxDevices  - Maximum number of of devices to enumerate into array

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate all available devices on the PCI bus into an array
of configuration information blocks.

SEE ALSO:
PCI_getNumDevices, PCI_enumerate, PCI_accessReg, PCI_readRegBlock
PCI_writeRegBlock
****************************************************************************/
int PCIAPI PCI_enumerate(
    PCIDeviceInfo info[])
{
    int numFound;

#ifdef __INTEL__
    /* First try via the direct access mechanisms which are faster if we
     * have them (nearly always). The BIOS is used as a fallback, and for
     * stuff we can't do directly.
     */
    if ((numFound = PCI_enumerateMech1(info)) == 0) {
        if ((numFound = PCI_enumerateMech2(info)) == 0) {
            if ((numFound = PCI_enumerateBIOS(info)) == 0)
                return 0;
            }
        }
#else
    if ((numFound = PCI_enumerateLinux(info)) == 0)
        return 0;
#endif
    return numFound;
}

/****************************************************************************
DESCRIPTION:
Returns number of devices on the PCI bus

HEADER:
pcilib.h

RETURNS:
Number of PCI devices found and enumerated on the PCI bus, 0 if not PCI.

REMARKS:
Function to enumerate the number of available devices on the PCI bus and
return the number found.

SEE ALSO:
PCI_getNumDevices, PCI_enumerate, PCI_accessReg, PCI_readRegBlock
PCI_writeRegBlock
****************************************************************************/
int PCIAPI PCI_getNumDevices(void)
{
    return PCI_enumerate(NULL);
}

/****************************************************************************
DESCRIPTION:
Function to read/write values to PCI congfiguration space registers

HEADER:
pcilib.h

PARAMETERS:
index   - Index of the register to access
value   - Value to write to the register for write access
func    - Function to implement (PCIAccessRegFlags)
info    - PCI device information block for device to access

RETURNS:
The value read from the register for read operations

REMARKS:
This function can be used to read or write, BYTE, WORD and DWORD values
to and from PCI configuration space registers. Please refer to the
PCIAccessRegFlags type for the different operations supported. The PCI
device that is accessed is the one described by the PCIDeviceInfo
structure passed in the info parameter (or more correctly the PCIslot
value stored within this structure).

SEE ALSO:
PCI_getNumDevices, PCI_enumerate, PCI_accessReg, PCI_readRegBlock
PCI_writeRegBlock
****************************************************************************/
u32 PCIAPI PCI_accessReg(
    int index,
    u32 value,
    int func,
    PCIDeviceInfo *info)
{
#ifdef __INTEL__
    int iobase;

    if (info->mech1 == 2) {
        /* Use PCI BIOS access since we dont have direct hardware access */
        switch (func) {
            case PCI_READ_BYTE:
                return (u8)_PCIBIOS_service(READ_CONFIG_BYTE,info->slot.i >> 8,index,0);
            case PCI_READ_WORD:
                return (u16)_PCIBIOS_service(READ_CONFIG_WORD,info->slot.i >> 8,index,0);
            case PCI_READ_DWORD:
                return (u32)_PCIBIOS_service(READ_CONFIG_DWORD,info->slot.i >> 8,index,0);
            case PCI_WRITE_BYTE:
                _PCIBIOS_service(WRITE_CONFIG_BYTE,info->slot.i >> 8,index,value);
                break;
            case PCI_WRITE_WORD:
                _PCIBIOS_service(WRITE_CONFIG_WORD,info->slot.i >> 8,index,value);
                break;
            case PCI_WRITE_DWORD:
                _PCIBIOS_service(WRITE_CONFIG_DWORD,info->slot.i >> 8,index,value);
                break;
            }
        }
    else {
        /* Use direct hardware access mechanisms */
        if (info->mech1) {
            /* PCI access mechanism 1 */
            iobase = 0xCFC + (index & 3);
            info->slot.p.Register = index >> 2;
            PM_outpd(0xCF8,info->slot.i);
            }
        else {
            /* PCI access mechanism 2 */
            PM_outpb(0xCF8,(uchar)((info->slot.p.Function << 1) | 0x10));
            PM_outpb(0xCFA,(uchar)info->slot.p.Bus);
            iobase = 0xC000 + (info->slot.p.Device << 8) + index;
            }
        switch (func) {
            case PCI_READ_BYTE:
            case PCI_READ_WORD:
            case PCI_READ_DWORD:    value = PM_inpd(iobase);        break;
            case PCI_WRITE_BYTE:    PM_outpb(iobase,(u8)value);     break;
            case PCI_WRITE_WORD:    PM_outpw(iobase,(u16)value);    break;
            case PCI_WRITE_DWORD:   PM_outpd(iobase,(u32)value);    break;
            }
        PM_outpd(0xCF8,0);
        }
    return value;
#else /* !__INTEL__ */
    return PCI_accessRegLinux(index, value, func, info);
#endif
}

/****************************************************************************
DESCRIPTION:
Function to read a block of PCI congfiguration space registers

HEADER:
pcilib.h

PARAMETERS:
info    - PCI device information block for device to access
index   - Index of register to start reading from
dst     - Place to store the values read from configuration space
count   - Count of bytes to read from configuration space

REMARKS:
This function is used to read a block of PCI configuration space registers
from the configuration space into the passed in data block. This function
will properly handle reading non-DWORD aligned data from the configuration
space correctly.

SEE ALSO:
PCI_getNumDevices, PCI_enumerate, PCI_accessReg, PCI_readRegBlock
PCI_writeRegBlock
****************************************************************************/
void PCIAPI PCI_readRegBlock(
    PCIDeviceInfo *info,
    int index,
    void *dst,
    int count)
{
    uchar   *pb;
    u32     *pd;
    int     i;
    int     startCount = (index & 3);
    int     middleCount = (count - startCount) >> 2;
    int     endCount = count - middleCount * 4 - startCount;

    for (i = 0,pb = dst; i < startCount; i++, index++) {
        *pb++ = (uchar)PCI_accessReg(index,0,PCI_READ_BYTE,info);
        }
    for (i = 0,pd = (u32*)pb; i < middleCount; i++, index += 4) {
        *pd++ = (u32)PCI_accessReg(index,0,PCI_READ_DWORD,info);
        }
    for (i = 0,pb = (uchar*)pd; i < endCount; i++, index++) {
        *pb++ = (uchar)PCI_accessReg(index,0,PCI_READ_BYTE,info);
        }
}

/****************************************************************************
DESCRIPTION:
Function to read a block of PCI congfiguration space registers

HEADER:
pcilib.h

PARAMETERS:
info    - PCI device information block for device to access
index   - Index of register to start reading from
src     - Place to store the values read from configuration space
count   - Count of bytes to read from configuration space

REMARKS:
This function is used to write a block of PCI configuration space registers
to the configuration space from the passed in data block. This function
will properly handle writing non-DWORD aligned data to the configuration
space correctly.

SEE ALSO:
PCI_getNumDevices, PCI_enumerate, PCI_accessReg, PCI_readRegBlock
PCI_writeRegBlock
****************************************************************************/
void PCIAPI PCI_writeRegBlock(
    PCIDeviceInfo *info,
    int index,
    void *src,
    int count)
{
    uchar   *pb;
    u32     *pd;
    int     i;
    int     startCount = (index & 3);
    int     middleCount = (count - startCount) >> 2;
    int     endCount = count - middleCount * 4 - startCount;

    for (i = 0,pb = src; i < startCount; i++, index++) {
        PCI_accessReg(index,*pb++,PCI_WRITE_BYTE,info);
        }
    for (i = 0,pd = (u32*)pb; i < middleCount; i++, index += 4) {
        PCI_accessReg(index,*pd++,PCI_WRITE_DWORD,info);
        }
    for (i = 0,pb = (uchar*)pd; i < endCount; i++, index++) {
        PCI_accessReg(index,*pb++,PCI_WRITE_BYTE,info);
        }
}

