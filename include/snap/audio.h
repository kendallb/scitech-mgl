/****************************************************************************
*
*                     SciTech SNAP Audio Architecture
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
* Description:  Header file for the SciTech SNAP Audio Architecture
*               device driver Hardware Abstraction Layer.
*
****************************************************************************/

#ifndef __SNAP_AUDIO_H
#define __SNAP_AUDIO_H

#include "snap/common.h"
#include "clib/modloadr.h"
#include "ztimerc.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Signature for the audio driver file */

#define AA_SIGNATURE        "AUDIO"

/* Define the interface version */

#define AA_VERSION          0x100

/* Define the maximum number of devices supported. Internally we can handle
 * any number, so if you need more than this many devices let us know and
 * we can build custom drivers that support more devices.
 */

#define AA_MAX_DEVICES          4

/****************************************************************************
REMARKS:
This enumeration defines the identifiers used to obtain the device context
function group pointer structures. As new features and capabilities are
added to the future versions of the specification, new identifiers will
be added to extract new function pointers from the drivers.

The AA_GET_FIRST_OEM defines the first identifier for OEM extensions. OEM's
are free to added their own private functional extensions to the drivers
as desired. Note that OEM's must verify the presence of their OEM drivers
via the the OemVendorName string before attempting to use OEM extension
functions.

HEADER:
snap/audio.h

MEMBERS:
AA_GET_INITFUNCS        - Get AA_initFuncs structure
AA_GET_PLAYBACKFUNCS    - Get AA_playbackFuncs structure
AA_GET_RECORDFUNCS      - Get AA_recordFuncs structure
AA_GET_MIDIFUNCS        - Get AA_midiFuncs structure
AA_GET_VOLUMEFUNCS      - Get AA_volumeFuncs structure
AA_GET_MIXERFUNCS       - Get AA_mixerFuncs structure
AA_GET_FIRST_OEM        - ID of first OEM extension function
****************************************************************************/
typedef enum {
    AA_GET_INITFUNCS,
    AA_GET_PLAYBACKFUNCS,
    AA_GET_RECORDFUNCS,
    AA_GET_MIDIFUNCS,
    AA_GET_VOLUMEFUNCS,
    AA_GET_MIXERFUNCS,
    AA_GET_FIRST_OEM                = 0x00010000
    } AA_funcGroupsType;

/****************************************************************************
REMARKS:
Flags for the Attributes member of the main AA_devCtx device context block
structure. These flags define the hardware capabilities of the particular
device.

The aaHaveDigitalAsync flag is used to determine if the device supports
hardware digital audio playback and recording at the same time. This is
sometime referred to as full duplex recording.

The aaHaveMultipleRecord flag is used to determine if the device supports
recording from multiple signal sources at the same time. If this flag is
not set, you can only enable one input signal at a time via the SelectInput
function. Otherwise you can enable multiple inputs and they will be
mixed together based on the settings of their respective volume controls.

The aaHaveWaveTable flag is used to determine if the device supports
hardware wave table output for music synthesis.

The aaHaveMIDIOut flag is used to determine if the device supports MIDI
output functionality.

The aaHaveMIDIIn flag is used to determine if the device supports MIDI
input functionality.

The aaHaveMIDIThru flag is used to determine if the device supports MIDI
through functionality.

The aaHaveMIDIAsync flag is used to determine if the device supports MIDI
input and output functionality at the same time.

The aaHaveHardwareVolume flag is used to determine if the device supports
hardware volume control (or input/output mixing).

The aaHaveHardwareMixing flag is used to determine if the device supports
hardware mixing of multiple audio streams together.

HEADER:
snap/audio.h

MEMBERS:
aaHaveDigitalAsync      - Digital audio playback and record at same time
aaHaveMultipleRecord    - Recording from multiple inputs is supported
aaHaveWaveTable         - Wave table digital audio is supported
aaHaveMIDIOut           - MIDI output is supported
aaHaveMIDIIn            - MIDI input in supported
aaHaveMIDIThru          - MIDI through is supported
aaHaveMIDIAsync         - MIDI input and output at the same time
aaHaveHardwareVolume    - Hardware volume control is supported
aaHaveHardwareMixing    - Hardware mixing is supported
****************************************************************************/
typedef enum {
    aaHaveDigitalAsync              = 0x00000001,
    aaHaveMultipleRecord            = 0x00000002,
    aaHaveWaveTable                 = 0x00000004,
    aaHaveMIDIOut                   = 0x00000008,
    aaHaveMIDIIn                    = 0x00000010,
    aaHaveMIDIThru                  = 0x00000020,
    aaHaveMIDIAsync                 = 0x00000040,
    aaHaveHardwareVolume            = 0x00000080,
    aaHaveHardwareMixing            = 0x00000100
    } AA_AttributeFlagsType;

