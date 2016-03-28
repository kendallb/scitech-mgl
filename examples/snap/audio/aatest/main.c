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
* Description:  Command line driven, text based front end for the SciTech
*               SNAP Audio test program.
*
****************************************************************************/

#include "aatest.h"
#ifdef __UNIX__
#include <unistd.h>
#endif

#define MAX_MENU        40
#define PLAY_BUF_SIZE   (8 * 1024)
#define REC_BUF_SIZE    (8 * 1024)

// Fake "line" to select internal speaker
#define OUT_INT_SPEAKER     -1

/*------------------------- Structures and types --------------------------*/

typedef struct {
    AA_VolumeFlagsType      type;
    const char              *text;
    int                     numChannels;
    N_uint8                 *volumes;
    ibool                   onOrOff;
    N_uint32                selectBit;
    char                    toggle;
    char                    up;
    char                    down;
    } VolControl;

typedef struct {
    AA_waveFile             *wave;
    N_uint32                currPos;
    ibool                   reloop;
    } WaveContext;

typedef struct {
    N_uint32                rate;
    N_uint32                flags;
    int                     bits;
    } MenuItem;

typedef ibool (*doTestType)(int rate, int bits, N_uint32 flags);

/*--------------------------- Global variables ----------------------------*/

AA_initFuncs        init;
AA_playbackFuncs    play;
AA_recordFuncs      record;
AA_volumeFuncs      volume;
static PM_HWND      hwndConsole;
static void         *stateBuf;
static char         exePathName[PM_MAX_PATH];
static N_uint32     recordSource;
static WaveContext  wc;

static N_uint32 RateList[] = {
    8000,
    11025,
    16000,
    22050,
    32000,
    44100,
    48000,
    88200,
    96000,
    176400,
    192000,
    384000,
    0
    };

#define NO_BUILD
#include "snap/audio/snapver.c"

const char *gaGetReleaseDate(void)
{ return release_date; };

const char *gaGetReleaseDate2(void)
{ return release_date2; };

#ifdef  ISV_LICENSE
#include "isv.c"
#endif

