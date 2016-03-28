/*****************************************************************************
File:      dwdsp.h
Version:   2.21
Tab stops: every 2 columns
Project:   DiamondWare's DSP add-on for its STK product
Copyright: 1995 DiamondWare, Ltd.  All rights reserved.
Written:   by Keith Weiner & Erik Lorenzen
Purpose:   Contains declarations for DSP functions
History:   see dwdsp.c
*****************************************************************************/



#ifndef dwdsp_INCLUDE

  #define dwdsp_INCLUDE



//  #include "dws.h"



  #define dwdsp_IDENTITY  0x100  /* for no change in volume level */


  /*
   . This is the complete list of possible return values for dwdsp_ErrNo.
   . An error may be flagged by any dwdsp_ function.  Call dwdsp_ErrNo
   . whenever the return value of a dwdsp_ function is 0 (error).
  */
  #define dwdsp_EZERO   0   /* no error */

  /* The following 2 errors may be triggered by any dwdsp_ function */
  #define dwdsp_NOTADWD 1   /* you passed a ptr that doesn't belong to a DWD */
  #define dwdsp_NULLPTR 2   /* you passed a NULL ptr */

  /* The following 2 errors may be triggered by dwdsp_ChngLen */
  #define dwdsp_BADLEN  3   /* the length is too short to be useable */
  #define dwdsp_SAMEPTR 4   /* this function does not support operation on */
                            /* the source buffer */


  #ifdef __cplusplus
    extern "C" {
  #endif

  /* dwdsp_ErrNo() returns the number of the last error which occured. */
  word dwdsp_ErrNo(void);


  /*
   . This function will copy the source DWD to the destination buffer, and
   . change the volume.  You may pass it srcdwd == desdwd), to change the
   . the volume in-place.
   .
   . volume is specified as a factor of dwdsp_IDENTITY; 0x100 will keep
   . the level unchanged, 0x80 will be half as loud, and 0x200 will be
   . twice as loud.  Although each sample is clipped to the limited
   . dynamic range, excessive clipping causes noise.  Experiment with
   . this function to find acceptable limits for volume; it will depend
   . heavily on the source sound effect.
  */
  word dwdsp_ChngVol(byte *desdwd, byte *srcdwd, word volume);


  /*
   . This function changes the length of a DWD, and thereby its pitch.
   .
   . newlen specifies the length of the destination DWD (your buffer
   . must be at least newlen big!)
   .
   . NB: Unlike with volume, length cannot be changed in place!
  */
  word dwdsp_ChngLen(byte *desdwd, byte *srcdwd, dword newlen);



  #ifdef __cplusplus
    }
  #endif



#endif
