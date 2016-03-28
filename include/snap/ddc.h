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
* Description:  Header file for the SciTech SNAP Graphics Display Data Channel
*               Architecture device driver Hardware Abstraction Layer.
*
****************************************************************************/

#ifndef __SNAP_DDC_H
#define __SNAP_DDC_H

#include "snap/common.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/****************************************************************************
REMARKS:
Returns values from DDC_initExt and MCS_beginExt functions.

The ddcOk value indicates that the DDC communications channel was initialised
successfully for both the graphics card and the monitor.

The ddcNotAvailable value indicates that the graphics card does not support
DDC communications or that this feature is disabled for the device driver.

The ddcNoCommunication value indicates that the graphics card does support
DDC communications, however the monitor is not responding on the DDC
communications channel. The most likely cause of this is that the monitor
attached to the graphics card is not DDC2B enabled.

HEADER:
snap/ddc.h

MEMBERS:
ddcOk               - DDC communications initialised correctly
ddcNotAvailable     - DDC is not available
ddcNoCommunication  - DDC is available but not communicating
****************************************************************************/
typedef enum {
    ddcOk                       = 0,
    ddcNotAvailable             = 1,
    ddcNoCommunication          = -1
    } DDC_errCode;

/****************************************************************************
REMARKS:
DPMS state values to pass to the DPMSsetState device driver call. This
enumeration defines the values to set a specific power down state, and are
based on the values defined in the VESA DPMS 1.0 specification. Please
consult this or later versions of this specification for more information.

HEADER:
snap/ddc.h

MEMBERS:
DPMS_on         - Return the controller to the ON state
DPMS_standby    - Set the controller to the Stand-By power down state
DPMS_suspend    - Set the controller to the Suspend power down state
DPMS_off        - Set the controller to the Off power down state
****************************************************************************/
typedef enum {
    DPMS_on         = 0,
    DPMS_standby    = 1,
    DPMS_suspend    = 2,
    DPMS_off        = 4
    } DDC_DPMSStatesType;

/****************************************************************************
REMARKS:
Flags for the level of Serial Control Interface supported by the hardware
and returned by the SCIdetect device driver call. Generally, application
software will not control the SCI interface directly, but will
use the higher level DDC and MCS functions, which implement packet
based protocols over the SCI interface.

HEADER:
snap/ddc.h

MEMBERS:
SCI_writeSCL        - Writing the SCL Clock Line is supported
SCI_writeSDA        - Writing the SDA Data Line is supported
SCI_readSCL         - Reading the SCL Clock Line is supported
SCI_readSDA         - Reading the SDA Data Line is supported
SCI_blankFlag       - Screen will be blanked during communications
****************************************************************************/
typedef enum {
    SCI_writeSCL        = 0x01,
    SCI_writeSDA        = 0x02,
    SCI_readSCL         = 0x04,
    SCI_readSDA         = 0x08,
    SCI_blankFlag       = 0x10
    } DDC_SCIFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the Serial Control Interface functions to determine what
I2C communications channel should be used. Usually the CRT monitor channel
is used, but you can use different channels to control different devices.

HEADER:
snap/ddc.h

MEMBERS:
SCI_channelMonitorPrimary   - Primary head display monitor channel
SCI_channelMonitorSecondary - Secondary head display monitor channel
SCI_channelI2C              - Card specific I2C communication channel
****************************************************************************/
typedef enum {
    SCI_channelMonitorPrimary   = 0x0000,
    SCI_channelMonitorSecondary = 0x0001,
    SCI_channelMonitorTernary   = 0x0002,
    SCI_channelI2C              = 0x0100
    } DDC_ChannelsType;

#define MAX_STANDARD_TIMINGS    35
#define MAX_DETAILED_TIMINGS    4