#define TEST_16STEREO_WAV   "44-16-2.wav"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Displays the copyright signon banner on the screen
****************************************************************************/
static void banner(void)
{
    CON_printf("AATest - SciTech SNAP Audio Driver Compliance Test\n");
    CON_printf("         Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    CON_printf("%s\n",copyright_str);
    CON_printf("\n");
}

/****************************************************************************
REMARKS:
Displays the chipset info banner on the screen
****************************************************************************/
static void showChipInfo(void)
{
    AA_configInfo   info;

    //CON_printf("Vendor Name:  %s\n",dc->OemVendorName);
    //CON_printf("Copyright:    %s\n",dc->OemCopyright);
    info.dwSize = sizeof(info);
    init.GetConfigInfo(&info);
    //CON_printf("Version:      %d.%d\n", (int)dc->Version >> 8,(int)dc->Version & 0xFF);
    CON_printf("Manufacturer:    %s\n", info.ManufacturerName);
    CON_printf("Chipset:         %s\n", info.ChipsetName);
    CON_printf("Audio Codec:     %s\n", info.CodecName);
    CON_printf("Driver Revision: %s\n", info.VersionInfo);
    CON_printf("Driver Build:    %s", info.BuildDate);
    if (info.Certified)
        CON_printf(" (Certified %s)\n", info.CertifiedDate);
    else
        CON_printf("\n");
    CON_printf("\n");
}

/****************************************************************************
REMARKS:
Resets the active device to 0 and displays the fatal error message.
****************************************************************************/
void PMAPI FatalErrorCleanup(void)
{
    EVT_exit();
    CON_restoreMode();
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
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
Handles the choice of audio format.
****************************************************************************/
ibool doChoice(
    MenuItem *menu,
    int maxmenu,
    doTestType doTest)
{
    int         choice;

    CON_printf("  [Esc] - Quit\n\n");
    CON_printf("Choice: ");
    choice = EVT_getch();
    choice = tolower(choice);
    if (choice == 0x1B)
        return true;
    if (choice >= 'a')
        choice = choice - 'a' + 10;
    else
        choice -= '0';
    if (0 <= choice && choice < maxmenu) {
        CON_clrscr();
        banner();
        doTest(menu[choice].rate, menu[choice].bits, menu[choice].flags);
        }
    return false;
}

/****************************************************************************
REMARKS:
Function to produce nice textual description of an audio format.
****************************************************************************/
static int getFormatName(
    char *buffer,
    int length,
    N_uint32 rate,
    N_uint32 flags,
    AA_formatInfo *fi)
{
    const char  *bits;
    int         bitCount;

    if (flags & aaDigital8Bit) {
        bits = "8-bit";
        bitCount = 8;
        }
    else if (flags & aaDigital16Bit) {
        bits = "16-bit";
        bitCount = 16;
        }
    else if (flags & aaDigital20Bit) {
        bits = "20-bit";
        bitCount = 20;
        }
    else if (flags & aaDigital24Bit) {
        bits = "24-bit";
        bitCount = 24;
        }
    else if (flags & aaDigital32Bit) {
        bits = "32-bit";
        bitCount = 32;
        }
    else if (flags & aaDigitalFloat32) {
        bits = "32-bit float";
        bitCount = 32;
        }
    else {
        bits = "?-bit";
        bitCount = 0;
        }

    sprintf(buffer, "%d Hz, %s, %s, %s, %s",
        rate, bits,
        fi->Flags & aaDigitalStereo ? "Stereo" : "Mono",
        fi->Flags & aaDigitalSigned ? "Signed" : "Unsigned",
        fi->Flags & aaDigitalBigEndian ? "Big Endian" : "Little Endian");

    return bitCount;
}

/****************************************************************************
REMARKS:
Function to add a format to the menu list and generate the name for the
format. Checks to verify that the format is supported.
****************************************************************************/
static int addFormatIfAvailable(
    AA_devCtx *dc,
    MenuItem *menu,
    int maxmenu,
    N_uint32 rate,
    N_uint32 flags)
{
    AA_formatInfo   *format;
    int             num, bits;
    char            buf[80];

    /* Check if rate/bits per sample is supported */
    for (format = dc->FormatInfo; format->Flags != 0; format++) {
        if ((format->Flags & flags) != flags)
            continue;
        if ((format->MinPhysRate <= rate) && (format->MaxPhysRate >= rate)) {
            if (maxmenu < 10)
                num = '0' + maxmenu;
            else
                num = 'A' + maxmenu - 10;
            bits = getFormatName(buf, sizeof(buf), rate, flags, format);
            CON_printf("  [%c] - %s\n",num,buf);
            menu[maxmenu].bits = bits;
            menu[maxmenu].rate = rate;
            menu[maxmenu].flags = format->Flags;
            ++maxmenu;
            }
        }
    return maxmenu;
}

/****************************************************************************
REMARKS:
Main menu for testing all 8-bit formats
****************************************************************************/
static void test8(
    AA_devCtx *dc,
    doTestType doTest,
    N_uint32 flags)
{
    int         maxmenu;
    MenuItem    menu[MAX_MENU];
    N_uint32    *rate;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Which 8-bit audio format to test:\n\n");
        maxmenu = 0;
        for (rate = RateList; *rate; ++rate) {
            maxmenu = addFormatIfAvailable(dc,menu,maxmenu,*rate,flags | aaDigital8Bit);
            }
        if (doChoice(menu,maxmenu,doTest))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all 16-bit formats
****************************************************************************/
static void test16(
    AA_devCtx *dc,
    doTestType doTest,
    N_uint32 flags)
{
    int         maxmenu;
    MenuItem    menu[MAX_MENU];
    N_uint32    *rate;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Which 16-bit audio format to test:\n\n");
        maxmenu = 0;
        for (rate = RateList; *rate; ++rate) {
            maxmenu = addFormatIfAvailable(dc,menu,maxmenu,*rate,flags | aaDigital16Bit);
            }
        if (doChoice(menu,maxmenu,doTest))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all 20-bit formats
****************************************************************************/
static void test20(
    AA_devCtx *dc,
    doTestType doTest,
    N_uint32 flags)
{
    int         maxmenu;
    MenuItem    menu[MAX_MENU];
    N_uint32    *rate;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Which 20-bit audio format to test:\n\n");
        maxmenu = 0;
        for (rate = RateList; *rate; ++rate) {
            maxmenu = addFormatIfAvailable(dc,menu,maxmenu,*rate,flags | aaDigital20Bit);
            }
        if (doChoice(menu,maxmenu,doTest))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all 24-bit formats
****************************************************************************/
static void test24(
    AA_devCtx *dc,
    doTestType doTest,
    N_uint32 flags)
{
    int         maxmenu;
    MenuItem    menu[MAX_MENU];
    N_uint32    *rate;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Which 24-bit audio format to test:\n\n");
        maxmenu = 0;
        for (rate = RateList; *rate; ++rate) {
            maxmenu = addFormatIfAvailable(dc,menu,maxmenu,*rate,flags | aaDigital24Bit);
            }
        if (doChoice(menu,maxmenu,doTest))
            break;
        }
}

/****************************************************************************
REMARKS:
Main menu for testing all 32-bit formats
****************************************************************************/
static void test32(
    AA_devCtx *dc,
    doTestType doTest,
    N_uint32 flags)
{
    int         maxmenu;
    MenuItem    menu[MAX_MENU];
    N_uint32    *rate;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Which 32-bit audio format to test:\n\n");
        maxmenu = 0;
        for (rate = RateList; *rate; ++rate) {
            maxmenu = addFormatIfAvailable(dc,menu,maxmenu,*rate,flags | aaDigital32Bit);
            }
        if (doChoice(menu,maxmenu,doTest))
            break;
        }
}

/****************************************************************************
REMARKS:
Callback function called by the driver to fill up the playback buffer
****************************************************************************/
N_uint32 NAPI PlayCallback(void *buffer, N_uint32 size, void *context)
{
    WaveContext *wc = context;
    AA_waveFile *wave = wc->wave;
    N_uint32    len;

    // Copy data from wave file
    len = MIN(size, wave->length - wc->currPos);
    memcpy(buffer, (char *)(wave->buffer) + wc->currPos, len);
    wc->currPos += len;

    // Reloop from beginning of wave buffer if continuous play
    if (wc->reloop && wc->currPos >= wave->length)
        wc->currPos = 0;

    // Clear rest of the buffer if no more data
    if (len < size)
        memset((char *)buffer + len, 0, size - len);

    return len;
}

/****************************************************************************
REMARKS:
Plays a waveform.
****************************************************************************/
void PlayWave(
    AA_waveFile *wave,
    const char *name,
    ibool reloop)
{
    N_int32     rate;
    N_uint32    flags;

    rate = wave->rate;
    flags = 0;
    if (wave->channel == 2)
        flags |= aaDigitalStereo;
    if (wave->bit == 8)
        flags |= aaDigital8Bit;
    else if (wave->bit == 16)
        flags |= aaDigital16Bit;
    else if (wave->bit == 24)
        flags |= aaDigital24Bit;

    if (play.SetPlaybackMode(&rate, flags) != nOK) {
        CON_printf("Unable to set playback mode!");
        EVT_getch();
        return;
        }

    if (name != NULL)
        CON_printf("\n\nPlaying waveform (%s)...\n\n", name);

    // Create self-contained wave context for callback
    wc.wave = wave;
    wc.currPos = 0;
    wc.reloop = reloop;
    play.StartPlayback(PLAY_BUF_SIZE, PlayCallback, true, &wc);

    if (reloop)
        return;

    // Wait until playback is done
    while (wc.currPos < wc.wave->length) {
        CON_printf("\r%d%%", wc.currPos / (wc.wave->length / 100 + 1));
        if (EVT_kbhit()) {
            EVT_getch();
            break;
            }
        PM_sleep(100);
        }

    play.StopPlayback();
}

/****************************************************************************
REMARKS:
Loads and plays a wave file from disk.
****************************************************************************/
void PlayWaveFile(
    const char *wavefile,
    ibool reloop)
{
    char        drive[PM_MAX_DRIVE];
    char        dir[PM_MAX_PATH];
    char        name[PM_MAX_PATH];
    char        ext[PM_MAX_PATH];

    char        filename[PM_MAX_PATH];
    AA_waveFile *wave;

    PM_splitpath(exePathName,drive,dir,name,ext);
    PM_makepath(filename,drive,dir,NULL,NULL);
    PM_backslash(filename);
    strcat(filename, "wav");
    PM_backslash(filename);
    strcat(filename, wavefile);

    if ((wave = WAV_load(filename)) == NULL) {
        CON_printf("Unable to load test file %s", filename);
        EVT_getch();
        return;
        }

    PlayWave(wave, wavefile, reloop);

    if (reloop)
        return;

    WAV_unload(wave);
    wave = NULL;
}

/****************************************************************************
REMARKS:
Stops looping wave file from playing and releases its resources.
****************************************************************************/
void StopWaveFile(void)
{
    play.StopPlayback();

    if (wc.wave)
        WAV_unload(wc.wave);
    wc.wave = NULL;
}

/****************************************************************************
REMARKS:
Run a playback test.
****************************************************************************/
ibool testPlayback(
    int rate,
    int bits,
    N_uint32 flags)
{
    char    filename[32];
    int     channels = 0;

    if (flags & aaDigitalMono)
        channels = 1;
    else if (flags & aaDigitalStereo)
        channels = 2;

    sprintf(filename, "%d-%d-%d.wav", rate / 1000, bits, channels);
    PlayWaveFile(filename, false);
    return false;
}

/****************************************************************************
REMARKS:
Displays the playback test screen
****************************************************************************/
void PlaybackScreen(
    AA_devCtx *dc)
{
    int             choice;

    /* Display selection screen and process user input */
    for (;;) {
        CON_clrscr();
        banner();
        showChipInfo();
        CON_printf("Select test to perform:\n\n");
        CON_printf("  [0] - Play 8-bit Digital Sound\n");
        CON_printf("  [1] - Play 16-bit Digital Sound\n");
        CON_printf("  [2] - Play 20-bit Digital Sound\n");
        CON_printf("  [3] - Play 24-bit Digital Sound\n");
        CON_printf("  [4] - Play 32-bit Digital Sound\n");
        CON_printf("  [Q] - Quit\n\n");
        CON_printf("Choice: ");
        choice = tolower(EVT_getch());
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test8(dc, testPlayback, aaDigitalPlayback);     break;
            case '1':   test16(dc, testPlayback, aaDigitalPlayback);    break;
            case '2':   test20(dc, testPlayback, aaDigitalPlayback);    break;
            case '3':   test24(dc, testPlayback, aaDigitalPlayback);    break;
            case '4':   test32(dc, testPlayback, aaDigitalPlayback);    break;
            }
        CON_printf("\n");
        }
    CON_clrscr();
}

/****************************************************************************
REMARKS:
Callback function called by the driver to empty the recording buffer
****************************************************************************/
N_uint32 NAPI RecordCallback(void *buffer, N_uint32 size, void *context)
{
    WaveContext     *wc = context;
    AA_waveFile     *recBuf = wc->wave;
    N_uint32        len;

    // Copy data to recording buffer
    len = MIN(size, recBuf->length - wc->currPos);
    if (len > 0) {
        memcpy((N_uint8 *)(recBuf->buffer) + wc->currPos, buffer, len);
        wc->currPos += len;
        }
    return len;
}

/****************************************************************************
REMARKS:
Run a recording test.
****************************************************************************/
ibool testRecord(
    int rate,
    int bits,
    N_uint32 flags)
{
    AA_waveFile     recWave;
    WaveContext     wc;

    recWave.length = 1024 * 1024;   // 1MB record buffer
    recWave.buffer = malloc(recWave.length);
    if (recWave.buffer == NULL) {
        CON_printf("Unable to allocate record buffer!");
        EVT_getch();
        return false;
        }

    if (record.SetRecordMode(&rate, flags) != nOK) {
        CON_printf("Unable to set record mode!");
        EVT_getch();
        return false;
        }

    recWave.rate = rate;
    recWave.format = 0;
    recWave.channel = flags & aaDigitalStereo ? 2 : 1;
    recWave.bit = 0;

    if (flags & aaDigital8Bit)
        recWave.bit = 8;
    else if (flags & aaDigital16Bit)
        recWave.bit = 16;
    else if (flags & aaDigital20Bit)
        recWave.bit = 20;
    else if (flags & aaDigital24Bit)
        recWave.bit = 24;
    else if (flags & aaDigital32Bit)
        recWave.bit = 32;

    // Select desired input
    volume.SelectInput(recordSource);
    volume.SetVolume(aaVolumeInputGain, aaChannelMaster, 0);

    if (recordSource == aaInputMicrophone) {
        if (volume.SetMicrophoneAGC)
            volume.SetMicrophoneAGC(true);
        volume.SetVolume(aaVolumeMicrophone, aaChannelMaster, 192);
        }

    CON_printf("\n\nRecording buffer...\n\n");

    // Create self-contained wave context for callback
    wc.wave = &recWave;
    wc.currPos = 0;
    wc.reloop = false;
    record.StartRecord(REC_BUF_SIZE, RecordCallback, true, &wc);

    // Wait until record buffer is full
    while (wc.currPos < recWave.length) {
        CON_printf("\r%d%%", wc.currPos / (recWave.length / 100 + 1));
        if (EVT_kbhit()) {
            EVT_getch();
            break;
            }
        PM_sleep(100);
        }
    record.StopRecord();

    // Select nothing for input
    volume.SelectInput(0);

    // Play back the recorded data
    recWave.length = wc.currPos;
    PlayWave(&recWave, "recorded buffer", false);
    free(recWave.buffer);

    return true;
}

/****************************************************************************
REMARKS:
Displays the recording bit depth selection screen
****************************************************************************/
void RecordFormatScreen(
    AA_devCtx *dc,
    N_uint32 source)
{
    int             choice;

    recordSource = source;  /* Nasty, evil global */
    /* Display selection screen and process user input */
    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Select test to perform:\n\n");
        CON_printf("  [0] - Record 8-bit Digital Sound\n");
        CON_printf("  [1] - Record 16-bit Digital Sound\n");
        CON_printf("  [2] - Record 20-bit Digital Sound\n");
        CON_printf("  [3] - Record 24-bit Digital Sound\n");
        CON_printf("  [4] - Record 32-bit Digital Sound\n");
        CON_printf("  [Q] - Quit\n\n");
        CON_printf("Choice: ");
        choice = tolower(EVT_getch());
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   test8(dc, testRecord, aaDigitalRecord);     break;
            case '1':   test16(dc, testRecord, aaDigitalRecord);    break;
            case '2':   test20(dc, testRecord, aaDigitalRecord);    break;
            case '3':   test24(dc, testRecord, aaDigitalRecord);    break;
            case '4':   test32(dc, testRecord, aaDigitalRecord);    break;
            }
        CON_printf("\n");
        }
    CON_clrscr();
}

