/******************************************************************************
File:      Sound.c
Project:   Fox and the bear
Copyright: 1996 DiamondWare, Ltd.  All rights reserved.*
Written:   Angelo Nunes and Erik Lorenzen
Purpose:   Contains the high-level sound interface, callable by Fox N Bear
           for DOS, Windows 95, and Windows NT.  (The WIN-STK supports
           Windows 3.1 also)
History:   03/25/96 AN started

*Permission is expressely granted to use this program or any derivitive made
 from it to registered users of the STK or WIN-STK.
******************************************************************************/



#if defined(_WIN32) && !defined(WIN32)
  #define WIN32
#endif

#if defined(__WIN32__) && !defined(WIN32)
  #define WIN32
  #include <windows.h>
#endif

#if defined(__NT__) && !defined(WIN32)
  #define WIN32

  #include <windows.h>
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sound.h"                     //header for this module



#ifdef WIN32
  #include "winstk.h"



  #define MIDISONG  "SOUNDS\\FUNK2.MID"
  #define word      WORD
  #define byte      BYTE
#else
  #include <sys\stat.h>



  #include "dosstk.h"
#endif



#define HIGH       60000               //sound priorities
#define LOW        10000



char *SndFileName[sound_TOTAL] =       //array of filenames
{
  "SOUNDS\\STRIKE.DWD",
  "SOUNDS\\STUNNED.DWD",
  "SOUNDS\\MISS.DWD",
  "SOUNDS\\THROW.DWD",
  "SOUNDS\\JUMP.DWD",
  "SOUNDS\\STOP.DWD",
  #ifdef WIN32
    "SOUNDS\\FOXNBEAR.DWD"
  #else
    "SOUNDS\\FUNK2.DWM"
  #endif
};


#ifdef WIN32
  static HGLOBAL hsound[sound_TOTAL];
  static word    diginited = 0;
  static word    musinited = 0;
#endif


static byte *sound[sound_TOTAL];   //array of dwd buffers

static word initted = 0;
static word loaded  = 0;
static word midi    = 0;

char error_string[255];



/**********************************************************************\
* terminate_game                                                       *
*                                                                      *
* If for some reason we can't continue, like for example if a data     *
* file is missing, do an abnormal termination.                         *
\**********************************************************************/
static void TerminateGame(void)
{
  sound_Kill();

  #ifdef WIN32
    MessageBox(0, error_string, "Fox and The Bear", MB_ICONSTOP | MB_OK);
  #else
    printf("\nFox and The Bear\n%s", error_string);
  #endif

  exit(-1);
}


/**********************************************************************\
*free_sounds                                                           *
*                                                                      *
*If the we're done using the sounds & their memory we need to          *
*deallocate and unlock memory.                                         *
\**********************************************************************/
static void FreeIt(void)
{
  word i;

  for (i=0;i<sound_TOTAL;i++)
  {
    /*if sound[i] is not null free it*/
    if (sound[i])
    {
      free(sound[i]);
      sound[i] = 0;
    }
  }
}