/****************************************************************************
REMARKS:
This enumeration defines the signal level types stored in the signalLevel
field of the EDID_record structure.

HEADER:
snap/ddc.h

MEMBERS:
EDID_Level_0700_0300_10P        - Analog 0.700 - 0.300 (1.0V p-p)
EDID_Level_0714_0286_10P        - Analog 0.714 - 0.286 (1.0V p-p)
EDID_Level_1000_0400_14P        - Analog 1.000 - 0.400 (1.4V p-p)
EDID_Level_0700_0300_07P        - Analog 0.700 - 0.300 (0.7V p-p)
EDID_Level_Digital              - Digital signal
****************************************************************************/
typedef enum {
    EDID_Level_0700_0300_10P,
    EDID_Level_0714_0286_10P,
    EDID_Level_1000_0400_14P,
    EDID_Level_0700_0300_07P,
    EDID_Level_Digital
    } EDID_signalLevels;

/****************************************************************************
REMARKS:
This enumeration defines the monitor display types stored in the displayType
field of the EDID_record structure.

HEADER:
snap/ddc.h

MEMBERS:
EDID_GrayScale      - Monochrome/Grayscale monitor
EDID_RGBColor       - RGB color monitor
EDID_NonRGBColor    - NonRGB color monitor
****************************************************************************/
typedef enum {
    EDID_GrayScale,
    EDID_RGBColor,
    EDID_NonRGBColor
    } EDID_displayTypes;

/****************************************************************************
REMARKS:
This enumeration defines the list of maximum resolutions as reported in the
EDID_record structure. Note that these values determine if the monitor
can handle the specific resolution in the 60Hz non-interlaced format.
These values may be less than what you would expect if the monitor can handle
1024x768 interlaced, but not in non-interlaced mode.

HEADER:
snap/ddc.h

MEMBERS:
MaxRes_640x480      - Maximum resolution is 640x480 @ 60Hz NI
MaxRes_800x600      - Maximum resolution is 800x600 @ 60Hz NI
MaxRes_1024x768     - Maximum resolution is 1024x768 @ 60Hz NI
MaxRes_1152x864     - Maximum resolution is 1152x864 @ 60Hz NI
MaxRes_1280x1024    - Maximum resolution is 1280x1024 @ 60Hz NI
MaxRes_1600x1200    - Maximum resolution is 1600x1200 @ 60Hz NI
MaxRes_1800x1350    - Maximum resolution is 1800x1350 @ 60Hz NI
MaxRes_1920x1440    - Maximum resolution is 1920x1440 @ 60Hz NI
MaxRes_2048x1536    - Maximum resolution is 2048x1536 @ 60Hz NI
MaxRes_Unrestricted - Maximum resolution is not limited (ie: unknown monitor)
****************************************************************************/
typedef enum {
    MaxRes_640x480,
    MaxRes_800x600,
    MaxRes_1024x768,
    MaxRes_1152x864,
    MaxRes_1280x1024,
    MaxRes_1600x1200,
    MaxRes_1800x1350,
    MaxRes_1920x1440,
    MaxRes_2048x1536,
    MaxRes_Unrestricted = 0xFF
    } EDID_maxResCodes;

