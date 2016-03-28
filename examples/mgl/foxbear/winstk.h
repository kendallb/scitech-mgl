/******************************************************************************
File:      dws.h
Version:   1.00
Tab stops: every 2 columns
Project:   DiamondWare's Sound ToolKit for Windows
Copyright: 1994-1996 DiamondWare, Ltd.  All rights reserved.
Written:   95/09/01 by John Lundy (based on dws.h for the dos STK)
Purpose:   Contains declarations for the DW Sound ToolKit for Windows
History:   95/09/01 JCL Started
           96/03/27 KW, EL & JCL finalized for 1.0
           96/04/14 JCL finalized 1.01
           96/05/11 JCL finalized 1.1
           96/05/27 JCL finalized 1.11
           96/09/21 KW finalized for v1.2 (support for IBM Visual Age C++)
******************************************************************************/



#ifndef dws_INCLUDE

  #define dws_INCLUDE



  /* For Powersoft Optima++ */
  #if defined (__WATCOMC__) && !defined (__WINDOWS__)
    typedef unsigned char  BYTE;
    typedef unsigned short WORD;
    typedef unsigned long  DWORD;
    typedef unsigned int   UINT;
  #endif

  #if defined(__NT__) && !defined(WIN32)
    #define WIN32
  #endif

  #if defined(_WIN32) && !defined(WIN32)
    #define WIN32
  #endif

  #if defined(WIN32) && !defined(__BORLANDC__)
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __export
  #endif

  #ifndef WIN32
    #if defined(__WATCOMC__) && defined(__WINDOWS_386__)
      #define dws_DIST
    #else
      #define dws_DIST _far
    #endif
  #else
    #define dws_DIST
  #endif

  /* For IBM Visual Age for C++ */
  #if defined (__IBMC__) || defined (__IBMCPP__)
    #undef CALLBACK
    #undef DLLEXPORT
    #define CALLBACK
    #define DLLEXPORT _System
  #endif



/*****************************************************************************/
  /*
   . If a call to a dws_ function returns 0 (FALSE), then this is an
   . indication that an error has occured.  Call dws_ErrNo to see which
   . one.  The following series of #defines is the complete list of
   . possible return values for dws_ErrNo.
  */
  #define dws_EZERO                        0  /* no error */

  /* The following 8 errors may be triggered by any dws_ function */
  #define dws_NOTINITTED                   1
  #define dws_ALREADYINITTED               2
  #define dws_NOTSUPPORTED                 3
  #define dws_INTERNALERROR                4
  #define dws_INVALIDPOINTER               5
  #define dws_RESOURCEINUSE                6
  #define dws_MEMORYALLOCFAILED            7
  #define dws_SETEVENTFAILED               8

  /*
   . dws_BUSY may be triggered during a call to a dws_ function, if any
   . dws_ function is already executing.  Ordinarily, this should never
   . occur.
  */
  #define dws_BUSY                         9

  /* This error may be triggered only by dws_Init */
  #define dws_Init_BUFTOOSMALL           101

  /* The following 3 errors may be triggered by any dws_D (dig) function */
  #define dws_D_NOTADWD                  201
  #define dws_D_NOTSUPPORTEDVER          202
  #define dws_D_BADDPLAY                 203

  /* The following error may be triggered by dws_DPlay */
  #define dws_DPlay_NOSPACEFORSOUND      251

  /* These 2 errors may be triggered by dws_WAV2DWD */
  #define dws_WAV2DWD_NOTAWAVE           301
  #define dws_WAV2DWD_UNSUPPORTEDFORMAT  302

  /* The following error may be triggered by any dws_M (music) function */
  #define dws_M_BADMPLAY                 401


