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
* Description:  Header file for the SciTech SNAP Graphics Architecture
*               device driver Hardware Abstraction Layer.
*
*               Note that the SNAP Graphics Architecture provides
*               support for multiple independent controllers and full plug
*               and play support. This allows for multiple drivers to
*               be present in a single system, and the proper driver will be
*               matched to the card's found in the system given the PCI
*               configuration IDs for the cards.
*
*               ISA and VLB devices are only supported in single controller
*               mode.
*
****************************************************************************/

#ifndef __SNAP_GRAPHICS_H
#define __SNAP_GRAPHICS_H

#include "snap/monitor.h"
#include "clib/modloadr.h"
#include "ztimerc.h"

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/* Signature for the graphics driver file */

#define GA_SIGNATURE            "GRAPHICS"

/* Define the interface version */

#define GA_VERSION              0x302

/* Define the maximum number of chained filter drivers */

#define GA_MAX_FILTERS          8

/* Define the maximum number of virtual displays */

#define GA_MAX_VIRTUAL_DISPLAYS 16

/* Define the maximum number of multiple heads supported */

#define GA_MAX_HEADS            4

/* Define the maximum number of DPVL heads supported */

#define GA_MAX_DPVL_HEADS       16

/* Define the flag to enable mixed mode for GA_setActiveDevice */

#define GA_DEVICE_MIXED_MODE    0x80

/* Flag to enable write combining for an MMIO region */

#define GA_MMIO_WRITECOMBINE    0x80000000

/* Macro to allow C++ style syntax checking of function pointers when
 * compiling the documentation. This allows us to ensure the docs have
 * the correct syntax compared to the actual functions when run through
 * DocJet.
 */

#ifdef CHECK_DOCS
#undef NAPIP
#define NAPIP   _cdecl
#endif

/****************************************************************************
REMARKS:
This enumeration defines the identifiers used to obtain the device context
function group pointer structures. As new features and capabilities are
added to the future versions of the specification, new identifiers will
be added to extract new function pointers from the drivers.

The GA_GET_FIRST_OEM defines the first identifier for OEM extensions. OEMs
are free to added their own private functional extensions to the drivers
as desired. Note that OEMs must verify the presence of their OEM drivers
via the OemVendorName string before attempting to use OEM extension
functions.

HEADER:
snap/graphics.h

MEMBERS:
GA_GET_RESERVED             - Reserved value
GA_GET_INITFUNCS            - Get GA_initFuncs structure
GA_GET_DRIVERFUNCS          - Get GA_driverFuncs structure
GA_GET_CURSORFUNCS          - Get GA_cursorFuncs structure
GA_GET_VIDEOFUNCS           - Get GA_videoFuncs strucure
GA_GET_DPMSFUNCS            - Get GA_DPMSFuncs structure
GA_GET_SCIFUNCS             - Get GA_SCIFuncs structure
GA_GET_2DSTATEFUNCS         - Get GA_2DStateFuncs structure
GA_GET_2DRENDERFUNCS        - Get GA_2DRenderFuncs structure
GA_GET_VBEFUNCS             - Get GA_VBEFuncs structure
GA_GET_REGIONFUNCS          - Get GA_regionFuncs structure
GA_GET_BUFFERFUNCS          - Get GA_bufferFuncs structure
GA_GET_CLIPPERFUNCS         - Get GA_clipperFuncs structure
GA_GET_HWMCFUNCS            - Get GA_HwMCFuncs structure
GA_GET_DPVLFUNCS            - Get GA_DPVLFuncs structure
GA_GET_VIDEOCAPTUREFUNCS    - Get GA_VideoCaptureFuncs structure
GA_GET_FIRST_OEM            - ID of first OEM extension function
****************************************************************************/
typedef enum {
    GA_GET_RESERVED,
    GA_GET_INITFUNCS,
    GA_GET_DRIVERFUNCS,
    GA_GET_CURSORFUNCS,
    GA_GET_VIDEOFUNCS,
    GA_GET_DPMSFUNCS,
    GA_GET_SCIFUNCS,
    GA_GET_2DSTATEFUNCS,
    GA_GET_2DRENDERFUNCS,
    GA_GET_RESERVED1,
    GA_GET_RESERVED2,
    GA_GET_RESERVED3,
    GA_GET_RESERVED4,
    GA_GET_VBEFUNCS,
    GA_GET_REGIONFUNCS,
    GA_GET_BUFFERFUNCS,
    GA_GET_CLIPPERFUNCS,
    GA_GET_HWMCFUNCS,
    GA_GET_DPVLFUNCS,
    GA_GET_VIDEOCAPTUREFUNCS,
    GA_GET_FIRST_OEM                = 0x00010000
    } GA_funcGroupsType;

/****************************************************************************
REMARKS:
This enumeration defines the values stored in the BusType field of the
GA_devCtx structure.

HEADER:
snap/graphics.h

MEMBERS:
gaUnknownBus    - Bus type is not known
gaISABus        - Device is an ISA bus device
gaMCABus        - Device is a Micro-Channel bus device
gaVLBBus        - Device is a VESA Local Bus device
gaPCIBus        - Device is a PCI bus device
gaAGPBus        - Device is an AGP bus device
gaPCIeBus       - Device is a PCI Express bus device
****************************************************************************/
typedef enum {
    gaUnknownBus                    = 0,
    gaISABus                        = 1,
    gaMCABus                        = 2,
    gaVLBBus                        = 3,
    gaPCIBus                        = 4,
    gaAGPBus                        = 5,
    gaPCIeBus                       = 6
    } GA_busType;

/****************************************************************************
REMARKS:
This enumeration defines the flags for combining with graphics mode numbers
to be passed to the SetVideoMode function.

HEADER:
snap/graphics.h

MEMBERS:
gaDontClear         - Don't clear display memory
gaLinearBuffer      - Enable linear framebuffer mode
ga6BitDAC           - Set the mode with a 6-bit RAMDAC instead of 8
gaNoAccel           - Set the mode without any acceleration support
gaRefreshCtrl       - Enable refresh rate control
gaWindowedMode      - Initialise for use in the current desktop mode
gaPartialModeSet    - Initialise the driver internals, but don't program hardware
gaModeMask          - Mask to remove flags and extract VBE mode number
****************************************************************************/
typedef enum {
    gaDontClear                     = 0x8000,
    gaLinearBuffer                  = 0x4000,
    ga6BitDAC                       = 0x2000,
    gaNoAccel                       = 0x1000,
    gaRefreshCtrl                   = 0x0800,
    gaWindowedMode                  = 0x0400,
    gaPartialModeSet                = 0x0200,
    gaModeMask                      = 0x01FF
    } GA_modeFlagsType;

/****************************************************************************
REMARKS:
Logical mix operation codes for accelerated rendering functions that
support mixes. The set of mix codes is the standard Microsoft Raster
Operation (ROP2) codes between two values. We define our ROP2 codes as being
between the source and destination pixels for blt's, between the foreground
or background color and the destination pixels for solid and mono pattern
fills and between the pattern pixels and the destination pixels for color
pattern fills. It is up to the driver to do any necessary translation
between these generic ROP2 codes and each different type of hardware mix
code internally. Next to each code is the equivalent Microsoft defined ROP3
code between source and destination pixels.

Note:   Some graphics controllers may not support all mix codes, so you must
        use the GetMixTable function to determine the set of mix codes that
        the controller supports. Setting a mix code that is not listed in
        the returned mix table will result in undefined behaviour.

HEADER:
snap/graphics.h

MEMBERS:
GA_R2_BLACK         - 0
GA_R2_NOTMERGESRC   - DSon
GA_R2_MASKNOTSRC    - DSna
GA_R2_NOTCOPYSRC    - Sn
GA_R2_MASKSRCNOT    - SDna
GA_R2_NOT           - Dn
GA_R2_XORSRC        - DSx
GA_R2_NOTMASKSRC    - DSan
GA_R2_MASKSRC       - DSa
GA_R2_NOTXORSRC     - DSxn
GA_R2_NOP           - D
GA_R2_MERGENOTSRC   - DSno
GA_R2_COPYSRC       - S
GA_R2_MERGESRCNOT   - SDno
GA_R2_MERGESRC      - DSo
GA_R2_WHITE         - 1
GA_REPLACE_MIX      - Replace mode
GA_AND_MIX          - AND mode
GA_OR_MIX           - OR mode
GA_XOR_MIX          - XOR mode
GA_NOP_MIX          - Destination pixel unchanged
****************************************************************************/
typedef enum {
    GA_R2_BLACK,
    GA_R2_NOTMERGESRC,
    GA_R2_MASKNOTSRC,
    GA_R2_NOTCOPYSRC,
    GA_R2_MASKSRCNOT,
    GA_R2_NOT,
    GA_R2_XORSRC,
    GA_R2_NOTMASKSRC,
    GA_R2_MASKSRC,
    GA_R2_NOTXORSRC,
    GA_R2_NOP,
    GA_R2_MERGENOTSRC,
    GA_R2_COPYSRC,
    GA_R2_MERGESRCNOT,
    GA_R2_MERGESRC,
    GA_R2_WHITE,
    GA_REPLACE_MIX = GA_R2_COPYSRC,
    GA_AND_MIX     = GA_R2_MASKSRC,
    GA_OR_MIX      = GA_R2_MERGESRC,
    GA_XOR_MIX     = GA_R2_XORSRC,
    GA_NOP_MIX     = GA_R2_NOP
    } GA_mixCodesType;

/****************************************************************************
REMARKS:
Raster Operation codes for accelerated rendering functions that support
ternary operations. The set of mix codes is the standard Microsoft ternary
Raster Operation (ROP3) codes between three values, a source, pattern and
destination. Note that we don't list the codes here for brevity.

Note:   Some graphics controllers may not support all ROP3 codes due to
        hardware bugs, so you must use the GetROP3ExceptionTable function to
        determine the set of ROP3 codes that the hardware does not properly
        handle.

HEADER:
snap/graphics.h
****************************************************************************/
typedef enum {
    GA_R3_0,
    GA_R3_DPSoon,
    GA_R3_DPSona,
    GA_R3_PSon,
    GA_R3_SDPona,
    GA_R3_DPon,
    GA_R3_PDSxnon,
    GA_R3_PDSaon,
    GA_R3_SDPnaa,
    GA_R3_PDSxon,
    GA_R3_DPna,
    GA_R3_PSDnaon,
    GA_R3_SPna,
    GA_R3_PDSnaon,
    GA_R3_PDSonon,
    GA_R3_Pn,
    GA_R3_PDSona,
    GA_R3_DSon,
    GA_R3_SDPxnon,
    GA_R3_SDPaon,
    GA_R3_DPSxnon,
    GA_R3_DPSaon,
    GA_R3_PSDPSanaxx,
    GA_R3_SSPxDSxaxn,
    GA_R3_SPxPDxa,
    GA_R3_SDPSanaxn,
    GA_R3_PDSPaox,
    GA_R3_SDPSxaxn,
    GA_R3_PSDPaox,
    GA_R3_DSPDxaxn,
    GA_R3_PDSox,
    GA_R3_PDSoan,
    GA_R3_DPSnaa,
    GA_R3_SDPxon,
    GA_R3_DSna,
    GA_R3_SPDnaon,
    GA_R3_SPxDSxa,
    GA_R3_PDSPanaxn,
    GA_R3_SDPSaox,
    GA_R3_SDPSxnox,
    GA_R3_DPSxa,
    GA_R3_PSDPSaox,
    GA_R3_DPSana,
    GA_R3_SSPxPDxaxn,
    GA_R3_SPDSoax,
    GA_R3_PSDnox,
    GA_R3_PSDPxox,
    GA_R3_PSDnoan,
    GA_R3_PSna,
    GA_R3_SDPnaon,
    GA_R3_SDPSoox,
    GA_R3_Sn,
    GA_R3_SPDSaox,
    GA_R3_SPDSxnox,
    GA_R3_SDPox,
    GA_R3_SDPoan,
    GA_R3_PSDPoax,
    GA_R3_SPDnox,
    GA_R3_SPDSxox,
    GA_R3_SPDnoan,
    GA_R3_PSx,
    GA_R3_SPDSonox,
    GA_R3_SPDSnaox,
    GA_R3_PSan,
    GA_R3_PSDnaa,
    GA_R3_DPSxon,
    GA_R3_SDxPDxa,
    GA_R3_SPDSanaxn,
    GA_R3_SDna,
    GA_R3_DPSnaon,
    GA_R3_DSPDaox,
    GA_R3_PSDPxaxn,
    GA_R3_SDPxa,
    GA_R3_PDSPDaoxxn,
    GA_R3_DPSDoax,
    GA_R3_PDSnox,
    GA_R3_SDPana,
    GA_R3_SSPxDSxoxn,
    GA_R3_PDSPxox,
    GA_R3_PDSnoan,
    GA_R3_PDna,
    GA_R3_DSPnaon,
    GA_R3_DPSDaox,
    GA_R3_SPDSxaxn,
    GA_R3_DPSonon,
    GA_R3_Dn,
    GA_R3_DPSox,
    GA_R3_DPSoan,
    GA_R3_PDSPoax,
    GA_R3_DPSnox,
    GA_R3_DPx,
    GA_R3_DPSDonox,
    GA_R3_DPSDxox,
    GA_R3_DPSnoan,
    GA_R3_DPSDnaox,
    GA_R3_DPan,
    GA_R3_PDSxa,
    GA_R3_DSPDSaoxxn,
    GA_R3_DSPDoax,
    GA_R3_SDPnox,
    GA_R3_SDPSoax,
    GA_R3_DSPnox,
    GA_R3_DSx,
    GA_R3_SDPSonox,
    GA_R3_DSPDSonoxxn,
    GA_R3_PDSxxn,
    GA_R3_DPSax,
    GA_R3_PSDPSoaxxn,
    GA_R3_SDPax,
    GA_R3_PDSPDoaxxn,
    GA_R3_SDPSnoax,
    GA_R3_PDSxnan,
    GA_R3_PDSana,
    GA_R3_SSDxPDxaxn,
    GA_R3_SDPSxox,
    GA_R3_SDPnoan,
    GA_R3_DSPDxox,
    GA_R3_DSPnoan,
    GA_R3_SDPSnaox,
    GA_R3_DSan,
    GA_R3_PDSax,
    GA_R3_DSPDSoaxxn,
    GA_R3_DPSDnoax,
    GA_R3_SDPxnan,
    GA_R3_SPDSnoax,
    GA_R3_DPSxnan,
    GA_R3_SPxDSxo,
    GA_R3_DPSaan,
    GA_R3_DPSaa,
    GA_R3_SPxDSxon,
    GA_R3_DPSxna,
    GA_R3_SPDSnoaxn,
    GA_R3_SDPxna,
    GA_R3_PDSPnoaxn,
    GA_R3_DSPDSoaxx,
    GA_R3_PDSaxn,
    GA_R3_DSa,
    GA_R3_SDPSnaoxn,
    GA_R3_DSPnoa,
    GA_R3_DSPDxoxn,
    GA_R3_SDPnoa,
    GA_R3_SDPSxoxn,
    GA_R3_SSDxPDxax,
    GA_R3_PDSanan,
    GA_R3_PDSxna,
    GA_R3_SDPSnoaxn,
    GA_R3_DPSDPoaxx,
    GA_R3_SPDaxn,
    GA_R3_PSDPSoaxx,
    GA_R3_DPSaxn,
    GA_R3_DPSxx,
    GA_R3_PSDPSonoxx,
    GA_R3_SDPSonoxn,
    GA_R3_DSxn,
    GA_R3_DPSnax,
    GA_R3_SDPSoaxn,
    GA_R3_SPDnax,
    GA_R3_DSPDoaxn,
    GA_R3_DSPDSaoxx,
    GA_R3_PDSxan,
    GA_R3_DPa,
    GA_R3_PDSPnaoxn,
    GA_R3_DPSnoa,
    GA_R3_DPSDxoxn,
    GA_R3_PDSPonoxn,
    GA_R3_PDxn,
    GA_R3_DSPnax,
    GA_R3_PDSPoaxn,
    GA_R3_DPSoa,
    GA_R3_DPSoxn,
    GA_R3_D,
    GA_R3_DPSono,
    GA_R3_SPDSxax,
    GA_R3_DPSDaoxn,
    GA_R3_DSPnao,
    GA_R3_DPno,
    GA_R3_PDSnoa,
    GA_R3_PDSPxoxn,
    GA_R3_SSPxDSxox,
    GA_R3_SDPanan,
    GA_R3_PSDnax,
    GA_R3_DPSDoaxn,
    GA_R3_DPSDPaoxx,
    GA_R3_SDPxan,
    GA_R3_PSDPxax,
    GA_R3_DSPDaoxn,
    GA_R3_DPSnao,
    GA_R3_DSno,
    GA_R3_SPDSanax,
    GA_R3_SDxPDxan,
    GA_R3_DPSxo,
    GA_R3_DPSano,
    GA_R3_PSa,
    GA_R3_SPDSnaoxn,
    GA_R3_SPDSonoxn,
    GA_R3_PSxn,
    GA_R3_SPDnoa,
    GA_R3_SPDSxoxn,
    GA_R3_SDPnax,
    GA_R3_PSDPoaxn,
    GA_R3_SDPoa,
    GA_R3_SPDoxn,
    GA_R3_DPSDxax,
    GA_R3_SPDSaoxn,
    GA_R3_S,
    GA_R3_SDPono,
    GA_R3_SDPnao,
    GA_R3_SPno,
    GA_R3_PSDnoa,
    GA_R3_PSDPxoxn,
    GA_R3_PDSnax,
    GA_R3_SPDSoaxn,
    GA_R3_SSPxPDxax,
    GA_R3_DPSanan,
    GA_R3_PSDPSaoxx,
    GA_R3_DPSxan,
    GA_R3_PDSPxax,
    GA_R3_SDPSaoxn,
    GA_R3_DPSDanax,
    GA_R3_SPxDSxan,
    GA_R3_SPDnao,
    GA_R3_SDno,
    GA_R3_SDPxo,
    GA_R3_SDPano,
    GA_R3_PDSoa,
    GA_R3_PDSoxn,
    GA_R3_DSPDxax,
    GA_R3_PSDPaoxn,
    GA_R3_SDPSxax,
    GA_R3_PDSPaoxn,
    GA_R3_SDPSanax,
    GA_R3_SPxPDxan,
    GA_R3_SSPxDSxax,
    GA_R3_DSPDSanaxxn,
    GA_R3_DPSao,
    GA_R3_DPSxno,
    GA_R3_SDPao,
    GA_R3_SDPxno,
    GA_R3_DSo,
    GA_R3_SDPnoo,
    GA_R3_P,
    GA_R3_PDSono,
    GA_R3_PDSnao,
    GA_R3_PSno,
    GA_R3_PSDnao,
    GA_R3_PDno,
    GA_R3_PDSxo,
    GA_R3_PDSano,
    GA_R3_PDSao,
    GA_R3_PDSxno,
    GA_R3_DPo,
    GA_R3_DPSnoo,
    GA_R3_PSo,
    GA_R3_PSDnoo,
    GA_R3_DPSoo,
    GA_R3_1
    } GA_rop3CodesType;

/****************************************************************************
REMARKS:
Flags for 2D alpha blending functions supported by the SNAP Graphics drivers.
The values in here define the alpha blending functions passed to
the srcBlendFunc and dstBlendFunc parameters of the SetBlendFunc function.
Essentially the blend function defines how to combine the source and
destination pixel color together to get the resulting destination color
during rendering. The formula used for this is defined as:

    DstColor = SrcColor * SrcFunc + DstColor * DstFunc;

If the source alpha blending function is set to gaBlendConstantAlpha, the
/SrcFunc/ above becomes:

    SrcFunc = ConstAlpha

If the destination alpha blending function is set to gaBlendOneMinusDstAlpha
then /DstFunc/ above becomes:

    DstFunc = (1-DstAlpha)

and the final equation becomes (note that each color channel is multiplied
individually):

    DstColor = SrcColor * ConstAlpha + DstColor * (1-DstAlpha)

Although the above is a completely contrived example, it does illustrate
how the functions defined below combine to allow you to build complex and
interesting blending functions. For simple source alpha transparency, the
following formula would usually be used:

    DstColor = SrcColor * SrcAlpha + DstColor * (1-SrcAlpha)

If you wish to use this type of blending and you do not care about the
resulting alpha channel information, you can set the optimised
gaBlendSrcAlphaFast blending mode. If you set both the source and
destination blending modes to this value, the above formula will
be used but an optimised fast path will be taken internally to make
this run as fast as possible. For normal blending operations this will
be much faster than setting the above formula manually. If however you need
the destination alpha to be preserved, you will need to use the slower
method instead.

For simple constant alpha transparency, the following formula would usually
be used:

    DstColor = SrcColor * ConstantAlpha + DstColor * (1-ConstantAlpha)

If you wish to use this type of blending and you do not care about the
resulting alpha channel information, you can set the optimised
gaBlendConstantAlphaFast blending mode. If you set both the source and
destination blending modes to this value, the above formula will
be used but an optimised fast path will be taken internally to make
this run as fast as possible. For normal blending operations this will
be much faster than setting the above formula manually. If however you need
the destination alpha to be preserved, you will need to use the slower
method instead.

Note:   All the above equations assume the color values and alpha values
        are in the range of 0 through 1 in floating point. In reality all
        blending is done with integer color and alpha components in the
        range of 0 to 255, when a value of 255 corresponds to a value of 1.0
        in the above equations.

Note:   The constant color value set by a call to SetForeColor, and the
        constant alpha value set by a call to SetAlphaValue.

Note:   Setting a blending function that uses the destination alpha
        components is only supported if the framebuffer currently supports
        destination alpha. Likewise setting a blending function that
        uses source alpha components is only supported if the framebuffer
        or incoming bitmap data contains an alpha channel. The results
        are undefined if these conditions are not met.

Note:   Enabling source or destination alpha blending overrides the
        setting of the current mix mode. Logical mix modes and blending
        cannot be used at the same time.

HEADER:
snap/graphics.h

MEMBERS:
gaBlendNone                     - No alpha blending
gaBlendZero                     - Blend factor is always zero
gaBlendOne                      - Blend factor is always one
gaBlendSrcColor                 - Blend factor is source color
gaBlendOneMinusSrcColor         - Blend factor is 1-source color
gaBlendSrcAlpha                 - Blend factor is source alpha
gaBlendOneMinusSrcAlpha         - Blend factor is 1-source alpha
gaBlendDstAlpha                 - Blend factor is destination alpha
gaBlendOneMinusDstAlpha         - Blend factor is 1-destination alpha
gaBlendDstColor                 - Blend factor is destination color
gaBlendOneMinusDstColor         - Blend factor is 1-destination color
gaBlendSrcAlphaSaturate         - Blend factor is src alpha saturation
gaBlendConstantColor            - Blend factor is a constant color
gaBlendOneMinusConstantColor    - Blend factor is 1-constant color
gaBlendConstantAlpha            - Blend factor is constant alpha
gaBlendOneMinusConstantAlpha    - Blend factor is 1-constant alpha
gaBlendSrcAlphaFast             - Common case of optimised src alpha
gaBlendConstantAlphaFast        - Common case of optimised constant alpha
****************************************************************************/
typedef enum {
    gaBlendNone,
    gaBlendZero,
    gaBlendOne,
    gaBlendSrcColor,
    gaBlendOneMinusSrcColor,
    gaBlendSrcAlpha,
    gaBlendOneMinusSrcAlpha,
    gaBlendDstAlpha,
    gaBlendOneMinusDstAlpha,
    gaBlendDstColor,
    gaBlendOneMinusDstColor,
    gaBlendSrcAlphaSaturate,
    gaBlendConstantColor,
    gaBlendOneMinusConstantColor,
    gaBlendConstantAlpha,
    gaBlendOneMinusConstantAlpha,
    gaBlendSrcAlphaFast,
    gaBlendConstantAlphaFast
    } GA_blendFuncType;

/****************************************************************************
REMARKS:
Flags for the Attributes member of the GA_modeInfo structure and in the
Attributes member of the main GA_devCtx device context block structure. These
flags define the hardware capabilities of the particular device or graphics
mode.

The gaHaveDisplayStart flag is used to determine whether the graphics mode
supports changing the CRTC display start address. This is used to implement
hardware virtual scrolling and multi-buffering for flicker free animation.
If this bit is 0, then the application cannot change the display start
address after initialising a display mode.

The gaHaveBankedBuffer flag is used to determine if the graphics mode
supports the banked framebuffer access modes. If this bit is 0, then the
application cannot use the banked framebuffer style access. Some controllers
may not support a banked framebuffer mode in some modes. In this case a
linear framebuffer mode will be provided (either a banked buffer or linear
buffer must be available for the mode to be valid).

The gaHaveLinearBuffer flag is used to determine if the graphics mode
supports the linear framebuffer access modes. If this bit is 0, then the
application cannot start the linear framebuffer graphics mode.

The gaHaveAccel2D flag is used to determine if the graphics mode supports 2D
accelerator functions. If this bit is 0, then the application can only use
direct framebuffer access in this video mode, and the 2D acceleration
functions are not available. The cases where this might crop up are more
common than you might think. This bit may be 0 for very low resolution
graphics modes on some controllers, and on older controllers for the 24 bit
and above graphics modes.

The gaHaveHWCursor flag is used to determine if the controller supports a
hardware cursor for the specified graphics mode. You must check this flag
for each graphics mode before attempting to use the hardware cursor functions
as some graphics modes will not be able to support the hardware cursor (but
may still support 2D acceleration).

The gaHave8BitDAC flag is used to determine if the controller will be using
the 8 bit wide palette DAC modes when running in 256 color index modes. The 8
bit DAC modes allow the palette to be selected from a range of 16.7 million
colors rather than the usual 256k colors available in 6 bit DAC mode. The 8
bit DAC mode allows the 256 color modes to display a full range of 256
grayscales, while the 6 bit mode only allows a selection of 64 grayscales.
Note that the 8 bit DAC mode is not selectable. If the hardware supports an
8 bit DAC, it will always be used by default.

The gaHaveNonVGAMode flag is used to determine if the mode is a VGA compatible
mode or a NonVGA mode. If this flag is set, the application software must
ensure that no attempts are made to directly program any of the standard VGA
compatible registers such as the RAMDAC control registers and input status
registers while the NonVGA graphics mode is used. Attempting to use these
registers in NonVGA modes generally results in the application program
hanging the system.

The gaHaveDoubleScan flag is used to determine if the mode requires double
scanning. If this bit is set, the double scan bit must be set for the
graphics mode if it is initialised with generic refresh control turned on.

The gaHaveTripleScan flag is used to determine if the mode requires triple
scanning. If this bit is set, the triple scan bit must be set for the
graphics mode if it is initialised with generic refresh control turned on.

The gaHaveInterlaced flag is used to determine if the mode supports
interlaced operation or not. If this bit is set, the mode may be initialized
for interlaced operation when using the refresh rate control to initialise
the mode.

The gaHaveTripleBuffer flag is used to determine if the mode supports
hardware triple buffering.

The gaHaveStereo flag is used to determine if the mode supports hardware
support for stereo LC shutter glasses.

The gaHaveHWStereoSync flag is used to determine if the controller supports
the hardware stereo LC shutter glasses sync signal via the VESA EVC Enhanced
Video Connector. The gaHaveEVCStereoSync flag is used to determine if the
controller supports the hardware stereo LC shutter glasses sync signal via
the VESA mini-DIN3 stereo connector. If either of these values are set, the
application can disable all software stero sync mechanisms and rely on the
hardware stereo sync for maximum performance.

The gaHaveAccelVideo flag is used to determine if the mode supports
hardware video acceleration. If this bit is not 0, then the application
can use the hardware video functions for video overlay windows.

The gaHaveAccel3D flag is used to determine if the mode supports hardware
3D acceleration. If this bit is not 0, then the application can use the
hardware 3D acceleration functions for high performance 3D graphics.

The gaHave8bppRGBCursor flag is used to determine if the color values for
the hardware cursor in 8bpp modes are defined as a color index or as
a TrueColor RGB tuple. Most cards require a color index in 8bpp modes,
but some new hardware uses a TrueColor cursor in 8bpp display modes and
this flag will be set if this is the case.

The gaHaveAccelIOPL flag indicates that the accelerated drawing functions
require IOPL access to be enabled. If this flag is not set, then the 2D
and 3D drawing functions use only memory mapped registers and hence can
be executed entirely in ring-3 without needing IOPL to be enabled. Note
that this does not include hardware cursor functions or hardware video
overlay functions, only 2D and 3D drawing functions. It is assumed that
all initialisation and driver functions require IOPL to be enabled.

The gaHaveVideoCapture flag is used to determine if the mode supports
hardware video capture. If this bit is not 0, then the application can
use the hardware video capture functions for capturing incoming video
stream data, or displaying it on the screen via video overlays.

The gaIsVirtualMode flag indicates that the mode is a special
multi-controller or multi-head virtual display mode that spans multiple
display heads. This is an informational flag so that any high
level OS drivers can know when one of these modes is in use.

The gaHaveMultiHead flag is used to determine if the controller is capable
of supporting dual head operation via two separate CRTC connector output.
This flag is generally only included the GA_devCtx Attribute member and
not in the Attributes member of the GA_modeInfo structure.

The gaHaveDFPOutput flag is used to determine if a mode can be displayed
on an LCD flat panel monitor using the DFP or DVI connectors. This flag is
generally only available for graphics cards that have DVI or DFP
connector and indicates display modes can support output to the LCD
flat panel monitor as well as simultaneous output to both displays
at the same time.

The gaHaveLCDOutput flag is used to determine if a mode can be displayed
on an LCD flat panel. This flag is generally only available for laptop
chipsets, and indicates display modes can support output to the LCD
panel as well as simultaneous output to both displays at the same time.

The gaHaveTVOutput flag is used to determine if a mode can be displayed
via the TVOut connector for the graphics card. If the graphics card does
not support TVOut capabilities this flag will never be set. Otherwise it
will be set for those display modes that can be displayed on the TV. Note
that both PAL and NTSC output may be supported, or only one or the other
depending on the underlying hardware.

The gaIsTextMode flag is used to determine if the mode is a graphics mode
or a text mode. If this flag is set to 1, then the mode is a hardware text
mode and not a graphics mode.

HEADER:
snap/graphics.h

MEMBERS:
gaHaveDisplayStart      - Mode supports changing the display start address
gaHaveBankedBuffer      - Mode supports banked framebuffer access
gaHaveLinearBuffer      - Mode supports linear framebuffer access
gaHaveAccel2D           - Mode supports 2D acceleration
gaHaveHWCursor          - Mode supports a hardware cursor
gaHave8BitDAC           - Mode uses an 8 bit palette DAC
gaHaveNonVGAMode        - Mode is a NonVGA mode
gaHaveDoubleScan        - Mode is double scanned
gaHaveTripleScan        - Mode is triple scanned
gaHaveInterlaced        - Mode supports interlacing
gaHaveTripleBuffer      - Mode supports triple buffering
gaHaveStereo            - Mode supports stereo LCD glasses
gaHaveHWStereoSync      - Mode supports stereo signalling
gaHaveEVCStereoSync     - Mode supports stereo sync via EVC connector
gaHaveAccelVideo        - Mode supports video playback acceleration
gaHaveAccel3D           - Mode supports 3D acceleration
gaHave8bppRGBCursor     - Mode requires RGB colors for 8bpp hardware cursor
gaHaveAccelMPEG         - Mode supports hardware motion compensation and iDCT
gaHaveAccelIOPL         - Mode needs IOPL for drawing functions
gaHaveVideoCapture      - Mode supports hardware video capture
gaHaveEngineClock       - Display adapter supports programmable engine clock
gaIsGUIDesktop          - The mode is the original GUI desktop mode
gaIsVirtualMode         - Mode is a multi-head or multi-controller virtual mode
gaHaveMultiHead         - Display adapter supports multi head operation
gaHaveDFPOutput         - Mode supports output to DFP digital flat panel
gaHaveLCDOutput         - Mode supports output to LCD laptop display
gaHaveTVOutput          - Mode supports output to TV connector
gaIsTextMode            - Mode is a text mode rather than a graphics mode
****************************************************************************/
typedef enum {
    gaHaveDisplayStart              = 0x00000001,
    gaHaveBankedBuffer              = 0x00000002,
    gaHaveLinearBuffer              = 0x00000004,
    gaHaveAccel2D                   = 0x00000008,
    gaHaveHWCursor                  = 0x00000010,
    gaHave8BitDAC                   = 0x00000020,
    gaHaveNonVGAMode                = 0x00000040,
    gaHaveDoubleScan                = 0x00000080,
    gaHaveTripleScan                = 0x00000100,
    gaHaveInterlaced                = 0x00000200,
    gaHaveTripleBuffer              = 0x00000400,
    gaHaveStereo                    = 0x00000800,
    gaHaveHWStereoSync              = 0x00001000,
    gaHaveEVCStereoSync             = 0x00002000,
    gaHaveAccelVideo                = 0x00004000,
    gaHaveAccel3D                   = 0x00008000,
    gaHave8bppRGBCursor             = 0x00010000,
    gaHaveAccelMPEG                 = 0x00020000,
    gaHaveAccelIOPL                 = 0x00040000,
    gaHaveVideoCapture              = 0x00080000,
    gaHaveEngineClock               = 0x00200000,
    gaIsGUIDesktop                  = 0x01000000,
    gaHaveMultiHead                 = 0x02000000,
    gaHaveDFPOutput                 = 0x04000000,
    gaIsVirtualMode                 = 0x08000000,
    gaHaveLCDOutput                 = 0x10000000,
    gaHaveTVOutput                  = 0x20000000,
    gaIsTextMode                    = 0x40000000
    } GA_AttributeFlagsType;