/****************************************************************************
REMARKS:
This enumeration defines the values stored in the flags field of the
EDID_record structure.

HEADER:
snap/ddc.h

MEMBERS:
EDID_DPMSStandBy        - The DPMS Standby state is supported
EDID_DPMSSuspend        - The DPMS Suspend state is supported
EDID_DPMSOff            - The DPMS Off state is supported
EDID_DPMSEnabled        - Monitor supports DPMS Power Management
EDID_GTFEnabled         - Monitor supports GTF timings
EDID_DDC2AB             - Monitor supports DDC2AB interface
EDID_Blank2Blank        - Monitor requires Blank-to-Blank setup
EDID_SyncSeparate       - Monitor supports separate syncs
EDID_SyncComposite      - Monitor supports composite syncs
EDID_SyncOnGreen        - Monitor supports Sync on Green
EDID_NeedSerration      - VSync serration is required
EDID_PreferredTiming    - Detailed timing 1 is preferred timing for monitor
****************************************************************************/
typedef enum {
    EDID_DPMSStandBy        = 0x0001,
    EDID_DPMSSuspend        = 0x0002,
    EDID_DPMSOff            = 0x0004,
    EDID_DPMSEnabled        = 0x0007,
    EDID_GTFEnabled         = 0x0008,
    EDID_DDC2AB             = 0x0010,
    EDID_Blank2Blank        = 0x0020,
    EDID_SyncSeparate       = 0x0040,
    EDID_SyncComposite      = 0x0080,
    EDID_SyncOnGreen        = 0x0100,
    EDID_NeedSerration      = 0x0200,
    EDID_PreferredTiming    = 0x0400
    } EDID_flags;

/****************************************************************************
REMARKS:
This enumeration defines the values stored in the flags field of the
EDID_standardTiming structure.

HEADER:
snap/ddc.h

MEMBERS:
EDID_VGACompatible  - VGA compatible timing
EDID_XGACompatible  - XGA compatible timing
EDID_MacCompatible  - Macintosh compatible timing
EDID_VESAStandard   - VESA standard timing
****************************************************************************/
typedef enum {
    EDID_VGACompatible,
    EDID_XGACompatible,
    EDID_MacCompatible,
    EDID_VESAStandard,
    EDID_MaxTimingType
    } EDID_timingTypes;

/****************************************************************************
REMARKS:
Structure to describe all established and standard timings

HEADER:
snap/ddc.h

MEMBERS:
xRes    - Horizontal resolution in pixels
yRes    - Vertical resolution in lines
Hz      - Vertical refresh rate in Hz
flags   - Flags (EDID_timingTypes)
****************************************************************************/
typedef struct {
    ushort  xRes;
    ushort  yRes;
    uchar   Hz;
    uchar   flags;
    } EDID_standardTiming;

/****************************************************************************
REMARKS:
Structure to describe all detailed timings

HEADER:
snap/ddc.h

MEMBERS:
pixelClock      - Pixel clock in Hz / 10,000
hActive         - Horizontal active display value (X resolution)
hBlank          - Horizontal blank start position
hSyncOffset     - Horizontal sync offset from blank start
hSyncWidth      - Horizontal sync width
hBorder         - Horizontal border width
vActive         - Vertical active display (Y resolution)
vBlank          - Vertical blank start position
vSyncOffset     - Vertical sync offset from blank start
vSyncWidth      - Vertical sync width
vBorder         - Vertical border width
hSize           - Horizontal image size in mm
vSize           - Vertical image size in mm
hSyncPol        - Horizontal sync polarity ('+' or '-')
vSyncPol        - Vertical sync polarity ('+' or '-')
hFreq           - Horizontal frequency in KHz * 100
Hz              - Vertical frequency in Hz
****************************************************************************/
typedef struct {
    ushort  pixelClock;
    ushort  hActive;
    ushort  hBlank;
    ushort  hSyncOffset;
    ushort  hSyncWidth;
    ushort  hBorder;
    ushort  vActive;
    ushort  vBlank;
    ushort  vSyncOffset;
    ushort  vSyncWidth;
    ushort  vBorder;
    ushort  hSize;
    ushort  vSize;
    char    hSyncPol;
    char    vSyncPol;
    ushort  hFreq;
    uchar   Hz;
    } EDID_detailedTiming;