/*---------------------------------------------------------------------------*/
  /*
   . This section #defines bitfields which are used by various dws_
   . functions.  Each bit in a bitfield, by definition, may be set/reset
   . independently of all other bits.
  */

  /* flags for muscaps value of dws_DETECTRESULTS structure */
  #define dws_muscap_NONE      0x0000 /* no music playback */
  #define dws_muscap_MIDIPORT  0x0001 /* output port */
  #define dws_muscap_SYNTH     0x0002 /* generic int synth */
  #define dws_muscap_SQSYNTH   0x0004 /* square wave int synth */
  #define dws_muscap_FMSYNTH   0x0008 /* FM int synth */
  #define dws_muscap_MAPPER    0x0010 /* MIDI mapper */
  #define dws_muscap_ANY       0x001f /* mask of all available modes */

  #define dws_digcap_NONE        0x0000 /* no digitized sound playback */
  #define dws_digcap_11025_08_1  0x0001 /* 11kHz, 8-bit, mono */
  #define dws_digcap_11025_08_2  0x0002 /* 11kHz, 8-bit, stereo */
  #define dws_digcap_11025_16_1  0x0004 /* 11kHz, 16-bit, mono */
  #define dws_digcap_11025_16_2  0x0008 /* 11kHz, 16-bit, stereo */
  #define dws_digcap_22050_08_1  0x0010 /* 22kHz, 8-bit, mono */
  #define dws_digcap_22050_08_2  0x0020 /* 22kHz, 8-bit, stereo */
  #define dws_digcap_22050_16_1  0x0040 /* 22kHz, 16-bit, mono */
  #define dws_digcap_22050_16_2  0x0080 /* 22kHz, 16-bit, stereo */
  #define dws_digcap_44100_08_1  0x0100 /* 44kHz, 8-bit, mono */
  #define dws_digcap_44100_08_2  0x0200 /* 44kHz, 8-bit, stereo */
  #define dws_digcap_44100_16_1  0x0400 /* 44kHz, 16-bit, mono */
  #define dws_digcap_44100_16_2  0x0800 /* 44kHz, 16-bit, stereo */
  #define dws_digcap_ANY         0x0fff /* mask of all available modes */

  /*
   . The following #defines are bitfields used in the flags field in the
   . dws_IDEAL struct.
   .
   . SWAPLR should be used to compensate for downstream electronics which
   . switch the left and right field.
   .
   . Since pitch and volume change both require some additional CPU time,
   . you may want to disable them for slow machines and enable for fast
   . machines.  This is an easy way for you to do this, without changing
   . your program around.
   .
   . Notes:
   .   1) Pitch change is slower than volume change.
   .   2) Pitch decrease (dws_DPLAY.pitch > dws_IDENTITY) is slower than
   .      pitch increase
   .   3) When the source sound, or the STK, is 8-bit, raising the volume
   .      is slower than lowering it.  Otherwise both are equal.
  */
  #define dws_ideal_NONE          0x0000  /* this is the normal case */
  #define dws_ideal_SWAPLR        0x0001  /* swap Left & Right on playback */
  #define dws_ideal_DISABLEPITCH  0x0002  /* disable the pitch feature */
  #define dws_ideal_DISABLEVOLUME 0x0004  /* disable the volume feature */
  #define dws_ideal_MAXSPEED      0x0006  /* use this for very slow machines */

  /*
   . These are the flag bitfields for the dws_DPLAY structure.
   . Each corresponds to one field in the struct.
  */
  #define dws_dplay_SND         0x00000001L
  #define dws_dplay_COUNT       0x00000002L
  #define dws_dplay_PRIORITY    0x00000004L
  #define dws_dplay_PRESND      0x00000008L
  #define dws_dplay_SOUNDNUM    0x00000010L
  #define dws_dplay_LVOL        0x00000020L
  #define dws_dplay_RVOL        0x00000040L
  #define dws_dplay_PITCH       0x00000080L
  #define dws_dplay_CALLBACK    0x00000100L
  #define dws_dplay_SYNCHRONOUS 0x00000200L
  #define dws_dplay_FIRSTSAMPLE 0x00000400L
  #define dws_dplay_CURSAMPLE   0x00000800L
  #define dws_dplay_LASTSAMPLE  0x00001000L

  /* The following 2 consts indicate the status of music playback */
  #define dws_MSONGSTATUSPLAYING  0x0001
  #define dws_MSONGSTATUSPAUSED   0x0002


/*---------------------------------------------------------------------------*/
  /*
   . When the WIN-STK sends a message to your window, the lParam field
   . indicates the purpose of the message.
  */
  #define dws_event_SOUNDCOMPLETE  0x0001
  #define dws_event_SOUNDSTARTED   0x0002
  #define dws_event_SOUNDABORTED   0x0003


  /* For no change in volume level or pitch */
  #define dws_IDENTITY 0x100

  /* Default priority, if not specified in call to dws_DPlay */
  #define dws_NORMALPRIORITY 0x1000



