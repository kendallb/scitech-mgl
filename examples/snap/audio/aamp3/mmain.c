/****************************************************************************
*
*                      SciTech SNAP Audio Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
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
* Description:  Simple command line driven mp3 player.
*
****************************************************************************/

#include <ctype.h>
#include "snap/audio.h"
#include "event.h"
#include "cmdline.h"
#include "mp3dec.h"

// Size of an audio buffer in samples (twice 576 samples, stereo)
#define HALF_BUF_SAMPLES (576 * 2 * 2)

/*--------------------------- Global variables ----------------------------*/

AA_playbackFuncs    play;
static N_uint8      audioBuf[HALF_BUF_SAMPLES * 4 * 2];
static N_uint32     bufOffset = 0;
static ibool        bufferEmpty;
static N_uint32     halfBufSize;    // Half buffer size in bytes

#define NO_BUILD
#include "snap/audio/snapver.c"

const char *aaGetReleaseDate(void)
{ return release_date; };

const char *aaGetReleaseDate2(void)
{ return release_date2; };

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Displays the copyright signon banner on the screen
****************************************************************************/
static void banner(void)
{
    printf("AAMP3 - SciTech SNAP Audio MP3 Player\n");
    printf("        Release %s.%s (%s)\n\n",release_major,release_minor,release_date2);
    printf("%s\n",copyright_str);
    printf("\n");
}

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    EVT_exit();
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to check if a key has
been hit. We check for keydown and keyrepeat events, and we also flush the
event queue of all non keydown events to avoid it filling up.
****************************************************************************/
int EVT_kbhit(void)
{
    int     hit;
    event_t evt;

    hit = EVT_peekNext(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
    EVT_flush(~(EVT_KEYDOWN | EVT_KEYREPEAT));
    return hit;
}

/****************************************************************************
REMARKS:
Simple utility function to use the event library to read an ASCII keypress
and return it. This function will block until a key is hit.
****************************************************************************/
int EVT_getch(void)
{
    event_t evt;

    do {
        EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT);
        } while (EVT_asciiCode(evt.message) == 0);
    return EVT_asciiCode(evt.message);
}

/****************************************************************************
REMARKS:
Callback function called by the driver to fill up the playback buffer
****************************************************************************/
N_uint32 NAPI PlayCallback(void *buffer, N_uint32 size, void *reserved)
{
    memcpy(buffer, (char *)audioBuf + bufOffset, halfBufSize);
    bufferEmpty = TRUE;
    return halfBufSize;
}

/****************************************************************************
REMARKS:
Loads and plays a wave file from disk.
****************************************************************************/
void PlayMP3(AA_devCtx *dc, const char *fname, int bits)
{
    N_int32     rate;
    N_uint32    flags;

    switch (bits) {
        case 16:
            flags = aaDigitalStereo | aaDigital16Bit;
            break;
        case 20:
            flags = aaDigitalStereo | aaDigital20Bit;
            break;
        case 24:
            flags = aaDigitalStereo | aaDigital24Bit;
            break;
        case 32:
            flags = aaDigitalStereo | aaDigital32Bit;
            break;
        default:
            PM_fatalError("Unsupported bit depth requested!");
        }

    // Calculate size of halfbuffer (holds one MPEG audio frame)
    if (bits == 16)
        halfBufSize = HALF_BUF_SAMPLES * 2;
    else
        halfBufSize = HALF_BUF_SAMPLES * 4;

    if (MPG_Stream_Open(fname, bits) == 0) {

        // Read first frame of data
        MPG_Read_Frame();

        rate = g_sampling_frequency[g_frame_header.id][g_frame_header.sampling_frequency];

        if (play.SetPlaybackMode(&rate, flags) != nOK)
            PM_fatalError("Unable to set playback mode!");

        printf("%d-bit output, sampling rate %d Hz\n", bits, rate);

        // Kick off playback
        play.StartPlayback(halfBufSize, PlayCallback, true, NULL);
        bufferEmpty = FALSE;

        // Wait until playback is done
        while (!EVT_kbhit() && (MPG_Get_Filepos() != C_MPG_EOF)) {
            if (bufferEmpty) {
                MPG_Read_Frame();
                MPG_Decode_L3((uint32_t *)(audioBuf + bufOffset));
                if (bufOffset == 0)
                    bufOffset = halfBufSize;
                else
                    bufOffset = 0;
                bufferEmpty = FALSE;
                }
            }

        play.StopPlayback();
        }
}