/****************************************************************************
REMARKS:
Main structure containing the information parsed from the binary EDID
data returned from the monitor.

HEADER:
snap/ddc.h

MEMBERS:
version             - EDID version Number (in BCD)
mfrID               - 3 byte EISA manufacturer ID
mfrName             - ASCII manufacturer name (Unknown if not found)
modelName           - ASCII model name for monitor (Unknown if not found)
serialNo            - ASCII serial number (Unknown if not found)
PNPID               - 8 character Plug and Play ID
productID           - 16-bit product ID code
serialID            - 32-bit product serial number
mfrWeek             - Week of manufacture (0-52)
mfrYear             - Year of manufacture
signalLevel         - Signal level code (EDID_signalLevels)
displayType         - Display type code (EDID_displayTypes)
maxResolution       - Maximum resolution ID (EDID_maxResCodes)
minHScan            - Minimum horizontal scan (kHz)
maxHScan            - Maximum horizontal scan (kHz)
minVScan            - Minimum vertical scan (Hz)
maxVScan            - Maximum vertical scan (Hz)
maxPClk             - Maximum pixel clock (MHz)
flags               - Capabilities flags (EDID_flags)
maxHSize            - Maximum horizontal size (cm)
maxVSize            - Maximum vertical size (cm)
gamma               - Display transfer characteristic (16.16 fixed point)
Rx                  - Red X chromaticity characteristic (16.16 fixed point)
Ry                  - Red Y chromaticity characteristic (16.16 fixed point)
Gx                  - Green X chromaticity characteristic (16.16 fixed point)
Gy                  - Green Y chromaticity characteristic (16.16 fixed point)
Bx                  - Blue X chromaticity characteristic (16.16 fixed point)
By                  - Blue Y chromaticity characteristic (16.16 fixed point)
Wx                  - Default white point X characteristic (16.16 fixed point)
Wy                  - Default white point Y characteristic (16.16 fixed point)
numStandardTimings  - Number of standard timings listed
numDetailedTimings  - Number of detailed timings listed
standardTimings     - List of standard timings
detailedTimings     - List of detailed timings
****************************************************************************/
typedef struct {
    ushort  version;
    char    mfrID[4];
    char    mfrName[40];
    char    modelName[40];
    char    serialNo[14];
    char    PNPID[8];
    ushort  productID;
    ulong   serialID;
    uchar   mfrWeek;
    ushort  mfrYear;
    uchar   signalLevel;
    uchar   displayType;
    uchar   maxResolution;
    uchar   minHScan;
    uchar   maxHScan;
    uchar   minVScan;
    uchar   maxVScan;
    ushort  maxPClk;
    ushort  flags;
    uchar   maxHSize;
    uchar   maxVSize;
    N_fix32 gamma;
    N_fix32 Rx,Ry;
    N_fix32 Gx,Gy;
    N_fix32 Bx,By;
    N_fix32 Wx,Wy;
    uchar   numStandardTimings;
    uchar   numDetailedTimings;
    EDID_standardTiming standardTimings[MAX_STANDARD_TIMINGS];
    EDID_detailedTiming detailedTimings[MAX_DETAILED_TIMINGS];
    } EDID_record;