/****************************************************************************
REMARKS:
Flags for the AttributesExt member of the GA_modeInfo structure and in the
AttributesExt member of the main GA_devCtx device context block structure.
These flags define the hardware capabilities of the particular device or
graphics mode.

The gaIsPannedMode flag is used to determine if the mode is a virtual
hardware panned display mode or if the mode is a non-panned display mode.
This flag is only ever set if you call the GetVideoModeInfoExt or
GetCustomVideoModeInfoExt functions, as the original versions of these
functions assume non panning modes will be reported. If the mode is
a hardware panned mode for the requested output device, it means that
if the mode is set while that output device is active hardware panning
will be enabled. It is then up to the shell driver to interface with
the mouse driver to implement the actual hardware panning.

The gaNoRefreshCtrl flag indicates that the device has no refresh
control. This will only be reported in the GA_devCtx variable, and is
only so that the VBE/Core fallback driver can indicate whether refresh
control is available or not.

The gaHaveDPVLMode flag indicates that the device has support for
VESA DPVL large monitor support built into the driver. See the VESA
DPVL specification for more information.

HEADER:
snap/graphics.h

MEMBERS:
gaIsPanningMode         - Mode is a virtual hardware panning display mode
gaNoRefreshCtrl         - Device has no refresh rate control
gaHaveDPVLMode          - Driver supports VESA DPVL output
gaIsDPVLMode            - Mode is a DPVL virtual mode
****************************************************************************/
typedef enum {
    gaIsPanningMode                 = 0x00000001,
    gaNoRefreshCtrl                 = 0x00000002,
    gaHaveDPVLMode                  = 0x00000004,
    gaIsDPVLMode                    = 0x00000008
    } GA_AttributeExtFlagsType;

/****************************************************************************
REMARKS:
Flags for the WorkArounds member of the GA_devCtx structure. These flags
define conditions for uncommon hardware bugs that can't easily be handled
via the generic SNAP Graphics information reporting mechanism. Any code that
calls the SNAP Graphics hardware drivers directly must be aware of these
workarounds and how to deal with them. However the SNAP Graphics Software Reference
Rasteriser knows how to deal with all currently known bugs, so application
developers should use the reference rasteriser at all times for maximum
compatibility with new hardware drivers.

The gaSlowBltSys flag indicates that the hardware BitBltSys function is
faster than a direct linear framebuffer software blit. Most modern hardware
can do a software blit as fast or faster than using the hardware, but some
hardware can be faster than a pure software blit. This is only true when the
mix mode is GA_REPLACE_MIX, since software reads from the framebuffer over
the PCI bus are terribly slow.

The gaHWCursor32x32 flag indicates that the hardware only supports a 32x32
hardware cursor, while the SNAP Graphics specification implements an interface
for 64x64 hardware cursors. SNAP Graphics drivers will still implement hardware
cursor support for hardware that only supports a 32x32 cursor, however this
flag will be set. If the high level operating system drivers require a
cursor larger than 32x32, then this flag should be checked and a software
cursor used in it's place when this is the case.

The gaSlow24bpp flag indicates that although the 24bpp display modes are
accelerated, they are only partially accelerated. Hence if there is an
equivalent 32bpp display mode, that mode should be used in preference to
the 24bpp display mode if possible.

The gaSlow32bpp flag indicates that although the 32bpp display modes are
accelerated, they are only partially accelerated. Hence if there is an
equivalent 24bpp display mode, that mode should be used in preference to
the 32bpp display mode if possible.

The gaBrokenLines flag indicates that the hardware line drawing produces
slightly different pixels than the software reference rasteriser and
cannot be made to produce correct pixels. For this reason, conformance
testing for line drawing will be skipped on this hardware.

The gaNoDDCBIOS flag is an internal flag to indicate that the card does
not have DDC BIOS support, and hence we should not attempt to use the
DDC BIOS functions to read the EDID for legacy devices.

The gaNoWriteCombine flag is an internal flag to indicate that the graphics
chipset does not work properly when write combining is enabled for
later processors. If write combining is used, it may cause the system
to lock or hang.

The gaNoInterlacedCursor flag is an internal flag to indicate that the
graphics chipset does not properly handle hardware cursors in interlaced
display modes. Hence a software cursor should be used instead for these
modes.

The gaNoLCDSwitching flag is set if the SetOptions function does not
properly implement LCD/CRT switching. This is usually set for situations
where the BIOS is not working properly, and it will be up to the user
to use the function keys on the laptop to do the switching.

The gaNoLCDExpandCursor flag is set if the hardware does not support the
hardware cursor correctly in LCD panel expansion modes. Hence a software
mouse cursor should be used instead.

The gaUsesBIOS flag is set if the driver internally uses the BIOS for
mode sets. This is mostly a flag to let the OS/2 driver know that it does
not need to implement BIOS specific hacks that can slow down mode switching
as the BIOS is already being used internally in the drivers. The BIOS is
generally only used for laptop support and for legacy drivers where there
is not enough information available to work without the BIOS.

The gaLittleEndianFramebuffer flag is set if the hardware framebuffer for
the driver is always little endian. This is important when running on big
endian CPU's, as it means that the software rendering code must do endian
swapping when accessing the video memory. Note that this flag only applies
to 15 and 16 bits per pixel display mode. 24 and 32 bits per pixel modes
are handled internally by swapping the pixel format masks around.

The gaNoUnboundedDrawing flag is set if the hardware cannot handle blits
or other drawing operations that fall partially outside the boundaries of
the destination surface. In this case, all drawing operations must be
properly clipped to fall entirely within the destination surface boundaries.

The gaNoInterlacedVideo flag is an internal flag to indicate that the
graphics chipset does not properly handle video overlays in interlaced
display modes.

HEADER:
snap/graphics.h

MEMBERS:
gaSlowBltSys                - Software is slower than hardware for GA_REPLACE_MODE
gaHWCursor32x32             - The hardware cursor is only 32x32 in size
gaHWCursorBlackBackground   - The hardware cursor requires that the
                              background color always be black (0's)
gaSlow24bpp                 - The 24bpp modes are only partially accelerated
gaSlow32bpp                 - The 32bpp modes are only partially accelerated
gaBrokenLines               - The hardware line drawing is not conformant
gaNoDDCBIOS                 - Card does not have DDC BIOS support
gaNoWriteCombine            - Card does not support write combining
gaNoInterlacedCursor        - HW cursor in interlaced modes is broken
gaHWCursorBlackAndWhite8bpp - The hardware cursor in 8bpp is always black
                              and white and cannot be changed.
gaNoLCDSwitching            - This flag is set if LCD switching does not work
gaNoLCDExpandCursor         - This flag is set to disable cursor in LCD expand modes
gaUsesBIOS                  - Internally this driver uses the BIOS
gaNeedFullBIOS              - This driver needs a full BIOS implementation
                              on OS/2 in order to function
gaNeedContiguousFlipBuffers - Contiguous flip buffers needed for Win32 DirectDraw
gaNeed3DTiledAddressing     - 3D tiled addressing needed for Win32 DirectDraw
gaNoWHQLTransparentBlit     - Chipset does not support WHQL style transparent
                              blits.
gaDDCNeedsGraphicsMode      - Chipset needs graphics mode for DDC operations
gaNo8bppMultiHead           - Controller does not support 8bpp multi-head
gaNo24bppMultiHead          - Controller does not support 24bpp multi-head
gaNoDDCHardware             - Chipset cannot support DDC2 communications
gaLittleEndianFramebuffer   - Framebuffer is little endian
gaNoUnboundedDrawing        - Drawing operations must be strictly clipped
gaNoInterlacedVideo         - Overlays are not available in interlaced modes
****************************************************************************/
typedef enum {
    gaSlowBltSys                    = 0x00000001,
    gaHWCursor32x32                 = 0x00000002,
    gaHWCursorBlackBackground       = 0x00000004,
    gaSlow24bpp                     = 0x00000008,
    gaSlow32bpp                     = 0x00000010,
    gaBrokenLines                   = 0x00000020,
    gaNoDDCBIOS                     = 0x00000040,
    gaNoWriteCombine                = 0x00000080,
    gaNoInterlacedCursor            = 0x00000100,
    gaHWCursorBlackAndWhite8bpp     = 0x00000200,
    gaNoLCDSwitching                = 0x00000400,
    gaNoLCDExpandCursor             = 0x00000800,
    gaUsesBIOS                      = 0x00001000,
    gaNeedFullBIOS                  = 0x00002000,
    gaNeedContiguousFlipBuffers     = 0x00004000,
    gaNeed3DTiledAddressing         = 0x00008000,
    gaNoWHQLTransparentBlit         = 0x00010000,
    gaDDCNeedsGraphicsMode          = 0x00020000,
    gaNo8bppMultiHead               = 0x00040000,
    gaNo24bppMultiHead              = 0x00080000,
    gaNoDDCHardware                 = 0x00100000,
    gaLittleEndianFramebuffer       = 0x00200000,
    gaNoUnboundedDrawing            = 0x00400000,
    gaNoInterlacedVideo             = 0x00800000
    } GA_WorkAroundsFlagsType;

/****************************************************************************
REMARKS:
Flags for the different output displays supported by the driver. These
flags are passed to the SetDisplayOutput function to change the currently
active display device and the GetDisplayDevice function to determine what
devices are currently being used to display output.

NOTE:   The color format for TV modes may be specified, or it may not. In
        the case where the color format is not specified, the currently
        active color format will be used. In some cases the color format
        is set in hardware and cannot be changed.

HEADER:
snap/graphics.h

MEMBERS:
gaOUTPUT_CRT            - Indicates output is sent to CRT display
gaOUTPUT_LCD            - Indicates output is sent to LCD panel
gaOUTPUT_DFP            - Indicates output is sent to external DFP connector
gaOUTPUT_TV             - Indicates output is sent to TV connector
gaOUTPUT_SELECTMASK     - Mask to mask out just the output selection
gaOUTPUT_TVNTSC         - Set TVOut connector color format to NTSC
gaOUTPUT_TVNTSC_J       - Set TVOut connector color format to NTSC-J
gaOUTPUT_TVPAL          - Set TVOut connector color format to PAL
gaOUTPUT_TVPAL_M        - Set TVOut connector color format to PAL-M
gaOUTPUT_TVPAL_60       - Set TVOut connector color format to PAL-60
gaOUTPUT_TVPAL_CN       - Set TVOut connector color format to PAL-CN
gaOUTPUT_TVSCART_PAL    - Set TVOut connector color format to SCART-PAL
gaOUTPUT_TVUNDERSCAN    - Indicates TV output should be underscanned
gaOUTPUT_TVOVERSCAN     - Indicates TV output should be overscanned
gaOUTPUT_TVCOLORMASK    - Mask to mask out TV color format
****************************************************************************/
typedef enum {
    gaOUTPUT_CRT            = 0x0001,
    gaOUTPUT_LCD            = 0x0002,
    gaOUTPUT_DFP            = 0x0400,
    gaOUTPUT_TV             = 0x0004,
    gaOUTPUT_SELECTMASK     = 0x0407,
    gaOUTPUT_TVNTSC         = 0x0008,
    gaOUTPUT_TVNTSC_J       = 0x0010,
    gaOUTPUT_TVPAL          = 0x0020,
    gaOUTPUT_TVPAL_M        = 0x0040,
    gaOUTPUT_TVPAL_60       = 0x0080,
    gaOUTPUT_TVPAL_CN       = 0x0100,
    gaOUTPUT_TVSCART_PAL    = 0x0200,
    gaOUTPUT_TVUNDERSCAN    = 0x0000,
    gaOUTPUT_TVOVERSCAN     = 0x8000,
    gaOUTPUT_TVCOLORMASK    = 0x03F8
    } GA_OutputFlagsType;

/****************************************************************************
REMARKS:
Flags for buffer flags passed to the AllocBuffer function. The flags
define how the buffer is allocated, and the type of buffer.

The gaBufferSysMem flag indicates that the buffer is currently located in
system memory only. It is possible for a buffer that was allocated with the
gaBufferPageable and gaBufferCached flags to initially be in video memory
but then get paged out to system memory to make space for higher priority
buffers. You can also set this flag when you allocate a buffer to cause the
buffer to be allocated in system memory instead of video memory. Also note
that if you request a buffer and do not set the gaBufferNoSysmem flag,
the buffer may end up in system memory if video memory resources have
been exhausted.

The gaBufferCached flag indicates that the buffer should have a system
memory cache allocated for it, so that it can be swapped in and out of
video memory as necessary. Sometimes it may be useful to have buffers
cached in system memory, but not have them pageable. Thus the system
memory cache can be used to refresh the video memory as necessary if the
video memory contents were lost (ie: on a focus switch etc). Note that
the system memory cache is /not/ maintained automatically by SNAP Graphics,
but rather it is up to the application code to maintain the contents of the
system memory cache if they need to be kept in sync. You can use the
UpdateCache and UpdateFromCache functions to keep the system memory cache
in sync as necessary.

The gaBufferMoveable flag indicates that the buffer should be allocated
on the moveable buffer heap, so that the buffer can be moved around as
necessary to compact the heap if it becomes fragmented. For buffers that
should never move in video memory, this flag should not be set and the
buffers will be allocated in the non-moveable or fixed heap.

The gaBufferPageable flag indicates that the buffer is a low priority
buffer and can be paged to system memory in order to make room for higher
priority buffers. Setting gaBufferPageable flag will automatically set the
gaBufferCached flag so that there is a system memory cache for the buffer.
Pageable buffers will be paged back into video memory when the heap becomes
free of all non-pageable buffers. Hence shell drivers using the buffer
manager to cache temporary bitmaps etc should make those bitmaps all pageable,
so that they will get paged to system memory if applications need more offscreen
memory (ie: 2D or 3D graphics intensive apps). When the graphics intensive
app exits, the pageable buffers will get pages back into video memory as all
non-pageable buffers will have been free.

The gaBufferPriority flag indicates that the buffer is a high priority
buffer. As long as there are any high priority buffers still allocated,
the buffer manager will not attempt to page back in pageable buffers from
system memory. Hence DirectDraw or 3D OpenGL application surfaces should be
marked as priority buffers, so that pageable buffers will not be brought back
into video memory until the DirectDraw or OpenGL app exits.

The gaBufferNoSysMem flag is used to indicate that the surface being created
should only ever be allocated in video memory. If there is no video memory
available, the buffer allocation function will fail (normally it will
attempt to allocate the buffer in system memory if there is no video
memory left).

The gaBuffer3D flag is used to indicate that the surface being created
should be capable for being the destination for 3D hardware rendering. If
you need hardware 3D capabilities for the primary and flippable buffers,
you should pass this flag to the InitBuffers function when you initialise
the buffer manager.

The gaBufferMonoLSB flag is used to indicate that the surface being created
should use LSB or Least Significant Bit first bit ordering. This flag is
only relevant for 1 bit per pixel bitmaps. By default the bit order for
1 bit bitmaps is MSB or Most Significant Bit first. When this flag is
set it indicates that the first pixel in a single byte of data is
considered to be in defined by the value in bit 0. When this flag is
not set, the first pixel is considered to be defined by the value in bit 7.
MSB mode is the default used by Windows on x86 platforms and LSB mode is
the default used by the X Window System.

The gaBufferCustom flag is set when the buffer has been allocated with the
AllocCustomBuffer function. AllocCustomBuffer is used to allocate a system
memory buffer but to populate the surface of the buffer with memory provided
by the application. This is useful if you wish to have a Windows DIB section
or other OS specific system bitmap and create a SNAP buffer to wrap around
it and use with the SNAP buffer manager. This flag is primarily an internal
flag used to keep track of who owns the surface memory for the buffer.

The gaBufferFlippable flag is used to indicate whether the buffer is a
flippable buffer that can be viewed and made visible via the
MakeVisibleBuffer function. All flippable buffers must be the same dimensions
as the primary display mode, and are allocated when you first call
the InitBuffers function to initialise the buffer manager.

The gaBufferVideo flag is an internal flag used to indicate that the buffer
is a video overlay window buffer.

The gaBufferDepth flag is an internal flag used to indicate that the buffer
is a hardware depth buffer.

The gaBufferTexture flag is an internal flag used to indicate that the buffer
is a hardware texture map.

The gaBufferStencil flag is an internal flag used to indicate that the buffer
is a hardware stencil buffer.

The gaBufferCommand flag is an internal flag used to indicate that the buffer
is a hardware command buffer.

Note:   These flags are also passed to the AllocBuffer function to
        determine how the buffer should be allocated. Some of the flags
        are internal and should /never/ be passed to AllocBuffer
        as they are used internally. Flags above or equal to gaVideo
        are used internally to indicate what type of buffer is in use
        (since all buffers are internally allocated from the same heap).

HEADER:
snap/graphics.h

MEMBERS:
gaBufferSysMem      - Buffer is currently located in system memory
gaBufferCached      - Buffer is cached in system memory
gaBufferFixed       - Buffer cannot move and comes from fixed heap
gaBufferMoveable    - Buffer can be moved around to compact buffer heap
gaBufferPageable    - Buffer can be paged to system memory
gaBufferPriority    - Buffer is a high priority bitmap
gaBufferNoSysMem    - Buffer should never be in system memory
gaBufferFlippable   - Buffer is a viewable, flippable surface
gaBuffer3D          - Buffer is a hardware 3D capable surface
gaBufferMonoLSB     - Buffer bit ordering is LSB first for 1bpp bitmaps
gaBufferCustom      - Buffer is a custom buffer with passed in surface pointer
gaBufferVideo       - Buffer is a video overlay window surface
gaBufferDepth       - Buffer is a hardware depth buffer
gaBufferTexture     - Buffer is a hardware texture map
gaBufferAlphaPixels - Buffer is a texture buffer with alpha pixels
gaBufferStencil     - Buffer is a hardware stencil buffer
gaBufferCommand     - Buffer is a hardware command buffer
gaBufferSpecial     - Mask to determine if buffer is special buffer
****************************************************************************/
typedef enum {
    gaBufferSysMem                  = 0x00000001,
    gaBufferCached                  = 0x00000002,
    gaBufferFixed                   = 0x00000000,
    gaBufferMoveable                = 0x00000004,
    gaBufferPageable                = 0x00000008,
    gaBufferPriority                = 0x00000010,
    gaBufferNoSysMem                = 0x00000020,
    gaBuffer3D                      = 0x00000040,
    gaBufferMonoLSB                 = 0x00000080,
    gaBufferCustom                  = 0x00000100,
    gaBufferFlippable               = 0x00010000,
    gaBufferVideo                   = 0x00020000,
    gaBufferDepth                   = 0x00040000,
    gaBufferTexture                 = 0x00080000,
    gaBufferAlphaPixels             = 0x00100000,
    gaBufferStencil                 = 0x00200000,
    gaBufferCommand                 = 0x00400000,
    gaBufferSpecial                 = 0x7FFF0000
    } GA_BufferFlagsType;

/****************************************************************************
REMARKS:
Flags for to determine the hardware depth buffer capabilities as reported
in the DepthFormats field of the GA_modeInfo structure, and are only valid
if the gaHaveAccel3D flag is defined in the Attributes member of the
GA_modeInfo structure. If the hardware supports depth buffering, it will
support one or more of the following formats. If the hardware supports
internal depth buffering where the resulting depth values for each pixel
are not stored in memory (ie: for hardware that does scene based rendering),
the gaDepthBufferInternal flag will be set and one of the other flags will
define the precision of the internal depth buffering available.

These flags are also used to specify the depth buffer format when depth
buffers are allocated via AllocDepthBuffer. Attempting to allocate a depth
buffer with a format that is not supported will cause the AllocDepthBufer
function to fail.

HEADER:
snap/graphics.h

MEMBERS:
gaDepthBuffer16         - 16-bit integer depth buffer
gaDepthBuffer24         - 24-bit integer depth buffer
gaDepthBuffer32         - 32-bit integer depth buffer
gaDepthBufferFlt16      - 16-bit floating point depth buffer
gaDepthBufferFlt24      - 24-bit floating point depth buffer
gaDepthBufferFlt32      - 32-bit floating point depth buffer
gaDepthBufferInternal   - Internal depth buffering, values not stored in mem
****************************************************************************/
typedef enum {
    gaDepthBuffer16               = 0x00000001,
    gaDepthBuffer24               = 0x00000002,
    gaDepthBuffer32               = 0x00000004,
    gaDepthBufferFlt16            = 0x00000010,
    gaDepthBufferFlt24            = 0x00000020,
    gaDepthBufferFlt32            = 0x00000040,
    gaDepthBufferInternal         = 0x40000000
    } GA_DepthFormatFlagsType;

/****************************************************************************
REMARKS:
Flags for hardware texture map formats supported by the graphics mode returned
in the TextureFormats member of the GA_modeInfo structure. These flags
define the hardware texture map pixel formats of the particular graphics
mode, and are only valid if the gaHaveAccel3D flag is defined in the
Attributes member of the GA_modeInfo structure.

These flags are also used to specify the texture format when texture
buffers are allocated via AllocTexture. Attempting to allocate a
texture buffer with a format that is not supported will cause the
AllocTexture function to fail.

HEADER:
snap/graphics.h

MEMBERS:
gaTexCI4            - 4-bit color index texture map
gaTexCI8            - 8-bit color index texture map
gaTexAlpha4         - 4-bit alpha only texture map
gaTexAlpha8         - 8-bit alpha only texture map
gaIntensity4        - 4-bit intensity only texture map
gaIntensity8        - 8-bit intensity only texture map
gaTexRGB332         - 3:3:2 RGB texture map
gaTexRGB444         - 4:4:4 RGB texture map
gaTexRGB555         - 5:5:5 RGB texture map
gaTexRGB565         - 5:6:5 RGB texture map
gaTexRGB888         - 8:8:8 RGB texture map
gaTexARGB2222       - 2:2:2:2 ARGB texture map
gaTexARGB4444       - 4:4:4:4 ARGB texture map
gaTexARGB1555       - 1:5:5:5 ARGB texture map
gaTexARGB8888       - 8:8:8:8 ARGB texture map
gaTexCompressed     - Compressed texture map
gaSquareTex         - Textures must be square in dimensions
gaPower2Tex         - Textures dimensions must be a power of two
****************************************************************************/
typedef enum {
    gaTexCI4                      = 0x00000002,
    gaTexCI8                      = 0x00000004,
    gaTexAlpha4                   = 0x00000008,
    gaTexAlpha8                   = 0x00000010,
    gaTexIntensity4               = 0x00000020,
    gaTexIntensity8               = 0x00000040,
    gaTexRGB332                   = 0x00000080,
    gaTexRGB444                   = 0x00000100,
    gaTexRGB555                   = 0x00000200,
    gaTexRGB565                   = 0x00000400,
    gaTexRGB888                   = 0x00000800,
    gaTexARGB2222                 = 0x00001000,
    gaTexARGB4444                 = 0x00002000,
    gaTexARGB1555                 = 0x00004000,
    gaTexARGB8888                 = 0x00008000,
    gaTexCompressed               = 0x00010000,
    gaSquareTex                   = 0x20000000,
    gaPower2Tex                   = 0x40000000
    } GA_TextureFormatFlagsType;

/****************************************************************************
REMARKS:
Flags for hardware stencil buffer formats supported by the graphics mode
returned in the StencilFormats member of the GA_modeInfo structure. These
flags define the hardware stencil buffer formats of the particular graphics
mode, and are only valid if the gaHaveAccel3D flag is defined in the
Attributes member of the GA_modeInfo structure.


These flags are also passed to the AllocStencilBuffer allocate

These flags are also used to specify the stencil buffer format when stencil
buffers are allocated via AllocStencilBuffer. Attempting to allocate a
stencil buffer with a format that is not supported will cause the
AllocStencilBuffer function to fail.

HEADER:
snap/graphics.h

MEMBERS:
gaStencilBuffer1  - 1-bit stencil buffer
gaStencilBuffer2  - 2-bit stencil buffer
gaStencilBuffer4  - 4-bit stencil buffer
gaStencilBuffer8  - 8-bit stencil buffer
****************************************************************************/
typedef enum {
    gaStencilBuffer1                = 0x00000001,
    gaStencilBuffer2                = 0x00000002,
    gaStencilBuffer4                = 0x00000004,
    gaStencilBuffer8                = 0x00000008
    } GA_StencilFormatFlagsType;

/****************************************************************************
REMARKS:
Flags passed to the FlipToBuffer function for the waitVRT parameter.

The gaTripleBuffer flag is used to indicate that the visible buffers should
be flipped using hardware or software triple buffering where available.
This may not be available on all platforms, and if not available gaDontWait
is used instead. Hence you may get tearing using this value if the hardware
or software triple buffering is not supported and the frame rate of your
application is faster than the vertical refresh rate of the display.

The gaWaitVRT flag is used to indicate that the visible buffers should be
flipped and that the code should wait for the vertical retrace period
before returning. This is necessary to avoid any tearing on the screen
if you are doing double buffering, and is the most common value passed
to the MakeVisibleBuffer function.

The gaDontWait flag is used to indicate that the visible buffers should be
flipped but the code should exit immediately and not wait for the vertical
retrace period.

NOTE:   If there are only two flippable buffers allocated, the gaTripleBuffer
        flag will be converted to the gaWaitVRT parameter.

HEADER:
snap/graphics.h

MEMBERS:
gaTripleBuffer  - Flip the buffers with triple buffering if available
gaWaitVRT       - Flip the buffers and wait for vertical retrace
gaDontWait      - Flip the buffers and don't wait for retrace
****************************************************************************/
typedef enum {
    gaTripleBuffer                  = 0,
    gaWaitVRT                       = 1,
    gaDontWait                      = 2
    } GA_FlipToBufferFlagsType;