/**********************************************************************\
*load_sounds                                                           *
*                                                                      *
* We need to load the sounds from disk, then allocate & lock memory.   *
\**********************************************************************/
#ifdef WIN32
static void LoadIt(void)
{
  byte *ptr;
  byte *nptr, ch;
  FILE *fp;
  long sz, much;
  int  index, i;

  if (!loaded)
  {
    loaded = 1;

    for (index=0;index<sound_TOTAL;index++)
    {
      fp = fopen(SndFileName[index], "rb");

      if ((fp == NULL) && (index == sound_SONG))
      {
        /*If the digital sound track is not available, use the midi version*/
        midi = 1;

        return;
      }

      /*if file not open correctly terminate*/
      if (fp == NULL)
      {
        wsprintf(error_string, "File %s missing.", SndFileName[index]);
        TerminateGame();
      }
      else
      {
        /*get file size*/
        sz = fseek(fp,0,SEEK_END);
        sz = ftell(fp);
        fseek(fp,0,SEEK_SET);

        hsound[index] = GlobalAlloc(GHND, sz);

        /*if outta memory terminate*/
        if (!hsound[index])
        {
          wsprintf(error_string, "Out of sound memory.");
          TerminateGame();
        }

        /*lock memory*/
        sound[index] = (byte far *)GlobalLock(hsound[index]);

        if (sound[index] == NULL)
        {
          wsprintf(error_string, "Can't lock sound memory.");
          TerminateGame();
        }

        nptr = malloc(4096);
        ptr  = sound[index];
        much = 0;

        /*load file in 4K chunks*/
        do
        {
          much = sz > 4096 ? 4096 : sz;

          fread(nptr, (size_t)much, 1, fp);

          for (i=0;i<much;i++)
          {
            ch = nptr[i];
            *ptr++ = ch;
          }

          sz -= much;

        } while (sz);

        free(nptr);
        fclose(fp);
      }
    }
  }
}
#else
static void LoadIt(void)               //Dos LoadIt
{
  FILE *fp;
  struct stat st;
  word i;

  if (!loaded)
  {
    loaded = 1;

    for (i=0;i<sound_TOTAL;i++)
    {
      if (!stat(SndFileName[i], &st))
      {
        sound[i] = (byte *)malloc((size_t)st.st_size);

        /*if outta memory terminate*/
        if (!sound[i])
        {
          sprintf(error_string, "\nOutta memory.\n");
          TerminateGame();
        }

        fp = fopen(SndFileName[i], "rb");

        /*if we can't open the file terminate*/
        if (!fp)
        {
          sprintf(error_string, "\nUnable to open:%s\n", SndFileName[i]);
          TerminateGame();
        }

        if (fread(sound[i], (word)st.st_size, 1, fp) != 1)
        {
          sprintf(error_string, "\nError reading file:%s\n", SndFileName[i]);
          TerminateGame();
        }

        if (fp)
        {
          if (fclose(fp))
          {
            printf("\nError closing file!!! \n");
            TerminateGame();
          }
        }
      }
      else
      {
        sprintf(error_string, "\nError getting file stats:%s\n", SndFileName[i]);
        TerminateGame();
      }
    }
  }
}
#endif


/**********************************************************************\
* sound_Update                                                         *
*                                                                      *
* Updates the stk to keep the sound from skipping.                     *
\**********************************************************************/
void sound_Update(void)
{
  if (initted)
  {
    dws_Update();
  }
}


/**********************************************************************\
* sound_PlaySound                                                      *
*                                                                      *
* sets up needed fields in dplay, and plays the sound.                 *
\**********************************************************************/
void sound_PlaySound(sound_NUM num)
{
  dws_MPLAY mplay;
  dws_DPLAY dplay;

  if (num >= sound_TOTAL)
  {
    sprintf(error_string, "Bad sound num");
    TerminateGame();
  }

  if (num == sound_SONG)
  {
    if (midi)
    {
      #ifdef WIN32
        mplay.track = MIDISONG;
      #else
        mplay.track = sound[num];
      #endif

      mplay.count = 0;

      dws_MPlay(&mplay);

      return;
    }
  }

  dplay.priority = LOW;
  dplay.presnd   = 0;
  dplay.count    = 1;
  dplay.snd      = sound[num];

  /*if using windows version, we need to set dplay.flags field*/
  #ifdef WIN32
    dplay.flags = dws_dplay_SND | dws_dplay_COUNT | dws_dplay_PRIORITY |
                  dws_dplay_PRESND;

    if (num == sound_SONG)
    {
      dplay.flags   |= dws_dplay_RVOL | dws_dplay_LVOL;
      dplay.priority = HIGH;
      dplay.count    = 0;
      dplay.rvol     = dws_IDENTITY / 2;         //volume cut in half
      dplay.lvol     = dws_IDENTITY / 2;         //volume cut in half
    }
  #endif

  dws_DPlay(&dplay);
}