/****************************************************************************
REMARKS:
This enumeration defines the known Monitor Control Command Set controls
that can be used to control a monitor via the DDC2Bi protocol. The monitor
controls listed here are defined by the VESA Monitor Control
Command Set V1.0 specification. Please consult the VESA specification (V1.0
or later) for more information.

HEADER:
snap/ddc.h
****************************************************************************/
typedef enum {
    /*------------------------------------------------------------------*/
    /* Continuous response functions                                    */
    /*------------------------------------------------------------------*/
    MCS_brightness                      = 0x10,
    MCS_contrast                        = 0x12,
    MCS_redVideoGain                    = 0x16,
    MCS_greenVideoGain                  = 0x18,
    MCS_blueVideoGain                   = 0x1A,
    MCS_redVideoBlackLevel              = 0x6C,
    MCS_greenVideoBlackLevel            = 0x6E,
    MCS_blueVideoBlackLevel             = 0x70,
    MCS_focus                           = 0x1C,
    MCS_horizontalPosition              = 0x20,
    MCS_horizontalSize                  = 0x22,
    MCS_horizontalPincushion            = 0x24,
    MCS_horizontalPincushionBalance     = 0x26,
    MCS_horizontalMisconvergence        = 0x28,
    MCS_horizontalLinearity             = 0x2A,
    MCS_horizontalLinearityBalance      = 0x2C,
    MCS_verticalPosition                = 0x30,
    MCS_verticalSize                    = 0x32,
    MCS_verticalPincushion              = 0x34,
    MCS_verticalPincushionBalance       = 0x36,
    MCS_verticalMisconvergence          = 0x38,
    MCS_verticalLinearity               = 0x3A,
    MCS_verticalLinearityBalance        = 0x3C,
    MCS_parallelogramDistortion         = 0x40,
    MCS_trapezoidalDistortion           = 0x42,
    MCS_tilt                            = 0x44,
    MCS_topCornerDistortionControl      = 0x46,
    MCS_topCornerDistortionBalance      = 0x48,
    MCS_bottomCornerDistortionControl   = 0x4A,
    MCS_bottomCornerDistortionBalance   = 0x4C,
    MCS_hue                             = 0x50,
    MCS_saturation                      = 0x52,
    MCS_colorCurveAdjust                = 0x54,
    MCS_horizontalMoire                 = 0x56,
    MCS_verticalMoire                   = 0x58,
    MCS_audioSpeakerVolume              = 0x62,
    MCS_microphoneSpeakerVolume         = 0x64,
    MCS_horAdd                          = 0x72,
    MCS_verAdd                          = 0x74,
    MCS_bufferAdd                       = 0x76,
    MCS_update                          = 0x78,
    MCS_adjustFocalPlane                = 0x7A,
    MCS_adjustZoom                      = 0x7C,
    MCS_trapezoid                       = 0x7E,
    MCS_keystone                        = 0x80,
    MCS_horFlip                         = 0x82,
    MCS_vertFlip                        = 0x84,
    MCS_displayScaling                  = 0x86,
    MCS_velocityScanModulation          = 0x88,
    MCS_tvColorSaturation               = 0x8A,
    MCS_tvSharpness                     = 0x8C,
    MCS_tvContrast                      = 0x8E,
    MCS_tvHue                           = 0x90,
    MCS_tvBlackLevel                    = 0x92,

    /*------------------------------------------------------------------*/
    /* Non-continuous functions (read and write)                        */
    /*------------------------------------------------------------------*/
    MCS_selectColorPreset               = 0x14,
    MCS_inputLevelSelect1               = 0x5E,
    MCS_inputLevelSelect2               = 0xCC,
    MCS_inputSourceSelect1              = 0x60,
    MCS_inputSourceSelect2              = 0xCE,
    MCS_outputSourceSelect1             = 0xD0,
    MCS_outputSourceSelect2             = 0xD2,
    MCS_onScreenDisplayEnable           = 0x66,
    MCS_onScreenDisplay                 = 0xCA,
    MCS_languageSelect                  = 0x68,
    MCS_stereoMode                      = 0xD4,
    MCS_displayPowerMode                = 0xD6,
    MCS_presetColorTemp                 = 0xD8,
    MCS_scanFormat                      = 0xDA,
    MCS_displayMode                     = 0xDC,
    MCS_operationMode                   = 0xDE,

    /*------------------------------------------------------------------*/
    /* Read only status functions                                       */
    /*------------------------------------------------------------------*/
    MCS_autoSizeCenter                  = 0xA2,
    MCS_polarityHorizontalSync          = 0xA4,
    MCS_polarityVerticalSync            = 0xA6,
    MCS_syncType                        = 0xA8,
    MCS_screenOrientation               = 0xAA,
    MCS_horFrequency                    = 0xAC,
    MCS_vertFrequency                   = 0xAE,

    /*------------------------------------------------------------------*/
    /* Write only functions                                             */
    /*------------------------------------------------------------------*/
    MCS_degauss                         = 0x00
    } MCS_controlsType;