/****************************************************************************
REMARKS:
Flags for the LockedMemFlags member of the main AA_devCtx device context
block structure. These flags define the requirements for the locked
physical memory needed by the driver for DMA operations.

HEADER:
snap/audio.h

MEMBERS:
aaDMABelow16M   - Locked DMA memory buffer must be below 16Mb memory mark
aaDMAContiguous - Locked DMA memory buffer must be physically contiguous
****************************************************************************/
typedef enum {
    aaDMABelow16M                   = 0x00000001,
    aaDMAContiguous                 = 0x00000002
    } AA_LockedMemFlagsType;

/****************************************************************************
REMARKS:
Flags for the Flags member of the AA_formatInfo structure. These flags
define the capabilities of the different hardware audio formats supported
directly by the hardware device. Individual supported formats are
enumerated in the FormatInfo member of the main AA_devCtx structure.

HEADER:
snap/audio.h

MEMBERS:
aaDigitalPlayback   - Digital audio playback is supported
aaDigitalRecord     - Digital audio recording is supported
aaDigitalMono       - Mono digital audio capability
aaDigitalStereo     - Stereo digital audio capability
aaDigitalAsync      - Digital audio playback and record at same time
aaDigitalSigned     - Digital audio data is signed
aaDigital8Bit       - 8-bit PCM digital audio capability
aaDigital16Bit      - 16-bit PCM digital audio capability
aaDigital20Bit      - 20-bit PCM digital audio capability
aaDigital24Bit      - 24-bit PCM digital audio capability
aaDigital32Bit      - 32-bit PCM digital audio capability
aaDigitalFloat32    - 32-bit floating point digital audio capability
aaDigitalAC3        - Dobly Digital AC3 format capability
aaDigitalBigEndian  - Format is big-endian
****************************************************************************/
typedef enum {
    aaDigitalPlayback               = 0x00000001,
    aaDigitalRecord                 = 0x00000002,
    aaDigitalMono                   = 0x00000004,
    aaDigitalStereo                 = 0x00000008,
    aaDigitalAsync                  = 0x00000010,
    aaDigitalSigned                 = 0x00000020,
    aaDigital8Bit                   = 0x00000040,
    aaDigital16Bit                  = 0x00000080,
    aaDigital20Bit                  = 0x00000100,
    aaDigital24Bit                  = 0x00000200,
    aaDigital32Bit                  = 0x00000400,
    aaDigitalFloat32                = 0x00000800,
    aaDigitalAC3                    = 0x00001000,
    aaDigitalBigEndian              = 0x40000000
    } AA_FormatFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the SelectOutput function, which is used to select what
outputs are mixed into the final output signal. Even though the hardware
may not specifically support turning off a signal, the driver will
emulate that where necessary by setting the volume to 0. If the hardware
supports disconnecting a source, that will be used to provide the best
signal quality.

NOTE:   You can determine what outputs are supported by examining the list
        of volume controls supported by the QueryAvailableVolumeControls
        function. Trying to control an output not supported by the hardware
        will have no effect.

NOTE:   The only exception to the above rule is the aaOutputDigital flag
        which is always available regardless of the corresponding
        aaVolumeDigital capability. It can be safely presumed that any
        SNAP Audio driver will at least have digital output capability,
        even though some recent high-definition audio chipsets do not
        support a volume control for the digital output line.

HEADER:
snap/audio.h

