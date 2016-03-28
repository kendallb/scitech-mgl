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
*               device driver Hardware Abstraction Layer, stereo specific
*               information.
*
****************************************************************************/

#ifndef __SNAP_STEREO_H
#define __SNAP_STEREO_H

/*---------------------- Macros and type definitions ----------------------*/

/****************************************************************************
REMARKS:
This enumeration defines the types of known LC shutter glasses supported
by SNAP Graphics for software stereo support. The values for the COM or LPT
port that the glasses are connected to, are only included when specific
glasses types are selected that are known to be connected to a COM or LPT
port. If the user selects a custom I/O port configuration with
gaGlassesIOPort, it is expected the correct I/O port information is manually
configured in the GA_options structure.

HEADER:
snap/stereo.h

MEMBERS:
gaGlassesBlueCode       - LC glasses that use the blue code system
gaGlassesLPTPort        - LC glasses toggled by parallel port
gaGlassesCOMPort        - LC glasses toggled by serial port
gaGlassesIOPort         - LC glasses toggled by custom I/O port
gaGlassesVSync          - LC glasses toggled by vSync modification
gaGlassesH3D            - LC glasses using original H3D key frames
gaGlassesLineBlanking   - LC glasses using external line blanking
gaGlassesSyncDouble     - LC glasses using external sync doubler
gaGlassesFreeRunning    - LC glasses that are free running
gaGlassesHardwareSync   - LC glasses connected to VESA stereo connector
gaGlassesH3DMod         - LC glasses using modified H3D key frames
gaGlassesH3DPlus        - LC glasses using H3D-Plus dongle
gaGlassesMask           - Mask to find the glasses type
gaGlassesPortMask       - Mask to find the port the glasses are attached to
gaGlassesCOM1           - Indicates glasses are on serial port COM1
gaGlassesCOM2           - Indicates glasses are on serial port COM2
gaGlassesCOM3           - Indicates glasses are on serial port COM3
gaGlassesCOM4           - Indicates glasses are on serial port COM4
gaGlassesLPT1           - Indicates glasses are on parallel port LPT1
gaGlassesLPT2           - Indicates glasses are on parallel port LPT2
gaGlassesLPT3           - Indicates glasses are on parallel port LPT3
****************************************************************************/
typedef enum {
    gaGlassesBlueCode,
    gaGlassesLPTPort,
    gaGlassesCOMPort,
    gaGlassesIOPort,
    gaGlassesVSync,
    gaGlassesH3D,
    gaGlassesLineBlanking,
    gaGlassesSyncDouble,
    gaGlassesFreeRunning,
    gaGlassesHardwareSync,
    gaGlassesH3DMod,
    gaGlassesH3DPlus,
    gaGlassesMask           = 0x0FFF,
    gaGlassesPortMask       = 0xF000,
    gaGlassesCOM1           = 0x1000,
    gaGlassesCOM2           = 0x2000,
    gaGlassesCOM3           = 0x3000,
    gaGlassesCOM4           = 0x4000,
    gaGlassesLPT1           = 0x5000,
    gaGlassesLPT2           = 0x6000,
    gaGlassesLPT3           = 0x7000
    } GA_glassesTypeFlags;