/****************************************************************************
REMARKS:
This enumeration defines the flags returned by the MCS_getTimingReport
function.

HEADER:
snap/ddc.h

MEMBERS:
MCS_vSyncPositive   - Indicates that vertical sync is positive
MCS_hSyncPositive   - Indicates that horizontal sync is positive
****************************************************************************/
typedef enum {
    MCS_vSyncPositive   = 0x01,
    MCS_hSyncPositive   = 0x02
    } MCS_polarityFlagsType;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all DPMS Display Power Management functions
available for the device. These functions are used to power down the external
CRT or LCD flat panel monitor via the VESA Dislay Power Management
Specification standard.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    int             (NAPIP DPMSdetect)(N_int16 *supportedStates);
    void            (NAPIP DPMSsetState)(N_int32 state);
    void            (NAPIP DPMSsetStateExt)(N_int32 outputHead,N_int32 state);
    } GA_DPMSFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all SCI Serial Control Interface functions
available for the device. These functions are used to communicate over the
I2C bus with external devices such as DDC (or Plug and Play) monitors, TV
encoders and TV tuners.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    int             (NAPIP SCIdetect)(N_uint8 *capabilities,N_int32 *numChannels);
    void            (NAPIP SCIbegin)(N_int32 channel);
    void            (NAPIP SCIwriteSCL)(N_int32 channel,N_int32 bit);
    void            (NAPIP SCIwriteSDA)(N_int32 channel,N_int32 bit);
    int             (NAPIP SCIreadSCL)(N_int32 channel);
    int             (NAPIP SCIreadSDA)(N_int32 channel);
    void            (NAPIP SCIend)(N_int32 channel);
    } GA_SCIFuncs;

#pragma pack()

/*-------------------------- Function Prototypes --------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Utility functions to implement DDC communications layered on SCI */

int         NAPI DDC_init(GA_devCtx *dc);
int         NAPI DDC_initExt(GA_devCtx *dc,N_int32 channel);
ibool       NAPI DDC_readEDID(N_int32 slaveAddr,uchar *edid,N_int32 length,N_int32 blockNumber,N_int32 channel);
ibool       NAPI DDC_writeEDID(GA_devCtx *dc,N_int32 slaveAddr,uchar *edid,N_int32 length,N_int32 blockNumber,N_int32 channel);

/* Utility function to parse the binary EDID information */

int         NAPI EDID_parse(uchar *edid,EDID_record *rec,N_int32 requireDescriptor);

/* Utility functions to implement DDC/CI communications layered on SCI */

int         NAPI MCS_begin(GA_devCtx *dc);
int         NAPI MCS_beginExt(GA_devCtx *dc,N_int32 channel);
int         NAPI MCS_getCapabilitiesString(char *data,N_int32 maxlen);
ibool       NAPI MCS_isControlSupported(uchar controlCode);
ibool       NAPI MCS_enableControl(uchar controlCode,N_int32 enable);
ibool       NAPI MCS_getControlMax(uchar controlCode,ushort *max);
ibool       NAPI MCS_getControlValue(uchar controlCode,ushort *value);
ibool       NAPI MCS_getControlValues(N_int32 numControls,uchar *controlCodes,ushort *values);
ibool       NAPI MCS_setControlValue(uchar controlCode,ushort value);
ibool       NAPI MCS_setControlValues(N_int32 numControls,uchar *controlCodes,ushort *values);
ibool       NAPI MCS_resetControl(uchar controlCode);
ibool       NAPI MCS_saveCurrentSettings(void);
ibool       NAPI MCS_getTimingReport(uchar *flags,ushort *hFreq,ushort *vFreq);
ibool       NAPI MCS_getSelfTestReport(uchar *flags,uchar *data,uchar *length);
void        NAPI MCS_end(void);

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_DDC_H */