MEMBERS:
aaOutputDigital     - Digital audio output is enabled
aaOutputMIDI        - MIDI output is enabled
aaOutputCD          - CD audio output is enabled
aaOutputLineIn      - Line in is enabled
aaOutputMicrophone  - Microphone is enabled
aaOutputPCSpeaker   - PC speaker is enabled
aaOutputAux1        - Aux 1 input is enabled
aaOutputAux2        - Aux 2 input is enabled
****************************************************************************/
typedef enum {
    aaOutputDigital                 = 0x00000001,
    aaOutputMIDI                    = 0x00000002,
    aaOutputCD                      = 0x00000004,
    aaOutputLineIn                  = 0x00000008,
    aaOutputMicrophone              = 0x00000010,
    aaOutputPCSpeaker               = 0x00000020,
    aaOutputAux1                    = 0x00000040,
    aaOutputAux2                    = 0x00000080
    } AA_OutputFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the SelectInput function, which is used to select what
inputs are mixed together for digital audio recording. Some combinations
may not be supported by the hardware, so make sure you check the return
status of SelectInput for failure conditions.

NOTE:   You can determine what inputs are supported by examining the list
        of volume controls supported by the QueryAvailableVolumeControls
        function. Trying to control an input not supported by the hardware
        will cause the SelectInput function to fail.

HEADER:
snap/audio.h

MEMBERS:
aaInputMIDI         - MIDI is enabled for recording
aaInputCD           - CD audio is enabled for recording
aaInputLineIn       - Line in is enabled for recording
aaInputMicrophone   - Microphone is enabled for recording
aaInputAux1         - Aux 1 is enabled for recording
aaInputAux2         - Aux 2 is enabled for recording
****************************************************************************/
typedef enum {
    aaInputMIDI                     = 0x00000001,
    aaInputCD                       = 0x00000002,
    aaInputLineIn                   = 0x00000004,
    aaInputMicrophone               = 0x00000008,
    aaInputAux1                     = 0x00000010,
    aaInputAux2                     = 0x00000020
    } AA_InputFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the SetVolume and GetVolume functions, which
are used to set the volume for independant input and output streams. These
flags are also returned by the QueryAvailableVolumeControls function, which
returns the list of all volume controls supported by the hardware. From the
list of supported volume controls, you can also determine what types of
input and output streams are supported by the hardware.

If the installed hardware does not support a certain volume control, the
flag will be cleared in the flags returned by the QueryAvailableVolumeControls
function.

HEADER:
snap/audio.h

MEMBERS:
aaVolumeMaster          - Master volume control
aaVolumeDigital         - Digital audio (wave) volume control
aaVolumeMIDI            - MIDI output volume control
aaVolumeCD              - CD player volume control
aaVolumeLineIn          - Line input volume control
aaVolumeMicrophone      - Microphone volume control
aaVolumePCSpeaker       - PC speaker connection volume control
aaVolumeAux1            - Auxiliary 1 volume control
aaVolumeAux2            - Auxiliary 2 volume control
aaVolumeBass            - Master Bass volume control
aaVolumeTreble          - Master Treble volume control
aaVolumeInputGain       - Input gain control
aaVolumeOutputGain      - Output gain control
aaVolumeBuiltinSpeaker  - Internal speaker volume control
****************************************************************************/
typedef enum {
    aaVolumeMaster                 = 0x00000001,
    aaVolumeDigital                = 0x00000002,
    aaVolumeMIDI                   = 0x00000004,
    aaVolumeCD                     = 0x00000008,
    aaVolumeLineIn                 = 0x00000010,
    aaVolumeMicrophone             = 0x00000020,
    aaVolumePCSpeaker              = 0x00000040,
    aaVolumeAux1                   = 0x00000080,
    aaVolumeAux2                   = 0x00000100,
    aaVolumeBass                   = 0x00000200,
    aaVolumeTreble                 = 0x00000400,
    aaVolumeInputGain              = 0x00000800,
    aaVolumeOutputGain             = 0x00001000,
    aaVolumeBuiltinSpeaker         = 0x00002000
    } AA_VolumeFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the SetVolume and GetVolume functions, which