/****************************************************************************
REMARKS:
Flags for hardware video input formats defined in the VideoInputFormats
member of the GA_videoInf structure. These flags define the hardware
video capabilities of the particular video overlay window, and are only valid
if the gaHaveAccelVideo flag is defined in the Attributes member of the
GA_modeInfo structure.

This is also used for the hardware motion compensation surface types supported
by the hardware as defined in the HwMCSurfaceFormats member of the GA_modeInfo
structure. These flags are also passed to the GetSurfaceInfo function to
retrieve the surface information structure for a specific format. Only one of
these flags should be passed at a time to GetSurfaceInfo.

The gaVideoYUV9 flag is used to determine whether the video overlay window can
support hardware video playback of frames stored in the YUV9 planar format.
Video overlay frames stored in YUV9 format have the Y plane stored 1 byte
per pixel, then immediately following that in video memory is the U or V plane
and then the V or U plane. The ordering is determined by the gaVideoYUV or
gaVideoYVU layout flags. The U and V planes are stored with 1 byte being shared
for a single 16 pixel block so the height of the U and V planes is 1/4 of
the video frame height, and the pitch of the U and V planes is 1/4 of
the pitch for the video frame Y plane.

The gaVideoYUV12 flag is used to determine whether the video overlay window can
support hardware video playback of frames stored in the YUV12 planar format.
Video overlay frames stored in YUV12 format have the Y plane stored 1 byte
per pixel, then immediately following that in video memory is the U or V plane
and then the V or U plane. The ordering is determined by the gaVideoYUV or
gaVideoYVU layout flags. The U and V planes are stored with 1 byte being shared
for a single 4 pixel block so the height of the U and V planes is 1/2 of
the video frame height, and the pitch of the U and V planes is 1/2 of
the pitch for the video frame Y plane.

The gaVideoYUV422 flag is used to determine whether the video overlay window can
support hardware video playback of frames stored in the YUV 4:2:2 packed
pixel format. The YUV 4:2:2 data can be stored with the YUV values in varying
formats, and you should check the gaVideoYUYV etc flags to determine which
formats are supported by this controller. For these modes there are 16-bits
per pixel with two pixels spread across 32-bits of video data. Each pixel
has an independent Y value and shares the U and V values with the two pixels.
Hence there are four different variations where the Y1, Y2, U and V values
can be stored.

The gaVideoMPEG1 value indicates that the hardware supports the MPEG1 motion
compensation surface format.

The gaVideoMPEG2 value indicates that the hardware supports the MPEG2 motion
compensation surface format.

The gaVideoH263 value indicates that the hardware supports the H263 motion
compensation surface format.

The gaVideoMPEG4 value indicates that the hardware supports the MPEG4 motion
compensation surface format.

The gaVideoIDCT value indicates that the hardware supports Inverse Discrete
Cosine Transformations as well as motion compensation for the surface. Some
cards support both motion compensation and IDCT for higher performance
video playback.

The gaVideoSubPicIA44 flag is used to determine whether the IA44 indexed alpha
pixel format is supported by the sub picture rendering engine when using
hardware motion compensation.

The gaVideoSubPicAI44 flag is used to determine whether the AI44 indexed alpha
pixel format is supported by the sub picture rendering engine when using
hardware motion compensation.

The gaVideoYUV and gaVideoYVU flags are used to determine the layout of the
video memory for YUV planar modes. If the gaVideoYUV flag is set, the Y plane
is followed by the U plane  and then the V plane. If this flag is not set,
the Y plane is followed by the V plane and then the U plane. The flag
combination of (gaVideoYUV12 | gaVideoYUV) is equivalent of the I420 FourCC
code.

The gaVideoYUYV, gaVideoYVYU, gaVideoUYVY and gaVideoVYUY flags are used to
determine what YUV pixel layouts are supported for the above supported YUV
packed pixel formats.

Note:   These flags are also passed to the AllocVideoBuffer function to
        determine the video input data type being displayed for the
        video window.

Note:   These flags are also passed to the AllocVideoTexture function to
        determine the video texture format. The RGB video formats are
        ignored by this function and only the YUV formats are accepted.

Note:   The gaVideoYUYV and related flags define the YUV pixel layouts that
        are supported by the hardware for the YUV input format it supports.
        For instance the hardware may report gaVideoYUV422 and the
        gaVideoYUYV flags, which means it supports the YUV422 format with
        the format 4:2:4:2 (Y1:U:Y2:V) in video memory.

HEADER:
snap/graphics.h

MEMBERS:
gaVideoYUV9                 - Supports YUV9 planar input format
gaVideoYUV12                - Supports YUV12 planar input format
gaVideoYUV422               - Supports YUV422 packed pixel input format
gaVideoMPEG1                - Supports hardware MPEG1 video format
gaVideoMPEG2                - Supports hardware MPEG2 video format
gaVideoH263                 - Supports hardware H263 video format
gaVideoMPEG4                - Supports hardware MPEG4 video format
gaVideoIDCT                 - Supports hardware IDCT
gaVideoSubPicIA44           - Supports IA44 sub picture format (HwMC)
gaVideoSubPicAI44           - Supports AI44 sub picture format (HwMC)
gaVideoYUV                  - Supports the YUV planar pixel layout
gaVideoYVU                  - Supports the YVU planar pixel layout
gaVideoYUYV                 - Supports the YUYV packed pixel layout
gaVideoYVYU                 - Supports the YVYU packed pixel layout
gaVideoUYVY                 - Supports the UYVY packed pixel layout
gaVideoVYUY                 - Supports the VYUY packed pixel layout
gaVideoYUVFormatMask        - Mask to check for YUV video formats
gaVideoFormatMask           - Mask to get the video format type
gaVideoHWMCMask             - Mask to get the hardware motion surface types
gaVideoSubPicMask           - Mask to get the sub picture formats
gaVideoYUVLayoutMask        - Mask to get the YUV pixel layout type
****************************************************************************/
typedef enum {
    gaVideoYUV9                     = 0x00000001,
    gaVideoYUV12                    = 0x00000002,
    gaVideoYUV422                   = 0x00000004,
    gaVideoMPEG1                    = 0x00001000,
    gaVideoMPEG2                    = 0x00002000,
    gaVideoH263                     = 0x00004000,
    gaVideoMPEG4                    = 0x00008000,
    gaVideoIDCT                     = 0x00010000,
    gaVideoSubPicIA44               = 0x00100000,
    gaVideoSubPicAI44               = 0x00200000,
    gaVideoYUV                      = 0x04000000,
    gaVideoYVU                      = 0x00000000,
    gaVideoYUYV                     = 0x08000000,
    gaVideoYVYU                     = 0x10000000,
    gaVideoUYVY                     = 0x20000000,
    gaVideoVYUY                     = 0x40000000,
    gaVideoFormatMask               = 0x00000FFF,
    gaVideoHWMCMask                 = 0x000FF000,
    gaVideoSubPicMask               = 0x00F00000,
    gaVideoYUVLayoutMask            = 0x7F000000
    } GA_VideoFormatsType;

/****************************************************************************
REMARKS:
Flags for hardware video output capabilities defined in the VideoOutputFlags
member of the GA_videoInf structure. These flags define the hardware
video capabilities of the particular graphics mode, and are only valid
if the gaHaveAccelVideo flag is defined in the Attributes member of the
GA_modeInfo structure.

The gaVideoXInterp flag is used to determine whether the video overlay
window can support hardware interpolation or filtering in the X axis when
scaling the input image to the display. If this bit is 1, then the hardware
can support filtering of values in the X direction resulting in better
looking images when scaled from a smaller input frame.

The gaVideoYInterp flag is used to determine whether the video overlay
window can support hardware interpolation or filtering in the Y axis when
scaling the input image to the display. If this bit is 1, then the hardware
can support filtering of values in the Y direction resulting in better
looking images when scaled from a smaller input frame.

The gaVideoColorKeyDstSingle flag is used to determine whether the video
overlay window can support hardware destination color keying with a single
destination color key value. When hardware destination color keying is
enabled (sometimes called blue-screening), any destination pixels that the
overlay window overlaps that match the currently set video color key,
will cause the source input pixels to be ignored, essentially
allowing the display data under the video overlay window to show through.

The gaVideoColorKeyDstRange flag is used to determine whether the video
overlay window can support hardware destination color keying with a range of
color key values. This is the same as single destination color keying, but
the color key values may be allows to fall within a range of available
colors.

The gaVideoColorKeyDstIndexed flag is used to determine whether the video
overlay window can support destination color keying over color index
display modes such as 8 bits per pixel. If this flag is not set, the
color keying will only work in 15-bit and higher RGB display modes.

Note:   These flags are also passed to the SetVideoOutput function to
        determine what features are enabled for the output window.

HEADER:
snap/graphics.h

MEMBERS:
gaVideoXInterp              - Supports X interpolation
gaVideoYInterp              - Supports Y interpolation
gaVideoColorKeyDstSingle    - Support destination color keying, single color
gaVideoColorKeyDstRange     - Support destination color keying, range of colors
gaVideoColorKeyDstIndexed   - Support destination color keying for color index modes
****************************************************************************/
typedef enum {
    gaVideoXInterp                  = 0x00000001,
    gaVideoYInterp                  = 0x00000002,
    gaVideoColorKeyDstSingle        = 0x00000010,
    gaVideoColorKeyDstRange         = 0x00000020,
    gaVideoColorKeyDstIndexed       = 0x00000040
    } GA_VideoOutputFlagsType;

/****************************************************************************
REMARKS:
Flags for the Flags field in the GA_HwMCSurfaceInf structure.

The gaHwMC_BackEndSubPicture flag indicates that the sub pictures can
be blended during the back end video display by the hardware. If this
flag is not set, sub pictures have to be blended from the source surface
into a different final surface before being displayed which requires an
additional rendering step.

The gaHwMC_SubPictureIndependentScaling flag indicates that the hardware can
support scaling the sub pictures during the blending process. If this flag
is not set, the source and destination widths must be the same for the
calls to BlendSubPicture and BlendSubPictureBackEnd.

The gaHwMC_IntraUnsigned flag indicates that the motion compensation
level Intra macroblock data should be in an unsigned format rather than the
signed format present in the MPEG stream.  This flag applies only to
motion compensation level acceleration.

The gaHwMC_HaveAltBlockFormat flag indicates that the motion compensation
hardware driver supports the potentially faster alternate block format for
processing macro block data.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_BackEndSubPicture            - Sub pictures are blended in the back end
gaHwMC_SubPictureIndependantScaling - Sub pictures can be scaled in hardware
gaHwMC_IntraUnsigned                - Macro block data is unsigned
gaHwMC_HaveAltBlockFormat           - Driver supports alternate block format
****************************************************************************/
typedef enum {
    gaHwMC_BackEndSubPicture            = 0x00000001,
    gaHwMC_SubPictureIndependentScaling = 0x00000002,
    gaHwMC_IntraUnsigned                = 0x00000004,
    gaHwMC_HaveAltBlockFormat           = 0x00000008
    } GA_HwMCSurfaceFlagsType;

/****************************************************************************
REMARKS:
Flags returned by the GetSurfaceStatus and GetSubPictureStatus functions,
to determine if the surface is still being rendered.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_Rendering    - Hardware surface is still rendering
****************************************************************************/
typedef enum {
    gaHwMC_Rendering                    = 0x00000001
    } GA_HwMCSurfaceStatusFlags;

/****************************************************************************
REMARKS:
Flags passed in the pictureStrucure parameter of the RenderSurface MPEG
acceleration function.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_TopField     - Rendering the top field in a frame
gaHwMC_BottomField  - Rendering the bottom field in a frame
gaHwMC_FramePicture - Rendering a complete frame
****************************************************************************/
typedef enum {
    gaHwMC_TopField                     = 0x00000001,
    gaHwMC_BottomField                  = 0x00000002,
    gaHwMC_FramePicture                 = 0x00000003
    } GA_HwMCPictureStructureFlags;

/****************************************************************************
REMARKS:
Flags passed in the flags parameter of the RenderSurface MPEG acceleration
function.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_SecondField      - This is the second field in an interlaced frame
gaHwMC_AltBlockFormat   - DCT coefficient are passed in alternate format,
                          encoded as series of runs and lengths instead of
                          8x8 matrix
****************************************************************************/
typedef enum {
    gaHwMC_SecondField                  = 0x00000004,
    gaHwMC_AltBlockFormat               = 0x00000008
    } GA_HwMCRenderFlags;

/****************************************************************************
REMARKS:
Flags passed in the MacroBlockType field of the GA_HwMCMacroBlock structure.

The gaHwMC_TypeMotionForward flag indicates that forward motion prediction
should be done.  This flag is ignored for Intra frames.

The gaHwMC_TypeMotionBackward flag indicates that backward motion prediction
should be done.  This flag is ignored when the frame is not bidirectionally
predicted.

The gaHwMC_TypePattern flag indicates that the blocks are referenced and
they contain differentials.  The CodedBlockPattern member of the
GA_HwMCMacroBlock structure will indicate the number of blocks and the
Index member will note their locations in the block array.

The gaHwMC_TypeIntra flag indicates that the blocks are referenced and they
are intra blocks. The CodedBlockPattern member of the GA_HwMCMacroBlock
structure will indicate the number of blocks and the Index member will
note their locations in the block array.

Note:   The gaHwMC_TypePattern and gaHwMC_TypeIntra flags are mutually
        exclusive. If both are specified, gaHwMC_TypeIntra takes precedence.

The gaHwMC_TypeAltZigZagOrder flag indicates that alternate zig zag scan
order is used, as specified by the MPEG2 standard. This flag is only valid if
gaHwMC_AltBlockFormat is in effect.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_TypeMotionForward    - Forward motion prediction (P/B frames only)
gaHwMC_TypeMotionBackward   - Backward motion prediction (B frames only)
gaHwMC_TypePattern          - Blocks are referenced and they contain differentials
gaHwMC_TypeIntra            - Blocks are referenced and they are intra blocks
gaHwMC_TypeAltZigZagOrder   - Alternate zig zag scan order
****************************************************************************/
typedef enum {
    gaHwMC_TypeMotionForward            = 0x02,
    gaHwMC_TypeMotionBackward           = 0x04,
    gaHwMC_TypePattern                  = 0x08,
    gaHwMC_TypeIntra                    = 0x10,
    gaHwMC_TypeAltZigZagOrder           = 0x80
    } GA_HwMCMacroBlockType;

/****************************************************************************
REMARKS:
Flags passed in the MotionType field of the GA_HwMCMacroBlock structure.

If the surface is a field, the following are valid:

    gaHwMC_PredictionField
    gaHwMC_PredictionDualPrime
    gaHwMC_Prediction16x8

If the surface is a frame, the following are valid:

    gaHwMC_PredictionField
    gaHwMC_PredictionFrame
    gaHwMC_PredictionDualPrime

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_PredictionField      - Prediction type is for a field
gaHwMC_PredictionFrame      - Prediction type is for a frame
gaHwMC_PredictionDualPrime  - Prediction type is dual frame
gaHwMC_Prediction16x8       - Prediction type is 16x8
gaHwMC_Prediction4MV        - Prediction type is 4MV
****************************************************************************/
typedef enum {
    gaHwMC_PredictionField              = 0x01,
    gaHwMC_PredictionFrame              = 0x02,
    gaHwMC_PredictionDualPrime          = 0x03,
    gaHwMC_Prediction16x8               = 0x02,
    gaHwMC_Prediction4MV                = 0x04
    } GA_HwMCMotionType;

/****************************************************************************
REMARKS:
Flags passed in the MotionVerticalFieldSelect field of the GA_HwMCMacroBlock
structure. These flags are combined together to determine the vertical
field motion select values.

    If the bit is set the bottom field is indicated.
    If the bit is clear the top field is indicated.

    X X X X D C B A
    ------- | | | |_  First vector forward
       |    | | |___  First vector backward
    unused  | |_____  Second vector forward
            |_______  Second vector backward

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_SelectFirstForward   - Select first vector forward
gaHwMC_SelectFirstBackward  - Select first vector backward
gaHwMC_SelectSecondForward  - Select second vector forward
gaHwMC_SelectSecondBackward - Select second vector backward
****************************************************************************/
typedef enum {
    gaHwMC_SelectFirstForward           = 0x01,
    gaHwMC_SelectFirstBackward          = 0x02,
    gaHwMC_SelectSecondForward          = 0x04,
    gaHwMC_SelectSecondBackward         = 0x08
    } GA_HwMCMotionVerticalType;

/****************************************************************************
REMARKS:
Flags passed in the DCTType field of the GA_HwMCMacroBlock structure. These
flags determine if the frame pictures are frame DCT coded or field DCT coded.

HEADER:
snap/graphics.h

MEMBERS:
gaHwMC_DCTTypeFrame - Frame picture is frame DCT coded
gaHwMC_DCTTypeField - Frame picture is field DCT coded
****************************************************************************/
typedef enum {
    gaHwMC_DCTTypeFrame                 = 0x00,
    gaHwMC_DCTTypeField                 = 0x01
    } GA_HwMCDCTType;

/****************************************************************************
REMARKS:
Flags for hardware line drawing using the bresenham engine line draw
function.

HEADER:
snap/graphics.h

MEMBERS:
gaLineXMajor        - Line is X major (ie: longer in the X direction)
gaLineXPositive     - Direction of line is positive in X
gaLineYPositive     - Direction of line is positive in Y
gaLineDoLastPel     - Draw the last pixel in the line
****************************************************************************/
typedef enum {
    gaLineXMajor                    = 0x00000001,
    gaLineXPositive                 = 0x00000002,
    gaLineYPositive                 = 0x00000004,
    gaLineDoLastPel                 = 0x00000008,
    gaLineDoFirstPel                = 0x00000010
    } GA_BresenhamLineFlagsType;

/* Applications can adjust the bias for the bresenham line drawing
 * functions by applying a small bias to the initial error terms. The bias
 * parameter is an OR'ing of the appropriate octant constants defined
 * below to indicate which octants to bias a line to prefer an axial step
 * when the Bresenham error term is exactly zero. By default SNAP Graphics uses
 * a line bias of 0 for all octants, so you may need to adjust as necessary
 * to compensate for different rasterisation rules. The octants are mapped
 * as follows:
 *
 *   \    |    /
 *    \ 3 | 2 /
 *     \  |  /
 *    4 \ | / 1
 *       \|/
 *   -----------
 *       /|\
 *    5 / | \ 8
 *     /  |  \
 *    / 6 | 7 \
 *   /    |    \
 *
 * For more information, see "Ambiguities in Incremental Line Rastering,"
 * Jack E. Bresenham, IEEE CG&A, May 1987.
 */

#define gaLineOctant1   (1 << (gaLineXPositive | gaLineXMajor))
#define gaLineOctant2   (1 << (gaLineXPositive))
#define gaLineOctant3   (1 << (0))
#define gaLineOctant4   (1 << (gaLineXMajor))
#define gaLineOctant5   (1 << (gaLineYPositive | gaLineXMajor))
#define gaLineOctant6   (1 << (gaLineYPositive))
#define gaLineOctant7   (1 << (gaLineXPositive | gaLineYPositive))
#define gaLineOctant8   (1 << (gaLineXPositive | gaLineYPositive | gaLineXMajor))

#define gaLineOctantMask \
    (gaLineXMajor | gaLineXPositive | gaLineYPositive)

/****************************************************************************
REMARKS:
Flags for hardware blitting with special effects, passed to the BltBltFx
family of functions. This family of functions exposes a wide variety of
special effects blitting if the hardware is capable of these functions.
You can determine what special effects are supported by the hardware by
examining the BitBltCaps member of the GA_modeInfo structure. However to
check whether a set of combined effects are supported, set the desired
effects flags in the GA_bltFx structure and call the BitBltFxTest function.
The driver will examine the passed in flags and return true if the
combination is supported, and false if not. Calling a BltBltFx function with
a combination of flags not supported by the hardware will produce undefined
results.

The gaBltMixEnable flag determines if the graphics mode supports arbitrary
mix modes for extended BitBlt functions.

The gaBltStretchNearest flag determines if the graphics mode supports
hardware stretch blitting, with nearest pixel filtering.

The gaBltStretchXInterp flag determines if the graphics mode supports
hardware stretch blitting, with linear interpolated filtering in the
X direction.

The gaBltStretchYInterp flag determines if the graphics mode supports
hardware stretch blitting, with linear interpolated filtering in the
Y direction.

The gaBltColorKeySrcSingle flag determines whether the graphics mode
supports hardware source transparent blitting with single source color key.
When hardware source color keying is enabled, any pixel data in the incoming
bitmap that matches the currently set color key will be ignored and not
displayed on the screen, essentially making those source pixels transparent.

The gaBltColorKeySrcRange flag determines whether the graphics mode
supports hardware source transparent blitting with a range of color keys.
This is the same as single source color keying, but the color key values may
be allows to fall within a range of available colors (useful if data has
been filtered causing the colors to shift slightly).

The gaBltColorKeyDstSingle flag determines whether the graphics mode
supports hardware destination transparent blitting with single destination color key.
When hardware destination color keying is enabled (sometimes called
blue-screening), any destination pixels in the framebuffer that match the
currently set color key, will cause the source input pixels to be ignored.

The gaBltColorKeyDstRange flag determines whether the graphics mode
supports hardware destination transparent blitting with a range of color keys.
This is the same as single destination color keying, but the color key values
may be allows to fall within a range of available colors (useful if data has
been filtered causing the colors to shift slightly).

The gaBltFlipX flag determines whether the graphics mode supports hardware
blitting with data flipped in the X axis. This is useful for 2D sprite
based games and animation where the same sprite data can be reused for
characters going left or right on the screen by flipping the data during
the blit operation.

The gaBltFlipY flag determines whether the graphics mode supports hardware
blitting with data flipped in the Y axis. This is useful for 2D sprite
based games and animation where the same sprite data can be reused for
characters going up or down on the screen by flipping the data during
the blit operation.

The gaBltBlend flag determines whether the hardware can support alpha
blended blit operations.

The gaBltConvert flag determines whether the hardware can support pixel
format conversion and palette translation.

The gaBltClip flag determines whether the hardware can support clipping
while blitting is in effect. This is usually only used to implement
proper clipping for stretching operations, where software clipping can get
complicated.

The gaBltDither flag determines whether the closest color is selected, or
if dithering is used when blitting an RGB bitmap where the destination is
an 8bpp, 15bpp or 16bpp device context. Dithering slows things down somewhat
for 15/16bpp modes, but produces better quality. Dithering in 8bpp looks
best if a halftone palette is used, and in fact is a lot faster than
using the closest color method. Dithering in 8bpp will however map to any
palette, but the quality is best if a halftone palette is used.

The gaBltTranslateVec flag is used to indicate that the color translation
vector supplied in the TranslateVec member. This is used in situations
where the calling code has already computed a color translation vector
for the source and destination bitmaps, and will then avoid the overhead
of computing the translation vector dynamically for each blit operation.
This is only useful when color converting between 4bpp and 8bpp bitmaps
where the destination is also 4bpp or 8bpp (potentially with a different
palette).

The gaBltMonoTransparent flag is used to indicate that the mono bitmap
should be drawn such that all the background pixels will be transparent.
Background pixels are pixels with a value of 0 and foreground pixels are
pixels with a value of 1. This flag is also passed to the MonoBlt family
of functions.

The gaBltMonoLSB is used to indicate that the least significant
bits in the source pixel values represent the first pixel in the bitmap
(ie: pixel 1 is bit 0). If this flag is not set, it is assumed the first
pixel comes from the most significant bit in the source data byte (ie:
pixel 1 is bit 7). MSB mode is the default used by Windows on x86
platforms and LSB mode is the default used by the X Window System.
This flag is also passed to the MonoBlt family of functions.


Note:   These flags are also passed to the BitBltFx family of functions to
        define the type of extended Blt that should be performed, as well
        as reporting the available capabilities via the GA_bltFx structure
        stored in the GA_modeInfo structure.

Note:   Availability of some features may be mutually exclusive on other
        features. Hence you /must/ call BitBltFxTest first to find out if
        the set of features that you require are all supported at the same
        time before attempting to perform an extended BitBlt operation.

Note:   In many cases stretching with X filtering is relatively cheap,
        while Y filtering is more expensive. Hence it may be faster on
        some hardware to enable only X filtering and not Y filtering to get
        improved performance.

HEADER:
snap/graphics.h

MEMBERS:
gaBltMixEnable          - Mix code enabled, defined in GA_bltFx structure
gaBltStretchNearest     - Enable stretching, nearest pixel
gaBltStretchXInterp     - Enable X axis filtering for stretch blit
gaBltStretchYInterp     - Enable Y axis filtering for stretch blit
gaBltColorKeySrcSingle  - Source color keying enabled, single color
gaBltColorKeySrcRange   - Source color keying enabled, range of colors
gaBltColorKeyDstSingle  - Destination color keying enabled, single color
gaBltColorKeyDstRange   - Destination color keying enabled, range of colors
gaBltFlipX              - Enable flip in X axis
gaBltFlipY              - Enable flip in Y axis
gaBltBlend              - Enable alpha blending
gaBltConvert            - Enable pixel format/palette conversion
gaBltClip               - Clip to destination clip rectangle for stretching
gaBltDither             - Dither if an 8/15/16bpp destination
gaBltTranslateVec       - Color translation vector supplied in TranslateVec
gaBltMonoTransparent    - Background color is transparent for 1bpp bitmaps
gaBltMonoLSB            - Least significant bit comes first in 1bpp bitmaps
****************************************************************************/
typedef enum {
    gaBltMixEnable                  = 0x00000001,
    gaBltStretchNearest             = 0x00000002,
    gaBltStretchXInterp             = 0x00000004,
    gaBltStretchYInterp             = 0x00000008,
    gaBltColorKeySrcSingle          = 0x00000010,
    gaBltColorKeySrcRange           = 0x00000020,
    gaBltColorKeyDstSingle          = 0x00000040,
    gaBltColorKeyDstRange           = 0x00000080,
    gaBltFlipX                      = 0x00000100,
    gaBltFlipY                      = 0x00000200,
    gaBltBlend                      = 0x00000400,
    gaBltConvert                    = 0x00000800,
    gaBltClip                       = 0x00001000,
    gaBltDither                     = 0x00002000,
    gaBltTranslateVec               = 0x00004000,
    gaBltMonoTransparent            = 0x00008000,
    gaBltMonoLSB                    = 0x00010000
    } GA_BitBltFxFlagsType;

/* Convenience macros for BitBltFx flags */

#define gaBltAnyStretch         (gaBltStretchNearest | gaBltStretchXInterp | gaBltStretchYInterp)
#define gaBltAnyColorKey        (gaBltColorKeySrcSingle | gaBltColorKeySrcRange | gaBltColorKeyDstSingle | gaBltColorKeyDstRange)
#define gaBltAnySingleColorKey  (gaBltColorKeySrcSingle | gaBltColorKeyDstSingle)
#define gaBltAnyRangeColorKey   (gaBltColorKeySrcRange | gaBltColorKeyDstRange)

/****************************************************************************
REMARKS:
Definitions for flags member of the GA_CRTCInfo structure. These flags
define the different flags required to complete a mode set with refresh
rate control enabled.

The gaInterlaced flag is used to determine whether the mode programmed into
the hardware is interlaced or non-interlaced. The CRTC timings passed in
will be identical for both interlaced and non-interlaced modes, and it is up
to the graphics driver to perform any necessary scaling between the hardware
values and the normalized CRTC values in interlaced modes. Note that you must
check the gaHaveInterlaced bit in the GA_modeInfo structure to determine if
interlaced mode is supported before attempting to initialise an interlaced
mode.

The gaDoubleScan flag is used to determine whether the mode programmed into
the hardware is double scanned or not. When double scanning is specified,
the vertical CRTC values passed in will be double what the real vertical
resolution will be. Double scanning is used to implement the 200, 240 and
300 line graphics modes on modern controllers. Note that you must check the
gaHaveDoubleScan bit in the GA_modeInfo structure to determine if double scan
mode is supported by the hardware in that display mode. Note also that all
modes with vertical resolutions below 300 scanline modes require the double
scanning to be enabled, and modes between 300 and 400 scanlines can usually
look better if it is enabled.

The gaHSyncNeg flag is used to determine if the horizontal sync polarity
should be set to a negative sync (gaHSyncNeg is set) or positive sync
(gaHSyncNeg is not set).

The gaVSyncNeg flag is used to determine if the vertical sync polarity
should be set to a negative sync (gaVSyncNeg is set) or positive sync
(gaVSyncNeg is not set).

HEADER:
snap/graphics.h

MEMBERS:
gaInterlaced    - Enable interlaced mode
gaDoubleScan    - Enable double scanned mode
gaTripleScan    - Enable triple scanned mode
gaHSyncNeg      - Horizontal sync is negative
gaVSyncNeg      - Vertical sync is negative
****************************************************************************/
typedef enum {
    gaInterlaced    = 0x01,
    gaDoubleScan    = 0x02,
    gaTripleScan    = 0x04,
    gaHSyncNeg      = 0x08,
    gaVSyncNeg      = 0x10
    } GA_CRTCInfoFlagsType;

/* Flags for common sync polarity combinations */

#define ga350Line       gaSyncVNeg
#define ga400Line       gaSyncHNeg
#define ga480Line       (gaSyncHNeg | gaSyncVNeg)

/****************************************************************************
REMARKS:
Definitions for values stored in the bLCDUseBIOS member of the
GA_globalOptions structure. These flags define the different values for this
particular option which is multi-state and not just an on/off option.

The gaLCDUseBIOS_Off value indicates that the BIOS should not be used
for LCD panel modes unless the user has manually switched the driver to
run in LCD only or Simultaneous mode. In this mode some laptops will not
be able to switch into LCD mode from CRT only mode if the user uses the
laptop hot-key switching (the SNAP Graphics API can always be used for correct
switching).

The gaLCDUseBIOS_Auto value will auto select the best option at driver load
time. If the system boots up in either LCD only mode or in Simultaneous
mode, the driver will always use the BIOS even if the user switches to CRT
only mode (ie: equivalent to gaLCDUseBIOS_On). If the system boot up
in CRT only mode, the driver will not use the BIOS unless the user manually
switches to LCD panel mode using the SNAP Graphics API (ie: equivalent to
gaLCDUseBIOS_Off).

gaLCDUseBIOS_On value indicates that the BIOS should always be used
for drivers with LCD panel support. This means that even when the user uses
the SNAP Graphics API to switch to CRT only mode, the BIOS will be used to
set the mode (with restricted modes and features). This mode is useful if
the user switches to and from LCD panel and CRT only modes and needs the
hot key switching provided in the BIOS to function correctly.

HEADER:
snap/graphics.h

MEMBERS:
gaLCDUseBIOS_Auto   - Auto select the best option at driver load time
gaLCDUseBIOS_Off    - Only use the BIOS modes when running on the LCD
gaLCDUseBIOS_On     - Always use the BIOS for drivers with LCD panel support
****************************************************************************/
typedef enum {
    gaLCDUseBIOS_Auto   = 0,
    gaLCDUseBIOS_Off    = 1,
    gaLCDUseBIOS_On     = 2
    } GA_LCDUseBIOSFlagsType;

/****************************************************************************
REMARKS:
Definitions for values stored in the accelType member of the GA_Options
structure. These flags define the different values for this particular
option which is multi-state and not just an on/off option. This option is
designed to be used as a fallback measure in the field to disable
problem functions in a driver that an end user might be experiencing.

The gaAccelType_Custom value indicates that the user has manually overridden
specific hardware acceleration options rather than using the four pre-defined
settings.

The gaAccelType_Full value indicates that full hardware acceleration should
be used, and is always the default option for all drivers.

The gaAccelType_Most value indicates that most acceleration functions should
be used in the driver. This basically disables support for hardware mouse
cursor and hardware video overlays functionality.

The gaAccelType_Basic value indicates that only basic acceleration functions
should be used in the driver. Basic acceleration includes solid fills,
mono and color pattern fills and screen to screen blits. All other functions
are disabled.

gaAccelType_None value indicates that no hardware acceleration functions
should be used in the driver.

HEADER:
snap/graphics.h

MEMBERS:
gaAccelType_Custom  - Custom hardware acceleration
gaAccelType_Full    - Full hardware acceleration (default)
gaAccelType_Most    - Most hardware acceleration
gaAccelType_Basic   - Basic hardware acceleration
gaAccelType_None    - No hardware acceleration
****************************************************************************/
typedef enum {
    gaAccelType_Custom  = 0,
    gaAccelType_Full    = 1,
    gaAccelType_Most    = 2,
    gaAccelType_Basic   = 3,
    gaAccelType_None    = 4
    } GA_AccelFlagsType;

/****************************************************************************
REMARKS:
Definitions for values stored in the CertifyFlags member of the
GA_certifyChipInfo and structure. These flags define what certification tests
have been run on the included drivers.

The gaCertified value indicates that the driver has been certified to have
passed all the base certification tests, and is usually the only important
flag for most situations.

The gaCertifiedDDC value indicates that the driver has also passed all
Windows DDC certification tests, running on the IHV Windows drivers. These
tests are independent of the base certification tests as they are dependant
on proper testing with the IHV Windows drivers as well as the base
certification tests. The gaCertifiedDDC_NT flag is equivalent but
indicates that the chipset driver is also certified for DDC on the
Windows NT platform.

The gaCertifiedStereo value indicates that the driver has also passed all
Windows Stereo certification tests, running on the IHV Windows drivers. These
tests are independent of the base certification tests as they are dependant
on proper testing with the IHV Windows drivers as well as the base
certification tests. The gaCertifiedStereo_NT flag is equivalent but
indicates that the chipset driver is also certified for stereo on the
Windows NT platform.

The gaCertifiedDFPOutput value indicates that the driver has also passed all
the external digital flat panel tests such that attaching an external
digital flat panel to the graphics card will function correctly.

The gaCertifiedTVOutput value indicates that the driver has also passed all
the external TV output tests such that attaching an external television
to either the composite or S-Video connectors of the graphics card
will function correctly.

HEADER:
snap/graphics.h

MEMBERS:
gaCertified             - Driver has passed all base certification tests
gaCertifiedDDC          - Driver has passed all Windows DDC tests
gaCertifiedStereo       - Driver has passed all Windows stereo tests
gaCertifiedDDC_NT       - Driver has passed all Windows NT DDC tests
gaCertifiedStereo_NT    - Driver has passed all Windows NT stereo tests
****************************************************************************/
typedef enum {
    gaCertified             = 0x01,
    gaCertifiedDDC          = 0x02,
    gaCertifiedStereo       = 0x04,
    gaCertifiedDDC_NT       = 0x08,
    gaCertifiedStereo_NT    = 0x10
    } GA_CertifyFlagsType;