/****************************************************************************
REMARKS:
Displays the recording test screen
****************************************************************************/
void RecordScreen(
    AA_devCtx *dc)
{
    int             choice;

    for (;;) {
        CON_clrscr();
        banner();
        CON_printf("Select input for recording:\n\n");
        CON_printf("  [0] - Microphone\n");
        CON_printf("  [1] - Line Input\n");
        CON_printf("  [2] - CD Audio\n");
        CON_printf("  [Q] - Quit\n\n");
        CON_printf("Choice: ");
        choice = tolower(EVT_getch());
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   RecordFormatScreen(dc, aaInputMicrophone);  break;
            case '1':   RecordFormatScreen(dc, aaInputLineIn);      break;
            case '2':   RecordFormatScreen(dc, aaInputCD);          break;
            }
        CON_printf("\n");
        }
    CON_clrscr();
}

/****************************************************************************
REMARKS:
Displays description for a single volume control, if present
****************************************************************************/
static void DisplayVolumeControl(
    VolControl *control)
{
    if (control->type) {
        if (control->selectBit)
            CON_printf("%20s : Toggle [%c] : Vol Up [%c] : Vol Down [%c] : %02d%% %s\n",
                control->text, control->toggle, control->up, control->down,
                control->volumes[0] * 100 / 255, control->onOrOff ? "On" : "Muted");
        else
            CON_printf("%20s : (no mute)  : Vol Up [%c] : Vol Down [%c] : %02d%%\n",
                control->text, control->up, control->down,
                control->volumes[0] * 100 / 255);
        }
}

