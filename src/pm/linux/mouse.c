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
* Portions taken from Rocklyte Systems pointer.c and hardware.c
* Copyright: Rocklyte Systems (c) 1998-2003. All rights reserved.
*
* Language:     ANSI C
* Environment:  Linux
*
* Description:  Mouse support code for PM.
*
*****************************************************************************/

#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>

#include "pmapi.h"
#include "scitech.h"
#include <event.h>

/*-------------------------- #define's -------------------------------*/

//#define MOUSE_DEBUG
//#define MOUSE_DEBUG_VERBOSE
//#define MOUSE_DEBUG_FILE

#ifdef MOUSE_DEBUG
#define debug(args)             _debug args
#else
#define debug(args)
#endif

#define PSMC_SET_SCALING11      0xe6
#define PSMC_SET_SCALING21      0xe7
#define PSMC_SET_RESOLUTION     0xe8
#define PSMC_SEND_DEV_STATUS    0xe9
#define PSMC_SET_STREAM_MODE    0xea
#define PSMC_SEND_DEV_DATA      0xeb
#define PSMC_SET_REMOTE_MODE    0xf0
#define PSMC_SEND_DEV_ID        0xf2
#define PSMC_SET_SAMPLING_RATE  0xf3
#define PSMC_ENABLE_DEV         0xf4
#define PSMC_DISABLE_DEV        0xf5
#define PSMC_SET_DEFAULTS       0xf6
#define PSMC_ACK                0xfa
#define PSMC_RESET_DEV          0xff

#define PSMD_RES_LOW            0       /* typically 25ppi */
#define PSMD_RES_MEDIUM_LOW     1       /* typically 50ppi */
#define PSMD_RES_MEDIUM_HIGH    2       /* typically 100ppi (default) */
#define PSMD_RES_HIGH           3       /* typically 200ppi */

#define MAX_PORTS               4
#define PORT_PS2                0
#define PORT_USB                1
#define PORT_NAMED              2

/*-------------------------- typdef's -------------------------------*/

typedef struct {
    char   *Name;          /* Mouse name */
    char   *ShortName;     /* Short identifying name */
    int    Device;         /* Identifying device type */
    int    ProtocolFlags;  /* Protocol flags */
    ushort ProtocolID;     /* Protcol processor to use */
    uchar  Proto[4];
    ushort PacketLength;
    uchar  *Sequence;
    int    SequenceSize;
    uchar  Response;
    } MouseDefn;

typedef struct {
    int             fd;
    char            *devName;
    uchar           ps2State;
    uchar           data [8];    // Holds data received from the mouse port
    uchar           index;       // Byte index for data received from the port
    uchar           badPackets;
    uchar           goodPackets;
    uchar           buttons;
    uchar           lastButtons;
    uchar           iface;
    ibool           reInsert;
    ibool           initialised;
    int             lastX;
    int             lastY;
    int             lastZ;
    int             x;
    int             y;
    int             z;
    MouseDefn       *md;
    } DeviceDefn;

enum {
    PRT_PS2 = 1,
    PRT_IMPS2,
    PRT_EXPS2,
    PRT_NETPS2,
    PRT_NETSCPS2,
    PRT_MMPS2,
    PRT_THINKPS2,
    PRT_GLIDEPS2,
    PRT_UNSUPPORTED
    };

enum {
    MTYPE_NONE,
    MTYPE_PS2,
    MTYPE_USB,
    MTYPE_TTY
    };

/*---------------------------- Global Variables ---------------------------*/

static uchar seqIntelliMouse[] = {
    PSMC_SET_SAMPLING_RATE, 200,
    PSMC_SET_SAMPLING_RATE, 100,
    PSMC_SET_SAMPLING_RATE, 80,
    PSMC_SEND_DEV_ID
    };

static uchar seqExplorer[] = {
    /*** Starts with the intellimouse sequence ***/
    PSMC_SET_SAMPLING_RATE, 200,
    PSMC_SET_SAMPLING_RATE, 100,
    PSMC_SET_SAMPLING_RATE, 80,
    /*** Then the explorer sequence ***/
    PSMC_SET_SAMPLING_RATE, 200,
    PSMC_SET_SAMPLING_RATE, 200,
    PSMC_SET_SAMPLING_RATE, 80,
    PSMC_SEND_DEV_ID
    };