/****************************************************************************
REMARKS:
This enumeration defines the flags stored in the bMultiDisplay field of
the GA_options structure.

HEADER:
snap/graphics.h

MEMBERS:
gaActiveHeadVirtual     - Used to query mode flags for virtual modes (which use
                          all heads at the same time)
gaActiveHeadClone       - Active head is cloned across all displays (default)
gaActiveHeadPrimary     - Active head is primary display
gaActiveHeadSecondary   - Active head is secondary display
gaActiveHeadTernary     - Active head is third display
****************************************************************************/
typedef enum {
    gaActiveHeadVirtual     = -2,
    gaActiveHeadClone       = -1,
    gaActiveHeadPrimary     = 0,
    gaActiveHeadSecondary   = 1,
    gaActiveHeadTernary     = 2
    } GA_multiHeadType;

/****************************************************************************
REMARKS:
This enumeration defines the identifiers passed to the SetVideoInput video
capture function.

HEADER:
snap/graphics.h

MEMBERS:
gaCaptureCompositeInput - Input comes from Composite video connector
gaCaptureSVideoInput    - Input comes from S-Video connector
gaCaptureRGBInput       - Input comes from RGB input connector
gaCaptureRS170Input     - Input comes from RS170 input connector
****************************************************************************/
typedef enum {
    gaCaptureCompositeInput = 0x00000001,
    gaCaptureSVideoInput    = 0x00000002,
    gaCaptureRGBInput       = 0x00000004,
    gaCaptureRS170Input     = 0x00000008
    } GA_captureInputTypes;

/****************************************************************************
REMARKS:
This enumeration defines the identifiers passed to the SetVideoInput video
capture function.

HEADER:
snap/graphics.h

MEMBERS:
gaCaptureStandardNTSC   - Capture video in NTSC input format
gaCaptureStandardPAL    - Capture video in PAL input format
gaCaptureStandardSECAM  - Capture video in SECAM input format
****************************************************************************/
typedef enum {
    gaCaptureStandardNTSC   = 0x00000001,
    gaCaptureStandardPAL    = 0x00000002,
    gaCaptureStandardSECAM  = 0x00000004
    } GA_captureStandardTypes;

/****************************************************************************
REMARKS:
This enumeration defines the identifiers passed to the FreezeLiveVideo and
WaitForField video capture functions.

HEADER:
snap/graphics.h

MEMBERS:
gaCaptureFieldAny   - Indicates the start of any field
gaCaptureFieldOdd   - Indicates the start of the next odd field
gaCaptureFieldEven  - Indicates the start of the next even field
****************************************************************************/
typedef enum {
    gaCaptureFieldAny,
    gaCaptureFieldOdd,
    gaCaptureFieldEven
    } GA_captureFieldTypes;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 monochrome
pattern. Each line in the pattern is represented as a single byte, with 8
bytes in total for the entire pattern.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8 bytes of pattern data
****************************************************************************/
typedef struct {
    N_uint8     p[8];
    } GA_pattern;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 1bpp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8 bytes of pattern data
****************************************************************************/
typedef struct {
    N_uint8     p[8];
    } GA_colorPattern_1;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 4bpp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8 bytes of pattern data
****************************************************************************/
typedef struct {
    N_uint8     p[8][4];
    } GA_colorPattern_4;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 8bpp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8 bytes of pattern data
****************************************************************************/
typedef struct {
    N_uint8     p[8][8];
    } GA_colorPattern_8;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 16bpp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8 words of pattern data
****************************************************************************/
typedef struct {
    N_uint16    p[8][8];
    } GA_colorPattern_16;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 24bpp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8x3 bytes of pattern data
****************************************************************************/
typedef struct {
    N_uint8     p[8][8][3];
    } GA_colorPattern_24;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern data for 32pp modes.

HEADER:
snap/graphics.h

MEMBERS:
p   - 8x8 dwords of pattern data
****************************************************************************/
typedef struct {
    N_uint32    p[8][8];
    } GA_colorPattern_32;

/****************************************************************************
REMARKS:
Fundamental type definition for an array element of an 8x8 color bitmap
pattern. Each line in the pattern is represented as an array of packed
pixel data. In 8bpp modes there is 8 bytes per line, for 16bpp modes
there are 16bytes per line, for 24bpp modes there are 24bytes per line
and for 32bpp modes there are 32 bytes per line. Hence the size of the
pattern data is different depending on the color depth currently active.

HEADER:
snap/graphics.h
****************************************************************************/
typedef union {
    GA_colorPattern_1   b1;
    GA_colorPattern_4   b4;
    GA_colorPattern_8   b8;
    GA_colorPattern_16  b16;
    GA_colorPattern_24  b24;
    GA_colorPattern_32  b32;
    } GA_colorPattern;

/****************************************************************************
REMARKS:
Fundamental type definition for a 16-bit line stipple pattern. Note that
we define it as a 32-bit value so it will be passed as a 32-bit argument
on the stack correctly when calling 32-bit code from a 16-bit segment.

HEADER:
snap/graphics.h
****************************************************************************/
typedef N_uint32 GA_stipple;

/****************************************************************************
REMARKS:
Fundamental type definition for a 32-bit color value. The color value is
interpreted differently depending on what graphics mode the system is in,
and in 15-bit and above modes will have the color values packed according
to the pixel format information stored in the GA_modeInfo structure.

HEADER:
snap/graphics.h
****************************************************************************/
typedef N_uint32 GA_color;

/****************************************************************************
REMARKS:
Fundamental type definition for an integer rectangle. Note that SNAP Graphics
defines and uses rectangles such that the bottom and right coordinates are
not actually included in the pixels that define a raster coordinate
rectangle. This allows for correct handling of overlapping rectangles
without drawing any pixels twice.

HEADER:
snap/graphics.h

MEMBERS:
left    - Left coordinate of the rectangle
top     - Top coordinate of the rectangle
right   - Right coordinate of the rectangle
bottom  - Bottom coordinate of the rectangle
****************************************************************************/
typedef struct {
    N_int32 left;
    N_int32 top;
    N_int32 right;
    N_int32 bottom;
    } GA_rect;

/****************************************************************************
REMARKS:
Fundamental type definition representing a segment within a span that forms
a complex region. The segments define the X coordinates of the segments
that make up the span. Segments are always in groups of two (start and end
segment).

HEADER:
snap/graphics.h

MEMBERS:
next    - Next segment in span
x       - X coordinates of this segment
****************************************************************************/
typedef struct GA_segment {
    struct GA_segment   *next;
    N_int32             x;
    } GA_segment;

/****************************************************************************
REMARKS:
Fundamental type definition representing a span within a complex region. A
span is represented as a list of segments that are included in the span.

HEADER:
snap/graphics.h

MEMBERS:
next    - Next span in region
seg     - Index of first segment in span
y       - Y coordinate of this span
****************************************************************************/
typedef struct GA_span {
    struct GA_span  *next;
    GA_segment      *seg;
    N_int32         y;
    } GA_span;

/****************************************************************************
REMARKS:
Fundamental type definition representing a complex region. Complex regions
are used to represent non-rectangular areas as unions of smaller rectangles
(the smallest being a single pixel). You can use complex regions to build
complex clipping regions for user interface library development.

If the Spans field for the region is NULL, then the region is a simple
region and is composed of only a single rectangle. Note however that you
can have a simple region that consists of only single rectangle in the
span structure (usually after complex region arithmetic). You can use the
GA_IsSimpleRegion macro to determine if the region contains only a single
rectangle.

HEADER:
snap/graphics.h

MEMBERS:
Rect    - Bounding rectangle for the region
spans   - Pointer to the internal region span structure.
****************************************************************************/
typedef struct {
    GA_rect rect;
    GA_span *spans;
    } GA_region;

/****************************************************************************
REMARKS:
Hardware video window information. There is a single structure for each
available hardware video window, and defines the capabilities of that
hardware video window.

The VideoInputFormats member contains a set of flags that define the
format of video input data that the overlay window can accept.

The VideoOutputFlags member contains flags that define the capabilities
of the video output window.

The VideoMaxXSource and VideoMaxYSource members define the maximum dimensions
of the source video input data that the video window can support.

The VideoMinXScale and VideoMinYScale members defines the inverse of minimum
scale ratio supported by the hardware in the each direction. For instance
if the value is 4, the hardware can only scale video down to a window that
is 1/4 the size of the source input video. The VideoMaxXScale and
VideoMaxYScale define the maximum scale ratio supported by the hardware.
For instance if the value is 4, the hardware can only scale video data up
to a window that 4 times the size of the source input video.

The VideoXXMin, VideoXXDefault and VideoXXMax values for Brightness, Contrast,
Saturation and Hue are used to define the range of values that the functions
may accept. These values may be negative, so a range could be set to -1000
to 1000 for a particular hardware device. The hardware device will set these
values to settings that will match the hardware granularity. The default
value is the recommended default setting to program into the hardware, and
also represents the value that will be set when the video overlay window
is first initialised.

HEADER:
snap/graphics.h

MEMBERS:
dwSize                  - Set to size of structure in bytes
VideoInputFormats       - Hardware video input format flags
VideoOutputFlags        - Hardware video output format capabilities
VideoMaxXSource         - Maximum X dimension for source video data
VideoMaxYSource         - Maximum Y dimension for source video data
VideoMinXScale          - Minimum X scale factor (1/value)
VideoMinYScale          - Minimum Y scale factor (1/value)
VideoMaxXScale          - Maximum X scale factor
VideoMaxYScale          - Maximum Y scale factor
VideoBrightnessMin      - Minimum brightness range value
VideoBrightnessDefault  - Brightness recommend default setting
VideoBrightnessMax      - Maximum brightness range value
VideoContrastMin        - Minimum contrast range value
VideoContrastDefault    - Contrast recommend default setting
VideoContrastMax        - Maximum contrast range value
VideoSaturationMin      - Minimum saturation range value
VideoSaturationDefault  - Saturation recommend default setting
VideoSaturationMax      - Maximum saturation range value
VideoHueMin             - Minimum hue range value
VideoHueDefault         - Hue recommend default setting
VideoHueMax             - Maximum hue range value
****************************************************************************/
typedef struct {
    N_uint32    dwSize;
    N_uint32    VideoInputFormats;
    N_uint32    VideoOutputFlags;
    N_uint16    VideoMaxXSource;
    N_uint16    VideoMaxYSource;
    N_uint16    VideoMinXScale;
    N_uint16    VideoMinYScale;
    N_uint16    VideoMaxXScale;
    N_uint16    VideoMaxYScale;
    N_int32     VideoBrightnessMin;
    N_int32     VideoBrightnessDefault;
    N_int32     VideoBrightnessMax;
    N_int32     VideoContrastMin;
    N_int32     VideoContrastDefault;
    N_int32     VideoContrastMax;
    N_int32     VideoSaturationMin;
    N_int32     VideoSaturationDefault;
    N_int32     VideoSaturationMax;
    N_int32     VideoHueMin;
    N_int32     VideoHueDefault;
    N_int32     VideoHueMax;
    } GA_videoInf;

/****************************************************************************
REMARKS:
Structure to describe the surface format information for a particular
hardware motion compensation format.

The VideoFormat member describes the format of the video surfaces that
will be created by the hardware motion compensation engine for this surface
for (defined by the flags in GA_VideoFormatsType). It may also have the
gaVideoIDCT flag enabled if the surface also supports hardware inverse
discrete cosine transformations.

The MaxWidth member determines the maximum width of the video input stream that
can be supported by the hardware motion compensation engine for this surface
format.

The MaxHeight member determines the maximum height of the video input stream
that can be supported by the hardware motion compensation engine for this
surface format.

The SubPictureMaxWidth member determines the maximum width of sub-pictures
supported by the motion compensation engine for this surface format.

The SubPictureMaxHeight member determines the maximum height of sub-pictures
supported by the motion compensation engine for this surface format.

The Flags member hold extra flags that define features of this particular
surface format, as defined by GA_HwMCSurfaceFlagsType.

HEADER:
snap/graphics.h

MEMBERS:
dwSize              - Set to size of structure in bytes
VideoFormat         - Video format as defined by GA_VideoFormatsType
MaxWidth            - Maximum video stream frame width
MaxHeight           - Maximum video stream frame height
SubPictureMaxWidth  - Maximum sub picture frame width
SubPictureMaxHeight - Maximum sub picture frame height
Flags               - Capability flags for the surface (GA_HwMCSurfaceFlagsType)
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint32        VideoFormat;
    N_int32         MaxWidth;
    N_int32         MaxHeight;
    N_int32         SubPictureMaxWidth;
    N_int32         SubPictureMaxHeight;
    N_uint32        Flags;
    } GA_HwMCSurfaceInf;

/****************************************************************************
REMARKS:
Structure for a single hardware macro block.

The X and Y member determine the location of the macroblock on the surface
in macro block units.

The MacroBlockType member defines the type of macro block and is a
combination of GA_HwMCMacroBlockType flags.

The MotionType member defines the type of motion for the macro block and is
a combination of GA_HwMCMotionType flags.

The MotionVerticalFieldSelect member is a combination of
GA_HwMCMotionVerticalType flags.

The DCTType member indicates whether frame pictures are frame DCT coded or
field DCT coded. ie gaHwMC_DCTTypeFrame or gaHwMC_DCTTypeField.

The PMV member holds the motion vectors for this macro block:

    PMV[c][b][a]

        a - This holds the vector. 0 = horizontal, 1 = vertical.
        b - 0 = forward, 1 = backward.
        c - 0 = first vector, 1 = second vector.

The motion vectors are used only when gaHwMC_TypeMotionForward or
gaHwMC_TypeMotionBackward are set in the MotionType member.

DualPrime vectors must be fully decoded and placed in the PMV array as
follows:

    Field structure:

        PMV[0][0][1:0]  from same parity
        PMV[0][1][1:0]  from opposite parity

    Frame structure:

        PMV[0][0][1:0]  top from top
        PMV[0][1][1:0]  bottom from bottom
        PMV[1][0][1:0]  top from bottom
        PMV[1][1][1:0]  bottom from top

The Index member is the offset in units of (64 * sizeof(N_int16)) from the
start of the block array where this macroblock's DCT blocks, as indicated
by the CodedBlockPattern member are stored.

The CodedBlockPattern member indicates the blocks to be updated. The
bitplanes are specific to the type of the MPEG surface. This field is
valid only if gaHwMC_TypePattern or gaHwMC_TypeIntra are flags are set in the
MacroBlockType member. In that case the blocks are differential or intra
blocks respectively. The bitplanes are described in ISO/IEC 13818-2
Figures 6.10-12.

Note:   This structure is laid out specifically to match the XvMC
        structure layout, so it should not be changed.

HEADER:
snap/graphics.h

MEMBERS:
X                           - X location of the macroblock on the surface in macroblock units
Y                           - Y location of the macroblock on the surface in macroblock units
MacroBlockType              - Combination of GA_HwMCMacroBlockType flags
MotionType                  - Combination of GA_HwMCMotionType flags
MotionVerticalFieldSelect   - Combination of GA_HwMCMotionVerticalType flags
DCTType                     - Indicates whether frame DCT or field DCT coded
PMV                         - The motion vector(s)
Index                       - Start of gaHwMC_TypePattern and gaHwMC_TypeIntra blocks
CodedBlockPattern           - Number of blocks for gaHwMC_TypePattern and gaHwMC_TypeIntra blocks
Pad0                        - Padding - unused
****************************************************************************/
typedef struct {
    ushort      X;
    ushort      Y;
    uchar       MacroBlockType;
    uchar       MotionType;
    uchar       MotionVerticalFieldSelect;
    uchar       DCTType;
    short       PMV[2][2][2];
    uint        Index;
    ushort      CodedBlockPattern;
    ushort      Pad0;
    } GA_HwMCMacroBlock;

/****************************************************************************
REMARKS:
Structure to describe the video capture input formats supported by the
underlying hardware, and the number of those inputs supported.

The VideoFormat member describes the format of the video surfaces that
will be created by the hardware capture engine for this input type
(defined by the flags in GA_VideoFormatsType).

The MaxWidth member determines the maximum width of the video input stream
that can be supported by the video capture engine for this format.

The MaxHeight member determines the maximum height of the video input stream
that can be supported by the video capture engine for this format.

The InputType member defines the type of input that this format describes, as
defined by GA_captureInputTypes, such as Composite, S-Video or component RGB
etc. This is a bit field that can be a combination of all input types
supported.

The InputStandard member defines the type of input standard that this format
supports, as defined by GA_captureStandardTypes, such as PAL, NTSC or both.
This is a bit field that can be a combination of all input standards
supported.

The MaxInputs member defines the number of inputs that can be selected for
this type of input. Some cards may support selecting between multiple video
inputs to support multiple cameras or input devices.

HEADER:
snap/graphics.h

MEMBERS:
dwSize          - Set to size of structure in bytes
VideoFormat     - Video format as defined by GA_VideoFormatsType
MaxWidth        - Maximum video stream frame width
MaxHeight       - Maximum video stream frame height
InputType       - Video capture input type (GA_captureInputTypes)
InputStandard   - Video capture input standard (GA_captureStandardTypes)
MaxInputs       - Maximum number of inputs supported by card
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint32        VideoFormat;
    N_int32         MaxWidth;
    N_int32         MaxHeight;
    N_uint32        InputType;
    N_uint32        InputStandard;
    N_int32         MaxInputs;
    } GA_VideoCaptureInf;

/****************************************************************************
REMARKS:
Palette entry structure, which defines a single entry in the hardware
color lookup table or gamma correction table.

HEADER:
snap/graphics.h

MEMBERS:
blue    - Blue component of palette entry, range [0-255]
green   - Green component of palette entry, range [0-255]
red     - Blue component of palette entry, range [0-255]
alpha   - Alpha or alignment byte
****************************************************************************/
typedef struct {
    N_uint8     Blue;
    N_uint8     Green;
    N_uint8     Red;
    N_uint8     Alpha;
    } GA_palette;

/****************************************************************************
REMARKS:
Extended palette entry structure, which defines a single entry in the
hardware color lookup table or gamma correction table.

HEADER:
snap/graphics.h

MEMBERS:
blue    - Blue component of palette entry, range [0-65535]
green   - Green component of palette entry, range [0-65535]
red     - Blue component of palette entry, range [0-65535]
****************************************************************************/
typedef struct {
    N_uint16    Blue;
    N_uint16    Green;
    N_uint16    Red;
    } GA_paletteExt;

/****************************************************************************
REMARKS:
Structure representing the format of an RGB pixel. This structure is used
to describe the RGB pixel format for SNAP graphics modes, as well as the
pixel format for system memory buffers converted on the fly by SNAP Graphics to
the destination pixel format. RGB pixel formats are required for pixel
depths greater than or equal to 15-bits per pixel. The pixel formats for 15
and 16-bit modes are constant and never change, however there are 2 possible
pixel formats for 24 bit RGB modes and 4 possible formats for 32 bit RGB
modes that are supported by the MGL. The possible modes for 24-bits per
pixel are:

24-bit  - Description
RGB     - Values are packed with Red in byte 2, Green in byte 1 and Blue
          in byte 0. This is the standard format used by all 24 bit
          Windows BMP files, and the native display format for most
          graphics hardware on the PC.
BGR     - Values are packed with Blue in byte 2, Green in byte 1 and Red
          in byte 0. This format is the native display format for some
          graphics hardware on the PC.

The possible modes for 32-bits per pixel are:

32-bit  - Description
ARGB    - Values are packed with Red in byte 2, Green in byte 1 and Blue
          in byte 0 and alpha in byte 3.
ABGR    - Values are packed with Blue in byte 2, Green in byte 1 and Red
          in byte 0 and alpha in byte 3.
RGBA    - Values are packed with Red in byte 3, Green in byte 2 and Blue
          in byte 1 and alpha in byte 0.
BGRA    - Values are packed with Blue in byte 3, Green in byte 2 and Red
          in byte 1 and alpha in byte 0.

If you intend to write your own direct rendering code for RGB graphics modes,
you will need to write your code so that it will adapt to the underlying
pixel format used by the hardware to display the correct colors on the
screen. SNAP Graphics has the ability to perform pixel format translation on the
fly using the ConvertBlt family of functions, but this can be time consuming
so directly rendering in the native pixel format can be more efficient.
The formula for packing the pixel data into the proper positions given three
8-bit RGB values is as follows:

    color = ((GA_color)((R >> RedAdjust) & RedMask)
            << RedPosition)
          | ((GA_color)((G >> GreenAdjust) & GreenMask)
            << GreenPosition)
          | ((GA_color)((B >> BlueAdjust) & BlueMask)
            << BluePosition);

Alternatively you can unpack the color values from the framebuffer with the
following code (note that you lose precision when unpacking values from the
framebuffer since the bottom bits always get set to 0):

    R = (((color) >> RedPosition) & RedMask)
        << RedAdjust;
    G = (((color) >> GreenPosition) & GreenMask)
        << GreenAdjust;
    B = (((color) >> BluePosition) & BlueMask)
        << BlueAdjust;

If you wish to create your own pixel formats (such as to create memory
custom bitmaps), the following list defines all the pixel formats that the
SNAP Graphics knows how to deal with:

    {0x1F,0x0A,3, 0x1F,0x05,3, 0x1F,0x00,3, 0x01,0x0F,7}, // 555 15bpp
    {0x1F,0x0B,3, 0x3F,0x05,2, 0x1F,0x00,3, 0x00,0x00,0}, // 565 16bpp
    {0xFF,0x10,0, 0xFF,0x08,0, 0xFF,0x00,0, 0x00,0x00,0}, // RGB 24bpp
    {0xFF,0x00,0, 0xFF,0x08,0, 0xFF,0x10,0, 0x00,0x00,0}, // BGR 24bpp
    {0xFF,0x10,0, 0xFF,0x08,0, 0xFF,0x00,0, 0xFF,0x18,0}, // ARGB 32bpp
    {0xFF,0x00,0, 0xFF,0x08,0, 0xFF,0x10,0, 0xFF,0x18,0}, // ABGR 32bpp
    {0xFF,0x18,0, 0xFF,0x10,0, 0xFF,0x08,0, 0xFF,0x00,0}, // RGBA 32bpp
    {0xFF,0x08,0, 0xFF,0x10,0, 0xFF,0x18,0, 0xFF,0x00,0}, // BGRA 32bpp

One special cased pixel format is used to represent and 8bpp color index
bitmap with an alpha channel. This pixel format is a 16-bit wide pixel
format, but the red channel is considered to contain the 8-bit color
index values. The pixel format structure for this type of bitmap looks
like the following:

    {0xFF,0x00,0, 0x00,0x00,0, 0x00,0x00,0, 0xFF,0x08,0}, // A8CI 8bpp + Alpha

HEADER:
snap/graphics.h

MEMBERS:
RedMask         - Unshifted 8-bit mask for the red color channel
RedPosition     - Bit position for bit 0 of the red color channel information
RedAdjust       - Number of bits to shift the 8-bit red value right
GreenMask       - Unshifted 8-bit mask for the green color channel
GreenPosition   - Bit position for bit 0 of the green color channel information
GreenAdjust     - Number of bits to shift the 8-bit green value right
BlueMask        - Unshifted 8-bit mask for the blue color channel
BluePosition    - Bit position for bit 0 of the blue color channel information
BlueAdjust      - Number of bits to shift the 8-bit blue value right
AlphaMask       - Unshifted 8-bit mask for the alpha channel
AlphaPosition   - Bit position for bit 0 of the alpha channel information
AlphaAdjust     - Number of bits to shift the 8-bit alpha value right
****************************************************************************/
typedef struct {
    N_uint8 RedMask;
    N_uint8 RedPosition;
    N_uint8 RedAdjust;
    N_uint8 GreenMask;
    N_uint8 GreenPosition;
    N_uint8 GreenAdjust;
    N_uint8 BlueMask;
    N_uint8 BluePosition;
    N_uint8 BlueAdjust;
    N_uint8 AlphaMask;
    N_uint8 AlphaPosition;
    N_uint8 AlphaAdjust;
    } GA_pixelFormat;