/****************************************************************************
REMARKS:
Increase/decrease volume for all channels in an output
****************************************************************************/
static void SetVolumes(
    VolControl *control,
    int step)
{
    int     i;

    for (i = 0; i < control->numChannels; ++i) {
        int     vol;

        vol = control->volumes[i];
        vol += step;
        if (vol > 255)
            vol = 255;
        if (vol < 0)
            vol = 0;

        control->volumes[i] = vol;
        volume.SetVolume(control->type, 1 << i, vol);
        }
}

/****************************************************************************
REMARKS:
Turn an output on or off
****************************************************************************/
static void ToggleLine(
    VolControl *control)
{
    N_uint32    state;

    control->onOrOff = control->onOrOff ^ 1;

    if (control->selectBit == OUT_INT_SPEAKER) {
        volume.SetBuiltinSpeaker(control->onOrOff);
        }
    else {
        state = volume.GetOutputSelect();
        state &= ~control->selectBit;

        if (control->onOrOff)
            state |= control->selectBit;

        volume.SelectOutput(state);
        }
}

/****************************************************************************
REMARKS:
Processes a keystroke for a volume control
****************************************************************************/
static int ProcessControlKey(
    VolControl *control,
    int choice)
{
    choice = toupper(choice);
    if (control->type) {
        if (toupper(control->up) == choice) {
            SetVolumes(control, 8);
            return true;
            }
        else if (toupper(control->down) == choice) {
            SetVolumes(control, -8);
            return true;
            }
        else if (toupper(control->toggle) == choice) {
            ToggleLine(control);
            }
        }
    return false;
}