/****************************************************************************
REMARKS:
Displays the main menu screen
****************************************************************************/
void MainScreen(
    int deviceIndex,
    const char *filename,
    int bits)
{
    AA_devCtx       *dc;
    AA_configInfo   info;
    AA_initFuncs    init;

    /* Load the device digital for this device */
    if ((dc = AA_loadDriver(deviceIndex)) == NULL)
        PM_fatalError(AA_errorMsg(AA_status()));
    init.dwSize = sizeof(init);
    if (!AA_queryFunctions(dc,AA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    play.dwSize = sizeof(play);
    AA_queryFunctions(dc,AA_GET_PLAYBACKFUNCS,&play);

    banner();
    //printf("Vendor Name:  %s\n",dc->OemVendorName);
    //printf("Copyright:    %s\n",dc->OemCopyright);
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    //printf("Version:      %d.%d\n", (int)dc->Version >> 8,(int)dc->Version & 0xFF);
    printf("Manufacturer:    %s\n", info.ManufacturerName);
    printf("Chipset:         %s\n", info.ChipsetName);
    printf("Audio Codec:     %s\n", info.CodecName);
    printf("Driver Revision: %s\n", info.VersionInfo);
    printf("Driver Build:    %s", info.BuildDate);
    if (info.Certified)
        printf(" (Certified %s)\n", info.CertifiedDate);
    else
        printf("\n");
    printf("\n");
    PlayMP3(dc, filename, bits);

    /* Unload the device driver for this device */
    AA_unloadDriver(dc);
}

/****************************************************************************
PARAMETERS:
x   - X coordinate of the mouse cursor position (screen coordinates)
y   - Y coordinate of the mouse cursor position (screen coordinates)

REMARKS:
This gets called periodically to move the mouse. It will get called when
the mouse may not have actually moved, so check if it has before redrawing
it.
****************************************************************************/
void EVTAPI moveMouse(
    int x,
    int y)
{
}

/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
    banner();
    printf("Usage: aamp3 filename.mp3 [options]\n");
    printf("\n");
    printf("Options are:\n");
    printf("    -b<bits> - Desired output bit depth. Supported values are\n");
    printf("               16, 20, 24, and 32. Default is 16.\n");
    exit(1);
}

int main(int argc,char *argv[])
{
    int         i,choice,numDevices;
    int         bits = 16;
    const char  *filename;

    if (argc < 2 || argc > 3)
        help();

    if (argc == 3) {
        if (strncmp(argv[2], "-b", 2))
            help();
        bits = atoi(argv[2] + 2);
        }
    filename = argv[1];

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    AA_registerLicense(OemLicense);
#endif

    PM_setFatalErrorCleanup(FatalErrorCleanup);
    PM_init();
    EVT_init(moveMouse);

    /* Find the number of audio devices attached */
    if ((numDevices = AA_enumerateDevices()) > 1) {
        for (;;) {
            banner();
            printf("Select audio adapter:\n\n");
            for (i = 0; i < numDevices; i++) {
                printf("  [%d] - Audio Adapter %d\n", i, i);
                }
            printf("  [Q] - Quit\n\n");
            printf("Choice: ");
            choice = EVT_getch();
            if (choice == 'q' || choice == 'Q' || choice == 0x1B)
                break;
            choice -= '0';
            if (choice >= 0 && choice < numDevices)
                MainScreen(choice, filename, bits);
            }
        }
    else
        MainScreen(0, filename, bits);

    /* Exit the event library */
    EVT_exit();

    return 0;
}