/****************************************************************************
REMARKS:
Hardware 2D BitBltFx information structure. This structure defines the type
of BitBlt operation that is performed by the BitBltFx family of functions.
The Flags member defines the type of BitBlt operation to be performed, and
can be any combination of the supported flags (be sure to call BitBltFxTest
first to determine if that combination of effects is supported).

If mixes are enabled, the Mix member is used to determine the mix operation
to apply. If mixes are not enabled, GA_REPLACE_MIX is assumed (some hardware
may not support mix operations for effects blits).

The ColorKeyLo and ColorKeyHi members define the color key ranges if range
based color keying is selected. If only a single color key is enabled,
the ColorKeyLo value is the value used as the color key. The ColorKeyHi
value is inclusive in that it is included in the color range.

If blending is enabled, the SrcBlendFunc, DstBlendFunc and Alpha values
are used to implement the blending operation.

If clipping is enabled, the destination clip rectangle is passed in the
ClipLeft, ClipTop, ClipRight and ClipBottom members. Clipping is most
useful for stretching operations, where clipping in software is problematic.

If color conversion is enabled and you are color converting between color
index pixel formats and other pixel formats (including palette remapping),
you can optionally pass in a pre-computed translation vector in the TranslateVec
member. This will be used in place of dynamically computing the color
translation vector on the fly during the blit operation, so it is faster
in cases where the translation vector is known in advance for a number
of blit operations.

If color conversion is enabled and the source bitmap is 1bpp, the ForeColor
and BackColor members define the colors that will be used to color expand
the mono bitmap data to the destination surface. You can also

The VideoFormat field defines the format of the incoming bitmap data
if the bitmap is in a YUV pixel format, which can be used to implement
hardware video color space conversion and stretching using the hardware
blitting engine. This field will be zero in the mode info structure if
the hardware does not support YUV color conversion.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

Note:   The ColorKeyLo and ColorKeyHi values are always color values
        in the format of the destination surface color depth and pixel
        format. Ie: if you are color converting an 8bpp bitmap to a 32bpp
        destination surface, the color key values will be 32bpp color
        key values, not 8bpp color key values.

HEADER:
snap/graphics.h

MEMBERS:
dwSize          - Set to size of structure in bytes
Flags           - BitBltFx flags to define the type of BitBlt operation (GA_BitBltFxFlagsType)
Mix             - Logical mix operation (if mixes enabled)
ColorKeyLo      - Color key low value of range (if color keying enabled)
ColorKeyHi      - Color key high value of range (if color keying enabled)
SrcBlendFunc    - Src blend function (GA_blendFuncType)
DstBlendFunc    - Dst blend function (GA_blendFuncType)
ConstColor      - Constant color value for blending if blending enabled
ConstAlpha      - Constant alpha blend factor (0-255 if blending enabled)
BitsPerPixel    - Color depth for the source bitmap
PixelFormat     - Pixel format for the source bitmap
DstPalette      - Color index palette for destination (if destination color index)
SrcPalette      - Color index palette for source bitmap (if source color index)
TranslateVec    - Pre-computed color translation vector for color conversion
ClipLeft        - Left coordinate for destination clip rectangle
ClipTop         - Top coordinate for destination clip rectangle
ClipRight       - Right coordinate for destination clip rectangle
ClipBottom      - Bottom coordinate for destination clip rectangle
ForeColor       - Foreground color for 1bpp bitmaps
BackColor       - Background color for 1bpp bitmaps
VideoFormat     - YUV video formats supported by the hardware blit engine
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint32        Flags;
    N_int32         Mix;
    GA_color        ColorKeyLo;
    GA_color        ColorKeyHi;
    N_int32         SrcBlendFunc;
    N_int32         DstBlendFunc;
    GA_color        ConstColor;
    N_uint32        ConstAlpha;
    N_int32         BitsPerPixel;
    GA_pixelFormat  *PixelFormat;
    GA_palette      *DstPalette;
    GA_palette      *SrcPalette;
    GA_color        *TranslateVec;
    N_int32         ClipLeft;
    N_int32         ClipTop;
    N_int32         ClipRight;
    N_int32         ClipBottom;
    GA_color        ForeColor;
    GA_color        BackColor;
    N_uint32        VideoFormat;
    } GA_bltFx;

/****************************************************************************
REMARKS:
Graphics mode information block. This structure contains detailed information
about the capabilities and layout of a specific graphic mode.

The Attributes member contains a number of flags that describes certain
important characteristics of the graphics mode, and the values this member
contains are defined in the GA_AttributeFlagsType.

The XResolution and YResolution specify the logical width and height in pixel
elements for this display mode. The logical resolution is the resolution of
all available pixels in the display which may be larger than the physical
resolution if the mode has hardware panning enabled. Hardware panning is
enabled if the maximum physical resolution of the display device does not
support the specific mode, such as when hot switching between a CRT monitor
and an LCD panel or TV output device.

The BytesPerScanLine member specifies how many full bytes are in each logical
scanline. The logical scanline could be equal to or larger than the displayed
scanline, and can be changed when the display mode is first initialized.

The MaxBytesPerScanLine and MaxScanLineWidth members define the maximum
virtual framebuffer coordinates that can be initialised for the mode, in
both bytes and pixels respectively. If an attempt is made to initialize a
graphics mode with values larger than these values, the mode set will fail.

The MaxScanLines member holds the total number of scanlines available in
that graphics mode when initialised with the default scanline width. This
field combined with BytesPerScanLine can be used to determine the maximum
addressable display memory for drawing operations. This can also be used to
determine how large a virtual screen image can be before initialising a
graphics mode. This field also determines the addressable limit for
X/Y based drawing functions in offscreen video memory.

The LinearHeapStart member determines the start of the linear only heap,
if one is available. Some hardware has restrictions on the addressable memory
for the (x,y) coordinates passed to the 2D drawing functions. If the
hardware supports DrawRectLin and the BitBltLin family of functions, the
memory past the (x,y) coordinate restriction can be accessed using those
functions. Hence this member determines the start of this linear only
heap as a byte offset from the beginning of display memory. Memory in the
linear only heap can only be accessed directly via the linear framebuffer,
or using DrawRectLin or the BitBitLin family of blitting functions. None of
the X/Y based drawing functions can be used to draw to the linear only heap.

The MaxLinearOffset member hold the maximum addressable display memory
offset for linear drawing functions (DrawRectLin, BitBltLin etc). If the
hardware has restrictions on the addressable memory for the X/Y drawing
functions, the linear only heap resides between LinearHeapStart and
MaxLinearOffset. If the maximum addressable scanline value for a display
mode is past the end of display memory (common for high resolution modes),
then LinearHeapStart = MaxLinearOffset which indicates that there is no
linear only heap for that display mode.

NOTE:   There may well be some memory used by the graphics hardware
        for caching the hardware cursor, patterns and other data between
        the end of (MaxScanLines * BytesPerScanLine) and LinearHeapStart.
        Hence the application software must /never/ write to the memory
        between (MaxScanLines * BytesPerScanLine) and LinearHeapStart.

The BitsPerPixel member specifies the number of bits per pixel for this
display mode. For 5:5:5 format RGB modes this should contain a value of 15,
and for 5:6:5 format RGB modes this should contain a value of 16. For 8:8:8
bit RGB modes this should contain a value of 24 and for 8:8:8:8 RGBA modes
this should contain a value of 32. For 24 and 32bpp modes, the application
should look at the pixel format mask values (see below) to determine the
actual format of the pixels within the display buffer.

The MaxBuffers member specified the maximum number of display buffers that
can be allocated in video memory for page flipping. This value is a
convenience function only, and can be computed manually from the value
of MaxScanLines / YResolution.

The MaxPixelClock member specifies the maximum possible pixel dot clock that
can be selected in this display mode when a refresh controlled mode is
selected. Any attempt to select a higher pixel clock will cause the mode set
to fail. This member can be used to determine what the maximum available
refresh rate for the display mode will be.

The RedMaskSize, GreenMaskSize, BlueMaskSize and RsvdMaskSize members define
the size, in bits, of the red, green, and blue components of an RGB pixel
respectively. A bit mask can be constructed from the MaskSize members using
simple shift arithmetic. For example, the MaskSize values for an RGB 5:6:5
mode would be 5, 6, 5, and 0, for the red, green, blue, and reserved members
respectively.

The RedFieldPosition, GreenFieldPosition, BlueFieldPosition and
RsvdFieldPosition members define the bit position within the RGB pixel of
the least significant bit of the respective color component. A color value
can be aligned with its pixel member by shifting the value left by the
FieldPosition. For example, the FieldPosition values for an RGB 5:6:5 mode
would be 11, 5, 0, and 0, for the red, green, blue, and reserved members
respectively.

The BitmapStartAlign member defines the alignment requirements in bytes for
offscreen memory bitmaps for this graphics mode. If the value in here is
set to 8 for instance, then the start for all offscreen bitmaps must be
aligned to an 8 byte boundary in order to be used for offscreen bitmap
blitting. Note that the BitmapStartAlign member also defines the alignment
requirements for all buffers passed to the SetDrawBuffer function.

The BitmapStridePad member defines the alignment requirements in bytes for
the stride of offscreen memory bitmaps (the number of bytes to move from
one line of the bitmap to the next). If the value in here is set to 8 for
instance, then the number of bytes for each scanline in the offscreen
bitmap must be padded out to a multiple of 8 (inserting zeros if necessary
when downloading a source bitmap to offscreen memory). Note that the
BitmapStridePad member also defines the padding requirements for all
buffers passed to the SetDrawBuffer function.

The MonoBitmapStartAlign member defines the alignment requirements in bytes
for monochrome offscreen memory bitmaps for this graphics mode, which are
used by the MonoBltLin function. If the value in here is set to 8 for
instance, then the start for all monochrome offscreen bitmaps must be
aligned to an 8 byte boundary in order to be used by the MonoBltLin function.

The MonoBitmapStridePad member defines the alignment requirements in bytes
for the stride of monochrome offscreen memory bitmaps (the number of bytes
to move from one line of the bitmap to the next). If the value in here is
set to 8 for instance, then the number of bytes for each scanline in the
monochrome offscreen bitmap must be padded out to a multiple of 8 (inserting
zeros if necessary when downloading a source bitmap to offscreen memory).

The RefreshRateList member contains a list of all valid refresh rates
supported by the display mode which can be passed to the SetVideoMode
function. Interlaced modes are indicated by a negative refresh rate
(ie: 48Hz Interlaced is -48). The current default refresh rate is stored
in the DefaultRefreshRate member, and except for special circumstances the
default refresh rate set by the user should be used rather than overriding
the refresh rate.

The BitBltCaps member defines the extended hardware BitBlt capabilities
for the graphics mode, and points to a static GA_bltFx structure. Refer
to the documentation of GA_bltFx to determine what this structure contains.

The VideoWindows member defines the hardware video capabilities for each of
up to a maximum number of hardware video overlay windows. The list of
hardware video overlay window capabilities is terminated with a NULL pointer.
For instance if only 2 hardware video windows are supported, the first two
entries in this array would point to valid GA_videoInf structures, while the
third would contain a NULL terminating the list. Refer to the documentation
of GA_videoInf to determine what these structures contains.

The HwMCSurfaceFormats member defines the hardware motion compensation
surface formats that are supported. This will be a combination of the
flags defined in the GA_HwMCTypes and will list all formats supported
by the hardware.

The HwMCSubPictureFormats member defines the hardware motion compensation
sub picture pixel formats that are supported. This will be a combination of
the flags defined in the GA_VideoFormatsType flags. Usually only the
AI44 and IA44 formats are supported.

The DepthFormats field defines the hardware depth buffer capabilities
as defined by the GA_DepthFormatFlagsType type. The DepthStartAlign and
DepthStridePad values determine the define the alignment requirement
for depth buffers similarly to the BitmapStartAlign and BitmapStridePad
values.

The TextureFormats field defines the hardware texturing capabilities
as defined by the GA_TextureFormatFlagsType type. The TextureStartAlign and
TextureStridePad values determine the define the alignment requirement
for depth buffers similarly to the BitmapStartAlign and BitmapStridePad
values. The TextureMaxX and TextureMaxY fields determine the maximum
texture dimensions supported by the hardware, while the TextureMaxAspect
defines the maximum aspect ratio for textures.

The VideoTextureFormats field defines the hardware texturing capabilities
for video texture formats (ie: YUV compressed texture streams etc) as
defined by the GA_VideoFormatsType type. Video textures are basically
similar to regular textures, but the data in the texture is stored in
the compressed YUV color space.

The StencilFormats field defines the hardware stencil buffer capabilities
as defined by the GA_StencilFormatFlagsType type. The StencilStartAlign and
StencilStridePad values determine the define the alignment requirement
for depth buffers similarly to the BitmapStartAlign and BitmapStridePad
values.

The LinearSize member is the 32-bit length of the linear frame buffer memory
in bytes. In can be any length up to the size of the available video memory.
The LinearBasePtr member is the 32-bit physical address of the start of frame
buffer memory when the controller is in linear frame buffer memory mode for
this particular graphics mode. If the linear framebuffer is not available,
then this member will be zero.

The AttributesExt member contains a number of extended flags that describes
certain important characteristics of the graphics mode, and the values this
member contains are defined in the GA_AttributeExtFlagsType.

The PhysicalXResolution and PhysicalYResolution specify the physical width and
height in pixel elements for this display mode. The physical resolution is the
resolution of all visible pixels on the display, and may be smaller than the
logical resolution if the mode has hardware panning enabled.

The DeskStartXMask member contains the mask that represents the granularity
that the hardware provides for doing panning of the desktop in the X
direction. This value is provided so that the mouse cursor code in the
display driver is able to correctly account for any X start address masking
that will occur in the hardware when the display start address is programmed.
This value is represented in pixels.

The HwMCSurfaces member defines the hardware video motion compensation
surface formats for each of the supported formats (ie: MPEG1, MPEG2 etc). The
list of hardware video motion compensation surface formats is terminated with
a NULL pointer. For instance if only 2 hardware video motion compensation
surface formats are supported, the first two entries in this array would
point to valid GA_HwMCSurfaceInf structures, while the third would contain
a NULL terminating the list.

The MaxVideoCaptureBuffers member defines the maximum number of video input
capture buffers that are supported by the hardware. Some devices will only
allow video capture from one video source at a time, in which case only
one video capture buffer can be created (it could be switched between multiple
inputs as necessary).

The VideoCapture member defines the hardware video capture formats for each
of the inputs on the card. The list of hardware video capture formats is
terminated with a NULL pointer. For instance if only 2 hardware video capture
formats are supported, the first two entries in this array would point to valid
GA_VideoCaptureInf structures, while the third would contain a NULL terminating
the list.

Note:   The LinearSize and LinearBasePtr members are duplicated in the mode
        information block because they may possibly change locations in
        memory depending on the display mode. Normally applications will
        always use the value stored in the GA_devCtx LinearMem pointer
        to directly access the framebuffer (which is automatically adjusted
        for you), however if the information about the framebuffer starting
        address needed to be reported to other applications directly, the
        values stored in this mode information block should be used.

Note:   The memory pointed to by the RefreshRateList, BitBltCaps,
        VideoWindows and HW3DCaps fields will be reused the next time
        GetVideoModeInfo is called, so do not rely on the information in
        these fields to remain the same across calls to this function.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize                  - Set to size of structure in bytes
Attributes              - Mode attributes
XResolution             - Logical horizontal resolution in pixels
YResolution             - Logical vertical resolution in lines
XCharSize               - Character cell X dimension for text modes
YCharSize               - Character cell Y dimension for text modes
BytesPerScanLine        - Bytes per horizontal scan line
MaxBytesPerScanLine     - Maximum bytes per scan line
MaxScanLineWidth        - Maximum pixels per scan line
MaxScanLines            - Maximum number of scanlines for default scanline width
LinearHeapStart         - Start of linear only heap (if any)
MaxLinearOffset         - Maximum display memory offset for linear drawing
BitsPerPixel            - Bits per pixel
PixelFormat             - Pixel format for the display mode
MaxBuffers              - Maximum number of display buffers
MaxPixelClock           - Maximum pixel clock for mode
DefaultRefreshRate      - Currently active default refresh rate
RefreshRateList         - List of all valid refresh rates terminated with -1.
BitmapStartAlign        - Linear bitmap start alignment in bytes
BitmapStridePad         - Linear bitmap stride pad in bytes
MonoBitmapStartAlign    - Linear bitmap start alignment in bytes
MonoBitmapStridePad     - Linear bitmap stride pad in bytes
BitBltCaps              - Hardware 2D BitBltFx capabilities
VideoWindows            - List of hardware video overlay window capabilities
HwMCSurfaceFormats      - Flags for hardware motion compensation formats supported
HwMCSubPictureFormats   - Flags for the sub picture types supported
DepthFormats            - Depth buffer formats flags
DepthStartAlign         - Depth buffer start alignment in bytes
DepthStridePad          - Depth buffer stride pad in bytes
TextureFormats          - Texture formats flags
TextureStartAlign       - Texture start alignment in bytes
TextureStridePad        - Texture stride pad in bytes
TextureMaxX             - Maximum texture X dimension
TextureMaxY             - Maximum texture Y dimension
TextureMaxAspect        - Maximum texture aspect ratio (1:x)
StencilFormats          - Stencil buffer formats flags
StencilStartAlign       - Stencil buffer start alignment in bytes
StencilStridePad        - Stencil buffer stride pad in bytes
LinearSize              - Linear buffer size in bytes
LinearBasePtr           - Physical addr of linear buffer
AttributesExt           - Extended mode attributes flags
PhysicalXResolution     - Physical horizontal resolution in pixels
PhysicalYResolution     - Physical vertical resolution in lines
DeskStartXMask          - Desktop X starting pixel mask
HwMCSurfaces            - List of hardware video motion compensation surfaces
MaxVideoCaptureBuffers  - Maximum number of video capture buffers
VideoCapture            - List of video capture input formats
****************************************************************************/
typedef struct {
    N_uint32            dwSize;
    N_uint32            Attributes;
    N_uint16            XResolution;
    N_uint16            YResolution;
    N_uint8             XCharSize;
    N_uint8             YCharSize;
    N_uint32            BytesPerScanLine;
    N_uint32            MaxBytesPerScanLine;
    N_uint32            MaxScanLineWidth;
    N_uint32            MaxScanLines;
    N_uint32            LinearHeapStart;
    N_uint32            MaxLinearOffset;
    N_uint16            BitsPerPixel;
    GA_pixelFormat      PixelFormat;
    N_uint16            MaxBuffers;
    N_uint32            MaxPixelClock;
    N_int32             DefaultRefreshRate;
    N_int32             _FAR_ *RefreshRateList;
    N_uint32            BitmapStartAlign;
    N_uint32            BitmapStridePad;
    N_uint32            MonoBitmapStartAlign;
    N_uint32            MonoBitmapStridePad;
    GA_bltFx            _FAR_ *BitBltCaps;
    GA_videoInf         _FAR_ * _FAR_ *VideoWindows;
    N_uint32            HwMCSurfaceFormats;
    N_uint32            reserved1[3];
    N_uint32            VideoTextureFormats;
    N_uint32            DepthFormats;
    N_uint32            DepthStartAlign;
    N_uint32            DepthStridePad;
    N_uint32            TextureFormats;
    N_uint32            TextureStartAlign;
    N_uint32            TextureStridePad;
    N_uint32            TextureMaxX;
    N_uint32            TextureMaxY;
    N_uint16            TextureMaxAspect;
    N_uint32            StencilFormats;
    N_uint32            StencilStartAlign;
    N_uint32            StencilStridePad;
    N_uint32            LinearSize;
    N_uint32            LinearBasePtr;
    N_uint32            AttributesExt;
    N_uint16            PhysicalXResolution;
    N_uint16            PhysicalYResolution;
    N_uint32            DeskStartXMask;
    GA_HwMCSurfaceInf   _FAR_ * _FAR_ *HwMCSurfaces;
    N_int32             MaxVideoCaptureBuffers;
    GA_VideoCaptureInf  _FAR_ * _FAR_ *VideoCapture;
    } GA_modeInfo;

/****************************************************************************
REMARKS:
CRTC information block for refresh rate control, passed in to the
SetVideoMode function.

The HorizontalTotal, HorizontalSyncStart, HorizontalSyncEnd, VerticalTotal,
VerticalSyncStart and VerticalSyncEnd members define the default normalized
CRTC values that will be programmed if the gaRefreshCtl flag is passed to
SetVideoMode. The CRTC values for a particular resolution will always be the
same regardless of color depth. Note also that the CRTC table does not
contain any information about the horizontal and vertical blank timing
positions. It is up the driver implementation to determine the correct
blank timings to use for the mode when it is initialized depending on the
constraints of the underlying hardware (some hardware does not require this
information, and most VGA compatible hardware can be very picky about the
values programmed for the blank timings).

The Flags member defines the flags that modify the operation of the mode,
and the values for this member are defined in the GA_CRTCInfoFlagsType
enumeration.

The PixelClock member defines the normalized pixel clock that will be
programmed into the hardware. This value is represented in a 32 bit unsigned
integer in units of Hz. For example to represent a pixel clock of 25.18MHz
one would code a value of 25,180,000. From the pixel clock and the horizontal
and vertical totals, you can calculate the refresh rate for the specific
graphics mode using the following formula:

    refresh rate = (PixelClock * 10,000) /
                   (HorizontalTotal * VerticalTotal)

For example a 1024x768 mode with a HTotal of 1360, VTotal of 802, a pixel
clock of 130MHz might be computed as follows:

    refresh rate = (130 * 10,000) / (1360 * 802)
                 = 59.59 Hz

The RefreshRate field defines the refresh rate that the CRTC information
values define. This value may not actually be used by the driver but must be
calculated by the application program using the above formulas before
initializing the mode. This entry may be used by the driver to identify any
special cases that may need to be handled when setting the mode for specific
refresh rates.  The value in this field should be represented in units if
0.01 Hz (ie: a value 7200 represents a refresh rate of 72.00Hz).

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
HorizontalTotal     - Horizontal total (pixels)
HorizontalSyncStart - Horizontal sync start position
HorizontalSyncEnd   - Horizontal sync end position
VerticalTotal       - Vertical Total (lines)
VerticalSyncStart   - Vertical sync start position
VerticalSyncEnd     - Vertical sync end position
PixelClock          - Pixel clock in units of Hz
RefreshRate         - Expected refresh rate in .01Hz
Flags               - Initialisation flags for mode
****************************************************************************/
typedef struct {
    N_uint16    HorizontalTotal;
    N_uint16    HorizontalSyncStart;
    N_uint16    HorizontalSyncEnd;
    N_uint16    VerticalTotal;
    N_uint16    VerticalSyncStart;
    N_uint16    VerticalSyncEnd;
    N_uint32    PixelClock;
    N_uint16    RefreshRate;
    N_uint8     Flags;
    } GA_CRTCInfo;

/****************************************************************************
REMARKS:
Structure to maintain the information about the current display mode. This
structure is used internally by SNAP Graphics drivers, and is only defined here
so that we can manage the shared state structure in shared environments.

HEADER:
snap/graphics.h

MEMBERS:
bytesPerLine    - Bytes per logical scanline
xRes            - Horizontal pixel resolution
yRes            - Vertical pixel resolution
bits            - Color depth per pixel (0 = text mode)
flags           - Mode flags defining the mode capabilities
{secret}
****************************************************************************/
typedef struct {
    short       bytesPerLine;
    short       xRes;
    short       yRes;
    uchar       bits;
    ulong       flags;
    } GA_modeEntry;

/****************************************************************************
REMARKS:
Structure to maintain global state information shared between instances
of SNAP Graphics drivers when running in shared environments. This allows the
separate SNAP Graphics driver instances to keep in sync with each other. This
information is not used externally by SNAP Graphics applications, but only
internally by the SNAP Graphics drivers.

HEADER:
snap/graphics.h

MEMBERS:
reloadCRTC  - True if the CRTC tables should be reloaded on the next mode set
cntModeNum  - Currently active SNAP Graphics mode number
cntMode     - Entry containing information about the current display mode
cntModeInfo - Full GA_modeInfo structure for the display mode
cntCRTC     - Copy of the current CRTC timings in use
{secret}
****************************************************************************/
typedef struct {
    N_int32         reloadCRTC;
    N_int32         cntModeNum;
    GA_modeEntry    cntMode;
    GA_modeInfo     cntModeInfo;
    GA_CRTCInfo     cntCRTC;
    } GA_sharedInfo;

/****************************************************************************
REMARKS:
Generic graphics buffer parameter block. This structure defines a generic
buffer in offscreen video memory, and is passed to the driver to make
such buffers active rendering operations. The Offset member is the offset
of the start of the buffer in video memory. The Stride member defines the
stride of the buffer in bytes, while the Width and Height members define the
dimensions of the buffer in logical pixel units.

Note:   All buffers are in packed pixel format, and the values of the Offset
        and Stride members must adhere to the format restrictions defined in
        the GA_modeInfo structure for the buffer type being enabled.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize          - Set to size of structure in bytes
Offset          - Buffer starting address in bytes
Stride          - Stride of the buffer in bytes (bytes for a line of data)
Width           - Width of buffer in pixels
Height          - Height of the buffer in pixels
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_int32         Offset;
    N_int32         Stride;
    N_int32         Width;
    N_int32         Height;
    } GA_buffer;

/****************************************************************************
REMARKS:
Generic managed rendering buffer structure, which is used to describe
both system memory and offscreen video memory managed buffers which are
allocated using the AllocBuffer function. Offscreen managed buffers are
used to allocate and manage offscreen video memory, while system memory
buffers are used to manage rendering buffers in main system memory. Offscreen
buffers may be cached, pageable and purgeable, allowing system memory
to be used automatically when video memory runs out. The PixelFormat and
Palette parameters will always be NULL for non-special surfaces, which
indicate that the buffer is an offscreen memory surface in the same pixel
format as the display mode. Special surfaces (texture memory, depth buffers,
stencil buffers etc) cannot be used as destination surfaces for regular 2D
rendering operations.

Note:   If the buffer is not X/Y based, the StartX and StartY members will
        contain a value of -1.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize          - Set to size of structure in bytes
Width           - Width of buffer in pixels
Height          - Height of the buffer in pixels
Stride          - Stride of the buffer in bytes (bytes for a line of data)
CacheStride     - Stride of the buffer in system memory buffer cache
StartX          - Starting X coordinate in framebuffer for buffer (if x,y based)
StartY          - Starting Y coordinate in framebuffer for buffer (if x,y based)
Offset          - Linear buffer starting address in bytes
Flags           - Flags for the buffer (GA_BufferFlagsType)
Format          - Internal format indicator for the buffer
UseageCount     - Usage count for tracking pageable buffers
Surface         - Pointer to start of the buffer surface
SurfaceCache    - Pointer to surface cache in system memory (NULL if uncached)
AppInfo         - Pointer to application data if necessary
BitsPerPixel    - Color depth for the buffer
PixelFormat     - Pixel format for the buffer
Size            - Actual amount of memory used by the buffer
HwStride        - Stride for the buffer for the hardware engine
Primary         - Pointer to primary buffer this buffer is attached to
Next            - Next buffer in buffer flip chain, NULL if last buffer
PalSize         - Number of entries in the color palette (0 if none)
Palette         - Pointer to the color palette, NULL if none
Tag             - Application specific buffer tag
****************************************************************************/
typedef struct _GA_buf {
    N_uint32        dwSize;
    N_int32         Width;
    N_int32         Height;
    N_int32         Stride;
    N_int32         CacheStride;
    N_int32         StartX;
    N_int32         StartY;
    N_int32         Offset;
    N_int32         Flags;
    N_int32         Format;
    N_int32         UseageCount;
    void            *Surface;
    void            *SurfaceCache;
    void            *AppInfo;
    N_int32         BitsPerPixel;
    GA_pixelFormat  PixelFormat;
    N_int32         Size;
    N_int32         HwStride;
    struct _GA_buf  *Primary;
    struct _GA_buf  *Next;
    N_int32         PalSize;
    GA_palette      *Palette;
    N_uint32        Tag;
    } GA_buf;

/****************************************************************************
REMARKS:
Structure for managing video capture input.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize          - Set to size of structure in bytes
VideoBuffers    - Pointer to offscreen video buffers used for capture
Info            - Pointer to input capture descriptor for this buffer
InputNumber     - Input number this buffer is connected to (0-MaxInputs)
InputStandard   - Input standard for this buffer (PAL, NTSC etc).
ConnectorType   - Connector type for this buffer (Composite, S-Video etc).
AppInfo         - Pointer to application data if necessary
Tag             - Application specific buffer tag
****************************************************************************/
typedef struct {
    N_uint32            dwSize;
    GA_buf              *VideoBuffers;
    GA_VideoCaptureInf  *Info;
    N_int32             InputNumber;
    N_uint32            InputStandard;
    N_uint32            ConnectorType;
    void                *AppInfo;
    N_uint32            Tag;
    } GA_captureBuf;

/****************************************************************************
REMARKS:
Defines the fundamental type for a SNAP Graphics clipper object. The internals
of this object are completely implementation dependant so we simply define
this type as a void pointer as the application code should never care
about the internals of a GA_clipper object.

HEADER:
snap/graphics.h
****************************************************************************/
typedef void GA_clipper;

/****************************************************************************
REMARKS:
Hardware monochrome cursor structure. This structure defines a monochrome hardware
cursor that is downloaded to the hardware. The cursor is defined as a 64x64
image with an AND and XOR mask. The definition of the AND mask, XOR mask and
the pixels that will appear on the screen is as follows (same as the
Microsoft Windows cursor format):

    AND XOR Result
    0   0   Transparent (color from screen memory)
    0   1   Invert (complement of color from screen memory)
    1   0   Cursor background color
    1   1   Cursor foreground color

The HotX and HotY members define the /hot spot/ for the cursor, which is
the location where the logical mouse pointer is located in the cursor
image. When you click the mouse, the pixel under the hot-spot is the pixel
selected.

HEADER:
snap/graphics.h

MEMBERS:
XORMask     - Cursor XOR mask
ANDMask     - Cursor AND mask
HotX        - Cursor X coordinate hot spot
HotY        - Cursor Y coordinate hot spot
****************************************************************************/
typedef struct {
    N_uint8     XORMask[512];
    N_uint8     ANDMask[512];
    N_uint32    HotX;
    N_uint32    HotY;
    } GA_monoCursor;

/****************************************************************************
REMARKS:
Hardware 16-color cursor structure. This structure defines a color hardware
cursor that is downloaded to the hardware. The cursor is defined as a 64x64
image with an AND mask and color data. The definition of the AND mask,
cursor data and the pixels that will appear on the screen is as follows:

    AND Color   Result
    0   0       Transparent (color from screen memory)
    0   not 0   Invert (complement of color from screen memory)
    1   xx      Cursor color data

Hence if the AND mask is a zero the color data should be either 00 to make
the pixel transparent or not 0 to make it the inversion of the screen pixel.

The color data is passed down to the driver as 4-bit packed color index
values, along with a 16-color lookup table containing the real 24-bit RGB
color values for the cursor image. It is up to the calling application to
translate and quantise cursor images of higher color depths down to the
format supported by the hardware.

The HotX and HotY members define the /hot spot/ for the cursor, which is
the location where the logical mouse pointer is located in the cursor
image. When you click the mouse, the pixel under the hot-spot is the pixel
selected.

HEADER:
snap/graphics.h

MEMBERS:
ColorData   - Cursor color data as a 64x64 array of packed 4-bit pixels
ANDMask     - Cursor AND mask
Palette     - 16-color palette for cursor image
HotX        - Cursor X coordinate hot spot
HotY        - Cursor Y coordinate hot spot
****************************************************************************/
typedef struct {
    N_uint8     ColorData[2048];
    N_uint8     ANDMask[512];
    GA_palette  Palette[16];
    N_uint32    HotX;
    N_uint32    HotY;
    } GA_colorCursor;

/****************************************************************************
REMARKS:
Hardware 256-color cursor structure. This structure defines a color hardware
cursor that is downloaded to the hardware. The cursor is defined as a 64x64
image with an AND mask and color data. The definition of the AND mask,
cursor data and the pixels that will appear on the screen is as follows:

    AND Color   Result
    0   0       Transparent (color from screen memory)
    0   not 0   Invert (complement of color from screen memory)
    1   xx      Cursor color data

Hence if the AND mask is a zero the color data should be either 00 to make
the pixel transparent or not 0 to make it the inversion of the screen pixel.

The color data is passed down to the driver as 8-bit packed color index
values, along with a 256-color lookup table containing the real 24-bit RGB
color values for the cursor image. It is up to the calling application to
translate and quantise cursor images of higher color depths down to the
format supported by the hardware.

The HotX and HotY members define the /hot spot/ for the cursor, which is
the location where the logical mouse pointer is located in the cursor
image. When you click the mouse, the pixel under the hot-spot is the pixel
selected.

HEADER:
snap/graphics.h

MEMBERS:
ColorData   - Cursor color data as a 64x64 array of packed 8-bit pixels
ANDMask     - Cursor AND mask
Palette     - 256-color palette for cursor image
HotX        - Cursor X coordinate hot spot
HotY        - Cursor Y coordinate hot spot
****************************************************************************/
typedef struct {
    N_uint8     ColorData[4096];
    N_uint8     ANDMask[512];
    GA_palette  Palette[256];
    N_uint32    HotX;
    N_uint32    HotY;
    } GA_colorCursor256;

/****************************************************************************
REMARKS:
Hardware 24-bit cursor structure. This structure defines a color hardware
cursor that is downloaded to the hardware. The cursor is defined as a 64x64
image with an AND mask and color data. The definition of the AND mask,
cursor data and the pixels that will appear on the screen is as follows:

    AND Color   Result
    0   0       Transparent (color from screen memory)
    0   not 0   Invert (complement of color from screen memory)
    1   xx      Cursor color data

Hence if the AND mask is a zero the color data should be either 00 to make
the pixel transparent or not 0 to make it the inversion of the screen pixel.

The color data is passed down to the driver as 24-bit packed RGB color
values. It is up to the calling application to translate cursor images of
lower color depths to the format supported by the hardware.

The HotX and HotY members define the /hot spot/ for the cursor, which is
the location where the logical mouse pointer is located in the cursor
image. When you click the mouse, the pixel under the hot-spot is the pixel
selected.

HEADER:
snap/graphics.h

MEMBERS:
ColorData   - Cursor color data as a 64x64 array of packed 24-bit RGB pixels
ANDMask     - Cursor AND mask
HotX        - Cursor X coordinate hot spot
HotY        - Cursor Y coordinate hot spot
****************************************************************************/
typedef struct {
    N_uint8     ColorData[12288];
    N_uint8     ANDMask[512];
    N_uint32    HotX;
    N_uint32    HotY;
    } GA_colorCursorRGB;

/****************************************************************************
REMARKS:
Hardware 24-bit RGBA alpha blended cursor structure. This structure defines
a color hardware cursor that is downloaded to the hardware. The cursor is
defined as a 64x64 24-bit RGBA image with alpha channel. The alpha channel
data is used to define the transparency level for the bitmap, with 0 being
fully transparent and 255 being full opaque. Since the color bitmap data
is alpha blended, the is no AND mask for the cursor image.

The HotX and HotY members define the /hot spot/ for the cursor, which is
the location where the logical mouse pointer is located in the cursor
image. When you click the mouse, the pixel under the hot-spot is the pixel
selected.

HEADER:
snap/graphics.h

MEMBERS:
ColorData   - Cursor color data as a 64x64 array of packed 24-bit RGBA pixels
HotX        - Cursor X coordinate hot spot
HotY        - Cursor Y coordinate hot spot
****************************************************************************/
typedef struct {
    N_uint8     ColorData[16384];
    N_uint32    HotX;
    N_uint32    HotY;
    } GA_colorCursorRGBA;

/****************************************************************************
REMARKS:
Parameter block for the 2D DrawTrap function. This structure is used to pass
in the information about a trapezoid to be rendered by the hardware to the
driver DrawTrap function.

HEADER:
snap/graphics.h

MEMBERS:
y       - Starting Y coordinate
count   - Number of scanlines to draw
x1      - Starting X1 coordinate in 16.16 fixed point
x2      - Starting X2 coordinate in 16.16 fixed point
slope1  - First edge slope in 16.16 fixed point format
slope2  - Second edge slope in 16.16 fixed point format
****************************************************************************/
typedef struct {
    N_uint32    y;
    N_uint32    count;
    N_fix32     x1;
    N_fix32     x2;
    N_fix32     slope1;
    N_fix32     slope2;
    } GA_trap;

/****************************************************************************
REMARKS:
Structure pointed to be the GA_certifyInfo structure, which contains
certification information about the specific chipsets in the device driver.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

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
    } GA_certifyChipInfo;

/****************************************************************************
REMARKS:
Structure returned by GetCertifyInfo, which contains configuration
information about the certification status of the drivers.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

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
    GA_certifyChipInfo  *CertifiedCards;
    } GA_certifyInfo;

/****************************************************************************
REMARKS:
Structure returned by GetConfigInfo, which contains configuration
information about the installed graphics hardware.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize              - Set to size of structure in bytes
ManufacturerName    - Name of graphics chipset manufacturer
ChipsetName         - Name of graphics chipset name
DACName             - Name of DAC on graphics card
ClockName           - Name of clock on graphics card
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
    char            DACName[80];
    char            ClockName[80];
    char            VersionInfo[80];
    char            BuildDate[80];
    char            Certified;
    char            CertifiedDate[20];
    N_uint16        CertifyVersion;
    } GA_configInfo;

/****************************************************************************
REMARKS:
Structure used to describe the available display modes in the SNAP Graphics
options structure. This allows the end user to add and delete available
display modes from the SNAP Graphics drivers easily using our generic SNAP
Graphics driver interface.

HEADER:
snap/graphics.h

MEMBERS:
xRes    - Horizontal pixel resolution
yRes    - Vertical pixel resolution
bits    - Color depth per pixel (0 = text mode)
****************************************************************************/
typedef struct {
    short       xRes;
    short       yRes;
    uchar       bits;
    } GA_mode;

