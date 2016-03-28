/******************************************************************************
File:      dws.h
Version:   2.21
Tab stops: every 2 columns
Project:   DiamondWare's Sound ToolKit
Copyright: 1994-1995 DiamondWare, Ltd.  All rights reserved.
Written:   by Keith Weiner and Erik Lorenzen
Purpose:   Contains declarations for the DW Sound ToolKit
History:   94/08/24 KW Started
           95/02/17 EL Finalized for 1.0
           95/03/19 EL Finalized for 1.01, Added new define dws_BUSY
           95/04/06 EL Finalized for 1.02, Added new define dws_IRQDISABLED
           95/06/16 EL Finalized for 1.03
           95/07/23 EL Finalized for 1.04, no changes
           95/08/01 EL Finalized for 2.00, added new defines for pmode errors
                                           & calling conventions
           95/08/29 EL Finalized for 2.10, no changes
           95/10/18 EL Finalized for 2.20, no changes
           95/12/09 EL Finalized for 2.21, no changes

******************************************************************************/



#ifndef dws_INCLUDE

  #define dws_INCLUDE



  #ifdef __FLAT__
    #define dws_CALLCON _cdecl
    #define dws_DIST
  #else
    #define dws_CALLCON _far _pascal
    #define dws_DIST _far
  #endif



/*****************************************************************************/
  /*
   . The following is intterrupt range that STK can become
   . resident in.
  */
  #define dws_FIRSTINT 0x60
  #define dws_LASTINT  0x66
/*****************************************************************************/
  /*
   . The following is the complete list of possible values for dws_ErrNo.
   . dws_ErrNo may be set by any dws_ function.  Check its value whenever
   . the return value of a dws_ function is 0 (error).
  */
  #define dws_EZERO                            0   /* no error */

  /* The following 3 errors may be triggered by any dws_ function */
  #define dws_NOTINITTED                       1
  #define dws_ALREADYINITTED                   2
  #define dws_NOTSUPPORTED                     3

  /* The following 4 errors may be triggered by dws_DetectHardWare */
  #define dws_DetectHardware_UNSTABLESYSTEM    4
  #define dws_DetectHardware_BADBASEPORT       5
  #define dws_DetectHardware_BADDMA            6
  #define dws_DetectHardware_BADIRQ            7

  /* The following error may be triggered by dws_Kill */
  #define dws_Kill_CANTUNHOOKISR               8

  /* The following error may be triggered by any dws_X (mixer) function */
  #define dws_X_BADINPUT                       9

  /* The following 3 errors may be triggered by any dws_D (dig) function */
  #define dws_D_NOTADWD                        10
  #define dws_D_NOTSUPPORTEDVER                11
  #define dws_D_INTERNALERROR                  12

  /* The following error may be triggered by dws_DPlay */
  #define dws_DPlay_NOSPACEFORSOUND            13

  /* The following 2 errors may be triggered by dws_DSetRate */
  #define dws_DSetRate_FREQTOLOW               14
  #define dws_DSetRate_FREQTOHIGH              15

  /* The following 3 errors may be triggered by dws_MPlay */
  #define dws_MPlay_NOTADWM                    16
  #define dws_MPlay_NOTSUPPORTEDVER            17
  #define dws_MPlay_INTERNALERROR              18

  /*
   . The following error may be triggered by any dws_ function
   . (except dws_ErrNo and dws_Init) if called from an Interrupt
   . Service Routine (ISR).  If you're not sure whether this applies
   . to you: it probably doesn't.
  */
  #define dws_BUSY                             19

  /*
   . The following error may be triggered by dws_Init, dws_Kill,
   . and dws_DetectHardware.  It will only occur if interrupts are
   . disabled.  Interrupts must be enabled when calling.  If you're
   . not sure whether this applies to you: it probably doesn't.
  */
  #define dws_IRQDISABLED                      20

  /*
   . The following errors will only come up when using the protected mode
   . extension to the STK
  */
  #define dws_NOTRESIDENT                      100
  #define dws_NOMEM                            101