are used to control which channel should be adjusted for independant input
and output streams. These flags are also returned by the
QueryAvailableChannels function, which returns the list of all channels
supported for a particule volume control. Some volume controls may support
multiplel channels, such as the master volume control, while others may only
support one (ie: PC Speaker) or two (CD etc).

If the installed hardware does not support a certain volume control channel,
the flag will be cleared in the flags returned by the QueryAvailableChannels
function.

HEADER:
snap/audio.h

MEMBERS:
aaChannelMaster         - Master channel control (controls all at the same time)
aaChannelLeft           - Left front speaker
aaChannelRight          - Right front speaker
aaChannelCenter         - Center front speaker
aaChannelLeftRear       - Rear left speaker
aaChannelRightRear      - Rear right speaker
aaChannelCenterRear     - Rear center speaker
aaChannelSideLeft       - Left side speaker
aaChannelSideRight      - Right side speaker
aaChannelSubWoofer      - Subwoofer speaker
aaChannelMono           - Channel is mono
aaChannelStereo         - Combination of flags for stereo output
aaChannelFivePointOne   - Combination of flags for 5.1 audio
aaChannelSixPointOne    - Combination of flags for 6.1 audio
aaChannelSevenPointOne  - Combination of flags for 7.1 audio
****************************************************************************/
typedef enum {
    aaChannelMaster                 = 0x7FFFFFFF,
    aaChannelLeft                   = 0x00000001,
    aaChannelRight                  = 0x00000002,
    aaChannelCenter                 = 0x00000004,
    aaChannelLeftRear               = 0x00000008,
    aaChannelRightRear              = 0x00000010,
    aaChannelCenterRear             = 0x00000020,
    aaChannelSideLeft               = 0x00000040,
    aaChannelSideRight              = 0x00000080,
    aaChannelSubWoofer              = 0x00000100,
    aaChannelMono                   = aaChannelLeft,
    aaChannelStereo                 = aaChannelLeft | aaChannelRight,
    aaChannelFivePointOne           = aaChannelLeft | aaChannelRight | aaChannelCenter |
                                      aaChannelLeftRear | aaChannelRightRear | aaChannelSubWoofer,
    aaChannelSixPointOne            = aaChannelLeft | aaChannelRight | aaChannelCenter |
                                      aaChannelLeftRear | aaChannelRightRear | aaChannelCenterRear |
                                      aaChannelSubWoofer,
    aaChannelSevenPointOne          = aaChannelLeft | aaChannelRight | aaChannelCenter |
                                      aaChannelSideLeft | aaChannelSideRight |
                                      aaChannelLeftRear | aaChannelRightRear | aaChannelSubWoofer
    } AA_ChannelFlagsType;

/****************************************************************************
REMARKS:
Function prototype for interrupt callbacks for audio playback and recording.
This callback is called when the DMA buffer needs to be filled or has been
filled.

PARAMETERS:
buffer  - pointer to the DMA buffer that needs to be filled or emptied
size    - length of the buffer in bytes
context - Context pointer passed to interrupt handler

NOTE:   The DMA buffer callback /will/ be called at interrupt time from
        within the driver, so it must be locked in memory and be addressable
        from an interrupt context. Any memory or variables it accesses
        must also be locked in memory and be addressable from an interrupt
        context.

HEADER:
snap/audio.h
****************************************************************************/
typedef N_uint32 (NAPIP AA_DMACallback)(void *buffer, N_uint32 size, void *context);

/****************************************************************************
REMARKS:
Structure defining the different digital audio formats supported by the
hardware. The flags field contains the information about the type of
format being enumerated, and will be a combination of the flags defined
in the AA_FormatFlagsType enumeration.

HEADER:
snap/audio.h

MEMBERS:
MinPhysRate - Minimum physical sample rate supported by this format
NomPhysRate - Nominal physical sample rate supported by this format
MaxPhysRate - Maximum physical sample rate supported by this format
Flags       - Flags for this format
****************************************************************************/
typedef struct {
    N_uint32    MinPhysRate;
    N_uint32    NomPhysRate;
    N_uint32    MaxPhysRate;
    N_uint32    Flags;
    } AA_formatInfo;