/****************************************************************************
REMARKS:
Structure used to describe the TV parameters specific to a particular TV
output mode. We store these values independently in the options structure
for different TV modes (ie: 640x480, 800x600, PAL, NTSC etc).

HEADER:
snap/graphics.h

MEMBERS:
hPos        - Horizontal position value (+-)
vPos        - Vertical position value (+-)
brightness  - Brightness control value
contrast    - Contrast control value
****************************************************************************/
typedef struct {
    N_int16     hPos;
    N_int16     vPos;
    N_uint16    brightness;
    N_uint16    contrast;
    } GA_TVParams;

/****************************************************************************
REMARKS:
Structure used to determine the layout in multi-controller modes. The
layout is defined as adjacent rectangles for each device, and should
be set up using the multi-controller setup program.

HEADER:
snap/graphics.h

MEMBERS:
left            - Left coordinate for layout rectangle
top             - Top coordinate for layout rectangle
right           - Right coordinate for layout rectangle
bottom          - Bottom coordinate for layout rectangle
****************************************************************************/
typedef struct {
    N_uint32    left;
    N_uint32    top;
    N_uint32    right;
    N_uint32    bottom;
    } GA_layout;

#define GA_MAX_RESOLUTIONS  20
#define GA_MAX_COLORDEPTHS  8

/****************************************************************************
REMARKS:
Structure returned by GetOptions, which contains configuration
information about the options for the installed device driver. All the
boolean configuration options are enabled by default and can be optionally
turned off by the user via the configuration functions.

This structure also contains the configuration information for the software
stereo page flipping support in SNAP Graphics.

If you select the gaGlassesIOPort type, then you need to fill
in the ioPort, ioAndMask, ioLeftOrMask, ioRightOrMask and ioOffOrMask
fields. These fields define the values used to toggle the specified I/O
port when the glasses need to be flipped. First the existing value is read
from the specified I/O port, the AND mask in applied and then the appropriate
OR mask is applied depending on the state of the glasses. This value is
then written back to the desired I/O port.

If you select the gaGlassesGenericVSync type, then you need to fill in the
VSyncWidthLeft and VSyncWidthRight fields, which define the vertical
sync width to program when the desired eye should be active.

The values in the RTCFrequency and RTCAdvanceTicks define the frequency
of the stereo timer interrupt, which can be used to fine tune the overheads
taken by the stereo page flip handler for maximum performance before
stuttering begins (ie: lost frames). The RTCFrequency field can be any
power of 2 frequency between 1024Hz and 8192H, and the RTCAdvanceTicks
should be a value larger than 1. For most systems an RTCFrequency value
of 2048 and an RTCAdvanceTicks of 2 will work well. SNAP Graphics will however
choose good defaults for the target OS if these values are not overridden.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize                  - Set to size of structure in bytes
memoryClock             - Currently configured memory clock
defaultMemoryClock      - Current hardware default memory clock
maxMemoryClock          - Maximum allowable memory clock
gammaRamp               - Default gamma ramp for RGB display modes
outputDevice            - Currently configured output device (primary head only)
TV640PALUnder           - TV parameters for 640x480 PAL underscan modes
TV640NTSCUnder          - TV parameters for 640x480 NTSC underscan modes
TV640PALOver            - TV parameters for 640x480 PAL overscan modes
TV640NTSCOver           - TV parameters for 640x480 NTSC overscan modes
TV800PALUnder           - TV parameters for 800x600 PAL underscan modes
TV800NTSCUnder          - TV parameters for 800x600 NTSC underscan modes
TV800PALOver            - TV parameters for 800x600 PAL overscan modes
TV800NTSCOver           - TV parameters for 800x600 NTSC overscan modes
numHorzDisplay          - Number of horizontal displays
numVertDisplay          - Number of vertical displays
RTCFrequency            - Frequency for real time clock for software stereo
RTCAdvanceTicks         - Number of ticks to advance for software stereo
glassesType             - Type of stereo glasses defined by GA_glassesTypeFlags
ioPort                  - Generic I/O port for controlling glasses
ioAndMask               - I/O port AND mask
ioLeftOrMask            - I/O port OR mask when left eye is active
ioRightOrMask           - I/O port OR mask when right eye is active
ioOffOrMask             - I/O port OR mask when glasses are off
vSyncWidthLeft          - Vertical sync width when left eye is active
vSyncWidthRight         - Vertical sync width when right eye is active
text80x43Height         - Character height for 80x43 text mode (8,14 or 16)
text80x50Height         - Character height for 80x50 text mode (8,14 or 16)
text80x60Height         - Character height for 80x60 text mode (8,14 or 16)
text100x43Height        - Character height for 100x43 text mode (8,14 or 16)
text100x50Height        - Character height for 100x50 text mode (8,14 or 16)
text100x60Height        - Character height for 100x60 text mode (8,14 or 16)
text132x43Height        - Character height for 132x43 text mode (8,14 or 16)
text132x50Height        - Character height for 132x50 text mode (8,14 or 16)
text132x60Height        - Character height for 132x60 text mode (8,14 or 16)
tripleScanLowRes        - True to triple scan low res modes
doubleScan512           - True to double scan 512x384 modes
stereoRefresh           - Value to use for stereo mode refresh rate
stereoRefreshInterlaced - Value to use for stereo mode interlaced refresh rate
stereoMode              - Stereo mode to be used for fullscreen applications
stereoModeWindowed      - Stereo mode to be used for windowed applications
stereoBlankInterval     - Stereo mode blank interval for above below format
stereoDevice            - Stereo device ID defined by stereo control panel
engineClock             - Currently configured graphics engine clock
defaultEngineClock      - Current hardware default graphics engine clock
maxEngineClock          - Maximum allowable graphics engine clock
bEnablePaletteDelay     - Enable short palette read/write delays for compatibility
bAllowDDCBIOS           - Enable support for using the DDC BIOS as a fallback
LCDPanelWidth           - Width of attached LCD panel in pixels
LCDPanelHeight          - Height of attached LCD panel in lines
bLCDExpand              - Enable expansion of modes to fill LCD panel
bDebugMode              - Enable debug log filter driver (0 is off)
bAlwaysOne              - Must always be set to 1!!
bDialAMode              - Enable Dial-A-Mode generic mode interface
bVirtualScroll          - Enable virtual scrolling functions
bDoubleBuffer           - Enable double buffering functions
bTripleBuffer           - Enable triple buffering functions
bHardwareStereoSync     - Enable hardware stereo sync flag
bStereo                 - Enable stereo display mode support
bMultiDisplay           - Enable multiple display mode support
bPortrait               - Enable portrait display mode
bFlipped                - Enable flipped display mode
bInvertColors           - Enable invert color mode
bVirtualDisplay         - Enable virtual display mode
bAGPFastWrite           - Enable AGP fast write (only here to be licensed)
bZoom                   - Enable zoom support
bMultiHead              - Enable multi-head support
bLCDPanel               - Enable LCD panel support
bTVOut                  - Enable TV Output support
bTVTuner                - Enable TV Tuner support
bUnused                 - Unused option
bDPMS                   - Enable DPMS Display Power Management support
bDDC                    - Enable DDC Display Data Channel functions
bDDCCI                  - Enable DDC/CI Control Interface functions
bGammaCorrect           - Enable gamma correction
bHardwareCursor         - Enable hardware cursor
bHardwareVideo          - Enable hardware video
bHardwareAccel2D        - Enable hardware 2D acceleration
bHardwareAccel3D        - Enable hardware 2D acceleration
bMonoPattern            - Enable 8x8 mono pattern fills
bTransMonoPattern       - Enable 8x8 mono transparent pattern fills
bColorPattern           - Enable 8x8 color pattern fills
bTransColorPattern      - Enable 8x8 color transparent pattern fills
bSysMem                 - Enable system memory blits
bLinear                 - Enable linear offscreen memory blits
bBusMaster              - Enable bus mastering functions
bDrawScanList           - Enable DrawScanList family of functions
bDrawEllipseList        - Enable DrawEllipseList family of functions
bDrawFatEllipseList     - Enable DrawFatEllipseList family of functions
bDrawRect               - Enable DrawRect family of functions
bDrawRectLin            - Enable DrawRectLin family of functions
bDrawTrap               - Enable DrawTrap family of functions
bDrawLine               - Enable DrawLine function
bDrawStippleLine        - Enable DrawStippleLine function
bMonoBlt                - Enable MonoBlt function
bBitBlt                 - Enable BitBlt family of functions
bBitBltPatt             - Enable BitBltPatt family of functions
bBitBltColorPatt        - Enable BitBltColorPatt family of functions
bSrcTransBlt            - Enable SrcTransBlt family of functions
bDstTransBlt            - Enable DstTransBlt family of functions
bStretchBlt             - Enable StretchBlt family of functions
bConvertBlt             - Enable ConvertBlt family of functions
bStretchConvertBlt      - Enable StretchConvertBlt family of functions
bBitBltFx               - Enable BitBltFx family of functions
bGetBitmap              - Enable GetBitmap family of functions
multiHeadSize           - Virtual size for multi-head displays
multiHeadRes            - Physical resolutions for multi-head displays
multiHeadBounds         - Virtual layout for multi-head displays
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_fix32         memoryClock;
    N_fix32         defaultMemoryClock;
    N_fix32         maxMemoryClock;
    GA_paletteExt   gammaRamp[256];
    N_int32         outputDevice;
    GA_TVParams     TV640PALUnder;
    GA_TVParams     TV640NTSCUnder;
    GA_TVParams     TV640PALOver;
    GA_TVParams     TV640NTSCOver;
    GA_TVParams     TV800PALUnder;
    GA_TVParams     TV800NTSCUnder;
    GA_TVParams     TV800PALOver;
    GA_TVParams     TV800NTSCOver;
    N_uint8         bRes1;
    N_uint8         bRes2;
    N_int32         RTCFrequency;
    N_int32         RTCAdvanceTicks;
    N_uint8         bRes3;
    N_uint16        ioPort;
    N_uint8         ioAndMask;
    N_uint8         ioLeftOrMask;
    N_uint8         ioRightOrMask;
    N_uint8         ioOffOrMask;
    N_uint8         vSyncWidthLeft;
    N_uint8         vSyncWidthRight;
    N_uint8         text80x43Height;
    N_uint8         text80x50Height;
    N_uint8         text80x60Height;
    N_uint8         text100x43Height;
    N_uint8         text100x50Height;
    N_uint8         text100x60Height;
    N_uint8         text132x43Height;
    N_uint8         text132x50Height;
    N_uint8         text132x60Height;
    N_uint8         tripleScanLowRes;
    N_uint8         doubleScan512;
    N_uint8         stereoRefresh;
    N_uint8         stereoRefreshInterlaced;
    N_uint8         stereoMode;
    N_uint8         stereoModeWindowed;
    N_uint8         stereoBlankInterval;
    N_uint8         stereoRefreshWindowed;
    N_uint8         stereoRefreshWindowedInterlaced;
    N_uint32        stereoDevice;
    N_uint16        glassesType;
    N_uint16        stereoBlankIntervalPercent;
    N_fix32         engineClock;
    N_fix32         defaultEngineClock;
    N_fix32         maxEngineClock;
    N_uint8         stereoControlPanelOptions;
    N_uint8         stereoCursorRedraw;
    N_uint8         bLoadedFromDisk;
    N_uint8         bEnablePaletteDelay;
    N_uint8         bAllowDDCBIOS;
    N_uint8         res0[11];
    N_int16         LCDPanelWidth;
    N_int16         LCDPanelHeight;
    N_uint8         bLCDExpand;
    N_uint8         bPrefer16bpp;
    N_uint8         bPrefer32bpp;
    N_int16         TVMaxWidth;
    N_int16         TVMaxHeight;
    N_uint8         bCompressedFB;
    N_uint8         bDVIReducedTimings;
    N_uint8         res1[93];
    N_uint32        resolutions[GA_MAX_RESOLUTIONS];
    N_uint8         colorDepths[GA_MAX_COLORDEPTHS];
    N_uint8         maxRefresh;
    N_uint8         accelType;
    N_uint8         res2[159];
    N_uint8         bDebugMode;
    N_uint8         bAlwaysOne;
    N_uint8         bDialAMode;
    N_uint8         bVirtualScroll;
    N_uint8         bDoubleBuffer;
    N_uint8         bTripleBuffer;
    N_uint8         bHardwareStereoSync;
    N_uint8         bStereo;
    N_uint8         bMultiDisplay;
    N_uint8         bPortrait;
    N_uint8         bFlipped;
    N_uint8         bInvertColors;
    N_uint8         bReserved1;
    N_uint8         bReserved2;
    N_uint8         bVirtualDisplay;
    N_uint8         bAGPFastWrite;
    N_uint8         bZoom;
    N_uint8         bMultiHead;
    N_uint8         bDPVLMode;
    N_uint8         res3[152];
    N_uint8         bLCDPanel;
    N_uint8         bTVOut;
    N_uint8         bTVTuner;
    N_uint8         bUnused;
    N_uint8         bDPMS;
    N_uint8         bDDC;
    N_uint8         bDDCCI;
    N_uint8         bGammaCorrect;
    N_uint8         bHardwareCursor;
    N_uint8         bHardwareColorCursor;
    N_uint8         bHardwareVideo;
    N_uint8         bHardwareAccel2D;
    N_uint8         bHardwareAccel3D;
    N_uint8         bMonoPattern;
    N_uint8         bTransMonoPattern;
    N_uint8         bColorPattern;
    N_uint8         bTransColorPattern;
    N_uint8         bSysMem;
    N_uint8         bLinear;
    N_uint8         bBusMaster;
    N_uint8         bDrawScanList;
    N_uint8         bDrawEllipseList;
    N_uint8         bDrawFatEllipseList;
    N_uint8         bDrawRect;
    N_uint8         bDrawRectLin;
    N_uint8         bDrawTrap;
    N_uint8         bDrawLine;
    N_uint8         bDrawStippleLine;
    N_uint8         bMonoBlt;
    N_uint8         bAlwaysOne2;
    N_uint8         bBitBlt;
    N_uint8         bBitBltPatt;
    N_uint8         bBitBltColorPatt;
    N_uint8         bSrcTransBlt;
    N_uint8         bDstTransBlt;
    N_uint8         bStretchBlt;
    N_uint8         bConvertBlt;
    N_uint8         bStretchConvertBlt;
    N_uint8         bBitBltFx;
    N_uint8         bGetBitmap;
    N_uint8         bOpenGL;
    N_uint8         bHardwareMPEG;
    N_uint8         bVidMemPackets;
    N_uint8         bHardwareVideoCapture;
    N_uint8         res5[252];
    GA_layout       multiHeadSize;
    GA_layout       multiHeadRes[GA_MAX_HEADS];
    GA_layout       multiHeadBounds[GA_MAX_HEADS];
    GA_layout       DPVLHeadSize;
    GA_layout       DPVLHeadRes[GA_MAX_DPVL_HEADS];
    GA_layout       DPVLHeadBounds[GA_MAX_DPVL_HEADS];
    } GA_options;

#define FIRST_OPTION    bDialAMode
#define LAST_OPTION     bGetBitmap

/****************************************************************************
REMARKS:
Structure used to store the recommended modes for SNAP Graphics.

HEADER:
snap/graphics.h

MEMBERS:
XResolution     - X resolution for the recommended mode
YResolution     - Y resolution for the recommended mode
BitsPerPixel    - BitsPerPixel for the recommended mode
RefreshRate     - RefreshRate for the recommended mode (up to 127Hz)
****************************************************************************/
typedef struct {
    N_uint16    XResolution;
    N_uint16    YResolution;
    N_uint8     BitsPerPixel;
    N_int8      RefreshRate;
    } GA_recMode;

/****************************************************************************
REMARKS:
Structure returned by GA_getGlobalOptions, which contains configuration
information about the options effective for all installed display devices.
This structure also contains the layout information used for multi-controller
options in SNAP Graphics (such as what screen is located where).

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize                  - Set to size of structure in bytes
bVirtualDisplay         - Enable virtual display mode
bPortrait               - Enable portrait display mode
bFlipped                - Enable flipped display mode
bInvertColors           - Enable invert color mode
bVBEOnly                - Enable VBE/Core fallback driver
bVGAOnly                - Enable VGA fallback driver
bReserved1              - Reserved option; must *always* be zero!
bAllowNonCertified      - Allow uncertified drivers to load
wCertifiedVersion       - Version of certify program to allow
bNoWriteCombine         - Disable write combining
bLCDUseBIOS             - Enable use of BIOS when on the LCD panel
bUseMemoryDriver        - Enable system memory driver
wSysMemSize             - Amount of memory to allocate for sysmem driver (Kb)
dwCPLFlags              - Place to store control panel UI settings
dwSharedAGPMemSize      - Amount of shared AGP memory to use for framebuffer
bUseVBECore             - Use the VBE/Core emulation driver
bUseVGACore             - Use the VGA/Core emulation driver
dwCheckForUpdates       - Time stamp to check for updates next
bNoDDCDetect            - Disable automatic DDC monitor detection
bDisableLogFile         - Disable logging of information to log file
bCheckWebSelection      - SDD GUI specific value for web check updates
wMonitorHSize           - Monitor width used for recommended modes
wMonitorVSize           - Monitor height used for recommended modes
wOptimizedModeXRes      - Optimized mode width (GUI CPL)
wOptimizedModeYRes      - Optimized mode height (GUI CPL)
wOptimizedModeBits      - Optimized mode color depth (GUI CPL)
recommendedMode         - Recommended mode overall
recommendedMode8        - Recommended mode for 8bpp
recommendedMode16       - Recommended mode for 16bpp
recommendedMode24       - Recommended mode for 24bpp
recommendedMode32       - Recommended mode for 32bpp
bAGPFastWrite           - Enable AGP fast write
bMaxAGPRate             - Maximum AGP rate (1x,2x,4x,8x)
recommendedMode15       - Recommended mode for 15bpp
bNoCPUCalibration       - Disable CPU clock speed calibration
dwCPUClockSpeed         - CPU clock speed if calibration disabled (MHz)
dwVideoMemLimit         - Video memory limit for useable linear address mapping
virtualSize             - Virtual size for multi-controller displays
resolutions             - Physical resolutions for multi-controller displays
bounds                  - Virtual layout for multi-controller displays
****************************************************************************/
typedef struct {
    N_uint32        dwSize;
    N_uint8         bVirtualDisplay;
    N_uint8         bPortrait;
    N_uint8         bFlipped;
    N_uint8         bInvertColors;
    N_uint8         bVBEOnly;
    N_uint8         bVGAOnly;
    N_uint8         bReserved1;
    N_uint16        wCertifiedVersion;
    N_uint8         bNoWriteCombine;
    N_uint8         bAllowNonCertified;
    N_uint8         bLCDUseBIOS;
    N_uint8         bUseMemoryDriver;
    N_uint16        wSysMemSize;
    N_uint32        dwReserved2;
    N_uint8         bVBEUseLinear;
    N_uint8         bVBEUsePal;
    N_uint8         bVBEUsePM32;
    N_uint8         bReserved2;
    N_uint8         bVBEUseVBE20;
    N_uint8         bVBEUseVBE30;
    N_uint8         bVBEUsePM;
    N_uint8         bVBEUseSCI;
    N_uint8         bVBEUseDDC;
    N_uint8         bGDIUseAccel;
    N_uint8         bGDIUseBrushCache;
    N_uint8         bGDIUseBitmapCache;
    N_uint8         bDXUseAccel2D;
    N_uint8         bDXUseAccel3D;
    N_uint8         bDXUseAccelVideo;
    N_uint8         bDXWaitRetrace;
    N_uint32        dwCPLFlags;
    N_uint32        dwSharedAGPMemSize;
    N_uint8         bUseVBECore;
    N_uint8         bUseVGACore;
    N_uint32        dwCheckForUpdates;
    N_uint8         bNoDDCDetect;
    N_uint8         bDisableLogFile;
    N_uint8         bCheckWebSelection;
    N_uint16        wMonitorHSize;
    N_uint16        wMonitorVSize;
    N_uint16        wOptimizedModeXRes;
    N_uint16        wOptimizedModeYRes;
    N_uint16        wOptimizedModeBits;
    GA_recMode      recommendedMode;
    GA_recMode      recommendedMode8;
    GA_recMode      recommendedMode16;
    GA_recMode      recommendedMode24;
    GA_recMode      recommendedMode32;
    N_uint8         bAGPFastWrite;
    N_uint8         bMaxAGPRate;
    GA_recMode      recommendedMode15;
    N_uint8         bNoCPUCalibration;
    N_uint32        dwCPUClockSpeed;
    N_uint32        dwVideoMemLimit;
    N_uint8         res1[51];
    GA_layout       virtualSize;
    GA_layout       resolutions[GA_MAX_VIRTUAL_DISPLAYS];
    GA_layout       bounds[GA_MAX_VIRTUAL_DISPLAYS];
    } GA_globalOptions;

/****************************************************************************
REMARKS:
Structure returned by GetModeProfile, which contains configuration
information about the mode profile for the installed device driver. A
default mode profile is shipped with the graphics device drivers, but a new
mode profile can be downloaded at any time (to implement new display modes
using the new Dial-A-Mode interface). Note that a mode must also have
the associated CRTC tables before it will work.

Note:   The dwSize member is intended for future compatibility, and
        should be set to the size of the structure as defined in this
        header file. Future drivers will be compatible with older software
        by examining this value.

HEADER:
snap/graphics.h

MEMBERS:
dwSize      - Set to size of structure in bytes
numModes    - Count for the number of configured display modes
modeList    - Array of up to 256 configured display modes
numModes    - Count for the number of configured display modes
vbeModeList - Array of up to 256 modes reported to the VBE driver
****************************************************************************/
typedef struct {
    N_uint32    dwSize;
    struct {
        N_uint8 numModes;
        GA_mode modeList[256];
        } m;
    struct {
        N_uint8 numModes;
        GA_mode modeList[256];
        } vbe;
    } GA_modeProfile;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all internal loading and unloading functions
for the device. These functions are used internally by the SNAP device
driver loading mechanism and should /never/ be called directly by
application or shell driver code.
****************************************************************************/
typedef struct {
    N_int32         (NAPIP InitDriver)(void);
    ibool           (NAPIP QueryFunctions)(N_uint32 id,N_int32 safetyLevel,void _FAR_ *funcs);
    void            (NAPIP UnloadDriver)(void);
    } GA_loaderFuncs;

/****************************************************************************
REMARKS:
Main graphics device context structure. This structure consists of a header
block that contains configuration information about the graphic device,
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

The AvailableModes is an pointer within the loaded driver to a list of mode
numbers for all display modes supported by the graphics driver.  Each mode
number occupies one word (16-bits), and is terminated by a -1 (0FFFFh). Any
modes found in this list are guaranteed to be available for the current
configuration.

The TotalMemory member indicates the maximum amount of memory physically
installed and available to the frame buffer in 1KB units. Note that not all
graphics modes will be able to address all of this memory.

The Attributes member contains a number of flags that describes certain
important characteristics of the graphics controller. The members are
exactly the same as those provided in the GA_modeInfo block for each video
mode, but the meaning is slightly different. For each flag defined in the
GA_AttributeFlagsType enumeration, it represents whether the controller can
support these modes in any available graphics modes. Please see the
GetVideoModeInfo function for a detailed description of each flags meaning.

The TextSize member contains the size of the text mode framebuffer in bytes.
It will generally be 64KB in length. The TextBasePtr member is a
32-bit physical memory address where the text mode framebuffer memory window
is located in the CPU address space. This will generally be 0xB0000 to cover
the VGA text framebuffer window (both color and monochrome modes).

The BankSize member contains the size of the banked memory buffer in bytes.
It can be either 4KB or 64KB in length. The BankedBasePtr member is a
32-bit physical memory address where the banked framebuffer memory window is
located in the CPU address space. If the banked framebuffer mode is not
available, then this member will be zero.

The LinearSize member is the 32-bit length of the linear frame buffer memory
in bytes. In can be any length up to the size of the available video memory.
The LinearBasePtr member is the 32-bit physical address of the start of frame
buffer memory when the controller is in linear frame buffer memory mode. If
the linear framebuffer is not available, then this member will be zero.

The ZBufferSize member is the 32-bit length of the local z-buffer (or depth
buffer) memory in bytes. In can be any length up to the size of the available
local z-buffer memory. The ZBufferBasePtr member is the 32-bit physical
address of the start of local z-buffer memory. Note that if the controller
does not have local z-buffer memory, but shares the z-buffer in the local
framebuffer memory, these two fields will be set to 0.

The TexMemSize member is the 32-bit length of the local texture memory in
bytes. In can be any length up to the size of the available local texture
memory. The TexMemBasePtr member is the 32-bit physical address of the start
of local texture memory. Note that if the controller does not have local
texture memory, but loads textures in the local framebuffer memory, this
field will be set to 0.

The LockedMemSize contains the amount of locked, contiguous memory in bytes
that the graphics driver requires for programming the hardware. If the graphics
accelerator requires DMA transfers for 2D and 3D rendering operations, this
member can be set to the length of the block of memory that is required by
the driver. The driver loader code will attempt to allocate a block of
locked, physically contiguous memory from the operating system and place a
pointer to this allocated memory in the LockedMem member for the driver, and
the physical address of the start of this memory block in LockedMemPhys. Note
that the memory must be locked so it cannot be paged out do disk, and it must
be physically contiguous so that DMA operations will work correctly across
4KB CPU page boundaries. If the driver does not require DMA memory, this
value should be set to 0.

The MMIOBase member contains the 32-bit physical base addresses pointing
to the start of up to 4 separate memory mapped register areas required by the
controller. The MMIOLen member contains the lengths of each of these
memory mapped IO areas in bytes. When the application maps the memory mapped
IO regions for the driver, the linear address of the mapped memory areas will
then be stored in the corresponding entries in the IOMemMaps array, and will
be used by the driver for accessing the memory mapped registers on the
controller. If any of these regions are not required, the MMIOBase
entries will be NULL and do not need to be mapped by the application.

Note:   The memory regions pointed to by the MMIOBase addresses have
        special meanings for the first two and second two addresses that are
        mapped. If the OS loader is running the driver in user space with a
        safety level of 2, then the only the first two base addresses will
        be mapped into user space, and the second two will be mapped only
        into kernel space (kernel space can also access the user space
        mappings). Please see QueryFunctions for a more detailed overview
        of the safety levels and how this relates to these regions.

The IOMemMaps member contains the mapped linear address of the memory mapped
register regions defined by the MMIOBase and MMIOLen members.

The TextMem member contains the mapped linear address of the text mode
framebuffer, and will be filled in by the application when it has loaded the
device driver. This provides the device driver with direct access to the
video memory on the controller when in text modes.

The BankedMem member contains the mapped linear address of the banked memory
framebuffer, and will be filled in by the application when it has loaded the
device driver. This provides the device driver with direct access to the
video memory on the controller when in the banked framebuffer modes.

The LinearMem member contains the mapped linear address of the linear memory
framebuffer, and will be filled in by the application when it has loaded the
device driver. This provides the device driver with direct access to the
video memory on the controller when in the linear framebuffer modes.

Note:   On some controllers the linear framebuffer address may be different
        for different color depths, so the value in this variable may change
        after initializing a mode. Applications should always reload the
        address of the linear framebuffer from this variable after
        initializing a mode set to ensure that the correct value is always
        used.

The ZBufferMem member contains the mapped linear address of the local
z-buffer memory, and will be filled in by the application when it has loaded
the device driver. This provides the device driver with direct access to the
local z-buffer memory on the controller. If the controller does not have
local z-buffer memory, this member will be set to NULL.

The TexBufferMem member contains the mapped linear address of the local
texture memory, and will be filled in by the application when it has loaded
the device driver. This provides the device driver with direct access to the
local texture memory on the controller. If the controller does not have
local texture memory, this member will be set to NULL.

The LockedMem member contains a pointer to the locked DMA memory buffer
allocated for the loaded driver. The graphics driver can use this pointer to
write data directly to the DMA buffer before transferring it to the hardware.
If the driver does not require DMA memory, this value will be set to NULL by
the loader.

The LockedMemPhys member contains the 32-bit physical memory address of the
locked DMA buffer memory allocated for the driver. The graphics driver can use
this physical address to set up DMA transfer operations for memory contained
within the DMA transfer buffer. If the driver does not require DMA memory,
this value will be set to 0 by the loader.

The TextFont8x8, TextFont8x14 and TextFont8x16 members contain pointers to
the 8x8, 8x14 and 8x16 text font bitmaps allocated by the OS loader. This
data is used by the driver for VGA and extended text modes that require the
bitmap font tables.

HEADER:
snap/graphics.h

MEMBERS:
Signature           - 'GRAPHICS\0' 20 byte signature
Version             - Driver Interface Version
DriverRev           - Driver revision number
OemVendorName       - Vendor Name string
OemCopyright        - Vendor Copyright string
AvailableModes      - Offset to supported mode table
DeviceIndex         - Device index for the driver when loaded from disk
TotalMemory         - Amount of memory in KB detected
Attributes          - Driver attributes
WorkArounds         - Hardware WorkArounds flags
TextSize            - Length of the text framebuffer in bytes
TextBasePtr         - Base address of the text framebuffer
BankSize            - Bank size in bytes (4KB or 64KB)
BankedBasePtr       - Physical addr of banked buffer
LinearSize          - Linear buffer size in bytes
LinearBasePtr       - Physical addr of linear buffer
ZBufferSize         - Z-buffer size in bytes
ZBufferBasePtr      - Physical addr of Z-buffer
TexBufferSize       - Texture buffer size in bytes
TexBufferBasePtr    - Physical addr of texture buffer
LockedMemSize       - Amount of locked memory for driver in bytes
IOBase              - Base address for I/O mapped registers (relocateable)
MMIOBase            - Base address of memory mapped I/O regions
MMIOLen             - Length of memory mapped I/O regions in bytes
DriverStart         - Pointer to the start of the driver in memory
DriverSize          - Size of the entire driver in memory in bytes
BusType             - Indicates the type of bus for the device (GA_busType)
AttributesExt       - Driver extended attributes flags
Shared              - True if the driver was loaded into shared memory
IOMemMaps           - Pointers to mapped I/O memory
BankedMem           - Ptr to mapped banked video mem
LinearMem           - Ptr to mapped linear video mem
ZBufferMem          - Ptr to mapped zbuffer mem
TexBufferMem        - Ptr to mapped texture buffer mem
LockedMem           - Ptr to allocated locked memory
LockedMemPhys       - Physical addr of locked memory
TextFont8x8         - Ptr to 8x8 text font data
TextFont8x14        - Ptr to 8x14 text font data
TextFont8x16        - Ptr to 8x16 text font data
VGAPal4             - Ptr to the default VGA 4bpp palette
VGAPal8             - Ptr to the default VGA 8bpp palette
loader              - Internal device driver loader functions
****************************************************************************/
typedef struct GA_devCtx {
    /*------------------------------------------------------------------*/
    /* Device driver header block                                       */
    /*------------------------------------------------------------------*/
    char            Signature[20];
    N_uint32        Version;
    N_uint32        DriverRev;
    char            OemVendorName[80];
    char            OemCopyright[80];
    N_uint16        _FAR_ *AvailableModes;
    N_int32         DeviceIndex;
    N_uint32        TotalMemory;
    N_uint32        Attributes;
    N_uint32        WorkArounds;
    N_uint32        TextSize;
    N_uint32        TextBasePtr;
    N_uint32        BankSize;
    N_uint32        BankedBasePtr;
    N_uint32        LinearSize;
    N_uint32        LinearBasePtr;
    N_uint32        ZBufferSize;
    N_uint32        ZBufferBasePtr;
    N_uint32        TexBufferSize;
    N_uint32        TexBufferBasePtr;
    N_uint32        LockedMemSize;
    N_uint32        IOBase;
    N_uint32        MMIOBase[4];
    N_uint32        MMIOLen[4];
    void            _FAR_ *DriverStart;
    N_uint32        DriverSize;
    N_uint32        BusType;
    N_uint32        AttributesExt;
    N_uint32        Shared;
    N_uint32        res1[17];

    /*------------------------------------------------------------------*/
    /* Near pointers mapped by loader for driver                        */
    /*------------------------------------------------------------------*/
    void            _FAR_ *IOMemMaps[4];
    void            _FAR_ *TextMem;
    void            _FAR_ *BankedMem;
    void            _FAR_ *LinearMem;
    void            _FAR_ *ZBufferMem;
    void            _FAR_ *TexBufferMem;
    void            _FAR_ *LockedMem;
    N_physAddr       LockedMemPhys;
    void            _FAR_ *TextFont8x8;
    void            _FAR_ *TextFont8x14;
    void            _FAR_ *TextFont8x16;
    GA_palette      _FAR_ *VGAPal4;
    GA_palette      _FAR_ *VGAPal8;
    N_uint32        res3[18];

    /*------------------------------------------------------------------*/
    /* Internal values user by OS specific code                         */
    /*------------------------------------------------------------------*/
    struct GA_devCtx _FAR_ *ring0DC;
    void            _FAR_ *pMdl;

    /*------------------------------------------------------------------*/
    /* Driver loading and initialization functions                      */
    /*------------------------------------------------------------------*/
    GA_loaderFuncs  loader;
    } GA_devCtx;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all the device driver functions related