static uchar seqNetMouse[] = {
    PSMC_SET_RESOLUTION, 3,
    230, 230, 230,
    PSMC_SEND_DEV_ID
    };

static uchar seqMouseMan[] = {
    PSMC_SET_SCALING11,
    PSMC_SET_RESOLUTION, 0,
    PSMC_SET_RESOLUTION, 3,
    PSMC_SET_RESOLUTION, 2,
    PSMC_SET_RESOLUTION, 1,
    PSMC_SET_SCALING11,
    PSMC_SET_RESOLUTION, 3,
    PSMC_SET_RESOLUTION, 1,
    PSMC_SET_RESOLUTION, 2,
    PSMC_SET_RESOLUTION, 3,
    PSMC_SEND_DEV_ID
    };

static uchar seqThinkingMouse[] = {
    PSMC_SET_SAMPLING_RATE, 10,
    PSMC_SET_RESOLUTION, 0,
    PSMC_SET_SAMPLING_RATE, 20,
    PSMC_SET_SAMPLING_RATE, 60,
    PSMC_SET_SAMPLING_RATE, 40,
    PSMC_SET_SAMPLING_RATE, 20,
    PSMC_SET_SAMPLING_RATE, 20,
    PSMC_SET_SAMPLING_RATE, 60,
    PSMC_SET_SAMPLING_RATE, 40,
    PSMC_SET_SAMPLING_RATE, 20,
    PSMC_SET_SAMPLING_RATE, 20,
    PSMC_SEND_DEV_ID
    };

static MouseDefn MouseDefinitions [] = {
    { "AutoDetect",                 "AutoDetect",    0,         0, 0,             { 0x00,0x00,0x00,0x00 }, 6, NULL,             0,                        0x00 },
    { "Standard PS/2 or USB Mouse", "GenericPS/2",   MTYPE_PS2, 0, PRT_PS2,       { 0xc0,0x00,0x00,0x00 }, 3, NULL,             0,                        0x00 },
    { "Microsoft IntelliMouse",     "IMPS/2",        MTYPE_PS2, 0, PRT_IMPS2,     { 0x08,0x08,0x00,0x00 }, 4, seqIntelliMouse,  sizeof(seqIntelliMouse),  0x03 },
    { "Microsoft Explorer Mouse",   "EXPS/2",        MTYPE_PS2, 0, PRT_EXPS2,     { 0x08,0x08,0x00,0x00 }, 4, seqExplorer,      sizeof(seqExplorer),      0x04 },
    { "ThinkingMouse",              "ThinkingPS/2",  MTYPE_PS2, 0, PRT_THINKPS2,  { 0x80,0x80,0x00,0x00 }, 3, seqThinkingMouse, sizeof(seqThinkingMouse), 0x00 },
    { "MouseMan+, FirstMouse+",     "MouseManPS/2",  MTYPE_PS2, 0, PRT_MMPS2,     { 0x08,0x08,0x00,0x00 }, 3, seqMouseMan,      sizeof(seqMouseMan),      0x00 },
    { "NetMouse",                   "NetMousePS/2",  MTYPE_PS2, 0, PRT_NETPS2,    { 0x08,0x08,0x00,0x00 }, 4, seqNetMouse,      sizeof(seqNetMouse),      0x00 },
    { "NetScroll",                  "NetScrollPS/2", MTYPE_PS2, 0, PRT_NETSCPS2,  { 0xc0,0x00,0x00,0x00 }, 6, seqNetMouse,      sizeof(seqNetMouse),      0x00 },
    { "GlidePoint",                 "GlidePS/2",     MTYPE_PS2, 0, PRT_GLIDEPS2,  { 0xc0,0x00,0x00,0x00 }, 3, NULL,             0 },
    { NULL,                         NULL,            0,         0, 0,             { 0x00,0x00,0x00,0x00 }, 0, NULL,             0 }
    };

static int                        MouseCount;
static DeviceDefn                 Meece [8];

/*-------------------------- Implementation -------------------------------*/