/****************************************************************************
REMARKS:
Structure returned by GetConfigInfo, which contains configuration
information about the installed audio hardware.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/audio.h

MEMBERS:
dwSize              - Set to size of structure in bytes
ManufacturerName    - Name of audio chipset manufacturer
ChipsetName         - Name of audio chipset name
VersionInfo         - String representation of version and build for driver
BuildDate           - String representation of the build date for driver
Certified           - True if the driver is certified
CertifiedDate       - Date that the driver was certified
CertifiedVersion    - Version info for certification program used
****************************************************************************/
typedef struct {
    N_uint32    dwSize;
    char        ManufacturerName[80];
    char        ChipsetName[80];
    char        CodecName[80];
    char        VersionInfo[80];
    char        BuildDate[80];
    char        Certified;
    char        CertifiedDate[80];
    char        CertifiedVersion[80];
    } AA_configInfo;

/****************************************************************************
REMARKS:
Parameter block to describe the audio driver IO configuration parameters.

The IOBase member contains the I/O base addresses for up to 4 individual
I/O addresses used by the driver for accessing the hardware I/O registers.

The IRQHook member contains the IRQ number of the device to be used for
handling hardware interrupts from the device.

The DMA8Bit member contains the 8-bit DMA channel used by the driver.
This is for legacy ISA bus based devices that use the old PC/AT DMA
controller. Newer PCI devices will leave these fields as zeros.

The DMA16Bit member contains the 16-bit DMA channel used by the driver.
This is for legacy ISA bus based devices that use the old PC/AT DMA
controller. Newer PCI devices will leave these fields as zeros.

HEADER:
snap/audio.h

MEMBERS:
IOBase              - Base addresses for I/O mapped registers
IRQHook             - Hardware interrupt to hook for driver
DMA8Bit             - 8-bit DMA channel to use
DMA16Bit            - 16-bit DMA channel to use
****************************************************************************/
typedef struct {
    N_uint32        IOBase[4];
    N_uint32        IRQHook;
    N_uint32        DMA8Bit;
    N_uint32        DMA16Bit;
    } AA_IOParams;