/*---------------------------------------------------------------------------*/


  /*
   . The follwing section defines bitfields which are used by various
   . dws_ functions.  Each bit in a bitfield, by definition, may be
   . set/reset independantly of all other bits.
  */

  /* The following 2 consts indicate the capabilities of the user's hardware */
  #define dws_capability_FM                    0x0001
  #define dws_capability_DIG                   0x0002

  /* The following 2 consts indicate the status of specified digital sounds */
  #define dws_DSOUNDSTATUSPLAYING              0x0001
  #define dws_DSOUNDSTATUSSEQUENCED            0x0002

  /* The following 2 consts indicate the status of music playback */
  #define dws_MSONGSTATUSPLAYING               0x0001
  #define dws_MSONGSTATUSPAUSED                0x0002
/*****************************************************************************/



  /*
   . The following 3 types are used by the STK
   .
   . If you are already typedefing these names, simply put the line
   .   #define stddef_INCLUDE
   . before the line
   .   #include <dws.h>
   . in every source file which references the STK.
  */
  #ifndef stddef_INCLUDE
    typedef unsigned char  byte;
    typedef unsigned short word;
    typedef unsigned long  dword;
  #endif



/*****************************************************************************/



  /*
   . The following section typedefs the structs used by the STK.  In each
   . case, the user must create an instance of the struct prior to making
   . a call to an STK function which takes a pointer to it.  The STK does
   . not keep a pointer to any of these structs internally; after the call
   . returns, you may deallocate it, if you wish.
   .
   . NB: The STK _does_ keep pointers to songs and digitized sound buffers!
  */


  /*
   . dws_DetectHardWare can be told _not_ to autodetect particular values
   . about the installed hardware.  This is useful if detecting DMA channel,
   . for example, consistently causes a machine lockup.  To override the
   . autodetect for a setting, set the corresponding field in this struct
   . to the correct value.  Otherwise, set the field to ffff hex.  Since
   . the autodetect is reliable, this is the recommended course of action,
   . except in cases of known problems.
  */
  typedef struct
  {
    word baseport;        /* base address of sound card (often 220 hex) */
    word digdma;          /* DMA channel */
    word digirq;          /* IRQ level */

    byte reserved[10];

  } dws_DETECTOVERRIDES;


  /*
   . A pointer to this struct is passed to dws_DetectHardWare, which fills
   . it in.  It is then passed unmodified to dws_Init.  If you plan on
   . writing this struct out to a file, it's important that you write
   . the entire contents.  There is information (for internal STK use only)
   . in the reserved[] field!
  */
  typedef struct
  {
    word baseport;        /* base address of sound card (often 220 hex) */
    word capability;      /* see #defines, above */

    /* The following 3 fields are only valid if FM music is supported */
    word mustyp;          /* 0=none, 1=OPL2 */
    word musnchan;        /* 1=mono */
    word musnvoice;       /* number of voices supported by hardware(11 for FM) */

    /* The following 2 fields are only valid if digitized sound is supported */
    word dignbits;        /* 0=none, 8=8 bit */
    word dignchan;        /* 1=mono */

    word digdma;          /* DMA channel */
    word digirq;          /* IRQ level */

    word mixtyp;          /* 1=software, 2+ is hardware */

    byte reserved[44];    /* there are important values in here... */

  } dws_DETECTRESULTS;


  /*
   . A pointer to this struct is passed as a parameter to dws_Init.  This
   . allows the user to tell the STK to use less than the full capabilities
   . of the installed sound hardware, and/or the user's sound board
   . may not support every feature of the STK.
  */
  typedef struct
  {
    word musictyp;        /* 0=No Music, 1=OPL2 */

    word digtyp;          /* 0=No Dig, 8=8bit */
    word digrate;         /* sampling rate, in Hz */
    word dignvoices;      /* number of voices (up to 16) */
    word dignchan;        /* 1=mono */

    byte reserved[6];

  } dws_IDEAL;


  /*
   . A pointer to this struct is passed to dws_DPlay.
   . Note that the soundnum field is filled in by dws_DPlay as a return value.
  */
  typedef struct
  {
    byte dws_DIST *snd;   /* pointer to buffer which holds a .DWD file */
    word count;           /* number of times to play, or 0=infinite loop */
    word priority;        /* higher numbers mean higher priority */
    word presnd;          /* soundnum to sequence sound _after_ */
    word soundnum;        /* dws_DPlay returns a snd number from 10-65535 */

    byte reserved[20];

  } dws_DPLAY;


  /* A pointer to this struct is passed to dws_MPlay. */
  typedef struct
  {
    byte dws_DIST *track; /* pointer to buffer which holds a .DWM file */
    word count;           /* number of times to play, or 0=infinite loop */

    byte reserved[10];

  } dws_MPLAY;