#ifdef MOUSE_DEBUG
/****************************************************************************
DESCRIPTION:
Debug output function.

PARAMETERS:
format  - Format string
...     - Remainder of arguments

REMARKS:
Formats debugging information and sends it to the screen and optionally
to a debug log file.
****************************************************************************/
static void _debug(
    char *format,
    ...)
{
    char                  str [2048];
    va_list               arg_ptr;
#ifdef MOUSE_DEBUG_FILE
    FILE                  *f;
#endif

    va_start (arg_ptr, format);
    vsprintf (str, format, arg_ptr);
    va_end (arg_ptr);

#ifdef MOUSE_DEBUG_FILE
    if ((f = fopen ("/scitech-debug.txt", "a+t")) != NULL) {
        fprintf (f, "%s", str);
        fclose (f);
        }
#endif
    fprintf (stderr, "%s", str);
}
#endif

/****************************************************************************
DESCRIPTION:
Reads a single byte from the input descriptor associated with the given mouse.

PARAMETERS:
m - a Device Definition (aka mouse)
Byte - pointer to location where data is to be stored.

RETURNS:
TRUE if a byte was read.

REMARKS:
Assumes input fd is in non-blocking mode.
****************************************************************************/
static ibool ReadByte (
    DeviceDefn *m,
    uchar *Byte)
{
    return (read (m->fd, Byte, 1) == 1);
}

/****************************************************************************
DESCRIPTION:
Writes a single byte

PARAMETERS:
m - a Device Definition (aka mouse)
Byte - data to write

RETURNS:
TRUE if write succeeds
****************************************************************************/
static ibool WriteByte (
    DeviceDefn *m,
    uchar Byte)
{
    return (write (m->fd, &Byte, 1) == 1);
}

/****************************************************************************
DESCRIPTION:
Writes a buffer of data

PARAMETERS:
m - a Device Definition (aka mouse)
Bytes - pointer to data to write
count - number of bytes to write

RETURNS:
TRUE if all bytes were written.
****************************************************************************/
static ibool WriteBytes (
    DeviceDefn *m,
    uchar *Bytes,
    int count)
{
    return (write (m->fd, Bytes, count) == count);
}

/****************************************************************************
DESCRIPTION:
Flushes input descriptor associated with mouse

PARAMETERS:
m - a Device Definition (aka mouse)
****************************************************************************/
static void FlushMouse (
    DeviceDefn *m)
{
    uchar     c;

    while (ReadByte (m, &c));
}

/****************************************************************************
DESCRIPTION:
waits up to a given amount of time for data to become available on the device.

PARAMETERS:
m - a Device Definition (aka mouse)
Timeout - maximum time to wait in milliseconds

RETURNS:
TRUE if data is available.

REMARKS:
May return early as a result of a signal.
****************************************************************************/
static ibool WaitForData (
    DeviceDefn *m,
    int Timeout)
{
    fd_set              fds;
    struct timeval      tv = { Timeout / 1000, (Timeout % 1000) * 1000 };

    FD_ZERO (&fds);
    FD_SET (m->fd, &fds);
    return (select (m->fd + 1, &fds, NULL, NULL, &tv) > 0);
}