/*****************************************************************************/
  /* struct member byte alignment - begin */
  #ifdef _MSC_VER
    #ifdef WIN32
      #pragma pack(push, dws_h, 2) /* Use 2 byte alignment for these structs */
    #else
      #pragma pack(2)
    #endif
  #endif

  #if defined(__SC__) || defined(__WATCOMC__)
    #pragma pack(push, 2) /* Use 2 byte alignment for these structs */
  #endif



  /*
   . This section declares the struct types used by the STK.  In each
   . case, the user must create an instance of the struct prior to making
   . a call to any STK function which takes a pointer to it.  The STK does
   . not keep a pointer to any of these structs internally; after the call
   . returns, you may deallocate it, if you wish.
   .
   . NB: The STK _does_ keep pointers to digitized sound buffers!
  */

  /*
   . A pointer to this struct is passed to dws_DetectHardWare, which fills
   . it in.  It should then be passed to dws_Init.  If you plan on writing
   . this struct out to a file, it's important that you write the entire
   . contents.  There is information (for internal STK use only) in the
   . reserved[] field!
  */
  typedef struct
  {
    /* This field indicates which music capabilities are available, if any */
    DWORD muscaps;      /* see dws_muscap_xxxxxx #defines above */

    /* This field indicates which dig capabilities are available, if any */
    DWORD digcaps;      /* see dws_digcap_xxxxxx #defines above */

    /* This field should be modified only with caution */
    DWORD digbfr;       /* minimum buffer size */

    BYTE  reserved[20];

  } dws_DETECTRESULTS;


  /*
   . A pointer to this struct is passed as a parameter to dws_Init.  This
   . struct allows the program to let the STK know what capabilities it
   . will actually use.
  */
  typedef struct
  {
    DWORD flags;         /* use combination of dws_ideal_xxxx */
    DWORD mustyp;        /* use dws_muscap_xxxxxx (mode) to select */

    DWORD digtyp;        /* use dws_digcap_xxxxxx (mode) to select */
    WORD  dignvoices;    /* maximum number of digitized voices you'll use */

    BYTE  reserved[18];

  } dws_IDEAL;


  /*
   . A pointer to this struct is passed to dws_DPlay.
   .
   . NB: The soundnum field is filled in by dws_DPlay as a return value.
  */
  typedef struct
  {
    DWORD flags;         /* flag indicates active fields in struct         */
    BYTE  dws_DIST *snd; /* pointer to buffer which holds a .DWD file      */
    WORD  count;         /* number of times to play, or 0=infinite loop    */
    WORD  priority;      /* higher numbers mean higher priority            */
    WORD  presnd;        /* soundnum to sequence sound _after_             */
    WORD  soundnum;      /* dws_DPlay returns a snd number from 10-65535   */
    WORD  lvol;          /* 0-65535, 0 is off, 256 is dws_IDENTITY         */
    WORD  rvol;          /* if the output is mono lvol & rvol are averaged */
    WORD  pitch;         /* 1-65535, 256 is dws_IDENTITY (0 is unuseable)  */
    WORD  dummy;         /* added to insure DWORD alignment                */
    HWND  hwndmsg;       /* handle of window to which to send msg          */
    UINT  message;       /* message sent to window hwndmsg                 */
    DWORD firstsample;   /* first sample of sound buffer for play or loop  */
    DWORD cursample;     /* current sample playing within sound            */
    DWORD lastsample;    /* last sample of sound buffer for play or loop   */

    #ifdef WIN32
      BYTE reserved[4];
    #else
      BYTE reserved[8];
    #endif

  } dws_DPLAY;


  /* A pointer to this struct is passed to dws_MPlay. */
  typedef struct
  {
    BYTE dws_DIST *track;   /* ptr to NULL-terminated .MID file name string */
    WORD count;             /* number of times to play, or 0=infinite loop */

    BYTE reserved[10];

  } dws_MPLAY;



  /* struct member byte alignment - end */
  #ifdef _MSC_VER
    #ifdef  WIN32
      #pragma pack(pop, dws_h) /* back to default packing */
    #else
      #pragma pack()
    #endif
  #endif

  #if defined(__SC__) || defined(__WATCOMC__)
    #pragma pack(pop) /* back to default packing */
  #endif

