/* mpeg2dec.c, main(), initialization, option processing                    */

/*
 * All modifications (mpeg2decode -> mpeg2play) are
 * Copyright (C) 1994, Stefan Eckart. All Rights Reserved.
 */

/* Copyright (C) 1994, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include "ztimerc.h"
#include "pmapi.h"

#define GLOBAL
#include "config.h"
#include "global.h"

/* private prototypes */
static void initdecoder _ANSI_ARGS_((void));
static void options _ANSI_ARGS_((int *argcp, char **argvp[]));
static int getval _ANSI_ARGS_((char *argv[]));

/* private data */
static int loopflag;

static void doframerate();
static int framerate;
static LZTimerObject timer;
static ulong tftarget;

void sig_int(int signal)
{
}

int main(argc,argv)
int argc;
char *argv[];
{
  int first, framenum;
  int runtime;
  ulong tstart,tstop;

  signal(SIGINT, sig_int);

  mode_xres = 640;
  mode_yres = 480;
  mode_bits = 16;

  options(&argc,&argv);

  ld = &base; /* select base layer context */

  /* open MPEG input file(s) */
  if (argv[1][0]=='-' || argv[1][1]=='\0')
  {
    base.infile = 0; /* stdin */
    loopflag = 0; /* can't rewind stdin */
  }
  else if ((base.infile=fopen(argv[1],"rb")) == NULL)
  {
    sprintf(errortext,"Input file %s not found\n",argv[1]);
    error(errortext);
  }

  if (base.infile != 0)
  {
    initbits();
    startcode();
    switch(showbits(32))
    {
    case SEQ_START_CODE:
      break;
    case PACK_START_CODE:
      sysstream = 1;
      break;
    default:
      error("This not a valid MPEG system or video stream\n");
      break;
    }
    fseek(base.infile,0l,0);
    initbits();
  }

  first = 1;

  do
  {
    /* (re)set to begin of file */
    if (base.infile!=0)
      fseek(base.infile,0l,0);
    initbits();

    framenum = 0;

    while (getheader())
    {
      if (first)
      {
        initdecoder();
        ZTimerInit();
        LZTimerOnExt(&timer);
        tftarget = tstart = LZTimerLapExt(&timer);
        first = 0;
      }

      if(!getpicture(framenum)) {
        loopflag = 0;
        break;
      }

      if (!secondfield)
      {
        if (framerate > 0)
          doframerate();
        framenum++;
        if((num_frames > 0) && (framenum == num_frames))
        break;
      }
    }

    if ((framenum!=0) && !loopflag)
    {
      /* put last frame */
      putlast(framenum);
    }

  }
  while (loopflag);

  if (base.infile!=0)
    fclose(base.infile);

  LZTimerOffExt(&timer);
  tstop = LZTimerCountExt(&timer);
  runtime = (tstop - tstart) / 10000;

  exit_display();

  if (!quiet && runtime!=0) {
    printf("%d.%02d seconds, %d frames, %d.%02d fps\n",runtime/100, runtime%100,
      framenum, ((10000*framenum+runtime/2)/runtime)/100,
      ((10000*framenum+runtime/2)/runtime)%100);
    printf("Video size = %d x %d\n", coded_picture_width,coded_picture_height);
    }

  return 0;
}

static void initdecoder()
{
  int i;
  static int blk_cnt_tab[3] = {6,8,12};

  /* clip table */
  if (!(clp=(unsigned char *)malloc(1024)))
    error("malloc failed\n");

  clp += 384;

  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  /* force MPEG-1 parameters */
  if (!base.mpeg2)
  {
    prog_seq = 1;
    prog_frame = 1;
    pict_struct = FRAME_PICTURE;
    frame_pred_dct = 1;
    chroma_format = CHROMA420;
    matrix_coefficients = 5;
  }

  /* round to nearest multiple of coded macroblocks */
  mb_width = (horizontal_size+15)/16;
  mb_height = (base.mpeg2 && !prog_seq) ? 2*((vertical_size+31)/32)
                                        : (vertical_size+15)/16;
  coded_picture_width = 16*mb_width;
  coded_picture_height = 16*mb_height;

  chrom_width = (chroma_format==CHROMA444) ? coded_picture_width
                                           : coded_picture_width>>1;
  chrom_height = (chroma_format!=CHROMA420) ? coded_picture_height
                                            : coded_picture_height>>1;
  blk_cnt = blk_cnt_tab[chroma_format-1];

  /* output */
  init_display();
}