/****************************************************************************
REMARKS:
Structure returned by GetOptions, which contains configuration
information about the options for the installed device driver. All the
boolean configuration options are enabled by default and can be optionally
turned off by the user via the configuration functions.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examiming this value.

HEADER:
snap/audio.h

MEMBERS:
dwSize              - Set to size of structure in bytes
bDigitalAudio       - True if digital audio is enabled
bDigital8Bit        - True if 8-bit digital output is enabled
bDigital16Bit       - True if 16-bit digital output is enabled
bDigitalStereo      - True if stereo digital output is enabled
bDigitalPlayback    - True if digital playback is enabled
bDigitalRecord      - True if digital recording is enabled
bWaveTable          - True if wave table audio is enabled
bMIDIAudio          - True if MIDI audio is enabled
bMIDIOut            - True if MIDI Out is enabled
bMIDIIn             - True if MIDI In is enabled
bMIDIThru           - True if MIDI Thru is enabled
bHardwareVolume     - True if hardware volume control is enabled
bHardwareMixing     - True if hardware mixing is enabled
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint8         bDigitalAudio;
    N_uint8         bDigital8Bit;
    N_uint8         bDigital16Bit;
    N_uint8         bDigitalStereo;
    N_uint8         bDigitalPlayback;
    N_uint8         bDigitalRecord;
    N_uint8         bWaveTable;
    N_uint8         bMIDIAudio;
    N_uint8         bMIDIOut;
    N_uint8         bMIDIIn;
    N_uint8         bMIDIThru;
    N_uint8         bHardwareVolume;
    N_uint8         bHardwareMixing;
    } AA_options;

#define AA_FIRST_OPTION     bDigitalAudio
#define AA_LAST_OPTION      bHardwareMixing

/****************************************************************************
REMARKS:
Main audio device context structure. This structure consists of a header
block that contains configuration information about the audio device,
as well as detection information and runtime state information.

The Signature member is filled with the null terminated string 'AUDIO\0'
by the driver implementation. This can be used to verify that the file loaded
really is an audio device driver.

The Version member is a BCD value which specifies what revision level of the
audio specification is implemented in the driver. The high byte specifies
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

The Attributes member contains a number of flags that describes certain
important characteristics of the audio controller.

The MMIOBase member contains the 32-bit physical base addresses pointing
to the start of up to 4 separate memory mapped register areas required by the
controller. The MMIOLen member contains the lengths of each of these
memory mapped IO areas in bytes. When the application maps the memory mapped
IO regions for the driver, the linear address of the mapped memory areas will
then be stored in the corresponding entries in the IOMemMaps array, and will
be used by the driver for accessing the memory mapped registers on the
controller. If any of these regions are not required, the MMIOBase
entries will be NULL and do not need to be mapped by the application.

The LockedMemSize contains the amount of locked, contiguous memory in bytes
that the audio driver requires for programming the hardware. If the audio
devices requires DMA transfers, this member can be set to the length of
the block of memory that is required by the driver. The driver loader code
will attempt to allocate a block of locked, physically contiguous memory
from the operating system and place a pointer to this allocated memory in
the LockedMem member for the driver, and the physical address of the start
of this memory block in LockedMemPhys. Note that the memory must be locked
so it cannot be paged out do disk, and it must be physically contiguous so
that DMA operations will work correctly across CPU page boundaries. If
the driver does not require DMA memory, this value should be set to 0.

The LockedMemFlags contains flags that define any special requirements
for the locked physical memory block, such as whether it needs to be
allocated below the 16Mb physical memory limit.

The IO member contains a structure describing the current I/O configuration
used by the driver.

The CommonIOConfig member is a pointer to list of common IO configurations
encountered for this device. This list is used by the device auto detection
code to attempt to determine the IO parameters automatically for legacy
ISA based audio devices. This field will be NULL for PCI audio devices.

The NumCommonIOConfig member is the number of common I/O configurations,
which indicates how many common IO configurations are listed in the
CommonIOConfig member. This field will be 0 for PCI audio devices.

The ServiceRate member is the rate (in Hz) required for the timer service
callback for the driver. Most drivers will not need a periodic service
callback, but this is provided for devices that are not interrupt driven
(such as PCMCIA or parallel port devices).

The DriverStart member is a pointer to the start of the driver in memory,
and is used to lock down the driver for interrupt handling so that all the
memory in the driver can be accessed at interrupt time.

The DriverSize member is the size of the entire driver in memory in bytes,
and is used to lock down the driver in memory for interrupt handling.

The IOMemMaps member contains the mapped linear address of the memory mapped
register regions defined by the MMIOBase and MMIOLen members.

The LockedMem member contains a pointer to the locked DMA memory buffer
allocated for the loaded driver. The audio driver can use this pointer to
write data directly to the DMA buffer before transferring it to the hardware.
If the driver does not require DMA memory, this value will be set to NULL by
the loader.

The LockedMemPhys member contains the 32-bit physical memory address of the
locked DMA buffer memory allocated for the driver. The audio driver can use
this physical address to set up DMA transfer operations for memory contained
within the DMA transfer buffer. If the driver does not require DMA memory,
this value will be set to 0 by the loader.

HEADER:
snap/audio.h

MEMBERS:
Signature           - 'audio\0' 20 byte signature
Version             - Driver Interface Version (1.0)
DriverRev           - Driver revision number
OemVendorName       - Vendor Name string
OemCopyright        - Vendor Copyright string
DeviceIndex         - Device index for the driver when loaded from disk
Attributes          - Driver attributes
MMIOBase            - Base addresses of memory mapped I/O regions
MMIOLen             - Length of memory mapped I/O regions
LockedMemSize       - Amount of locked memory for driver
LockedMemFlags      - Flags for locked physical memory allocation
IO                  - Structure describing the I/O configuration for driver
CommonIOConfig      - Pointer to list of common IO configurations
NumCommonIOConfig   - Number of common I/O configurations
MinHalfSize         - Minimum half buffer size for DMA transfers
MaxHalfSize         - Maximum half buffer size for DMA transfers
ServiceRate         - Service rate requires for timer service callback
DriverStart         - Pointer to the start of the driver in memory
DriverSize          - Size of the entire driver in memory in bytes
IOMemMaps           - Pointers to mapped I/O memory
LockedMem           - Ptr to allocated locked memory
LockedMemPhys       - Physical addr of locked memory
DMABuffer           - Pointer to the allocate primary DMA buffer
DMABufferPhys       - Physical address of primary DMA buffer
DMABufferSize       - Size of primary DMA buffer
HalfBufferAlignment - Alignment of DMA half-buffer size
****************************************************************************/
typedef struct {
    /*------------------------------------------------------------------*/
    /* Device driver header block                                       */
    /*------------------------------------------------------------------*/
    char            Signature[20];
    N_uint32        Version;
    N_uint32        DriverRev;
    AA_formatInfo  *FormatInfo;
    char            OemVendorName[80];
    char            OemCopyright[80];
    N_int32         DeviceIndex;
    N_uint32        Attributes;
    N_uint32        MMIOBase[4];
    N_uint32        MMIOLen[4];
    N_uint32        LockedMemSize;
    N_uint32        LockedMemFlags;
    AA_IOParams     IO;
    AA_IOParams     *CommonIOConfig;
    N_int32         NumCommonIOConfig;
    N_int32         MinHalfSize;
    N_int32         MaxHalfSize;
    N_int32         ServiceRate;
    void            *DriverStart;
    N_uint32        DriverSize;
    N_uint32        res1[20];

    /*------------------------------------------------------------------*/
    /* Near pointers mapped by loader for driver                        */
    /*------------------------------------------------------------------*/
    void            _FAR_ *IOMemMaps[4];
    void            _FAR_ *LockedMem;
    void            _FAR_ *DMABuffer;
    N_physAddr      LockedMemPhys;
    N_physAddr      DMABufferPhys;
    N_uint32        DMABufferSize;
    N_uint32        HalfBufferAlignment;
    N_uint32        res2[19];

    /*------------------------------------------------------------------*/
    /* Driver initialization functions                                  */
    /*------------------------------------------------------------------*/
    void            (NAPIP InitDriver)(void);
    N_int32         (NAPIP VerifyIOConfig)(AA_IOParams *IOParams);
    N_int32         (NAPIP OpenDevice)(void);
    void            (NAPIP CloseDevice)(void);
    void            (NAPIP ServiceHandler)(void);
    ibool           (NAPIP QueryFunctions)(N_uint32 id,N_int32 safetyLevel,void _FAR_ *funcs);
    } AA_devCtx;