/****************************************************************************
REMARKS:
Sets up control structure for a volume control
****************************************************************************/
static void SetupVolumeControl(
    AA_VolumeFlagsType available,
    AA_VolumeFlagsType kind,
    AA_OutputFlagsType selectBit,
    const char *description,
    VolControl *control,
    char keyToggle,
    char keyUp,
    char keyDown)
{
    int     i;

    if (available & kind) {
        control->type = kind;
        control->text = description;
        control->selectBit = selectBit;

        if (selectBit == OUT_INT_SPEAKER)
            control->onOrOff = volume.GetBuiltinSpeaker();
        else
            control->onOrOff = (volume.GetOutputSelect() & selectBit) != 0;

        control->numChannels = 8;   /* Maximum currently supported */
        control->volumes = malloc(sizeof(*(control->volumes)) * control->numChannels);
        if (control->volumes == NULL) {
            CON_printf("Out of memory!");
            EVT_getch();
            return;
            }
        /* Query current volume settings (so we can increase/decrease) */
        for (i = 0; i < control->numChannels; ++i) {
            /* Sort of hardcoded for the channel bits... but that isn't gonna change */
            control->volumes[i] = volume.GetVolume(kind, 1 << i);
            }
        control->toggle = keyToggle;
        control->up = keyUp;
        control->down = keyDown;
        }
}