/****************************************************************************
REMARKS:
Defines the type of fullscreen stereo mode supported by the end user system.
The modes are described below:

The gaStereoPageFlip mode is the preferred mode for fullscreen display modes,
and uses fullscreen page flipping with quad buffering. The system is put
into a high refresh rate non-interlaced display mode. Four display pages
are used, and each vertical retrace the screen flips between the two left
and right buffers while the application draws to the hidden left and right
buffers. In this mode all buffers are created by the application with the
full width and height of the primary surface, and one of those buffers
should be the primary surface. None of the buffers need to be attached in
a DirectDraw flip ring.

The gaStereoAboveBelow mode is an alterate mode for fullscreen display modes,
and uses one or two fullsize buffers for page flipping, and two half height
back buffers for the stereo images. In this mode a full size buffer is
created for the primary surface, which you may also optionally attach a
flipping back buffer to (if you want no tearing). The stereo buffers are
also created by the application but with the full width and half the height
of the primary surface. All rendering to the stereo buffers should be done
using double the aspect ratio of the primary surface. The stereo library will
automatically take care of blitting the stereo back buffers to the primary
surface and flipping the display if an attached back buffer is present.

The gaStereoSideBySide mode is an alterate mode for fullscreen display modes,
and uses one or two fullsize buffers for page flipping, and two half width
back buffers for the stereo images. In this mode a full size buffer is
created for the primary surface, which you may also optionally attach a
flipping back buffer to (if you want no tearing). The stereo buffers are
also created by the application but with half the width and the full height
of the primary surface. All rendering to the stereo buffers should be done
using half the aspect ratio of the primary surface. The stereo library will
automatically take care of blitting the stereo back buffers to the primary
surface and flipping the display if an attached back buffer is present.

The gaStereoInterleaved mode is a mode for fullscreen display modes.
The graphics controller is put into a low refresh rate non-interlaced
display mode, and the scanlines from the left and right buffers are
interleaved together in horizontal scanlines. In this mode two stereo back
buffers are created by the application with the full width and half the
height of the display mode. All rendering to the stereo buffers should be
done using double the aspect ratio of the primary surface. The stereo library
will automatically take care of interleaving the stereo buffers to the
primary surface.

The gaStereoInterlaced mode is a mode for fullscreen display modes.
The graphics controller is put into a high refresh rate interlaced display
mode, and the scanlines from the left and right buffers are interleaved
together in horizontal scanlines. In this mode two stereo back buffers are
created by the application with the full width and half the height of the
display mode. All rendering to the stereo buffers should be done using double
the aspect ratio of the primary surface. The stereo library will
automatically take care of interleaving the stereo buffers to the primary
surface.

The gaStereoDualDisplay mode is a mode that uses two display controllers.
One controller displays the left eye image, and another controller displays
the right eye image. Each display device must be configured with the same
fullscreen resolution.

The gaStereoViewportFlip mode is a variation of the fullscreen page-flipped
display mode where the left and right image components are expected to be
rendered as viewports on the same surface. The graphics controller is put
into a high refresh rate non-interlaced display mode at half the vertical
resolution of the currently set display mode, and the upper and lower halves
of the display surface are automatically page-flipped. In this mode a single
stereo back buffer is created by the application with the full width and
height of the primary surface, and the left and right image components
are rendered into upper and lower viewports. All rendering to the stereo
viewports should be done using double the aspect ratio of the primary surface.

The gaStereoAnaglyph mode is where the left and right image components
are filtered with red and blue colors for viewing with anaglyph glasses.
Ideally the image components should be rendered in monochrome gray-scale
values prior to filtering by the driver for optimal appearance.

The gaStereoAnaglyphCyan mode is similar to gaStereoAnaglyph mode except
the stereo image components are filtered with red and cyan colors.

The gaStereoAnaglyphGreen mode is similar to gaStereoAnaglyph mode except
the stereo image components are filtered with red and green colors.

The gaStereoLenticular mode is a fullscreeen stereo display mode for
output to an autostereoscopic lenticular display. Multiple stereo views
are formatted onto a single drawing surface, and the stereo library will
interdigitate the views into pixel patterns compatible with the lenticules
(vertical lens elements) of the output device. This particular format
requires that the application subdivide the fullscreen surface into 9
equal size viewports arranged in 3x3 format in order for the library to
convert into vertical stripes corresponding to the lenticule layout.

HEADER:
snap/stereo.h

MEMBERS:
gaStereoNone            - Stereo is not supported on this system
gaStereoPageFlip        - Fullscreen page flipped stereo
gaStereoAboveBelow      - Fullscreen above below format
gaStereoSideBySide      - Fullscreen side by side format
gaStereoInterleaved     - Fullscreen line interleaved stereo format
gaStereoInterlaced      - Fullscreen interlaced stereo format
gaStereoDualDisplay     - Fullscreen dual display stereo format
gaStereoViewportFlip    - Fullscreen viewport flipped stereo
gaStereoAnaglyph        - Fullscreen red/blue anaglyph stereo format
gaStereoAnaglyphCyan    - Fullscreen red/cyan anaglyph stereo format
gaStereoAnaglyphGreen   - Fullscreen red/green anaglyph stereo format
gaStereoLenticular      - Fullscreen lenticular display stereo format
****************************************************************************/
typedef enum {
    gaStereoNone,
    gaStereoPageFlip,
    gaStereoAboveBelow,
    gaStereoSideBySide,
    gaStereoInterleaved,
    gaStereoInterlaced,
    gaStereoDualDisplay,
    gaStereoViewportFlip,
    gaStereoAnaglyph,
    gaStereoAnaglyphCyan,
    gaStereoAnaglyphGreen,
    gaStereoLenticular
    } GA_stereoModeType;