/****************************************************************************
REMARKS:
Main device driver init functions, including setup and initialisation
functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    void        (NAPIP GetConfigInfo)(AA_configInfo *info);
    void        (NAPIP GetOptions)(AA_options *options);
    void        (NAPIP SetOptions)(AA_options *options);
    void        (NAPIP GetUniqueFilename)(char *filename,int type);
    } AA_initFuncs;

/****************************************************************************
REMARKS:
Device driver hardware digital audio playback functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    N_int32     (NAPIP SetPlaybackMode)(N_int32 *sampleRate,N_int32 flags);
    N_int32     (NAPIP StartPlayback)(N_int32 halfBufferSize,AA_DMACallback bufferEmpty,N_int32 restart,void *context);
    N_uint32    (NAPIP GetPlaybackPosition)(void);
    void        (NAPIP StopPlayback)(void);
    } AA_playbackFuncs;

/****************************************************************************
REMARKS:
Device driver hardware digital audio record functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    N_int32     (NAPIP SetRecordMode)(N_int32 *sampleRate,N_int32 flags);
    N_int32     (NAPIP StartRecord)(N_int32 halfBufferSize,AA_DMACallback bufferFull,N_int32 restart,void *context);
    N_uint32    (NAPIP GetRecordPosition)(void);
    void        (NAPIP StopRecord)(void);
    } AA_recordFuncs;

/****************************************************************************
REMARKS:
Device driver hardware MIDI functions go in here.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    } AA_MIDIFuncs;

/****************************************************************************
REMARKS:
Device driver hardware mixer functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr   dwSize;
    N_int32     (NAPIP QueryAvailableVolumeControls)(void);
    N_int32     (NAPIP QueryAvailableChannels)(N_int32 volumeID);
    void        (NAPIP SelectOutput)(N_int32 mask);
    N_int32     (NAPIP GetOutputSelect)(void);
    N_int32     (NAPIP SelectInput)(N_int32 mask);
    N_int32     (NAPIP GetInputSelect)(void);
    void        (NAPIP SetVolume)(N_int32 volumeID,N_int32 channel,N_uint8 volume);
    N_uint8     (NAPIP GetVolume)(N_int32 volumeID,N_int32 channel);
    void        (NAPIP SetMicrophoneAGC)(N_int32 enable);
    N_int32     (NAPIP GetMicrophoneAGC)(void);
    void        (NAPIP SetBuiltinSpeaker)(N_int32 enable);
    N_int32     (NAPIP GetBuiltinSpeaker)(void);
    } AA_volumeFuncs;

/****************************************************************************
REMARKS:
Device driver hardware mixer functions.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    } AA_mixerFuncs;

/****************************************************************************
REMARKS:
Structure defining all the SciTech SNAP Audio API functions as exported from
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    int             (NAPIP AA_status)(void);
    const char *    (NAPIP AA_errorMsg)(N_int32 status);
    int             (NAPIP AA_getDaysLeft)(void);
    int             (NAPIP AA_registerLicense)(uchar *license);
    int             (NAPIP AA_enumerateDevices)(void);
    AA_devCtx *     (NAPIP AA_loadDriver)(N_int32 deviceIndex);
    void            (NAPIP AA_unloadDriver)(AA_devCtx *dc);
    void            (NAPIP AA_saveOptions)(AA_devCtx *dc,AA_options *options);
    ibool           (NAPIP AA_queryFunctions)(AA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);
    } AA_exports;

/****************************************************************************
REMARKS:
Structure defining all the SciTech SNAP Audio API functions as imported into
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
    } AA_imports;

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef AA_exports * (NAPIP AA_initLibrary_t)(const char *path,const char *bpdname,PM_imports *pmImp,N_imports *nImp,AA_imports *gaImp);

#pragma pack()

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Pointer to global exports structure. Should not be used by application programs. */
/* {secret} */
extern AA_exports   _VARAPI __AA_exports;

