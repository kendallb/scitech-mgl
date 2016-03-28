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
* Description:  Command line driven, audio file playback utility.
*
****************************************************************************/

#include "aaplay.h"

/*--------------------------- Global variables ----------------------------*/

AA_playbackFuncs    play;
static AA_waveFile  *wave;
static N_uint32     currPos;

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
    printf("AAPlay - SciTech SNAP Audio Wave Playback Utility\n");
    printf("         Release %s.%s (%s)\n\n",release_major,release_minor,release_date2);
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
PARAMETERS:
x   - X coordinate of the mouse cursor position (screen coordinates)
y   - Y coordinate of the mouse cursor position (screen coordinates)

REMARKS:
This gets called periodically to move the mouse. It will get called when
the mouse may not have actually moved, so check if it has before redrawing
it.
****************************************************************************/
void EVTAPI moveMouse(int x,int y)
{ 
    //we don't care about the mouse movement
}

/****************************************************************************
REMARKS:
Callback function called by the driver to fill up the playback buffer
****************************************************************************/
N_uint32 NAPI PlayCallback(void *buffer, N_uint32 size, void *reserved)
{
    N_uint32    len;

    // Copy data from wave file
    len = MIN(size, wave->length - currPos);
    memcpy(buffer, (char *)(wave->buffer) + currPos, len);
    currPos += len;

    // Clear rest of the buffer if no more data
    if (len < size)
        memset((char *)buffer + len, 0, size - len);

    return len;
}

/****************************************************************************
REMARKS:
Loads and plays a wave file from disk.
****************************************************************************/
void PlayWave(
    const char *filename)
{
    char        errmsg[64];
    N_int32     rate;
    N_uint32    flags;

    if ((wave = WAV_load(filename)) == NULL) {
        sprintf(errmsg, "Unable to load test file:\n%s", filename);
        PM_fatalError(errmsg);
        }

    rate = wave->rate;
    flags = 0;
    if (wave->channel == 2)
        flags |= aaDigitalStereo;
    if (wave->bit == 16)
        flags |= aaDigital16Bit;

    if (play.SetPlaybackMode(&rate, flags) != nOK)
        PM_fatalError("Unable to set playback mode!");

    printf("\n\nPlaying waveform (%s)...\n\n", filename);
    currPos = 0;
    play.StartPlayback(8 * 1024, PlayCallback, true, NULL);

    // Wait until playback is done
    while (currPos < wave->length) {
        printf("\r%d%%", currPos / (wave->length / 100 + 1));
        if (EVT_kbhit()) {
            EVT_getch();
            break;
            }
        PM_sleep(100);
        }

    play.StopPlayback();

    WAV_unload(wave);
    wave = NULL;
}

/****************************************************************************
REMARKS:
Displays the main menu screen
****************************************************************************/
void MainScreen(
    int deviceIndex,
    const char *filename)
{
    AA_devCtx       *dc;
    AA_configInfo   info;
    AA_initFuncs    init;

    /* Load the device driver for this device */
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
    printf("\n");
    PlayWave(filename);

    /* Unload the device driver for this device */
    AA_unloadDriver(dc);
}

/****************************************************************************
REMARKS:
Display the command line usage information.
****************************************************************************/
static void help(void)
{
    banner();
    printf("Usage: aaplay filename.wav\n");
    exit(1);
}

int main(int argc,char *argv[])
{
    int         i,choice,numDevices;
    const char  *filename;

    if (argc != 2)
        help();

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
                MainScreen(choice, filename);
            }
        }
    else
        MainScreen(0, filename);

    /* Exit the event library */
    EVT_exit();

    return 0;
}