to managing the hardware 2D graphics accelerator state. This group of
functions does not contain any functions that do any drawing on the
screen, just state management.

Generally applications or shell drivers should request this block of
functions from the 2d reference rasteriser library, not directly from
the graphics accelerator. This will allows the library to fill in
all rendering functions with software rendering as necessary automatically.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!

Note:   The dwSize member is of type N_uintptr to ensure that it has the
        same size as each of the function pointers in the structure.
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    N_int32         (NAPIP SetDrawBuffer)(GA_buffer *drawBuf);
    N_int32         (NAPIP IsIdle)(void);
    void            (NAPIP WaitTillIdle)(void);
    void            (NAPIP EnableDirectAccess)(void);
    void            (NAPIP DisableDirectAccess)(void);
    N_int32         (NAPIP SetMix)(N_int32 mix);
    void            (NAPIP SetForeColor)(GA_color color);
    void            (NAPIP SetBackColor)(GA_color color);
    void            (NAPIP Set8x8MonoPattern)(N_int32 index,GA_pattern *pattern);
    void            (NAPIP Use8x8MonoPattern)(N_int32 index);
    void            (NAPIP Use8x8TransMonoPattern)(N_int32 index);
    void            (NAPIP Set8x8ColorPattern)(N_int32 index,GA_colorPattern *pattern);
    void            (NAPIP Use8x8ColorPattern)(N_int32 index);
    void            (NAPIP Use8x8TransColorPattern)(N_int32 index,GA_color transparent);
    void            (NAPIP SetLineStipple)(GA_stipple stipple);
    void            (NAPIP SetLineStippleCount)(N_uint32 count);
    void            (NAPIP SetPlaneMask)(N_uint32 mask);
    void            (NAPIP SetSrcBlendFunc_Old)(N_int32 res1);
    void            (NAPIP SetDstBlendFunc_Old)(N_int32 res2);
    void            (NAPIP SetAlphaValue)(N_uint8 alpha);
    void            (NAPIP SetupForWindows)(void);
    void            (NAPIP ResetForWindows)(void);
    void            (NAPIP SetLineStyle)(N_uint32 styleMask,N_uint32 styleStep,N_uint32 styleValue);
    void            (NAPIP BuildTranslateVector)(GA_color *translate,GA_palette *dstPal,GA_palette *srcPal,int srcColors);
    void            (NAPIP SetBlendFunc)(N_int32 srcBlendFunc,N_int32 dstBlendFunc);
    void            (NAPIP Flush)(void);
    } GA_2DStateFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all the device driver functions related
to managing drawing in the framebuffer using the 2D graphics accelerator.
This group of functions does not contain any functions related to state
management, just drawing.

Generally applications or shell drivers should request this block of
functions from the 2d reference rasteriser library, not directly from
the graphics accelerator. This will allows the library to fill in
all rendering functions with software rendering as necessary automatically.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_color        (NAPIP GetPixel)(N_int32 x,N_int32 y);
    void            (NAPIP PutPixel)(N_int32 x,N_int32 y);
    void            (NAPIP DrawScanList)(N_int32 y,N_int32 length,N_int16 *scans);
    void            (NAPIP DrawPattScanList)(N_int32 y,N_int32 length,N_int16 *scans);
    void            (NAPIP DrawColorPattScanList)(N_int32 y,N_int32 length,N_int16 *scans);
    void            (NAPIP DrawEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawPattEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawColorPattEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawFatEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawPattFatEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawColorPattFatEllipseList)(N_int32 y,N_int32 length,N_int32 height,N_int16 *scans);
    void            (NAPIP DrawRect)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
    void            *res1;
    void            (NAPIP DrawPattRect)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
    void            (NAPIP DrawColorPattRect)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
    void            (NAPIP DrawTrap)(GA_trap *trap);
    void            (NAPIP DrawPattTrap)(GA_trap *trap);
    void            (NAPIP DrawColorPattTrap)(GA_trap *trap);
    void            (NAPIP DrawLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast);
    void            (NAPIP DrawBresenhamLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags);
    void            (NAPIP DrawStippleLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast,N_int32 transparent);
    void            (NAPIP DrawBresenhamStippleLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags,N_int32 transparent);
    void            (NAPIP DrawEllipse)(N_int32 left,N_int32 top,N_int32 A,N_int32 B);
    void            (NAPIP ClipEllipse)(N_int32 left,N_int32 top,N_int32 A,N_int32 B,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP PutMonoImageMSBSys_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 transparent);
    void            (NAPIP PutMonoImageMSBLin_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_int32 imageOfs,N_int32 transparent);
    void            (NAPIP PutMonoImageMSBBM_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 imagePhysAddr,N_int32 transparent);
    void            (NAPIP PutMonoImageLSBSys_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 transparent);
    void            (NAPIP PutMonoImageLSBLin_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_int32 imageOfs,N_int32 transparent);
    void            (NAPIP PutMonoImageLSBBM_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 imagePhysAddr,N_int32 transparent);
    void            (NAPIP ClipMonoImageMSBSys_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP ClipMonoImageMSBLin_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_int32 imageOfs,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP ClipMonoImageMSBBM_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 imagePhysAddr,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP ClipMonoImageLSBSys_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP ClipMonoImageLSBLin_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_int32 imageOfs,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP ClipMonoImageLSBBM_Old)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 imagePhysAddr,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP BitBlt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP BitBltLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP BitBltSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,N_int32 flipY);
    void            (NAPIP BitBltBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP BitBltPatt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltPattLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltPattSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3,N_int32 flipY);
    void            (NAPIP BitBltPattBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltColorPatt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltColorPattLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltColorPattSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3,N_int32 flipY);
    void            (NAPIP BitBltColorPattBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP SrcTransBlt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP SrcTransBltLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP SrcTransBltSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent,N_int32 flipY);
    void            (NAPIP SrcTransBltBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP DstTransBlt)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP DstTransBltLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP DstTransBltSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent,N_int32 flipY);
    void            (NAPIP DstTransBltBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP StretchBlt)(N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 mix);
    void            (NAPIP StretchBltLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 mix);
    void            (NAPIP StretchBltSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 mix,N_int32 flipY);
    void            (NAPIP StretchBltBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 mix);
    N_int32         (NAPIP ConvertBltSys_Old)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,GA_palette *dstPal,GA_palette *srcPal,N_int32 dither,N_int32 mix,N_int32 flipY);
    N_int32         (NAPIP ConvertBltBM_Old)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,GA_palette *dstPal,GA_palette *srcPal,N_int32 dither,N_int32 mix);
    N_int32         (NAPIP StretchConvertBltSys_Old)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,GA_palette *dstPal,GA_palette *srcPal,N_int32 dither,N_int32 mix,N_int32 flipY);
    N_int32         (NAPIP StretchConvertBltBM_Old)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,GA_palette *dstPal,GA_palette *srcPal,N_int32 dither,N_int32 mix);
    N_int32         (NAPIP BitBltFxTest)(GA_bltFx *fx);
    void            (NAPIP BitBltFx)(N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,GA_bltFx *fx);
    void            (NAPIP BitBltFxLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,GA_bltFx *fx);
    void            (NAPIP BitBltFxSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,GA_bltFx *fx);
    void            (NAPIP BitBltFxBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,GA_bltFx *fx);
    void            (NAPIP GetBitmapSys)(void *dstAddr,N_int32 dstPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP GetBitmapBM)(void *dstAddr,N_int32 dstPhysAddr,N_int32 dstPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP UpdateScreen)(N_int32 left,N_int32 top,N_int32 width,N_int32 height);
    N_int32         (NAPIP DrawClippedLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    N_int32         (NAPIP DrawClippedBresenhamLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    N_int32         (NAPIP DrawClippedStippleLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    N_int32         (NAPIP DrawClippedBresenhamStippleLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP BitBltPlaneMasked)(N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_uint32 planeMask);
    void            (NAPIP BitBltPlaneMaskedLin)(N_int32 srcOfs,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_uint32 planeMask);
    void            (NAPIP BitBltPlaneMaskedSys)(void *srcAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_uint32 planeMask,N_int32 flipY);
    void            (NAPIP BitBltPlaneMaskedBM)(void *srcAddr,N_int32 srcPhysAddr,N_int32 srcPitch,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_uint32 planeMask);
    void            (NAPIP DrawRectLin)(N_int32 dstOfs,N_int32 dstPitch,N_int32 left,N_int32 top,N_int32 width,N_int32 height,GA_color color,N_int32 mix);
    void            (NAPIP DrawRectExt)(N_int32 left,N_int32 top,N_int32 width,N_int32 height,GA_color color,N_int32 mix);
    void            (NAPIP DrawStyleLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast,N_int32 transparent);
    void            (NAPIP DrawBresenhamStyleLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags,N_int32 transparent);
    N_int32         (NAPIP DrawClippedStyleLineInt)(N_int32 x1,N_int32 y1,N_int32 x2,N_int32 y2,N_int32 drawLast,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    N_int32         (NAPIP DrawClippedBresenhamStyleLine)(N_int32 x1,N_int32 y1,N_int32 initialError,N_int32 majorInc,N_int32 diagInc,N_int32 count,N_int32 flags,N_int32 transparent,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom);
    void            (NAPIP MonoBltSys)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 skipLeft,GA_color foreColor,GA_color backColor,N_int32 mix,N_int32 flags);
    void            (NAPIP MonoBltLin)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_int32 imageOfs,N_int32 skipLeft,GA_color foreColor,GA_color backColor,N_int32 mix,N_int32 flags);
    void            (NAPIP MonoBltBM)(N_int32 x,N_int32 y,N_int32 width,N_int32 height,N_int32 byteWidth,N_uint8 *image,N_int32 imagePhysAddr,N_int32 skipLeft,GA_color foreColor,GA_color backColor,N_int32 mix,N_int32 flags);
    } GA_2DRenderFuncs;

/* {secret} Forward declaration for REF2D_driver structure */
struct _REF2D_driver;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all device driver init functions available
for the device. These functions include all mode information, setup and
initialisation functions.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    void            (NAPIP GetConfigInfo)(GA_configInfo *info);
    void            *res1;
    void            (NAPIP SetModeProfile)(GA_modeProfile *profile);
    void            (NAPIP GetOptions)(GA_options *options);
    void            (NAPIP SetOptions)(GA_options *options);
    N_int32         (NAPIP GetVideoModeInfo)(N_uint32 mode,GA_modeInfo *modeInfo);
    N_int32         (NAPIP SetVideoMode)(N_uint32 mode,N_int32 *virtualX,N_int32 *virtualY,N_int32 *bytesPerLine,N_int32 *maxMem,N_int32 refreshRate,GA_CRTCInfo *crtc);
    N_uint32        (NAPIP GetVideoMode)(void);
    N_int32         (NAPIP GetCustomVideoModeInfo)(N_int32 xRes,N_int32 yRes,N_int32 virtualX,N_int32 virtualY,N_int32 bitsPerPixel,GA_modeInfo *modeInfo);
    N_int32         (NAPIP SetCustomVideoMode)(N_int32 xRes,N_int32 yRes,N_int32 bitsPerPixel,N_uint32 flags,N_int32 *virtualX,N_int32 *virtualY,N_int32 *bytesPerLine,N_int32 *maxMem,GA_CRTCInfo *crtc);
    N_uint32        (NAPIP GetClosestPixelClockV1)(N_int32 xRes,N_int32 yRes,N_int32 bitsPerPixel,N_uint32 pixelClock);
    void            (NAPIP GetCRTCTimings)(GA_CRTCInfo *crtc);
    void            (NAPIP SaveCRTCTimings)(GA_CRTCInfo *crtc);
    void            (NAPIP SetGlobalRefresh)(N_int32 refresh,N_int32 outputHead);
    N_int32         (NAPIP SaveRestoreState)(N_int32 subfunc,void *saveBuf);
    N_int32         (NAPIP SetDisplayOutput)(N_int32 device);
    N_int32         (NAPIP GetDisplayOutput)(void);
    void            (NAPIP SetSoftwareRenderFuncs)(GA_2DRenderFuncs *softwareFuncs);
    void            (NAPIP GetUniqueFilename)(char *filename,int type);
    void            *res2;
    void            (NAPIP GetMonitorInfo)(GA_monitor *monitor,N_int32 outputHead);
    void            (NAPIP SetMonitorInfo)(GA_monitor *monitor,N_int32 outputHead);
    void            (NAPIP GetCurrentVideoModeInfo)(GA_modeInfo *modeInfo);
    void            (NAPIP GetCertifyInfo)(GA_certifyInfo *info);
    void            (NAPIP SetCRTCTimings)(GA_CRTCInfo *crtc);
    ibool           (NAPIP AlignLinearBuffer)(N_int32 height,N_int32 *stride,N_int32 *offset,N_int32 *size,N_int32 growUp);
    void            *res3;
    N_int32         (NAPIP GetCurrentRefreshRate)(void);
    N_int32         (NAPIP PollForDisplaySwitch)(void);
    void            (NAPIP PerformDisplaySwitch)(void);
    N_int32         (NAPIP GetVideoModeInfoExt)(N_uint32 mode,GA_modeInfo *modeInfo,N_int32 outputDevice,N_int32 outputHead);
    N_int32         (NAPIP GetCustomVideoModeInfoExt)(N_int32 xRes,N_int32 yRes,N_int32 virtualX,N_int32 virtualY,N_int32 bitsPerPixel,GA_modeInfo *modeInfo,N_int32 outputDevice,N_int32 outputHead);
    N_int32         (NAPIP SwitchPhysicalResolution)(N_int32 physicalXResolution,N_int32 physicalYResolution,N_int32 refreshRate);
    N_int32         (NAPIP GetNumberOfHeads)(void);
    N_int32         (NAPIP SetActiveHead)(N_int32 headIndex);
    N_int32         (NAPIP GetActiveHead)(void);
    void            (NAPIP SetRef2dPointer)(struct _REF2D_driver *ref2d);
    N_int32         (NAPIP SetDisplayOutputExt)(N_int32 device,N_int32 outputHead);
    N_int32         (NAPIP GetDisplayOutputExt)(N_int32 outputHead);
    N_uint32        (NAPIP GetClosestPixelClock)(N_int32 outputHead,N_int32 xRes,N_int32 yRes,N_int32 bitsPerPixel,N_uint32 pixelClock);
    N_int32         (NAPIP RoundSoftwareStride)(N_int32 hwStride,N_int32 bitsPerPixel);
    void *          (NAPIP MapDrawBuffer)(N_int32 mapIndex,GA_buf *buf);
    void            (NAPIP UnMapDrawBuffer)(N_int32 mapIndex,GA_buf *buf);
    } GA_initFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all main device driver functions not related
to mode initialisation and setup, and not related to drawing and state
management. This includes support for things such as changing framebuffer
banks, changing the displayed video memory start address and program the
hardware palette.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    void            (NAPIP SetBank)(N_int32 bank);
    void            (NAPIP SetDisplayStart)(N_int32 offset,N_int32 waitVRT);
    void            (NAPIP SetDisplayStartXY_Obsolete)(N_int32 x,N_int32 y,N_int32 waitVRT);
    void            (NAPIP SetStereoDisplayStart)(N_int32 leftOffset,N_int32 rightOffset,N_int32 waitVRT);
    N_int32         (NAPIP GetDisplayStartStatus)(void);
    void            (NAPIP EnableStereoMode)(N_int32 enable);
    N_int32         (NAPIP IsVSync)(void);
    void            (NAPIP WaitVSync)(void);
    void            (NAPIP SetVSyncWidth)(N_int32 width);
    N_int32         (NAPIP GetVSyncWidth)(void);
    void            (NAPIP SetPaletteData)(GA_palette *pal,N_int32 num,N_int32 index,N_int32 waitVRT);
    void            (NAPIP GetPaletteData)(GA_palette *pal,N_int32 num,N_int32 index);
    void            (NAPIP SetGammaCorrectData)(GA_palette *pal,N_int32 num,N_int32 index,N_int32 waitVRT);
    void            (NAPIP GetGammaCorrectData)(GA_palette *pal,N_int32 num,N_int32 index);
    N_int32         (NAPIP GetCurrentScanLine)(void);
    void            (NAPIP SetPaletteDataExt)(GA_paletteExt *pal,N_int32 num,N_int32 index,N_int32 waitVRT);
    void            (NAPIP GetPaletteDataExt)(GA_paletteExt *pal,N_int32 num,N_int32 index);
    void            (NAPIP SetGammaCorrectDataExt)(GA_paletteExt *pal,N_int32 num,N_int32 index,N_int32 waitVRT);
    void            (NAPIP GetGammaCorrectDataExt)(GA_paletteExt *pal,N_int32 num,N_int32 index);
    } GA_driverFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all VBE/Core emulation functions. These functions
should generally not be used by application programs directly, as they are
only intended to be used by the VBE/Core emulation driver.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    ibool           (NAPIP SetBytesPerLine)(int bytesPerLine,int *newBytes);
    ibool           (NAPIP Set8BitDAC)(ibool enable);
    void            (NAPIP SetPaletteData)(GA_palette *pal,N_int32 num,N_int32 index,N_int32 waitVRT);
    void            (NAPIP GetPaletteData)(GA_palette *pal,N_int32 num,N_int32 index);
    } GA_VBEFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all hardware video overlay functions available
for the device. These functions are used to create, destroy and manage
hardware video overlay buffers used for hardware video playback.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_buf *        (NAPIP AllocVideoBuffers)(N_int32 width,N_int32 height,N_int32 format,N_int32 numBuffers);
    ibool           (NAPIP FreeVideoBuffers)(GA_buf *buf);
    GA_buf *        (NAPIP GetAttachedVideoBuffer)(GA_buf *buf,N_int32 index);
    N_int32         (NAPIP SetVideoOutput)(N_int32 overlayWindow,N_int32 outputHead,GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 flags);
    void            (NAPIP DisableVideoOutput)(N_int32 overlayWindow);
    void            (NAPIP MoveVideoOutput)(N_int32 overlayWindow,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 waitVRT);
    void            (NAPIP FlipVideoOutput)(N_int32 overlayWindow,GA_buf *buf,N_int32 waitVRT);
    void            (NAPIP SetDstVideoColorKey)(N_int32 overlayWindow,GA_color colorKeyLo,GA_color colorKeyHi);
    void            (NAPIP SetVideoBrightness)(N_int32 overlayWindow,N_int32 brightness);
    void            (NAPIP SetVideoContrast)(N_int32 overlayWindow,N_int32 contrast);
    void            (NAPIP SetVideoSaturation)(N_int32 overlayWindow,N_int32 saturation);
    void            (NAPIP SetVideoHue)(N_int32 overlayWindow,N_int32 hue);
    void            (NAPIP SetVideoGammaCorrect)(N_int32 overlayWindow,N_int32 gamma);
    } GA_videoFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all hardware video capture functions available
for the device. These functions are used to create, destroy and manage
hardware video capture buffers used for hardware video input.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_captureBuf * (NAPIP AllocCaptureBuffer)(N_int32 width,N_int32 height,N_int32 format,N_int32 numBuffers);
    ibool           (NAPIP FreeCaptureBuffer)(GA_captureBuf *buf);
    N_int32         (NAPIP SetVideoInput)(GA_captureBuf *buf,N_int32 inputType,N_int32 inputStandard,N_int32 inputSource,N_int32 syncOnGreen);
    N_int32         (NAPIP StartLiveVideo)(GA_captureBuf *buf);
    N_int32         (NAPIP FreezeLiveVideo)(GA_captureBuf *buf,N_int32 alignField);
    N_int32         (NAPIP WaitField)(GA_captureBuf *buf,N_int32 fieldType);
    N_int32         (NAPIP WaitHSync)(GA_captureBuf *buf,N_int32 syncCount);
    N_int32         (NAPIP WaitVSync)(GA_captureBuf *buf,N_int32 syncCount);
    N_int32         (NAPIP GetLiveStatus)(GA_captureBuf *buf);
    N_int32         (NAPIP GetInputType)(GA_captureBuf *buf);
    N_int32         (NAPIP GetInputStandard)(GA_captureBuf *buf);
    N_int32         (NAPIP GetInputSource)(GA_captureBuf *buf);
    } GA_videoCaptureFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all Hardware Motion Compensation and IDCT functions
available for the device. These functions are used for hardware video
playback of MPEG compressed video streams.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_buf *        (NAPIP AllocSurface)(N_int32 mcType,N_int32 width,N_int32 height,N_uint32 flags);
    void            (NAPIP FreeSurface)(GA_buf *surf);
    N_int32         (NAPIP RenderSurface)(N_int32 pictureStructure,GA_buf *targetSurface,GA_buf *pastSurface,GA_buf *futureSurface,N_uint32 flags,N_int32 numMacroBlocks,N_int32 firstMacroBlock,GA_HwMCMacroBlock *macroBlocks,N_int16 *blocks);
    void            (NAPIP SyncSurface)(GA_buf *surf);
    void            (NAPIP FlushSurface)(GA_buf *surf);
    N_int32         (NAPIP GetSurfaceStatus)(GA_buf *surf);
    GA_buf *        (NAPIP AllocSubPicture)(N_int32 width,N_int32 height,N_uint32 format);
    void            (NAPIP FreeSubPicture)(GA_buf *subPicture);
    N_int32         (NAPIP BlendSubPicture)(GA_buf *sourceSurface,GA_buf *targetSurface,GA_buf *subPicture,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight);
    N_int32         (NAPIP BlendSubPictureBackEnd)(GA_buf *targetSurface,GA_buf *subPicture,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight);
    void            (NAPIP SyncSubPicture)(GA_buf *subPicture);
    void            (NAPIP FlushSubPicture)(GA_buf *subPicture);
    N_int32         (NAPIP GetSubPictureStatus)(GA_buf *subPicture);
    } GA_HwMCFuncs;

/****************************************************************************
REMARKS:
Structure defining all the software stereo functions passed into the
buffer manager to enable stereo support. This is presently undocumented
as it is Windows specific and not very portable to other platforms.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    ibool           (NAPIP SoftStereoInit)(GA_devCtx *dc);
    void            (NAPIP SoftStereoOn)(void);
    void            (NAPIP SoftStereoScheduleFlip)(N_uint32 leftAddr,N_uint32 rightAddr);
    N_int32         (NAPIP SoftStereoGetFlipStatus)(void);
    void            (NAPIP SoftStereoWaitTillFlipped)(void);
    void            (NAPIP SoftStereoOff)(void);
    void            (NAPIP SoftStereoExit)(void);
    } GA_softStereoFuncs;

/* {partOf:GA_regionFuncs::TraverseRegion} */
typedef void (NAPIP GA_regionCallback)(const GA_rect *r,void *parms);

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all region management functions available
via the SNAP API's. These functions may be used externally for
complex region management, but they are also used extensively by SNAP
Graphics for offscreen buffer management.

Note also that this function group is /only/ returned by the 2D reference
rasteriser library, and not by hardware drivers.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_region *     (NAPIP NewRegion)(void);
    GA_region *     (NAPIP NewRectRegion)(N_int32 left,N_int32 top,N_int32 right,N_int32 bottom);
    GA_region *     (NAPIP CopyRegion)(const GA_region *s);
    void            (NAPIP CopyIntoRegion)(GA_region *d,const GA_region *s);
    void            (NAPIP ClearRegion)(GA_region *r);
    void            (NAPIP FreeRegion)(GA_region *r);
    GA_region *     (NAPIP SectRegion)(const GA_region *r1,const GA_region *r2);
    GA_region *     (NAPIP SectRegionRect)(const GA_region *r1,const GA_rect *r2);
    ibool           (NAPIP UnionRegion)(GA_region *r1,const GA_region *r2);
    ibool           (NAPIP UnionRegionRect)(GA_region *r1,const GA_rect *r2);
    ibool           (NAPIP UnionRegionOfs)(GA_region *r1,const GA_region *r2,N_int32 dx,N_int32 dy);
    ibool           (NAPIP DiffRegion)(GA_region *r1,const GA_region *r2);
    ibool           (NAPIP DiffRegionRect)(GA_region *r1,const GA_rect *r2);
    void            (NAPIP OptimizeRegion)(GA_region *r);
    void            (NAPIP OffsetRegion)(GA_region *r,N_int32 dx,N_int32 dy);
    ibool           (NAPIP IsEmptyRegion)(const GA_region *r);
    ibool           (NAPIP IsEqualRegion)(const GA_region *r1,const GA_region *r2);
    ibool           (NAPIP PtInRegion)(const GA_region *r,N_int32 x,N_int32 y);
    void            (NAPIP TraverseRegion)(GA_region *rgn,GA_regionCallback doRect,void *parms);
    } GA_regionFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all offscreen buffer management function available
via the SNAP API's. This function group is /only/ returned by the 2D
reference rasteriser library, and not by hardware drivers.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    ibool           (NAPIP InitBuffers)(N_int32 numBuffers,N_uint32 flags,GA_softStereoFuncs *stereo);
    GA_buf *        (NAPIP GetPrimaryBuffer)(void);
    GA_buf *        (NAPIP GetFlippableBuffer)(N_int32 index);
    GA_buf *        (NAPIP AllocBuffer)(N_int32 width,N_int32 height,N_int32 flags);
    ibool           (NAPIP FreeBuffer)(GA_buf *buf);
    N_uint32        (NAPIP LockBuffer)(GA_buf *buf);
    void            (NAPIP UnlockBuffer)(GA_buf *buf);
    N_int32         (NAPIP SetActiveBuffer)(GA_buf *buf);
    void            (NAPIP FlipToBuffer)(GA_buf *buf,N_int32 waitVRT);
    void            (NAPIP FlipToStereoBuffer)(GA_buf *left,GA_buf *right,N_int32 waitVRT);
    int             (NAPIP GetFlipStatus)(void);
    void            (NAPIP WaitTillFlipped)(void);
    void            (NAPIP UpdateCache)(GA_buf *buf);
    void            (NAPIP UpdateFromCache)(GA_buf *buf);
    void            (NAPIP BitBltBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix);
    void            (NAPIP BitBltPattBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP BitBltColorPattBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 rop3);
    void            (NAPIP SrcTransBltBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP DstTransBltBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_int32 mix,GA_color transparent);
    void            (NAPIP StretchBltBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,N_int32 doClip,N_int32 clipLeft,N_int32 clipTop,N_int32 clipRight,N_int32 clipBottom,N_int32 mix);
    void            (NAPIP BitBltPlaneMaskedBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 width,N_int32 height,N_int32 dstLeft,N_int32 dstTop,N_uint32 planeMask);
    void            (NAPIP BitBltFxBuf)(GA_buf *buf,N_int32 srcLeft,N_int32 srcTop,N_int32 srcWidth,N_int32 srcHeight,N_int32 dstLeft,N_int32 dstTop,N_int32 dstWidth,N_int32 dstHeight,GA_bltFx *fx);
    void            (NAPIP DrawRectBuf)(GA_buf *buf,N_int32 left,N_int32 top,N_int32 width,N_int32 height,GA_color color,N_int32 mix);
    ibool           (NAPIP SetRef2d)(struct _REF2D_driver *ref2d);
    GA_clipper *    (NAPIP GetClipper)(GA_buf *buf);
    void            (NAPIP SetClipper)(GA_buf *buf,GA_clipper *clipper);
    GA_buf *        (NAPIP AllocDepthBuffer)(N_int32 width,N_int32 height,N_int32 format);
    GA_buf *        (NAPIP AllocStencilBuffer)(N_int32 width,N_int32 height,N_int32 format);
    GA_buf *        (NAPIP AllocTexture)(N_int32 width,N_int32 height,N_int32 format);
    GA_buf *        (NAPIP AllocVideoTexture)(N_int32 width,N_int32 height,N_int32 format);
    ibool           (NAPIP SetTexturePalette)(GA_buf *buf,GA_palette *pal,N_int32 num,N_int32 index);
    ibool           (NAPIP GetTexturePalette)(GA_buf *buf,GA_palette *pal,N_int32 num,N_int32 index);
    ibool           (NAPIP SetTexturePaletteExt)(GA_buf *buf,GA_paletteExt *pal,N_int32 num,N_int32 index);
    ibool           (NAPIP GetTexturePaletteExt)(GA_buf *buf,GA_paletteExt *pal,N_int32 num,N_int32 index);
    GA_buf *        (NAPIP GetActiveBuffer)(void);
    GA_buf *        (NAPIP GetVisibleBuffer)(void);
    GA_buf *        (NAPIP GetVisibleRightBuffer)(void);
    GA_buf *        (NAPIP AllocBufferExt)(N_int32 width,N_int32 height,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,N_int32 flags);
    GA_buf *        (NAPIP AllocCustomBuffer)(N_int32 width,N_int32 height,N_int32 bitsPerPixel,GA_pixelFormat *pixelFormat,void *surface);
    void            (NAPIP FlipToBufferXY)(GA_buf *buf,N_int32 x,N_int32 y,N_int32 waitVRT);
    GA_buf *        (NAPIP AllocCommandBuffer)(N_int32 size,N_int32 flags);
    } GA_bufferFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all offscreen buffer management window clipper
functions available via the SNAP API's. These functions manage the creation
and destruction of complex clip regions for handling clipping to window
manager windows. These functions manage translation of OS specific clip
regions to SciTech SNAP Graphics complex clip regions. This function group
is /only/ returned by the 2D reference rasteriser library, and not by hardware
drivers.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_clipper *    (NAPIP CreateClipper)(PM_HWND hwnd);
    ibool           (NAPIP IsClipListChanged)(GA_clipper *clipper);
    GA_rect *       (NAPIP GetClipList)(GA_clipper *clipper,N_int32 *count);
    void            (NAPIP DestroyClipper)(GA_clipper *clipper);
    } GA_clipperFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all hardware cursor functions available