/****************************************************************************
DESCRIPTION:
Attempts to initialise the given mouse.

PARAMETERS:
m - a Device Definition (aka mouse)

RETURNS:
TRUE if a mouse was found.

REMARKS:
If successful, and this is the second mouse found, the other devices in the
mouse list which had not yet had a mouse detected on them will be closed.
****************************************************************************/
static ibool InitMouse (
    DeviceDefn *m)
{
    uchar     c;
    MouseDefn *md;

    FlushMouse (m);

    if (m->reInsert) {
        /*** Tell the mouse to reinitialise itself from scratch ***/
        WriteByte (m, PSMC_RESET_DEV);
        WaitForData (m, 100);
        FlushMouse (m);

        /*** Now enable the device so that we can talk to it ***/
        WriteByte (m, PSMC_ENABLE_DEV);
        WaitForData (m, 100);
        FlushMouse (m);
        m->reInsert = FALSE;
        }

    /*
     * PS/2 mouse controllers almost always start out with standard PS/2 mouse driver
     * support, which caters for 3 buttons and 2 dimensional movement.  If we talk to
     * the PS/2 mouse, we can find out if it supports advanced protocols.
     */
    if (m->md == NULL) {
        /*** Try different initialisation sequences ***/
        for (md = MouseDefinitions; md->Name != NULL; md++) {
            if (md->Sequence && md->Response) {
                debug (("%s: trying protocol %s\n", m->devName, md->Name));

                FlushMouse (m);

                /*** Write the initialisation sequence to the mouse ***/
                if (WriteBytes (m, md->Sequence, md->SequenceSize)) {
                    // Wait for the first response from the mouse, then wait a little longer to
                    // make sure the mouse has sent all response bytes before continuing
                    WaitForData (m, 100);
                    usleep (10000);

                    // Skip any junk to reach the first ACK signal
                    c = 0;
                    while (ReadByte (m, &c) && c != PSMC_ACK);

                    if (c == PSMC_ACK) {
                        /*** The mouse may send a stream of ACK signals, just ignore them ***/
                        while (ReadByte (m, &c) && c == PSMC_ACK);
                        if (c == md->Response) {
                            debug (("%s: device reports itself as %s.\n", m->devName, md->ShortName));
                            m->md = md;
                            break;
                            }
                        }
                    else {
                        // this device is probably not a mouse. we should release the port.
                        close (m->fd);
                        m->fd = -1;
                        return (FALSE);
                        }
                    }
                }
            }

        if (!m->md) {
            debug (("%s: protocol unknown - switching to default PS/2 type.\n", m->devName));

            m->md = MouseDefinitions + 1;
            for (md = MouseDefinitions; md->Name != NULL; md++) {
                if (md->ProtocolID == PRT_PS2) {
                    m->md = md;
                    break;
                    }
                }
            }
        }
    else if (m->md->Sequence) {
        debug (("%s: attempting initialisation of protocol %s\n", m->devName, m->md->Name));
        WriteBytes (m, m->md->Sequence, m->md->SequenceSize);
        WaitForData (m, 100);
        FlushMouse (m);
        }

    /*** Set 1:1 scaling as the default ***/

    WriteByte (m, PSMC_SET_SCALING11);
    WriteByte (m, PSMC_ENABLE_DEV);
    WriteByte (m, PSMC_SET_SAMPLING_RATE);
    WriteByte (m, 100); /* Sample rate: 40, 60, 80, 100, 200 */

    WriteByte (m, PSMC_SET_RESOLUTION);
    WriteByte (m, PSMD_RES_MEDIUM_HIGH);

    usleep(30000); /* Wait for our commands to be executed */
    FlushMouse(m);

    m->initialised = TRUE;
    if (++MouseCount == 2) {
        // two is enough; release any other ports to user
        for (m = Meece; m->iface != MTYPE_NONE; m++) {
            if (!m->initialised && m->fd != -1) {
                debug (("%s: closing (fd==%d)\n", m->devName, m->fd));
                close (m->fd);
                m->fd = -1;
                }
            }
        }

    return(TRUE);
}