/*****************************************************************************/
  /* This section prototypes the WIN-STK functions */

  #ifdef __cplusplus
    extern "C" {
  #endif



  /*
   . This function is callable at any time.  It returns the number of the
   . last error which occured.
  */
  WORD DLLEXPORT CALLBACK dws_ErrNo(void);

/*---------------------------------------------------------------------------*/


  /*
   . Each function in this section has a boolean return value.  A 0 (FALSE)
   . indicates that the function failed in some way.  In this case, call
   . dws_ErrNo to get the specific error.  Otherwise, a return value of 1
   . (TRUE) indicates that all is well.
  */
  WORD DLLEXPORT CALLBACK dws_DetectHardWare(dws_DETECTRESULTS dws_DIST *dr);

  WORD DLLEXPORT CALLBACK dws_Init(dws_DETECTRESULTS dws_DIST *dr,
                                   dws_IDEAL dws_DIST *ideal);


  /*
   . If the program has called dws_Init, it _MUST_ call dws_Kill before it
   . terminates.
  */
  WORD DLLEXPORT CALLBACK dws_Kill(void);


  /*
   . The following function is the digital mixer of the STK.  A
   . value of 0 is off; dws_IDENTITY is normal, and 0xffff is maximum
   . volume (grossly distorted).
  */
  WORD DLLEXPORT CALLBACK dws_XDig(WORD lvolume, WORD rvolume);


  /*
   . The following 9 functions comprise the digitized sound functions of
   . the STK.  See the documentation for complete details.
  */
  WORD DLLEXPORT CALLBACK dws_DPlay(dws_DPLAY dws_DIST *dplay);

  /*
   . Allows you to change the following items:
   .   count
   .   priority
   .   lvol
   .   rvol
   .   pitch
   .   hwndmsg and message (simultaneously)
  */
  WORD DLLEXPORT CALLBACK dws_DSetInfo(dws_DPLAY dws_DIST *dplay1,
                                       dws_DPLAY dws_DIST *dplay2);

  /*
   . Takes a ptr to a dws_DPLAY struct; soundnum specifies which sound.
   .
   . Can retrieve the current values for any or all of the following:
   .   snd
   .   count
   .   priority
   .   presnd
   .   lvol
   .   rvol
   .   pitch
   .   hwndmsg and message (simultaneously)
  */
  WORD DLLEXPORT CALLBACK dws_DGetInfo(dws_DPLAY dws_DIST *dplay1,
                                       dws_DPLAY dws_DIST *dplay2);

  /* Callable at any time--even before dws_Init or after dws_Kill */
  WORD DLLEXPORT CALLBACK dws_DGetRateFromDWD(BYTE dws_DIST *snd,
                                              WORD dws_DIST *rate);

  WORD DLLEXPORT CALLBACK dws_DDiscard(WORD soundnum);

  WORD DLLEXPORT CALLBACK dws_DDiscardAO(BYTE dws_DIST *snd);

  WORD DLLEXPORT CALLBACK dws_DClear(void);       /* global */

  WORD DLLEXPORT CALLBACK dws_DPause(void);       /* global */

  WORD DLLEXPORT CALLBACK dws_DUnPause(void);     /* global */

  /*
   . Converts a .WAV buffer to a .DWD buffer
   .
   . This function has two usages.  In the first, pass the wave pointer, the
   . wave length in *len and a NULL pointer for dwd.  The routine will return
   . the length of the buffer required to hold the resulting DWD in *len.  In
   . the second usage, allocate a buffer of the correct size and pass its
   . address in dwd.  Make sure you pass the wave pointer in wave and the size
   . of the wave in *len.  It will then perform the conversion, returning the
   . number of bytes used in *len.
  */
  WORD DLLEXPORT CALLBACK dws_WAV2DWD(BYTE dws_DIST *wave, DWORD dws_DIST *len,
                                      BYTE dws_DIST *dwd);

  /* The following 5 functions comprise the music functions of the STK. */
  WORD DLLEXPORT CALLBACK dws_MPlay(dws_MPLAY dws_DIST *mplay);

  WORD DLLEXPORT CALLBACK dws_MSongStatus(WORD dws_DIST *result);

  WORD DLLEXPORT CALLBACK dws_MClear(void);

  WORD DLLEXPORT CALLBACK dws_MPause(void);

  WORD DLLEXPORT CALLBACK dws_MUnPause(void);

  /*
   . If your 16-bit application can't yield CPU time to the system, it must
   . call this function periodically to prevent the sound from pausing.
   . This is not required for 32-bit applications, but the function is
   . supported for compatibility.
  */
  WORD DLLEXPORT CALLBACK dws_Update(void);        /* Affects all sounds */



  #ifdef __cplusplus
    }
  #endif

/*****************************************************************************/



#endif
