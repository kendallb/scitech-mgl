/* getvlc.c, variable length decoding                                       */

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

#include <stdio.h>

#include "config.h"
#include "global.h"
#include "getvlc.h"

/* private prototypes */
static int getIMBtype _ANSI_ARGS_((void));
static int getPMBtype _ANSI_ARGS_((void));
static int getBMBtype _ANSI_ARGS_((void));
static int getDMBtype _ANSI_ARGS_((void));
static int getspIMBtype _ANSI_ARGS_((void));
static int getspPMBtype _ANSI_ARGS_((void));
static int getspBMBtype _ANSI_ARGS_((void));
static int getSNRMBtype _ANSI_ARGS_((void));

int getMBtype()
{
  int mb_type;

  if (ld->scalable_mode==SC_SNR)
    mb_type = getSNRMBtype();
  else
  {
    switch (pict_type)
    {
    case I_TYPE:
      mb_type = ld->pict_scal ? getspIMBtype() : getIMBtype();
      break;
    case P_TYPE:
      mb_type = ld->pict_scal ? getspPMBtype() : getPMBtype();
      break;
    case B_TYPE:
      mb_type = ld->pict_scal ? getspBMBtype() : getBMBtype();
      break;
    case D_TYPE:
      /* MPEG-1 only, not implemented */
      mb_type = getDMBtype();
      break;
    }
  }

  return mb_type;
}

static int getIMBtype(void)
{
   int code = showbits(2);

   if(code & 2) {
      flushbits(1);
      return 1;
   } else if(code == 1) {
      flushbits(2);
   } else {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
     fault = 1;
   }

   return 17;
}

static int getPMBtype()
{
  int code;

  if ((code = showbits(6))>=8)
  {
    code >>= 3;
    flushbits(PMBtab0[code].len);
    return PMBtab0[code].val;
  }

  if (code==0)
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  flushbits(PMBtab1[code].len);

  return PMBtab1[code].val;
}

static int getBMBtype()
{
  int code;

  if ((code = showbits(6))>=8)
  {
    code >>= 2;
    flushbits(BMBtab0[code].len);

    return BMBtab0[code].val;
  }

  if (code==0)
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  flushbits(BMBtab1[code].len);

  return BMBtab1[code].val;
}

static int getDMBtype()
{
  if (!getbits(1))
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault=1;
  }

  return 1;
}

/* macroblock_type for pictures with spatial scalability */
static int getspIMBtype()
{
  int code;

  code = showbits(4);

  if (code==0)
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  flushbits(spIMBtab[code].len);
  return spIMBtab[code].val;
}

static int getspPMBtype()
{
  int code;

  code = showbits(7);

  if (code<2)
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  if (code>=16)
  {
    code >>= 3;
    flushbits(spPMBtab0[code].len);

    return spPMBtab0[code].val;
  }

  flushbits(spPMBtab1[code].len);

  return spPMBtab1[code].val;
}

static int getspBMBtype()
{
  int code;
  VLCtab *p;

  code = showbits(9);

  if (code>=64)
    p = &spBMBtab0[(code>>5)-2];
  else if (code>=16)
    p = &spBMBtab1[(code>>2)-4];
  else if (code>=8)
    p = &spBMBtab2[code-8];
  else
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  flushbits(p->len);

  return p->val;
}

static int getSNRMBtype()
{
  int code;

  code = showbits(3);

  if (code==0)
  {
    if (!quiet)
      fprintf(stderr,"Invalid macroblock_type code\n");
    fault = 1;
    return 0;
  }

  flushbits(SNRMBtab[code].len);
  return SNRMBtab[code].val;
}