for the device. These functions are used to manage the hardware cursor
functions for the device, such as setting the cursor, setting the cursor
position and changing the cursor colors.

Note also that this function group is also returned by the 2D reference
rasteriser code to implement software cursor functions when there is no
hardware cursor support in the hardware. Unless you have a specific need
to directly manage the hardware cursor, you should always use the
functions provided by the 2D reference rasteriser. The 2D reference
rasteriser will handle any hardware specific limitations for you,
defaulting back to a software cursor when necessary to ensure correct
operation.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    void            (NAPIP SetMonoCursor)(GA_monoCursor *cursor);
    void            (NAPIP SetMonoCursorColor)(GA_palette *foreground,GA_palette *background);
    void            (NAPIP SetColorCursor)(GA_colorCursor *cursor);
    N_int32         (NAPIP SetCursorPos)(N_int32 x,N_int32 y);
    void            (NAPIP ShowCursor)(N_int32 visible);
    void            (NAPIP BeginAccess)(N_int32 left,N_int32 top,N_int32 right,N_int32 bottom);
    void            (NAPIP EndAccess)(void);
    void            (NAPIP SetColorCursor256)(GA_colorCursor256 *cursor);
    void            (NAPIP SetColorCursorRGBA)(GA_colorCursorRGBA *cursor);
    N_int32         (NAPIP IsHardwareCursor)(void);
    void            (NAPIP SetColorCursorRGB)(GA_colorCursorRGB *cursor);
    } GA_cursorFuncs;

/****************************************************************************
HEADER:
snap/graphics.h

REMARKS:
Function group containing all the functions for implementing the VESA DPVL
mode support in the device driver.

Note:   Be sure to fill in the dwSize member of this structure when you
        call GA_queryFunctions to the correct size of the structure at
        compile time!
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    N_int32         (NAPIP Start)(void);
    void            (NAPIP UpdateRect)(N_int32 x,N_int32 y,N_int32 width,N_int32 height);
    void            (NAPIP UpdateNextRect)(void);
    void            (NAPIP Stop)(void);
    } GA_DPVLFuncs;

#pragma pack()

/* {secret} */
struct __GA_exports;

/* {secret} */
struct GA_glCtx;

#include "snap/ddc.h"
#include "snap/stereo.h"
#include "snap/ref2d.h"

#pragma pack(1)

#ifndef __WINDOWS16__
/****************************************************************************
REMARKS:
Structure defining all the SNAP Graphics API functions as exported from
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct __GA_exports {
    N_uintptr       dwSize;
    int             (NAPIP GA_status)(void);
    const char *    (NAPIP GA_errorMsg)(N_int32 status);
    int             (NAPIP GA_getDaysLeft)(N_int32 shared);
    int             (NAPIP GA_registerLicense)(uchar *license,N_int32 shared);
    int             (NAPIP GA_enumerateDevices)(N_int32 shared);
    GA_devCtx *     (NAPIP GA_loadDriver)(N_int32 deviceIndex,N_int32 shared);
    ibool           (NAPIP GA_setActiveDevice)(N_int32 deviceIndex);
    void *          reserved1;
    void            (NAPIP GA_unloadDriver)(GA_devCtx *dc);
    ibool           (NAPIP REF2D_loadDriver)(GA_devCtx *hwCtx,N_int32 unused,N_int32 shared,REF2D_driver _FAR_ *_FAR_ *drv,MOD_MODULE _FAR_ *_FAR_ *hModule,ulong _FAR_ *size);
    void            (NAPIP REF2D_unloadDriver)(REF2D_driver *drv,MOD_MODULE *hModule);
    ibool           (NAPIP GA_loadRef2d)(GA_devCtx _FAR_ *dc,N_int32 unused,GA_modeInfo _FAR_ *modeInfo,N_int32 transferStart,REF2D_driver _FAR_ *_FAR_ *drv);
    void            (NAPIP GA_unloadRef2d)(GA_devCtx _FAR_ *dc);
    ibool           (NAPIP GA_softStereoInit)(GA_devCtx *dc);
    void            (NAPIP GA_softStereoOn)(void);
    void            (NAPIP GA_softStereoScheduleFlip)(N_uint32 leftAddr,N_uint32 rightAddr);
    N_int32         (NAPIP GA_softStereoGetFlipStatus)(void);
    void            (NAPIP GA_softStereoWaitTillFlipped)(void);
    void            (NAPIP GA_softStereoOff)(void);
    void            (NAPIP GA_softStereoExit)(void);
    ibool           (NAPIP GA_saveModeProfile)(GA_devCtx *dc,GA_modeProfile *profile);
    ibool           (NAPIP GA_saveOptions)(GA_devCtx *dc,GA_options *options);
    ibool           (NAPIP GA_saveCRTCTimingsV1)(GA_devCtx *dc);
    ibool           (NAPIP GA_restoreCRTCTimingsV1)(GA_devCtx *dc);
    int             (NAPIP DDC_init)(GA_devCtx *dc);
    ibool           (NAPIP DDC_readEDID)(N_int32 slaveAddr,uchar *edid,N_int32 length,N_int32 blockNumber,N_int32 channel);
    int             (NAPIP EDID_parse)(uchar *edid,EDID_record *rec,N_int32 requireDescriptor);
    int             (NAPIP MCS_begin)(GA_devCtx *dc);
    int             (NAPIP MCS_getCapabilitiesString)(char *data,N_int32 maxlen);
    ibool           (NAPIP MCS_isControlSupported)(uchar controlCode);
    ibool           (NAPIP MCS_enableControl)(uchar controlCode,N_int32 enable);
    ibool           (NAPIP MCS_getControlMax)(uchar controlCode,ushort *max);
    ibool           (NAPIP MCS_getControlValue)(uchar controlCode,ushort *value);
    ibool           (NAPIP MCS_getControlValues)(N_int32 numControls,uchar *controlCodes,ushort *values);
    ibool           (NAPIP MCS_setControlValue)(uchar controlCode,ushort value);
    ibool           (NAPIP MCS_setControlValues)(N_int32 numControls,uchar *controlCodes,ushort *values);
    ibool           (NAPIP MCS_resetControl)(uchar controlCode);
    ibool           (NAPIP MCS_saveCurrentSettings)(void);
    ibool           (NAPIP MCS_getTimingReport)(uchar *flags,ushort *hFreq,ushort *vFreq);
    ibool           (NAPIP MCS_getSelfTestReport)(uchar *flags,uchar *data,uchar *length);
    void            (NAPIP MCS_end)(void);
    ibool           (NAPIP GA_loadInGUI)(N_int32 shared);
    ibool           (NAPIP DDC_writeEDID)(GA_devCtx *dc,N_int32 slaveAddr,uchar *edid,N_int32 length,N_int32 blockNumber,N_int32 channel);
    ibool           (NAPIP GA_useDoubleScan)(GA_modeInfo *modeInfo);
    void            (NAPIP GA_getMaxRefreshRateV1)(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 interlaced,float *maxRefresh);
    ibool           (NAPIP GA_computeCRTCTimingsV1)(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 refreshRate,N_int32 interlaced,GA_CRTCInfo *crtc,N_int32 pureGTF);
    ibool           (NAPIP GA_addMode)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
    ibool           (NAPIP GA_addRefresh)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 refresh,N_int32 saveToDisk);
    ibool           (NAPIP GA_delMode)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
    const char *    (NAPIP N_getLogName)(void);
    void            (NAPIP N_log)(const char *fmt,...);
    int             (NAPIP MDBX_getErrCode)(void);
    const char *    (NAPIP MDBX_getErrorMsg)(void);
    ibool           (NAPIP MDBX_open)(const char *filename);
    void            (NAPIP MDBX_close)(void);
    int             (NAPIP MDBX_first)(GA_monitor *rec);
    int             (NAPIP MDBX_last)(GA_monitor *rec);
    int             (NAPIP MDBX_next)(GA_monitor *rec);
    int             (NAPIP MDBX_prev)(GA_monitor *rec);
    int             (NAPIP MDBX_insert)(GA_monitor *rec);
    int             (NAPIP MDBX_update)(GA_monitor *rec);
    int             (NAPIP MDBX_flush)(void);
    int             (NAPIP MDBX_importINF)(const char *filename,char *mfr);
    void            (NAPIP GA_getGlobalOptions)(GA_globalOptions *options,ibool shared);
    void            (NAPIP GA_setGlobalOptions)(GA_globalOptions *options);
    ibool           (NAPIP GA_saveGlobalOptions)(GA_globalOptions *options);
    const char *    (NAPIP GA_getInternalName)(N_int32 deviceIndex);
    const char *    (NAPIP GA_getSNAPConfigPath)(void);
    const char *    (NAPIP GA_getFakePCIID)(void);
    MOD_MODULE *    (NAPIP GA_loadLibrary)(const char *szBPDName,ulong *size,ibool shared);
    int             (NAPIP GA_isOEMVersion)(ibool shared);
    int             (NAPIP GA_isLiteVersion)(ibool shared);
    const char *    (NAPIP GA_getDisplaySerialNo)(ibool shared);
    const char *    (NAPIP GA_getDisplayUserName)(ibool shared);
    void *          reserved2;
    REF2D_driver *  (NAPIP GA_getCurrentRef2d)(N_int32 deviceIndex);
    N_uint32 *      (NAPIP GA_getLicensedDevices)(ibool shared);
    int             (NAPIP DDC_initExt)(GA_devCtx *dc,N_int32 channel);
    int             (NAPIP MCS_beginExt)(GA_devCtx *dc,N_int32 channel);
    ibool           (NAPIP GA_loadRegionMgr)(GA_regionFuncs *funcs,MOD_MODULE **hModule,ulong *size,ibool shared);
    void            (NAPIP GA_unloadRegionMgr)(MOD_MODULE *hModule);
    void *          (NAPIP GA_getProcAddress)(MOD_MODULE *hModule,const char *szProcName);
    ibool           (NAPIP GA_enableVBEMode)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
    ibool           (NAPIP GA_disableVBEMode)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
    void            (NAPIP GA_loadModeProfile)(GA_devCtx *dc,GA_modeProfile *profile);
    ibool           (NAPIP GA_getCRTCTimingsV1)(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 refreshRate,GA_CRTCInfo *crtc);
    ibool           (NAPIP GA_setCRTCTimingsV1)(GA_devCtx *dc,GA_modeInfo *modeInfo,GA_CRTCInfo *crtc,ibool makeDefault);
    ibool           (NAPIP GA_setDefaultRefreshV1)(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 refreshRate,ibool saveToDisk);
    ibool           (NAPIP GA_saveMonitorInfoV1)(GA_devCtx *dc,GA_monitor *monitor);
    int             (NAPIP GA_detectPnPMonitorV1)(GA_devCtx *dc,GA_monitor *monitor,ibool *hasChanged);
    ibool           (NAPIP GA_queryFunctions)(GA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);
    ibool           (NAPIP REF2D_queryFunctions)(REF2D_driver *ref2d,N_uint32 id,void _FAR_ *funcs);
    int             (NAPIP GA_getParsedEDID)(GA_devCtx *dc,EDID_record *rec);
    void            (NAPIP GA_programMTRRegisters)(GA_devCtx *dc);
    void            (NAPIP GA_setExportTable)(struct __GA_exports *table);
    void            (NAPIP GA_setMinimumDriverVersion)(N_uint32 version,N_int32 allowFallback,N_int32 shared);
    ibool           (NAPIP GA_getRecommendedModesV1)(GA_devCtx *dc,GA_recMode *recommendedMode,GA_recMode *recommendedMode8,GA_recMode *recommendedMode16,GA_recMode *recommendedMode24,GA_recMode *recommendedMode32);
    struct GA_glCtx * (NAPIP GA_glLoadDriver)(GA_devCtx *hwCtx);
    ibool           (NAPIP GA_glQueryFunctions)(struct GA_glCtx *glCtx,N_uint32 id,void _FAR_ *funcs);
    void            (NAPIP GA_glUnloadDriver)(struct GA_glCtx *glCtx);
    ibool           (NAPIP GA_getCRTCTimings)(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 refreshRate,GA_CRTCInfo *crtc);
    ibool           (NAPIP GA_setCRTCTimings)(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,GA_CRTCInfo *crtc,ibool makeDefault);
    ibool           (NAPIP GA_setDefaultRefresh)(GA_devCtx *dc,N_int32 outputHead,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 refreshRate,ibool saveToDisk);
    ibool           (NAPIP GA_saveCRTCTimings)(GA_devCtx *dc,N_int32 outputHead);
    ibool           (NAPIP GA_restoreCRTCTimings)(GA_devCtx *dc,N_int32 outputHead);
    ibool           (NAPIP GA_saveMonitorInfo)(GA_devCtx *dc,N_int32 outputHead,GA_monitor *monitor);
    int             (NAPIP GA_detectPnPMonitor)(GA_devCtx *dc,N_int32 outputHead,GA_monitor *monitor,ibool *hasChanged);
    ibool           (NAPIP GA_getRecommendedModes)(GA_devCtx *dc,N_int32 outputDevice,N_int32 outputHead,GA_recMode *recommendedMode,GA_recMode *recommendedMode8,GA_recMode *recommendedMode15,GA_recMode *recommendedMode16,GA_recMode *recommendedMode24,GA_recMode *recommendedMode32);
    void            (NAPIP GA_getMaxRefreshRate)(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 interlaced,float *maxRefresh);
    ibool           (NAPIP GA_computeCRTCTimings)(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 refreshRate,N_int32 interlaced,GA_CRTCInfo *crtc,N_int32 pureGTF);
    void            (NAPIP GA_freeLibrary)(MOD_MODULE *hModule);
    PCIDeviceInfo * (NAPIP GA_getPCIDeviceInfo)(N_int32 deviceIndex,N_int32 shared);
    N_int32         (NAPIP GA_getActiveDevice)(void);
    } GA_exports;

/****************************************************************************
REMARKS:
Defines the structure for holding 64-bit integers used for storing the values
returned by the precision timing functions. The precision timing functions
are used internally by the SNAP Graphics drivers for software stereo
support, however the granularity of the timing functions is variable.
Generally a granularity of around 1us is desired for maximum accuracy.
Where possible these import functions should be implemented using the
Intel Pentium RDTSC instruction or equivalent (with time readings
normalised to 1us granularity to avoid overflow internally).

HEADER:
snap/graphics.h

MEMBERS:
low     - Low 32-bits of the 64-bit integer
high    - High 32-bits of the 64-bit integer
****************************************************************************/
typedef u64 GA_largeInteger;

/****************************************************************************
REMARKS:
Structure defining all the SNAP Graphics API functions as imported into
the Binary Portable DLL.
{secret}
****************************************************************************/
typedef struct {
    N_uintptr       dwSize;
    GA_sharedInfo * (NAPIP GA_getSharedInfo)(int device);
    ibool           (NAPIP TimerInit)(void);
    void            (NAPIP TimerRead)(GA_largeInteger *value);
    N_uint32        (NAPIP TimerDifference)(GA_largeInteger *a,GA_largeInteger *b);
#ifndef __INTEL__
    void            (NAPIP outpb)(ulong port,uchar val);
    void            (NAPIP outpw)(ulong port,ushort val);
    void            (NAPIP outpd)(ulong port,ulong val);
    uchar           (NAPIP inpb)(ulong port);
    ushort          (NAPIP inpw)(ulong port);
    ulong           (NAPIP inpd)(ulong port);
#endif
    } GA_imports;

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point.
{secret}
****************************************************************************/
typedef GA_exports * (NAPIP GA_initLibrary_t)(ibool shared,const char *path,const char *bpdname,PM_imports *pmImp,N_imports *nImp,GA_imports *gaImp);

/****************************************************************************
REMARKS:
Function pointer type for the Binary Portable DLL initialisation entry point
for detecting all graphics chipsets in the system.
{secret}
****************************************************************************/
typedef ibool (NAPIP GA_detectChipsets_t)(const char *bpdname,PM_imports *pmImp,N_imports *nImp,int *numCards,char **mfrNames,char **chipNames);
#endif

#pragma pack()

/*---------------------------- Global Variables ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

/* Pointer to global exports structure. Should not be used by application programs. */
#ifdef __WINDOWS16__
#else
/* {secret} */
extern GA_exports   _VARAPI __GA_exports;
#endif

/*---------------------- Inline functions as Macros -----------------------*/

/* Useful inline macros for rectangle arithmetic */

#define GA_equalRect(r1,r2)     ((r1).left == (r2).left &&          \
                                 (r1).top == (r2).top &&            \
                                 (r1).right == (r2).right &&        \
                                 (r1).bottom == (r2).bottom)

#define GA_emptyRect(r)         ((r).bottom <= (r).top ||           \
                                 (r).right <= (r).left)

#define GA_disjointRect(r1,r2)  ((r1).right <= (r2).left ||         \
                                 (r1).left >= (r2).right ||         \
                                 (r1).bottom <= (r2).top ||         \
                                 (r1).top >= (r2).bottom)

#define GA_sectRect(s1,s2,d)                                        \
   ((d)->left = MAX((s1).left,(s2).left),                           \
    (d)->right = MIN((s1).right,(s2).right),                        \
    (d)->top = MAX((s1).top,(s2).top),                              \
    (d)->bottom = MIN((s1).bottom,(s2).bottom),                     \
    !GA_emptyRect(*d))

#define GA_sectRectFast(s1,s2,d)                                    \
    (d)->left = MAX((s1).left,(s2).left);                           \
    (d)->right = MIN((s1).right,(s2).right);                        \
    (d)->top = MAX((s1).top,(s2).top);                              \
    (d)->bottom = MIN((s1).bottom,(s2).bottom)

#define GA_sectRectCoord(l1,t1,r1,b1,l2,t2,r2,b2,d)                 \
   ((d)->left = MAX(l1,l2),                                         \
    (d)->right = MIN(r1,r2),                                        \
    (d)->top = MAX(t1,t2),                                          \
    (d)->bottom = MIN(b1,b2),                                       \
    !GA_emptyRect(*d))

#define GA_sectRectFastCoord(l1,t1,r1,b1,l2,t2,r2,b2,d)             \
    (d)->left = MAX(l1,l2);                                         \
    (d)->right = MIN(r1,r2);                                        \
    (d)->top = MAX(t1,t2);                                          \
    (d)->bottom = MIN(b1,b2)

#define GA_unionRect(s1,s2,d)                                       \
    (d)->left = MIN((s1).left,(s2).left);                           \
    (d)->right = MAX((s1).right,(s2).right);                        \
    (d)->top = MIN((s1).top,(s2).top);                              \
    (d)->bottom = MAX((s1).bottom,(s2).bottom)

#define GA_unionRectCoord(l1,t1,r1,b1,l2,t2,r2,b2,d)                \
    (d)->left = MIN(l1,l2);                                         \
    (d)->right = MAX(r1,r2);                                        \
    (d)->top = MIN(t1,t2);                                          \
    (d)->bottom = MAX(b1,b2)

#define GA_offsetRect(r,dx,dy)                                      \
    {   (r).left += dx; (r).right += dx;                            \
        (r).top += dy; (r).bottom += dy; }

#define GA_insetRect(r,dx,dy)                                       \
    {   (r).left += dx; (r).right -= dx;                            \
        (r).top += dy; (r).bottom -= dy;                            \
        if (GA_emptyRect(r))                                        \
            (r).left = (r).right = (r).top = (r).bottom = 0; }

#define GA_ptInRect(x,y,r)      ((x) >= (r).left &&                 \
                                 (x) < (r).right &&                 \
                                 (y) >= (r).top &&                  \
                                 (y) < (r).bottom)

#define GA_isSimpleRegion(r)    ((r)->spans == NULL)

/*-------------------------- Function Prototypes --------------------------*/

#ifndef __WINDOWS16__

/* Error handling functions for SNAP Graphics drivers */

int             NAPI GA_status(void);
const char *    NAPI GA_errorMsg(N_int32 status);

/* Function to get the number of days left in evaluation period */

int             NAPI GA_getDaysLeft(N_int32 shared);
int             NAPI GA_isOEMVersion(ibool shared);
int             NAPI GA_isLiteVersion(ibool shared);
const char *    NAPI GA_getDisplaySerialNo(ibool shared);
const char *    NAPI GA_getDisplayUserName(ibool shared);
N_uint32 *      NAPI GA_getLicensedDevices(ibool shared);

/* Utility function to register a linkable library license */

int             NAPI GA_registerLicense(uchar *license,N_int32 shared);

/* {secret} Utility function to override the local driver path for SNAP Graphics */
void            NAPI GA_setLocalPath(const char *path);

/* Utility Functions to load a graphics driver and initialise it */

ibool           NAPI GA_isSharedDriverLoaded(void);
int             NAPI GA_enumerateDevices(N_int32 shared);
PCIDeviceInfo * NAPI GA_getPCIDeviceInfo(N_int32 deviceIndex,N_int32 shared);
GA_devCtx *     NAPI GA_loadDriver(N_int32 deviceIndex,N_int32 shared);
ibool           NAPI GA_setActiveDevice(N_int32 deviceIndex);
N_int32         NAPI GA_getActiveDevice(void);
void            NAPI GA_unloadDriver(GA_devCtx *dc);
const char *    NAPI GA_getInternalName(N_int32 deviceIndex);
const char *    NAPI GA_getFakePCIID(void);
const char *    NAPI GA_getSNAPConfigPath(void);
ibool           NAPI GA_loadInGUI(N_int32 shared);
void            NAPI GA_programMTRRegisters(GA_devCtx *dc);
void            NAPI GA_setExportTable(GA_exports *table);
void            NAPI GA_setMinimumDriverVersion(N_uint32 version,N_int32 allowFallback,N_int32 shared);

/* Function to obtain callback function pointers to the device driver.
 * Application level code should *always* use this function to ensure
 * that any necessary thunking steps are taken when calling device
 * driver code from ring 3 that may need to call down into a ring 0
 * device driver.
 */

ibool           NAPI GA_queryFunctions(GA_devCtx *dc,N_uint32 id,void _FAR_ *funcs);
ibool           NAPI REF2D_queryFunctions(REF2D_driver *ref2d,N_uint32 id,void _FAR_ *funcs);

/* Utility functions to interface to the 2D reference rasteriser */

ibool           NAPI GA_loadRef2d(GA_devCtx _FAR_ *dc,N_int32 unused,GA_modeInfo _FAR_ *modeInfo,N_int32 transferStart,REF2D_driver _FAR_ *_FAR_ *drv);
void            NAPI GA_unloadRef2d(GA_devCtx _FAR_ *dc);

/* Utility Function to get the currently active reference rasteriser */

REF2D_driver *  NAPI GA_getCurrentRef2d(N_int32 deviceIndex);

/* Functions to initialise, enable, disable and clean up the software
 * stereo functions provided by the SNAP Graphics driver. For ring 3 user space
 * code, these functions will generally transition down to the system
 * helper driver (SDDHELP) to implement this as the interrupt handler
 * needs to run in kernel space.
 */

ibool           NAPI GA_softStereoInit(GA_devCtx *dc);
void            NAPI GA_softStereoOn(void);
void            NAPI GA_softStereoScheduleFlip(N_uint32 leftAddr,N_uint32 rightAddr);
N_int32         NAPI GA_softStereoGetFlipStatus(void);
void            NAPI GA_softStereoWaitTillFlipped(void);
void            NAPI GA_softStereoOff(void);
void            NAPI GA_softStereoExit(void);

/* Functions to load and initialiase the region manager */

ibool           NAPI GA_loadRegionMgr(GA_regionFuncs *funcs,MOD_MODULE **hModule,ulong *size,ibool shared);
void            NAPI GA_unloadRegionMgr(MOD_MODULE *hModule);

/* Utility functions to manipulate global options effective for all devices */

void            NAPI GA_getGlobalOptions(GA_globalOptions *options,ibool shared);
void            NAPI GA_setGlobalOptions(GA_globalOptions *options);
ibool           NAPI GA_saveGlobalOptions(GA_globalOptions *options);
ibool           NAPI GA_readGlobalOptions(GA_globalOptions *options);

/* Utility functions to save options and configuration information */

ibool           NAPI GA_saveModeProfile(GA_devCtx *dc,GA_modeProfile *profile);
void            NAPI GA_loadModeProfile(GA_devCtx *dc,GA_modeProfile *profile);
ibool           NAPI GA_saveOptions(GA_devCtx *dc,GA_options *options);
ibool           NAPI GA_getCRTCTimings(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 refreshRate,GA_CRTCInfo *crtc);
ibool           NAPI GA_setCRTCTimings(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,GA_CRTCInfo *crtc,ibool makeDefault);
ibool           NAPI GA_setDefaultRefresh(GA_devCtx *dc,N_int32 outputHead,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 refreshRate,ibool saveToDisk);
ibool           NAPI GA_saveCRTCTimings(GA_devCtx *dc,N_int32 outputHead);
ibool           NAPI GA_restoreCRTCTimings(GA_devCtx *dc,N_int32 outputHead);

/* Utility functions for monitor management */

ibool           NAPI GA_saveMonitorInfo(GA_devCtx *dc,N_int32 outputHead,GA_monitor *monitor);
int             NAPI GA_detectPnPMonitor(GA_devCtx *dc,N_int32 outputHead,GA_monitor *monitor,ibool *hasChanged);
int             NAPI GA_getParsedEDID(GA_devCtx *dc,EDID_record *rec);

/* Utility functions to create GTF display timings */

ibool           NAPI GA_useDoubleScan(GA_modeInfo *modeInfo);
void            NAPI GA_getMaxRefreshRate(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 interlaced,float *maxRefresh);
ibool           NAPI GA_computeCRTCTimings(GA_devCtx *dc,N_int32 outputHead,GA_modeInfo *modeInfo,N_int32 refreshRate,N_int32 interlaced,GA_CRTCInfo *crtc,N_int32 pureGTF);

/* Utility functions to manipulate the SNAP Graphics Dial-A-Mode features */

ibool           NAPI GA_addMode(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
ibool           NAPI GA_addRefresh(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 refresh,N_int32 saveToDisk);
ibool           NAPI GA_delMode(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);

/* Utility functions to manipulate the SNAP Graphics VBE mode filtering tables */

ibool           NAPI GA_enableVBEMode(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);
ibool           NAPI GA_disableVBEMode(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 saveToDisk);

/* Utility function to find the recommended display modes for the device */

ibool           NAPI GA_getRecommendedModes(GA_devCtx *dc,N_int32 outputDevice,N_int32 outputHead,GA_recMode *recommendedMode,GA_recMode *recommendedMode8,GA_recMode *recommendedMode15,GA_recMode *recommendedMode16,GA_recMode *recommendedMode24,GA_recMode *recommendedMode32);

/* Functions to load and unload the GL3D device driver */

struct GA_glCtx * NAPI GA_glLoadDriver(GA_devCtx *hwCtx);
ibool           NAPI GA_glQueryFunctions(struct GA_glCtx *glCtx,N_uint32 id,void _FAR_ *funcs);
void            NAPI GA_glUnloadDriver(struct GA_glCtx *glCtx);

/* Prototypes for OS provided timer functions */

#ifndef __DRIVER__
/* {secret} */
ibool           NAPI GA_TimerInit(void);
/* {secret} */
void            NAPI GA_TimerRead(GA_largeInteger *value);
/* {secret} */
N_uint32        NAPI GA_TimerDifference(GA_largeInteger *a,GA_largeInteger *b);
#else
#define GA_TimerInit()          _GA_imports.TimerInit()
#define GA_TimerRead(value)     _GA_imports.TimerRead(value)
#define GA_TimerDifference(a,b) _GA_imports.TimerDifference(a,b)
#endif

/* External Intel assembler functions for high precision timing */

#ifdef  __INTEL__
/* {secret} */
ibool           NAPI _GA_haveCPUID(void);
/* {secret} */
uint            NAPI _GA_getCPUIDFeatures(void);
/* {secret} */
void            NAPI _GA_readTimeStamp(GA_largeInteger *time);
/* {secret} */
#define         CPU_HaveRDTSC   0x00000010
#endif

/* Prototypes for OS provided I/O functions for non x86 systems */

#ifndef __INTEL__
/* {secret} */
void            NAPI GA_outpb(ulong port,uchar val);
/* {secret} */
void            NAPI GA_outpw(ulong port,ushort val);
/* {secret} */
void            NAPI GA_outpd(ulong port,ulong val);
/* {secret} */
uchar           NAPI GA_inpb(ulong port);
/* {secret} */
ushort          NAPI GA_inpw(ulong port);
/* {secret} */
ulong           NAPI GA_inpd(ulong port);
#endif

/* {secret} Function to return the shared info instance for a particular device */
GA_sharedInfo * NAPI GA_getSharedInfo(int device);

/* {secret} Function to return the system wide shared exports if needed */
ibool           NAPI GA_getSharedExports(GA_exports *gaExp,ibool shared);

/* {secret} Function to return the system wide PM library functions */
PM_imports *    NAPI GA_getSystemPMImports(void);

/* Internal function to load/unload a BPD file */
/* {secret} */
MOD_MODULE *    NAPI GA_loadLibrary(const char *szBPDName,ulong *size,ibool shared);
/* {secret} */
void *          NAPI GA_getProcAddress(MOD_MODULE *hModule,const char *szProcName);
/* {secret} */
void            NAPI GA_freeLibrary(MOD_MODULE *hModule);

/* Legacy functions for backwards compatibility */
/* {secret} */
ibool       NAPI GA_getCRTCTimingsV1(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 refreshRate,GA_CRTCInfo *crtc);
/* {secret} */
ibool       NAPI GA_setCRTCTimingsV1(GA_devCtx *dc,GA_modeInfo *modeInfo,GA_CRTCInfo *crtc,ibool makeDefault);
/* {secret} */
ibool       NAPI GA_setDefaultRefreshV1(GA_devCtx *dc,N_int32 xRes,N_int32 yRes,N_int32 bits,N_int32 refreshRate,ibool saveToDisk);
/* {secret} */
ibool       NAPI GA_saveCRTCTimingsV1(GA_devCtx *dc);
/* {secret} */
ibool       NAPI GA_restoreCRTCTimingsV1(GA_devCtx *dc);
/* {secret} */
ibool       NAPI GA_saveMonitorInfoV1(GA_devCtx *dc,GA_monitor *monitor);
/* {secret} */
int         NAPI GA_detectPnPMonitorV1(GA_devCtx *dc,GA_monitor *monitor,ibool *hasChanged);
/* {secret} */
ibool       NAPI GA_getRecommendedModesV1(GA_devCtx *dc,GA_recMode *recommendedMode,GA_recMode *recommendedMode8,GA_recMode *recommendedMode16,GA_recMode *recommendedMode24,GA_recMode *recommendedMode32);
/* {secret} */
void        NAPI GA_getMaxRefreshRateV1(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 interlaced,float *maxRefresh);
/* {secret} */
ibool       NAPI GA_computeCRTCTimingsV1(GA_devCtx *dc,GA_modeInfo *modeInfo,N_int32 refreshRate,N_int32 interlaced,GA_CRTCInfo *crtc,N_int32 pureGTF);

#endif  /* !__WINDOWS16__ */

#ifdef  __cplusplus
}                                   /* End of "C" linkage for C++       */
#endif

#endif  /* __SNAP_GRAPHICS_H */