/****************************************************************************
DESCRIPTION:
Attempts to read and process data for the given mouse.

PARAMETERS:
m - a Device Definition (aka mouse)
buttons - pointer to location to store button state.
dx - where to store delta X value.
dy - where to store delta Y value.
dz - where to store delta Z (aka wheel) value.

RETURNS:
TRUE if the mouse state changed (in which case buttons, dx, dy and dz are valid).
FALSE otherwise (in which case state of passed variables is not changed).

REMARKS:
May also auto-detect a mouse on the passed device via InitMouse().
****************************************************************************/
ibool ReadMouseData (
    DeviceDefn *m,
    uchar *buttons,
    int *dx,
    int *dy,
    int *dz)
{
    ibool               packet = FALSE;
    uchar               c;
    uchar               *data;

    *dx = 0;
    *dy = 0;
    *dz = 0;
    *buttons = 0;

    while (ReadByte (m, &c)) {
        if (m->md == NULL) {
            if (m->iface == PORT_USB) {
                InitMouse (m);
                m->index = 0;
                }
            else {
                /*
                 * A note on auto-detection:  We cannot send initialisation sequences or
                 * talk to the mouse until we start receiving data (this indicates that
                 * a mouse is plugged in).  If we try to talk to the mouse beforehand,
                 * we will only screw up devices that share the same device port (e.g. the
                 * keyboard shares /dev/psaux).
                 */
                m->data[m->index++] = c;

                if (m->index >= MouseDefinitions [0].PacketLength) {
                    InitMouse (m);
                    m->index = 0;
                    }
                }
            }
        else {
            if (m->index == 0 && ((c & m->md->Proto[0]) != m->md->Proto[1])) {
                m->badPackets++;
                // are a quarter or worse of all packets bad ?
                if (m->badPackets > 8 && m->goodPackets < 255 && m->goodPackets / m->badPackets <= 4) {
                    /*** If we get lots of bad packets in sequence, try a different mouse driver ***/
                    m->badPackets = m->goodPackets = 0;
                    m->md++;
                    if (m->md->Name == NULL)
                        m->md = MouseDefinitions + 1;
                    debug (("%s: too many bad packets, trying protocol %s\n", m->devName, m->md->Name));
                    }
                continue;
                }

            m->data[m->index++] = c;

            if (m->index < m->md->PacketLength)
                continue;

            if ((m->data[1] & m->md->Proto[2]) != m->md->Proto[3]) {
                m->index = 0; /* Reset packet start */
                continue;
                }

            /*** Success - we have read a full packet ***/

            packet = TRUE;
            break;
            }
        }

    if (!packet || m->md == NULL)
        return (FALSE);

#if 0
    printf ("%s: ", m->devName);
    for (c = 0; c < m->index; c++)
        printf ("%02x ", m->data [c]);
    printf ("\n");
#endif

    m->index = 0;

    if (m->goodPackets < 255)
        m->goodPackets++;

    /*** Now decode the protocol packet ***/

    data = m->data;
    switch (m->md->ProtocolID) {
        case PRT_PS2:
            *buttons = (data[0] & 0x04)>>1 | (data[0] & 0x02)>>1 | (data[0] & 0x01)<<2;
            *dx = (data[0] & 0x10) ?    (int)data[1]-256  :  (int)data[1];
            *dy = (data[0] & 0x20) ?  -((int)data[2]-256) : -(int)data[2];
            break;

        case PRT_IMPS2:
            *buttons = (data[0] & 0x04)>>1 | (data[0] & 0x02)>>1 | (data[0] & 0x01)<<2 | (data[0] & 0x40)>>3 | (data[0] & 0x80)>>3; /* MRL */
            *dx = (data[0] & 0x10) ? data[1]-256 : data[1];
            *dy = (data[0] & 0x20) ? -(data[2]-256) : -data[2];
            *dz = (int)(char)data[3];
            if (*dz >= 7 || *dz <= -7)
                *dz = 0;
            break;

        case PRT_EXPS2:
            *buttons = (data[0] & 0x04)>>1 | (data[0] & 0x02)>>1 | (data[0] & 0x01)<<2 | (data[3] & 0x10)>>1 | (data[3] & 0x20)>>1; /* MRL45 */
            *dx = (data[0] & 0x10) ? data[1]-256 : data[1];
            *dy = (data[0] & 0x20) ? -(data[2]-256) : -data[2];
            *dz = (int)(char) ((data[3] & 0x08) ? (data[3] & 0x0f) - 16 : (data[3] & 0x0f));
            if (*dz >= 7 || *dz <= -7)
                *dz = 0;
            break;

        case PRT_MMPS2: /* MouseMan+ PS/2 */
            *buttons = (data[0] & 0x04) >> 1 | (data[0] & 0x02) >> 1 | (data[0] & 0x01) << 2; /* MRL */
            *dx = (data[0] & 0x10) ? data[1] - 256 : data[1];
            if (((data[0] & 0x48) == 0x48) && (abs(*dx) > 191) && ((((data[2] & 0x03) << 2) | 0x02) == (data[1] & 0x0f))) {
                /* extended data packet */
                switch ((((data[0] & 0x30) >> 2) | ((data[1] & 0x30) >> 4))) {
                    case 1: /* wheel data packet */
                        *buttons |= ((data[2] & 0x10) ? 0x08 : 0) | /* 4th button */
                            ((data[2] & 0x20) ? 0x10 : 0);  /* 5th button */
                        *dx = *dy = 0;
                        *dz = (int)(char) ((data[2] & 0x08) ? (data[2] & 0x0f) - 16 : (data[2] & 0x0f));
                        break;

                    case 2: /* Logitech reserves this packet type */
                            /* IBM ScrollPoint uses this packet to encode its
                            ** stick movement.
                        */
                        *buttons |= (m->lastButtons & ~0x07);
                        *dx = *dy = 0;
                        *dz = (int)(char) ((data[2] & 0x80) ? ((data[2] >> 4) & 0x0f) - 16 : ((data[2] >> 4) & 0x0f));
                        break;

                    case 0: /* device type packet - shouldn't happen */
                    default:
                        *buttons |= (m->lastButtons & ~0x07);
                        *dx = *dy = 0;
                        *dz = 0;
                        break;
                    }
                }
            else {
                *buttons |= (m->lastButtons & ~0x07);
                *dx = (data[0] & 0x10) ?    data[1]-256  :  data[1];
                *dy = (data[0] & 0x20) ?  -(data[2]-256) : -data[2];
                }
            break;

        case PRT_GLIDEPS2:      /* GlidePoint PS/2 */
            *buttons = (data[0] & 0x04) >> 1 | (data[0] & 0x02) >> 1 | (data[0] & 0x01) << 2 | ((data[0] & 0x08) ? 0 : 0x08); /* MRL4 */
            *dx = (data[0] & 0x10) ?    data[1]-256  :  data[1];
            *dy = (data[0] & 0x20) ?  -(data[2]-256) : -data[2];
            break;

        case PRT_NETSCPS2:      /* NetScroll PS/2 */
            *buttons = (data[0] & 0x04) >> 1 | (data[0] & 0x02) >> 1 | (data[0] & 0x01)<<2 | ((data[3] & 0x02) ? 0x08 : 0) | ((data[3] & 0x01) ? 0x10 : 0);  /* MRL45 */
            *dx = (data[0] & 0x10) ?    data[1]-256  :  data[1];
            *dy = (data[0] & 0x20) ?  -(data[2]-256) : -data[2];
            *dz = (int)(char)((data[3] & 0x10) ? data[4] - 256 : data[4]);
            break;

        case PRT_THINKPS2:      /* ThinkingMouse PS/2 */
            *buttons = (data[0] & 0x04) >> 1 | (data[0] & 0x02) >> 1 | (data[0] & 0x01) << 2 | ((data[0] & 0x08) ? 0x08 : 0); /* MRL4 */
            data[1] |= (data[0] & 0x40) ? 0x80 : 0x00;
            *dx = (data[0] & 0x10) ?    data[1]-256  :  data[1];
            *dy = (data[0] & 0x20) ?  -(data[2]-256) : -data[2];
            break;

        default:
            return(FALSE);
        }

    return(TRUE);
}