/*-------------------------- Function Prototypes --------------------------*/

/* Error handling functions for SciTech SNAP Audio drivers */

int             NAPI AA_status(void);
const char *    NAPI AA_errorMsg(N_int32 status);

/* Function to get the number of days left in evaluation period */

int             NAPI AA_getDaysLeft(void);

/* Utility function to register a linkable library license */

int             NAPI AA_registerLicense(uchar *license);

/* Utility functions to load an audio driver and initialise it */

int             NAPI AA_enumerateDevices(void);
AA_devCtx *     NAPI AA_loadDriver(N_int32 deviceIndex);
void            NAPI AA_unloadDriver(AA_devCtx *dc);
void            NAPI AA_saveOptions(AA_devCtx *dc,AA_options *options);
ibool           NAPI AA_queryFunctions(AA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);

/* Utility functions to force the I/O configuration for a device. If you
 * force the I/O parameters for a secondary ISA sound device,
 * AA_enumerateDevices will enumerate those devices automatically. Using
 * these functions is the only way to get multiple ISA sound cards
 * working, as auto-detection is not possible for secondary sound cards
 * (except where PnP is supported). Forcing the I/O configuration for
 * a primary ISA device is only necessary if we can't auto-detect it.
 */

char **         NAPI AA_enumerateISADrivers(void);
void            NAPI AA_setIOParams(char *deviceName,N_int32 deviceIndex,AA_IOParams *IO);
void            NAPI AA_unsetIOParams(char *deviceName,N_int32 deviceIndex);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_AUDIO_H */
