/******************************************************************************
File:      Sound.h
Project:   Fox and the bear
Copyright: 1996 DiamondWare, Ltd.  All rights reserved.
Written:   Angelo Nunes and Erik Lorenzen
Purpose:   Contains the high-level sound interface, callable by Fox & the bear
History:   03/25/96 AN started
******************************************************************************/



#ifndef SOUND_H_INCLUDE

  #define SOUND_H_INCLUDE



  typedef enum
  {
    sound_STRIKE,                          //Enumeration of avail. sounds
    sound_STUNNED,
    sound_MISS,
    sound_THROW,
    sound_JUMP,
    sound_STOP,
    sound_SONG,
    sound_TOTAL

  } sound_NUM;



  #ifdef __cplusplus
    extern "C" {
  #endif



  void sound_Init(void);                   //Inititializes sounds

  void sound_Kill(void);                   //Ends game

  void sound_ClearAll(void);               //Clears all sounds

  void sound_PlaySound(sound_NUM num);     //Plays a sound

  void sound_Update(void);                 //Update the stk



  #ifdef __cplusplus
    }
  #endif

#endif