/****************************************************************************
DESCRIPTION:
Returns a mouse update, if available. Will return first mouse to have data.

RETURNS:
TRUE if an update was available.
****************************************************************************/
ibool GetMouseUpdate (
    uchar *buttons,
    uchar *lastButtons,
    int *dx,
    int *dy,
    int *dz)
{
    int         x;
    int         y;
    int         z;
    uchar       btns;
    DeviceDefn  *mickey;

    for (mickey = Meece; mickey->iface != MTYPE_NONE; mickey++) {
        if (mickey->fd <= 0)
            continue;
        if (ReadMouseData (mickey, &btns, &x, &y, &z)) {
            if (btns != mickey->lastButtons || x || y || z) {
                *lastButtons = mickey->lastButtons;
                *buttons = mickey->lastButtons = btns;
                *dx = x;
                *dy = y;
                *dz = z;
                return (TRUE);
                }
            }
        }
    return (FALSE);
}

/****************************************************************************
DESCRIPTION:
Sets up initial global mouse list. Auto-detects USB mice.

RETURNS:
TRUE.

REMARKS:
Will open /dev/mouse, /dev/input/mice, /dev/psaux, /dev/ttyS0, and /dev/ttyS1.
Stats all of the above and if /dev/mouse is a symlink or hard link to any of
the others, it will skip it. Sets O_NOCTTY when opening the TTY's.
****************************************************************************/
ibool SetupMice (void)
{
    ibool               noDevMouse = FALSE;
    DeviceDefn          *m = Meece;
    struct stat         st;
    struct stat         mst;

    if (stat ("/dev/mouse", &mst) == 0) {
        if (stat ("/dev/psaux", &st) == 0 && st.st_ino == mst.st_ino)
          noDevMouse = TRUE;
        else if (stat ("/dev/input/mice", &st) == 0 && st.st_ino == mst.st_ino)
          noDevMouse = TRUE;
        else if (stat ("/dev/ttyS0", &st) == 0 && st.st_ino == mst.st_ino)
          noDevMouse = TRUE;
        else if (stat ("/dev/ttyS1", &st) == 0 && st.st_ino == mst.st_ino)
          noDevMouse = TRUE;
        }

    if (!noDevMouse) {
        /* could be anything, we'll assume it's a PS/2 for safety */
        m->devName = "/dev/mouse";
        if ((m->fd = open (m->devName, O_RDWR)) != -1)
            m++->iface = MTYPE_PS2;
        }

    /* USB */
    m->devName = "/dev/input/mice";
    if ((m->fd = open (m->devName, O_RDWR)) != -1)
        m++->iface = MTYPE_USB;

    /* PS/2 */
    m->devName = "/dev/psaux";
    if ((m->fd = open (m->devName, O_RDWR)) != -1)
        m++->iface = MTYPE_PS2;

#ifndef __PPC__
    /* On certain sysetms (Motorola Sandpoint) the following open()
     * call hangs. Just don't try serial mice at all, because most
     * likely there's a tty on the serial ports anyway.
     */
    /* serial port */
    m->devName = "/dev/ttyS0";
    if ((m->fd = open (m->devName, O_RDWR | O_NOCTTY)) != -1)
        m++->iface = MTYPE_TTY;

    /* serial port */
    m->devName = "/dev/ttyS1";
    if ((m->fd = open (m->devName, O_RDWR | O_NOCTTY)) != -1)
        m++->iface = MTYPE_TTY;
#endif

    // set ports to non-blocking
    for (m = Meece; m->iface != MTYPE_NONE; m++) {
        fcntl (m->fd, F_SETFL, fcntl (m->fd, F_GETFL) | O_NONBLOCK);
        FlushMouse (m);
        }

    // initialise any USB meece
    for (m = Meece; m->iface != MTYPE_NONE; m++) {
        if (m->iface != MTYPE_USB || m->fd == -1)
            continue;
        if (!InitMouse (m)) {
            close (m->fd);
            m->fd = -1;
            }
        else {
            debug (("Mouse %d: %s on %s\n", MouseCount, m->md->Name, m->devName));
            }
        }

    return (true);
}