/*****************************************************************************/



  #ifdef __cplusplus
    extern "C" {
  #endif



/*****************************************************************************/


  /*
   . This function is callable at any time.  It returns the number of the
   . last error which occured.
  */
  word  dws_CALLCON dws_ErrNo(void);
/*---------------------------------------------------------------------------*/


  /*
   . This function is called at the end of the timer ISR (interrupt service
   . routine).  If you're using the optional DWT (DW Timer), this happens
   . automagically.  If you wrote your own timer handler routine, you must
   . call this function regularly.
  */
  void  dws_CALLCON dws_Update(void);
/*---------------------------------------------------------------------------*/


  /*
   . Each function in this section has a boolean return value.  A 0 (false)
   . indicates that the function failed in some way.  In this case, call
   . dws_ErrNo to get the specific error.  Otherwise, a return value of 1
   . (true) indicates that all is well.
  */
  word  dws_CALLCON dws_DetectHardWare(dws_DETECTOVERRIDES dws_DIST *dov,
                                       dws_DETECTRESULTS dws_DIST *dr);

  word  dws_CALLCON dws_Init(dws_DETECTRESULTS dws_DIST *dr,
                             dws_IDEAL dws_DIST *ideal);

  /*
   . If the program has called dws_Init, it _MUST_ call dws_Kill before it
   . terminates.
   .
   . NB: Trap critical errors.  Don't let DOS put up the
   .     "Abort, Retry, Fail?" text.  ('sides, it'll destroy your pretty gfx)
  */
  word  dws_CALLCON dws_Kill(void);


  /*
   . The following 3 functions comprise the mixer section of the STK.  A
   . value of 0 turns a channel off; a value of 255 is the loudest.
  */
  word  dws_CALLCON dws_XMaster(word volume);

  word  dws_CALLCON dws_XMusic(word volume);

  word  dws_CALLCON dws_XDig(word volume);


  /*
   . The following 10 functions comprise the digitized sound functions of
   . the STK.  See the documentation for complete details.
  */
  word  dws_CALLCON dws_DPlay(dws_DPLAY dws_DIST *dplay);

  word  dws_CALLCON dws_DSoundStatus(word soundnumber,
                                     word dws_DIST *result);

  word  dws_CALLCON dws_DSetRate(word frequency);

  word  dws_CALLCON dws_DGetRate(word dws_DIST *result);

  /* This function is callable at any time*/
  word  dws_CALLCON dws_DGetRateFromDWD(byte dws_DIST *snd,
                                        word dws_DIST *result);

  word  dws_CALLCON dws_DDiscard(word soundnum);

  word  dws_CALLCON dws_DDiscardAO(byte dws_DIST *snd);

  word  dws_CALLCON dws_DClear(void);            /* Affects all sounds */

  word  dws_CALLCON dws_DPause(void);            /* Affects all sounds */

  word  dws_CALLCON dws_DUnPause(void);          /* Affects all sounds */


  /*
   . The following 5 functions comprise the music functions of the STK.
   . See the documentation for complete details.
  */
  word  dws_CALLCON dws_MPlay(dws_MPLAY dws_DIST *mplay);

  word  dws_CALLCON dws_MSongStatus(word dws_DIST *result);

  word  dws_CALLCON dws_MClear(void);            /* Kill playing song */

  word  dws_CALLCON dws_MPause(void);            /* Pauses music playback */

  word  dws_CALLCON dws_MUnPause(void);          /* UnPauses music playback */



  #ifndef __FLAT__
    /*
     . The following function is for creating a real-mode wrapper (for use
     . by protected-mode).  It's installed as an interrupt handler on a
     . "user" interrupt vector (0x60-0x66).  See stkrun.c.
    */
    void  dws_CALLCON dws_WRAP(void);
  #endif
/*****************************************************************************/



  #ifdef __cplusplus
    }
  #endif



#endif