/****************************************************************************
REMARKS:
Displays the playback test screen
****************************************************************************/
void VolumeScreen(
    AA_devCtx *dc)
{
    int                 choice, i, maxControls;
    AA_VolumeFlagsType  avail;
    VolControl          controls[12];

    /* Set up volume controls */
    memset(controls, 0, sizeof(controls));
    maxControls = sizeof(controls)/sizeof(controls[0]);
    avail = volume.QueryAvailableVolumeControls();

    /* If internal speaker on/off switch is available, always show the control even
     * if the volume can't be controlled separately.
     */
    if (volume.SetBuiltinSpeaker)
        avail |= aaVolumeBuiltinSpeaker;

    SetupVolumeControl(avail, aaVolumeMaster, 0, "Master", &controls[0], 'Q', 'A', 'Z');
    SetupVolumeControl(avail, aaVolumeDigital, aaOutputDigital, "Digital", &controls[1], 'W', 'S', 'X');
    SetupVolumeControl(avail, aaVolumeMIDI, aaOutputMIDI, "MIDI", &controls[2], 'E', 'D', 'C');
    SetupVolumeControl(avail, aaVolumeCD, aaOutputCD, "CD", &controls[3], 'R', 'F', 'V');
    SetupVolumeControl(avail, aaVolumeLineIn, aaOutputLineIn, "Line In", &controls[4], 'T', 'G', 'B');
    SetupVolumeControl(avail, aaVolumeMicrophone, aaOutputMicrophone, "Microphone", &controls[5], 'Y', 'H', 'N');
    SetupVolumeControl(avail, aaVolumePCSpeaker, aaOutputPCSpeaker, "PC Speaker", &controls[6], 'U', 'J', 'M');
    SetupVolumeControl(avail, aaVolumeAux1, aaOutputAux1, "Auxiliary 1", &controls[7], 'I', 'K', ',');
    SetupVolumeControl(avail, aaVolumeAux2, aaOutputAux2, "Auxiliary 2", &controls[8], 'O', 'L', '.');
    SetupVolumeControl(avail, aaVolumeBuiltinSpeaker, OUT_INT_SPEAKER, "Builtin Speaker", &controls[9], 'P', ';', '/');

    for (;;) {
        CON_clrscr();
        banner();
        showChipInfo();
        CON_printf("Available Volume Controls:\n\n");
        for (i = 0; i < maxControls; ++i) {
            DisplayVolumeControl(&controls[i]);
            }

        CON_printf("\n  [0] - Play Test Digital Sound\n");
        CON_printf("  [1] - Stop Test Digital Sound\n");
        CON_printf("  [~] - Quit\n\n");
        CON_printf("Choice: ");
        choice = tolower(EVT_getch());
        if (choice == '`' || choice == '~' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   StopWaveFile();
                        PlayWaveFile(TEST_16STEREO_WAV, true);
                        break;
            case '1':   StopWaveFile();
                        break;
            default:
                for (i = 0; i < maxControls; ++i) {
                    if (ProcessControlKey(&controls[i], choice))
                        break;
                    }
            }
        CON_printf("\n");
        }

    // Stop looping test sound and release wave file resources
    StopWaveFile();

    /* Free memory used to store current state of controls */
    for (i = 0; i < maxControls; ++i) {
        if (controls[i].volumes != NULL)
            free(controls[i].volumes);
        }
    CON_clrscr();
}