/****************************************************************************
DESCRIPTION:
Shuts down mice by closing open FD's.
****************************************************************************/
void ShutdownMice (void)
{
    DeviceDefn          *m = Meece;

    for (m = Meece; m->iface != MTYPE_NONE; m++)
        if (m->fd != -1)
            close (m->fd);
    memset (Meece, 0, sizeof (Meece));
}

#ifdef MOUSE_STANDALONE
/****************************************************************************
DESCRIPTION:
Simple main() which functions as a test harness.
****************************************************************************/
int main (int argc, char **argv)
{
    int         i;
    int         x;
    int         y;
    int         z;
    uchar       buttons;
    DeviceDefn  *rat;

    SetupMice ();
    while (TRUE) {
        for (rat = Meece; rat->iface != MTYPE_NONE; rat++) {
            if (rat->fd <= 0)
                continue;
            if (ReadMouseData (rat, &buttons, &x, &y, &z)) {
                if (buttons != rat->lastButtons || x || y || z) {
                    rat->lastButtons = buttons;
                    rat->x += x;
                    rat->y += y;
                    rat->z += z;
                    printf ("%s: buttons=", rat->devName);
                    for (i = 0; i < 8; i++, buttons <<= 1)
                        printf ("%c", buttons & 0x80 ? 'D' : 'U');
                    printf (", x=%d, y=%d, z=%d\n", rat->x, rat->y, rat->z);
                    }
                }
            }
        usleep (10000);
        }
    return (0);
}
#endif

