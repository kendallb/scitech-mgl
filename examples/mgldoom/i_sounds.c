// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//      System interface for sound.
//      Adapted for the SNAP Audio interface
//
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

//#define  NOVER  // do not use the 'VERSION' enum (clash with that in os2me.h)

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

#include <snap/audio.h>

/*
// Update all 30 millisecs, approx. 30fps synchronized.
// Linux resolution is allegedly 10 millisecs,
//  scale is microseconds.
#define SOUND_INTERVAL     500

// Get the interrupt. Set duration in millisecs.
int I_SoundSetTimer( int duration_of_tick );
void I_SoundDelTimer( void );
*/

// A quick hack to establish a protocol between
// synchronous mix buffer updates and asynchronous
// audio writes. Probably redundant with gametic.
static int flag = 0;

// The number of internal mixing channels,
//  the samples calculated for each mixing step,
//  the size of the 16bit, 2 hardware channel (stereo)
//  mixing buffer, and the samplerate of the raw data.


// Needed for calling the actual sound output.
#define SAMPLECOUNT             512
#define NUM_CHANNELS            8
// It is 2 for 16bit, and 2 for two channels.
#define BUFMUL                  4
#define MIXBUFFERSIZE           (SAMPLECOUNT*BUFMUL)

#define SAMPLERATE              11025   // Hz
#define SAMPLESIZE              2       // 16bit

// The actual lengths of all sound effects.
int             lengths[NUMSFX];

// The channel step amount...
unsigned int    channelstep[NUM_CHANNELS];
// ... and a 0.16 bit remainder of last step.
unsigned int    channelstepremainder[NUM_CHANNELS];


// The channel data pointers, start and end.
unsigned char*  channels[NUM_CHANNELS];
unsigned char*  channelsend[NUM_CHANNELS];


// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
int             channelstart[NUM_CHANNELS];