/****************************************************************************
REMARKS:
Displays the main menu screen
****************************************************************************/
void MainScreen(
    int deviceIndex)
{
    int             choice;
    AA_devCtx       *dc;

    /* Load the device driver for this device */
    if ((dc = AA_loadDriver(deviceIndex)) == NULL)
        PM_fatalError(AA_errorMsg(AA_status()));
    init.dwSize = sizeof(init);
    if (!AA_queryFunctions(dc,AA_GET_INITFUNCS,&init))
        PM_fatalError("Unable to get device driver functions!");
    play.dwSize = sizeof(play);
    AA_queryFunctions(dc,AA_GET_PLAYBACKFUNCS,&play);
    volume.dwSize = sizeof(volume);
    AA_queryFunctions(dc,AA_GET_VOLUMEFUNCS,&volume);
    record.dwSize = sizeof(record);
    AA_queryFunctions(dc,AA_GET_RECORDFUNCS,&record);

    for (;;) {
        CON_clrscr();
        banner();
        showChipInfo();
        CON_printf("Select test to perform:\n\n");
        CON_printf("  [0] - Test Playback\n");
        CON_printf("  [1] - Test Recording\n");
        CON_printf("  [2] - Test Volume Controls\n");
        CON_printf("  [Q] - Quit\n\n");
        CON_printf("Choice: ");
        choice = tolower(EVT_getch());
        if (choice == 'q' || choice == 'Q' || choice == 0x1B)
            break;
        switch (choice) {
            case '0':   PlaybackScreen(dc);         break;
            case '1':   RecordScreen(dc);           break;
            case '2':   VolumeScreen(dc);           break;
            }
        CON_printf("\n");
        }
    CON_clrscr();

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
    printf("AATest - SciTech SNAP Audio Driver Compliance Test\n");
    printf("         Release %s.%s (%s)\n\n",release_major,release_minor,release_date);
    printf("%s\n",copyright_str);
    printf("\n");
    printf("Options are:\n");
    printf("    -h       - Display this help screen\n");
    exit(1);
}