void error(text)
char *text;
{
  fprintf(stderr,text);
  exit(1);
}

/* compliance warning messages to user, but don't exit() */
void warning(text)
char *text;
{
  fprintf(stderr,"%s\n",text);
}

/* option processing */
static void options(argcp,argvp)
int *argcp;
char **argvp[];
{
  slices = 1;  /* default */
  use_idct = -1;
  bob = 0;

  while (*argcp>1 && (*argvp)[1][0]=='-')
  {
    if ((*argvp)[1][1]=='\0') /* single '-' */
      break;

    while ((*argvp)[1][1])
    {
      switch (toupper((*argvp)[1][1]))
      {
      case 'F':
        framerate = getval(*argvp);
        break;
      case 'V':
        verbose = getval(*argvp);
        break;
      case 'X':
        mode_xres = getval(*argvp);
        break;
      case 'Y':
        mode_yres = getval(*argvp);
        break;
      case 'Z':
        mode_bits = getval(*argvp);
        break;
      case 'I':
        use_idct = getval(*argvp);
        if((use_idct < 0) || (use_idct > 1)) {
          fprintf(stderr, "acceleration level must be 0 or 1. Ingoring.\n");
          use_idct = -1;
        }
        break;
      case 'L':
        loopflag = 1;
        break;
      case 'B':
        bob = 1;
        break;
      case 'A':
        alt_block_format = 1;
        break;
      case 'S':
        slices = getval(*argvp);
        if(slices <= 0) {
          fprintf(stderr,"number of slices must be >= 1. Forcing 1.\n");
          slices = 1;
        }
        break;
      case 'Q':
        quiet = 1;
        break;
      case 'N':
        num_frames = getval(*argvp);
        break;
      default:
        fprintf(stderr,"undefined option -%c ignored\n",(*argvp)[1][1]);
      }

      (*argvp)[1]++;
    }

    (*argvp)++;
    (*argcp)--;
  }

  if (sflag)
  {
    /* input file for spatial prediction */
    llinputname = (*argvp)[1];
    (*argvp)++;
    (*argcp)--;
  }

  if (*argcp!=2 && *argcp!=3)
  {
    printf("\n%s, %s\n",version,author);
    printf("Usage:   mpeg2play {options} input.m2v {upper.m2v}\n\
Options: -vn  verbose output (n: level)\n\
         -l   loop sequence\n\
         -x   set display X resolution (default 640)\n\
         -y   set display Y resolution (default 480)\n\
         -z   set display color depth (default 16)\n\
         -q   quiet (no error output)\n\
         -s   slices per render request (default: 1)\n\
         -i   force acceleration level (0: mocomp, 1: IDCT)\n\
         -b   bob video (default: weave)\n\
         -a   use alternate IDCT block format (not XvMC compatible)\n\
         -nn  stop after playing n frames (0 = play all)\n\
         -fn  frame rate (default = as fast as possible)\n");
    exit(0);
  }
}

static int getval(argv)
char *argv[];
{
  int val;

  if (sscanf(argv[1]+2,"%d",&val)!=1)
    return 0;

  while (isdigit(argv[1][2]))
    argv[1]++;

  return val;
}

static void doframerate()
{
  /* Compute desired frame rate */
  if (framerate <= 0)
    return;

  /* this is where we should be */
  tftarget += 1000000 / framerate;

  /* See if we are already lagging behind */
  if (LZTimerLapExt(&timer) >= tftarget)
    return;

  /* Spin for awhile */
  while (LZTimerLapExt(&timer) < tftarget)
    PM_sleep(1);
}