// The sound in channel handles,
//  determined on registration,
//  might be used to unregister/stop/modify,
//  currently unused.
int             channelhandles[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
int             channelids[NUM_CHANNELS];

// Pitch to stepping lookup, unused.
int             steptable[256];

// Volume lookups.
int             vol_lookup[128*256];

// Hardware left and right channel volume lookup.
int*            channelleftvol_lookup[NUM_CHANNELS];
int*            channelrightvol_lookup[NUM_CHANNELS];

// SNAP Audio stuff
static AA_devCtx            *audio_dc;
static AA_initFuncs         init;
static AA_playbackFuncs     play;
static int                  sound_44k;  // 44.1kHz flag
//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
void*
getsfx
( char*         sfxname,
  int*          len )
{
    unsigned char*      sfx;
    unsigned char*      paddedsfx;
    int                 i;
    int                 size;
    int                 paddedsize;
    char                name[20];
    int                 sfxlump;


    // Get the sound data from the WAD, allocate lump
    //  in zone memory.
    sprintf(name, "ds%s", sfxname);

    // Now, there is a severe problem with the
    //  sound handling, in it is not (yet/anymore)
    //  gamemode aware. That means, sounds from
    //  DOOM II will be requested even with DOOM
    //  shareware.
    // The sound list is wired into sounds.c,
    //  which sets the external variable.
    // I do not do runtime patches to that
    //  variable. Instead, we will use a
    //  default sound for replacement.
    if ( W_CheckNumForName(name) == -1 )
      sfxlump = W_GetNumForName("dspistol");
    else
      sfxlump = W_GetNumForName(name);

    size = W_LumpLength( sfxlump );

    // Debug.
    // fprintf( stderr, "." );
    //fprintf( stderr, " -loading  %s (lump %d, %d bytes)\n",
    //       sfxname, sfxlump, size );
    //fflush( stderr );

    sfx = (unsigned char*)W_CacheLumpNum( sfxlump, PU_STATIC );

    // Pads the sound effect out to the mixing buffer size.
    // The original realloc would interfere with zone memory.
    paddedsize = ((size-8 + (SAMPLECOUNT-1)) / SAMPLECOUNT) * SAMPLECOUNT;

    // Allocate from zone memory.
    paddedsfx = (unsigned char*)Z_Malloc( paddedsize+8, PU_STATIC, 0 );
    // ddt: (unsigned char *) realloc(sfx, paddedsize+8);
    // This should interfere with zone memory handling,
    //  which does not kick in in the soundserver.

    // Now copy and pad.
    memcpy(  paddedsfx, sfx, size );
    for (i=size ; i<paddedsize+8 ; i++)
        paddedsfx[i] = 128;

    // Remove the cached lump.
    Z_Free( sfx );

    // Preserve padded length.
    *len = paddedsize;

    // Return allocated padded data.
    return (void *) (paddedsfx + 8);
}





//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//
int
addsfx
( int           sfxid,
  int           volume,
  int           step,
  int           seperation )
{
    static unsigned short       handlenums = 0;

    int         i;
    int         rc = -1;

    int         oldest = gametic;
    int         oldestnum = 0;
    int         slot;

    int         rightvol;
    int         leftvol;

    // Chainsaw troubles.
    // Play these sound effects only one at a time.
    if ( sfxid == sfx_sawup
         || sfxid == sfx_sawidl
         || sfxid == sfx_sawful
         || sfxid == sfx_sawhit
         || sfxid == sfx_stnmov
         || sfxid == sfx_pistol  )
    {
        // Loop all channels, check.
        for (i=0 ; i<NUM_CHANNELS ; i++)
        {
            // Active, and using the same SFX?
            if ( (channels[i])
                 && (channelids[i] == sfxid) )
            {
                // Reset.
                channels[i] = 0;
                // We are sure that iff,
                //  there will only be one.
                break;
            }
        }
    }

    // Loop all channels to find oldest SFX.
    for (i=0; (i<NUM_CHANNELS) && (channels[i]); i++)
    {
        if (channelstart[i] < oldest)
        {
            oldestnum = i;
            oldest = channelstart[i];
        }
    }

    // Tales from the cryptic.
    // If we found a channel, fine.
    // If not, we simply overwrite the first one, 0.
    // Probably only happens at startup.
    if (i == NUM_CHANNELS)
        slot = oldestnum;
    else
        slot = i;

    // Okay, in the less recent channel,
    //  we will handle the new SFX.
    // Set pointer to raw data.
    channels[slot] = (unsigned char *) S_sfx[sfxid].data;
    // Set pointer to end of raw data.
    channelsend[slot] = channels[slot] + lengths[sfxid];

    // Reset current handle number, limited to 0..100.
    if (handlenums == 100)
        handlenums = 0;

    // Assign current handle number.
    // Preserved so sounds could be stopped (unused).
    channelhandles[slot] = rc = handlenums++;

    // Set stepping???
    // Kinda getting the impression this is never used.
    // It _is_ used, really. All the time. -mikeN
    channelstep[slot] = step;
    // ???
    channelstepremainder[slot] = 0;
    // Should be gametic, I presume.
    channelstart[slot] = gametic;

    // Separation, that is, orientation/stereo.
    //  range is: 1 - 256
    seperation += 1;

    // Per left/right channel.
    //  x^2 seperation,
    //  adjust volume properly.
    leftvol =
        volume - ((volume*seperation*seperation) >> 16); ///(256*256);
    seperation = seperation - 257;
    rightvol =
        volume - ((volume*seperation*seperation) >> 16);

    // Sanity check, clamp volume.
    if (rightvol < 0 || rightvol > 127)
        I_Error("rightvol out of bounds");

    if (leftvol < 0 || leftvol > 127)
        I_Error("leftvol out of bounds");

    // Get the proper lookup table piece
    //  for this volume level???
    channelleftvol_lookup[slot] = &vol_lookup[leftvol*256];
    channelrightvol_lookup[slot] = &vol_lookup[rightvol*256];

    // Preserve sound SFX id,
    //  e.g. for avoiding duplicates of chainsaw.
    channelids[slot] = sfxid;

    // You tell me.
    return rc;
}


//
// SFX API
// Note: this was called by S_Init.
// However, whatever they did in the
// old DPMS based DOS version, this
// were simply dummies in the Linux
// version.
// See soundserver initdata().
//
void I_SetChannels()
{
  // Init internal lookups (raw data, mixing buffer, channels).
  // This function sets up internal lookups used during
  //  the mixing process.
  int           i;
  int           j;

  int*  steptablemid = steptable + 128;

  // Okay, reset internal mixing channels to zero.
  /*for (i=0; i<NUM_CHANNELS; i++)
  {
    channels[i] = 0;
  }*/

  // This table provides step widths for pitch parameters.
  // I fail to see that this is currently used.
  for (i=-128 ; i<128 ; i++)
    steptablemid[i] = (int)(pow(2.0, (i/64.0))*65536.0);


  // Generates volume lookup tables
  //  which also turn the unsigned samples
  //  into signed samples.
  for (i=0 ; i<128 ; i++)
    for (j=0 ; j<256 ; j++)
      vol_lookup[i*256+j] = (i*(j-128)*256)/127;
}


void I_SetSfxVolume(int volume)
{
  // Identical to DOS.
  // Basically, this should propagate
  //  the menu/config file setting
  //  to the state variable used in
  //  the mixing.
  snd_SfxVolume = volume;
}

// MUSIC API - dummy. Some code from DOS version.
void I_SetMusicVolume(int volume)
{
  // Internal state variable.
  snd_MusicVolume = volume;
  // Now set volume on output device.
  // Whatever( snd_MusciVolume );
}


//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is not ignored!
//
int I_StartSound
( int           id,
  int           vol,
  int           sep,
  int           pitch,
  int           priority )
{

  // UNUSED
  priority = 0;

  // Debug.
  //fprintf( stderr, "starting sound %d", id );

  // Returns a handle (not used).
  id = addsfx( id, vol, steptable[pitch], sep );

  //fprintf( stderr, "/handle is %d\n", id );

  return id;
}



void I_StopSound(int handle)
{
  // You need the handle returned by StartSound.
  // Would be looping all channels,
  //  tracking down the handle,
  //  an setting the channel to zero.

  int     chan = 0;

  while ((channelhandles[chan] != handle) && chan < NUM_CHANNELS)
     chan++;

  if (chan < NUM_CHANNELS)
     channels[chan] = NULL;

}


int I_SoundIsPlaying(int handle)
{
    int    chan = 0;

    while(channelhandles[chan] != handle  &&  chan < NUM_CHANNELS)
       chan++;

    return chan < NUM_CHANNELS ? 1 : 0;
}


//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the global
//  mixbuffer, clamping it to the allowed range,
//  and sets up everything for transferring the
//  contents of the mixbuffer to the (two)
//  hardware channels (left and right, that is).
//
// This function currently supports only 16bit.
//
void doom_mixBuffer( void *buffer )
{
   // Mix current sound data.
   // Data, from raw sound, for right and left.
   register unsigned int sample;
   register int          dl;
   register int          dr;

   // Pointers in global mixbuffer, left, right, end.
   signed short*         leftout;
   signed short*         rightout;
   signed short*         leftend;
   // Step in mixbuffer, left and right, thus two.
   int                           step;

   // Mixing channel index.
   int                           chan;

//   fprintf(stderr, "mixing...\n");
   // Left and right channel
   // are in global mixbuffer, alternating.
   leftout = buffer;
   rightout = leftout + 1;
   step = 2;

   // Determine end, for left channel only
   //  (right channel is implicit).
   leftend = leftout + SAMPLECOUNT*step;

   // Mix sounds into the mixing buffer.
   // Loop over step*SAMPLECOUNT,
   //  that is 512 values for two channels.
   while (leftout != leftend)
   {
       // Reset left/right value.
       dl = 0;
       dr = 0;

       // Love thy L2 cache - made this a loop.
       // Now more channels could be set at compile time
       //  as well. Thus loop those  channels.
       for ( chan = 0; chan < NUM_CHANNELS; chan++ )
       {
           // Check channel, if active.
           if (channels[ chan ])
           {
               // Get the raw data from the channel.
               sample = *channels[ chan ];
               // Add left and right part
               //  for this channel (sound)
               //  to the current data.
               // Adjust volume accordingly.
               dl += channelleftvol_lookup[ chan ][sample];
               dr += channelrightvol_lookup[ chan ][sample];
               // Increment index ???
               channelstepremainder[ chan ] += channelstep[ chan ];
               // MSB is next sample???
               channels[ chan ] += channelstepremainder[ chan ] >> 16;
               // Limit to LSB???
               channelstepremainder[ chan ] &= 65536-1;

               // Check whether we are done.
               if (channels[ chan ] >= channelsend[ chan ])
                   channels[ chan ] = 0;
           }
       }

       // Clamp to range. Left hardware channel.
       // Has been char instead of short.
       // if (dl > 127) *leftout = 127;
       // else if (dl < -128) *leftout = -128;
       // else *leftout = dl;

       if (dl > 0x7fff)
           *leftout = 0x7fff;
       else if (dl < -0x8000)
           *leftout = -0x8000;
       else
           *leftout = dl;

       // Same for right hardware channel.
       if (dr > 0x7fff)
           *rightout = 0x7fff;
       else if (dr < -0x8000)
           *rightout = -0x8000;
       else
           *rightout = dr;

       // Increment current pointers in mixbuffer.
       leftout += step;
       rightout += step;
   }
}


//
// A clone of the above that mixes a 44.1kHz output stream.
// For use on sound chips that can't support 11.025kHz audio.
//
void doom_mixBuffer_44k( void *buffer )
{
   // Mix current sound data.
   // Data, from raw sound, for right and left.
   register unsigned int sample;
   register int          dl;
   register int          dr;

   // Pointers in global mixbuffer, left, right, end.
   signed short*         leftout;
   signed short*         rightout;
   signed short*         leftend;
   signed short          lsample;
   signed short          rsample;
   // Step in mixbuffer, left and right, thus two.
   int                           step;

   // Mixing channel index.
   int                           chan;

//   fprintf(stderr, "mixing...\n");
   // Left and right channel
   // are in global mixbuffer, alternating.
   leftout = buffer;
   rightout = leftout + 1;
   step = 8;

   // Determine end, for left channel only
   //  (right channel is implicit).
   leftend = leftout + SAMPLECOUNT*step;

   // Mix sounds into the mixing buffer.
   // Loop over step*SAMPLECOUNT,
   //  that is 512 values for two channels.
   while (leftout != leftend)
   {
       // Reset left/right value.
       dl = 0;
       dr = 0;

       // Love thy L2 cache - made this a loop.
       // Now more channels could be set at compile time
       //  as well. Thus loop those  channels.
       for ( chan = 0; chan < NUM_CHANNELS; chan++ )
       {
           // Check channel, if active.
           if (channels[ chan ])
           {
               // Get the raw data from the channel.
               sample = *channels[ chan ];
               // Add left and right part
               //  for this channel (sound)
               //  to the current data.
               // Adjust volume accordingly.
               dl += channelleftvol_lookup[ chan ][sample];
               dr += channelrightvol_lookup[ chan ][sample];
               // Increment index ???
               channelstepremainder[ chan ] += channelstep[ chan ];
               // MSB is next sample???
               channels[ chan ] += channelstepremainder[ chan ] >> 16;
               // Limit to LSB???
               channelstepremainder[ chan ] &= 65536-1;

               // Check whether we are done.
               if (channels[ chan ] >= channelsend[ chan ])
                   channels[ chan ] = 0;
           }
       }

       // Clamp to range. Left hardware channel.
       // Has been char instead of short.
       // if (dl > 127) *leftout = 127;
       // else if (dl < -128) *leftout = -128;
       // else *leftout = dl;

       if (dl > 0x7fff)
           lsample = 0x7fff;
       else if (dl < -0x8000)
           lsample = -0x8000;
       else
           lsample = dl;

       // Same for right hardware channel.
       if (dr > 0x7fff)
           rsample = 0x7fff;
       else if (dr < -0x8000)
           rsample = -0x8000;
       else
           rsample = dr;

       *leftout = lsample;
       *rightout = rsample;
       *(leftout + 2)= lsample;
       *(rightout + 2) = rsample;
       *(leftout + 4)= lsample;
       *(rightout + 4) = rsample;
       *(leftout + 6)= lsample;
       *(rightout + 6) = rsample;

       // Increment current pointers in mixbuffer.
       leftout += step;
       rightout += step;
   }
}



// SNAP Audio callback routine. Fills up audio buffer with brand
// spanking new fresh smelling audio data.
N_uint32 NAPI doom_sndCallback(void *buffer, N_uint32 size, void *reserved)
{
    if (sound_44k)
        doom_mixBuffer_44k(buffer);
    else
        doom_mixBuffer(buffer);
    return size;
}



//
// This would be used if we were doing synchronous sound
// mixing, but we're not. So it's a dummy.
//
void I_UpdateSound( void )
{
}



//
// This would be used to write out the mixbuffer
//  during each game loop update.
// Updates sound buffer and audio device at runtime.
// It is called during Timer interrupt with SNDINTR.
// Mixing now done synchronous, and
//  only output be done asynchronous?
//
void I_SubmitSound(void)
{
  // Write it to DSP device.
  // write(audio_fd, mixbuffer, SAMPLECOUNT*BUFMUL);
}



void
I_UpdateSoundParams
( int   handle,
  int   vol,
  int   sep,
  int   pitch)
{
  // I fail too see that this is used.
  // Would be using the handle to identify
  //  on which channel the sound might be active,
  //  and resetting the channel parameters.

  // UNUSED.
  handle = vol = sep = pitch = 0;
}



void I_ShutdownSound(void)
{
    // Wait till all pending sounds are finished.
    int done = 0;
    int i;


    if (audio_dc == NULL) {
        // Audio wasn't initialized.
        return;
    }

    // FIXME (below).
    fprintf( stderr, "I_ShutdownSound: NOT finishing pending sounds\n");
    fflush( stderr );

    while (!done) {
        for (i = 0 ; i < 8 && !channels[i] ; i++);

        // FIXME. No proper channel output.
        //if (i==8)
        done = 1;
    }

    // Cleaning up - make sure playback is stopped and unload driver.
    play.StopPlayback();
    AA_unloadDriver(audio_dc);

    // Done.
    return;
}



void I_InitSound()
{
    int         i;
    int         mix_buf_size;
    N_int32     rate;

    // Secure and configure audio device first.
    fprintf( stderr, "I_InitSound: " );

    /* Load the device digital for this device */
    if ((audio_dc = AA_loadDriver(0)) == NULL) {
        fprintf(stderr, "%s\n", AA_errorMsg(AA_status()));
        return;
    }

    init.dwSize = sizeof(init);
    if (!AA_queryFunctions(audio_dc, AA_GET_INITFUNCS, &init)) {
        fprintf(stderr, "unable to get audio device driver functions!");
        AA_unloadDriver(audio_dc);
        audio_dc = NULL;
        return;
    }
    play.dwSize = sizeof(play);
    if (!AA_queryFunctions(audio_dc, AA_GET_PLAYBACKFUNCS, &play)) {
        fprintf(stderr, "unable to get audio device playback functions!");
        AA_unloadDriver(audio_dc);
        audio_dc = NULL;
    }

    fprintf( stderr, "configured audio device\n" );

    // Initialize external data (all sounds) at start, keep static.
    fprintf( stderr, "I_InitSound: ");

    for (i=1 ; i<NUMSFX ; i++)
    {
        // Alias? Example is the chaingun sound linked to pistol.
        if (!S_sfx[i].link)
        {
            // Load data from WAD file.
            S_sfx[i].data = getsfx( S_sfx[i].name, &lengths[i] );
        }
        else
        {
            // Previously loaded already?
            S_sfx[i].data = S_sfx[i].link->data;
            lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
        }
    }

    fprintf( stderr, " pre-cached all sound data\n" );

    rate = SAMPLERATE;
    mix_buf_size = MIXBUFFERSIZE;
    if (play.SetPlaybackMode(&rate, aaDigitalStereo | aaDigital16Bit) != nOK) {
        // Unsupported sampling rate; Try 44.1 kHz, because many newer audio
        // chips can't do 11.025 kHz.
        rate = SAMPLERATE * 4;
        if (play.SetPlaybackMode(&rate, aaDigitalStereo | aaDigital16Bit) != nOK) {
            fprintf(stderr, "Failed to set playback mode\n");
            AA_unloadDriver(audio_dc);
            audio_dc = NULL;
            return;
        }
        sound_44k = 1;
        mix_buf_size = MIXBUFFERSIZE * 4;
    }

    // Start playback.
    if (play.StartPlayback(mix_buf_size, doom_sndCallback, true, NULL) != nOK) {
        fprintf(stderr, "Failed to start playback\n");
        AA_unloadDriver(audio_dc);
        audio_dc = NULL;
        return;
    }

    // Finished initialization.
    fprintf(stderr, "I_InitSound: sound module ready\n");
}



//
// MUSIC API.
// Still no music done.
// Remains. Dummies.
//
void I_InitMusic(void)          { }
void I_ShutdownMusic(void)      { }

static int      looping=0;
static int      musicdies=-1;

void I_PlaySong(int handle, int looping)
{
  // UNUSED.
  handle = looping = 0;
  musicdies = gametic + TICRATE*30;
}

void I_PauseSong (int handle)
{
  // UNUSED.
  handle = 0;
}

void I_ResumeSong (int handle)
{
  // UNUSED.
  handle = 0;
}

void I_StopSong(int handle)
{
  // UNUSED.
  handle = 0;

  looping = 0;
  musicdies = 0;
}

void I_UnRegisterSong(int handle)
{
  // UNUSED.
  handle = 0;
}

int I_RegisterSong(void* data)
{
  // UNUSED.
  data = NULL;

  return 1;
}

// Is the song playing?
int I_QrySongPlaying(int handle)
{
  // UNUSED.
  handle = 0;
  return looping || musicdies > gametic;
}