/****************************************************************************
REMARKS:
Parses the command line arguments.
****************************************************************************/
static void parseArguments(
    int argc,
    char *argv[])
{
    int     option;
    char    *argument;

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"h",&argument);
        switch (option) {
            case ALLDONE:
                break;
            case 'h':
            case PARAMETER:
            case INVALID:
            default:
                help();
            }
        } while (option != ALLDONE);
}

int main(int argc,char *argv[])
{
    int     i,choice,numDevices;

    /* Locate the executable path for local sound resources */
#ifdef __UNIX__
    if (readlink("/proc/self/exe", exePathName, PM_MAX_PATH) == -1)
#endif
        strcpy(exePathName,argv[0]);

    /* Parse the command line */
    parseArguments(argc,argv);

    /* Save the state of the console */
    hwndConsole = PM_openConsole(0,0,0,0,0,true);
    if ((stateBuf = malloc(PM_getConsoleStateSize())) == NULL)
        PM_fatalError("Out of memory!");
    PM_saveConsoleState(stateBuf,hwndConsole);

    /* Initialise the event library */
    EVT_init(moveMouse);

    /* Register our fatal error cleanup handler */
    PM_setFatalErrorCleanup(FatalErrorCleanup);

    /* Register the ISV license file if desired */
#ifdef  ISV_LICENSE
    AA_registerLicense(OemLicense);
#endif

    /* Find the number of display devices attached */
    ZTimerInit();
    PM_init();
    CON_init();
    if ((numDevices = AA_enumerateDevices()) > 1) {
        for (;;) {
            CON_clrscr();
            banner();
            CON_printf("Select audio adapter:\n\n");
            for (i = 0; i < numDevices; i++) {
                CON_printf("  [%d] - Audio Adapter %d\n", i, i);
                }
            CON_printf("  [Q] - Quit\n\n");
            CON_printf("Choice: ");
            choice = EVT_getch();
            if (choice == 'q' || choice == 'Q' || choice == 0x1B)
                break;
            choice -= '0';
            if (choice >= 0 && choice < numDevices)
                MainScreen(choice);
            }
        }
    else
        MainScreen(0);
    CON_restoreMode();

    /* Exit the event library */
    EVT_exit();

    /* Restore the console */
    PM_restoreConsoleState(stateBuf,hwndConsole);
    PM_closeConsole(hwndConsole);
    return 0;
}