/****************************************************************************
REMARKS:
Defines the type of windowed stereo mode supported by the end user system.
The modes are described below:

The gaWinStereoInterlaced mode is a mode for displaying stereo in a window
on the desktop. The graphics controller is put into a high refresh rate
interlaced display mode, and the scanlines from the left and right buffers
are interleaved together in horizontal scanlines. In this mode two stereo
back buffers are created by the application with the full width and half the
height of the application window. All rendering to the stereo buffers should
be done using double the aspect ratio of the primary surface. The stereo
library will automatically take care of interleaving the stereo buffers
onto the application window surface.

The gaWinStereoInterleaved mode is a mode for displaying stereo in a window
on the desktop. The graphics controller is put into a user selectable
refresh rate non-interlaced display mode. The mode is basically the same
as gaWinStereoInterlaced, but interlaced mode is not used.

The gaWinStereoPageFlip mode is a mode for displaying stereo in a window
on the desktop. The graphics controller is put into a high refresh rate
non-interlaced display mode, and the stereo left and right buffers are
automatically composited into the application window every retrace interval.
In this mode four stereo back buffers are created by the application with
the full width and height of the application window. All rendering to the
stereo buffers should be done using the full aspect ratio of the primary
surface. The stereo library automatically takes care of blitting the buffers
to the application window every retrace.

The gaWinStereoPageFlipLoRes mode is a variation of the windowed page-flipped
display mode which uses less video memory resources internally than the
default gaWinStereoPageFlip mode. It could be used as a fallback contingency
mode when video memory resources for the current desktop resolution are
marginal. The trade-off for using less video memory is that dirty rectangle
management for both stereo and non-stereo display regions will not be updated
synchronously between the left and right eyes. This low-res stereo mode may
appear acceptable for simple windowed applications, but less so with greater
number of windows displayed, especially overlapping and popup windows.

The gaWinStereoAnaglyph mode is where the left and right image components
are filtered with red and blue colors for viewing with anaglyph glasses.
Ideally the image components should be rendered in monochrome gray-scale
values prior to filtering by the driver for optimal appearance.

The gaWinStereoAnaglyphCyan mode is similar to gaWinStereoAnaglyph mode
except the stereo image components are filtered with red and cyan colors.

The gaWinStereoAnaglyphGreen mode is similar to gaWinStereoAnaglyph mode
except the stereo image components are filtered with red and green colors.

The gaWinStereoDualDisplay mode is a mode that uses two display controllers.
One controller displays the left eye image, and another controller displays
the right eye image. This is similar to the fullscreen version of this mode,
except the left and right images are composited in a window on the desktop
for each display device. Each display must be configured with the same
desktop resolution.

The gaWinStereoLenticular mode is a windowed stereo display mode for
output to an autostereoscopic lenticular display. Multiple stereo views
are formatted onto a single drawing surface similar to the fullscreen
version of this mode, except that only the window client region is used.

HEADER:
snap/stereo.h

MEMBERS:
gaWinStereoNone             - Stereo is not supported on this system
gaWinStereoInterlaced       - Windowed interlaced stereo mode
gaWinStereoInterleaved      - Windowed interleaved stereo mode
gaWinStereoPageFlip         - Windowed page flipped stereo mode
gaWinStereoPageFlipLoRes    - Windowed page flipped low-res stereo mode
gaWinStereoAnaglyph         - Windowed red/blue anaglyph stereo mode
gaWinStereoAnaglyphCyan     - Windowed red/cyan anaglyph stereo mode
gaWinStereoAnaglyphGreen    - Windowed red/green anaglyph stereo mode
gaWinStereoDualDisplay      - Windowed dual display stereo mode
gaWinStereoLenticular       - Windowed lenticular display stereo mode
****************************************************************************/
typedef enum {
    gaWinStereoNone,
    gaWinStereoInterlaced,
    gaWinStereoInterleaved,
    gaWinStereoPageFlip,
    gaWinStereoPageFlipLoRes,
    gaWinStereoAnaglyph,
    gaWinStereoAnaglyphCyan,
    gaWinStereoAnaglyphGreen,
    gaWinStereoDualDisplay,
    gaWinStereoLenticular
    } GA_winStereoModeType;

#endif  /* __SNAP_STEREO_H */