int getMV(void)
{
    VLCtab *tab;
    int code = showbits(11);

    if(code >= 1024) {
       flushbits(1);
       return 0;
    } else if(code >= 128) {
        tab = &MVtab0[code>>7];
    } else if(code >= 48) {
        tab = &MVtab1[code>>4];
    } else if(code >= 24) {
        tab = &MVtab2[(code>>1)-12];
    } else {
       flushbits(1);
       if (!quiet)
         fprintf(stderr,"Invalid motion_vector code\n");
       fault=1;
       return 0;
    }

    flushbits(tab->len + 2);

    code >>= (9 - tab->len);

    return (code & 1) ? -tab->val : tab->val;
}



/* get differential motion vector (for dual prime prediction) */
int getDMV()
{

  if (getbits(1))
  {
    return getbits(1) ? -1 : 1;
  }
  else
  {
    return 0;
  }
}

int getCBP()
{
  int code;

  if ((code = showbits(9))>=128)
  {
    code >>= 4;
    flushbits(CBPtab0[code].len);

    return CBPtab0[code].val;
  }

  if (code>=8)
  {
    code >>= 1;
    flushbits(CBPtab1[code].len);

    return CBPtab1[code].val;
  }

  if (code<1)
  {
    if (!quiet)
      fprintf(stderr,"Invalid coded_block_pattern code\n");
    fault = 1;
    return 0;
  }

  flushbits(CBPtab2[code].len);

  return CBPtab2[code].val;
}

int getMBA()
{
  int code, val;

  val = 0;

  while ((code = showbits(11))<24)
  {
    if (code!=15) /* if not macroblock_stuffing */
    {
      if (code==8) /* if macroblock_escape */
      {
        val+= 33;
      }
      else
      {
        if (!quiet)
          fprintf(stderr,"Invalid macroblock_address_increment code\n");

        fault = 1;
        return 1;
      }
    }
    flushbits(11);
  }

  if (code>=1024)
  {
    flushbits(1);
    return val + 1;
  }

  if (code>=128)
  {
    code >>= 6;
    flushbits(MBAtab1[code].len);

    return val + MBAtab1[code].val;
  }

  code-= 24;
  flushbits(MBAtab2[code].len);

  return val + MBAtab2[code].val;
}

int getDClum()
{
  unsigned int code;
  int val, size;
  VLCtab *tab;

  /* decode length */
  code = showbits(32);

  if ((code >> (32 - 5)) < 31) {
    tab = &DClumtab0[code >> (32 - 5)];
  } else {
    tab = &DClumtab1[(code >> (32 - 9)) - 0x1f0];
  }

  size = tab->val;

  if (size==0) {
    val = 0;
  } else {
    val = (code << tab->len) >> (32 - size);
    if ((val & (1<<(size-1)))==0)
      val-= (1<<size) - 1;
  }

  flushbits(tab->len + size);

  return val;
}

#if 1

int getDCchrom()
{
  int code, size, val;

  /* decode length */
  code = showbits(5);

  if (code<31)
  {
    size = DCchromtab0[code].val;
    flushbits(DCchromtab0[code].len);
  }
  else
  {
    code = showbits(10) - 0x3e0;
    size = DCchromtab1[code].val;
    flushbits(DCchromtab1[code].len);

  }

  if (size==0) {
    val = 0;
  } else
  {
    val = getbits(size);
    if ((val & (1<<(size-1)))==0)
      val-= (1<<size) - 1;
  }

  return val;
}

#else

int getDCchrom()
{
  unsigned int code;
  int val, size;
  VLCtab *tab;

  /* decode length */
  code = showbits(32);

  if ((code >> (32 - 5)) < 31) {
    tab = &DCchromtab0[code >> (32 - 5)];
  } else {
    tab = &DCchromtab1[(code >> (32 - 10)) - 0x3e0];
  }

  size = tab->val;

  if (size==0) {
    val = 0;
  } else {
    val = (code << tab->len) >> (32 - size);
    if ((val & (1<<(size-1)))==0)
      val-= (1<<size) - 1;
  }

  flushbits(tab->len + size);

  return val;
}

#endif