/**********************************************************************\
* sound_ClearAll                                                       *
*                                                                      *
* Clears all sounds. (eg. if the demo is restarted).                   *
\**********************************************************************/
void sound_ClearAll(void)
{
  dws_DClear();
  dws_MClear();
}


/**********************************************************************\
* sound_kill                                                           *
*                                                               
* De-initializes the stk and free's sounds                             *
\**********************************************************************/
void sound_Kill(void)
{
  if (initted)
  {
    dws_DClear();                      //clear all sounds

    dws_Kill();

    initted = 0;

    FreeIt();                          //Free allocated memory

    loaded = 0;

    #ifdef WIN32
      diginited = 0;
    #endif
  }
}


/**********************************************************************\
* sound_init                                                           *
*                                                                      *
* Initializes the STK and other global variables                       *
\**********************************************************************/
void sound_Init(void)
{
  #ifndef WIN32
    dws_DETECTOVERRIDES dov;
  #endif
  dws_DETECTRESULTS dres;
  dws_IDEAL ideal;

  memset(sound, 0, sizeof(sound));

  if (!initted)
  {
    /*If we're using Windows95/NT*/
    #ifdef WIN32
      if (initted)
      {
        wsprintf(error_string, "Already Initted.");
        TerminateGame();
      }

      memset(hsound, 0, sizeof(hsound));

      dws_DetectHardWare(&dres);

      /* Ordered from "best" to "worst" choice for music devices */
      if (dres.muscaps & dws_muscap_MAPPER)
      {
        ideal.mustyp = dws_muscap_MAPPER;
      }
      else if (dres.muscaps & dws_muscap_FMSYNTH)
      {
        ideal.mustyp = dws_muscap_FMSYNTH;
      }
      else if (dres.muscaps & dws_muscap_SYNTH)
      {
        ideal.mustyp = dws_muscap_SYNTH;
      }
      else if (dres.muscaps & dws_muscap_SQSYNTH)
      {
        ideal.mustyp = dws_muscap_SQSYNTH;
      }
      else if (dres.muscaps & dws_muscap_MIDIPORT)
      {
        ideal.mustyp = dws_muscap_MIDIPORT;
      }
      else
      {
        ideal.mustyp = dws_muscap_NONE;
      }

      if (dres.digcaps & dws_digcap_22050_08_1)
      {
        ideal.digtyp = dws_digcap_22050_08_1;   //type of digital to play
      }

      ideal.dignvoices = 4;                     //number of voices (up to 16)

      /* If the STK isn't initted or initable we don't want to cause any errors.
         So we just blow the calls off. */
      if (dws_Init(&dres, &ideal))
      {
        initted = 1;

        if (ideal.mustyp != dws_muscap_NONE)
        {
          musinited = 1;
        }

        if (ideal.digtyp != dws_digcap_NONE)
        {
          diginited = 1;
        }
      }
    /*else we're using DOS*/
    #else
      dov.baseport = (word)-1;             //-1 signifies autodetect
      dov.digdma   = (word)-1;             //-1 signifies autodetect
      dov.digirq   = (word)-1;             //-1 signifies autodetect

      if (dws_DetectHardWare(&dov, &dres))
      {
        ideal.musictyp   = 1;              //0=No Music, 1=OPL2
        ideal.digtyp     = 8;              //0=No Dig, 8=8bit
        ideal.digrate    = 22000;          //sampling rate, in Hz
        ideal.dignvoices = 16;             //1 - 16 channels
        ideal.dignchan   = 1;              //1=mono

        if (dws_Init(&dres, &ideal))
        {
          initted = 1;
          midi    = 1;

          atexit(sound_Kill);
        }
        else
        {
          sprintf(error_string, "Error Initting the stk.");
          TerminateGame();
        }
      }
      else
      {
          /* No sound hardware detected, but keep going anyway. */
      }
    #endif

    LoadIt();
  }
}
