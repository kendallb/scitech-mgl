/* getbits.c, bit level routines                                            */

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

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "pmapi.h"

static int nextpacket(void);

/* initialize buffer, call once before first getbits or showbits */

void initbits()
{
  ld->outcnt = MAXBITS;
  ld->bytesInPacket = 0;
  flushbits32();
}

/* align to start of next startcode */

void startcode()
{
  /* byte align */
  if(ld->outcnt & 7)
     flushbits(8 - (ld->outcnt & 7));
  while (showbits(24) != 1)
    flushbits(8);
}

static void padbuffer(void)
{
    unsigned char *ptr = (unsigned char*)(ld->rdBfr);
    unsigned int *rdptr;

    ptr += (MAXBITS - ld->outcnt) >> 3;

    while((unsigned long)ptr & 3L) {
       *ptr++ = 0;
       ld->outcnt -= 8;
    }

    rdptr = (unsigned int*)ptr;

    while(ld->outcnt > 0) {
#ifdef __BIG_ENDIAN__
        *rdptr++ = SEQ_END_CODE;
#else
        *rdptr++ = 0xB7010000;
#endif
        ld->outcnt -= 32;
    }
}

static void readbytes(int n)
{
    unsigned char *ptr = (unsigned char*)(ld->rdBfr);
    int num;

    ptr += (MAXBITS - ld->outcnt) >> 3;

    num = fread(ptr, 1, n, ld->infile);

    if(num < 0) num = 0;

    ld->outcnt -= (num << 3);

    if(num != n) /* file came up short */
       padbuffer();
}

void flushend(int n)
{
#ifndef __BIG_ENDIAN__
   int i;
#endif   

   if(MAXBITS > ld->outcnt) {
      if(n == 32)
        ld->Bfr = (ld->rdBfr[MAXDWORDS - 1] << (ld->outcnt & 31));
      else
        ld->Bfr |= (ld->rdBfr[MAXDWORDS - 1] << (ld->outcnt & 31)) >> (32 - n);
      n -= (MAXBITS - ld->outcnt);
      ld->outcnt = MAXBITS;
   }

   if(sysstream) {
      while(ld->outcnt) { /* ld->outcnt is bits needed at this point */
         if(ld->bytesInPacket) {
             int bytesToRead = ld->outcnt >> 3;

             if(bytesToRead > ld->bytesInPacket)
                bytesToRead = ld->bytesInPacket;
             readbytes(bytesToRead);
             ld->bytesInPacket -= bytesToRead;
         } else ld->bytesInPacket = nextpacket();
      }
   } else readbytes(MAXBYTES);

#ifndef __BIG_ENDIAN__
   for(i = 0; i < MAXDWORDS; i++)
      ld->rdBfr[i] = PM_getBE_32(ld->rdBfr[i]);
#endif

   if(n) {
       if(n == 32)
           ld->Bfr = ld->rdBfr[0];
       else
           ld->Bfr |= ld->rdBfr[0] >> (32 - n);
       ld->outcnt += n;
   }
}


/* MPEG-1 system layer demultiplexer */

static inline int getbyte()
{
   unsigned char data;

   fread(&data, 1, 1, ld->infile);

   return data;
}

static inline int getword()
{
   unsigned short data;

   fread(&data, 1, 2, ld->infile);

   PM_convBE_16(data);

   return data;
}

static inline int getlong()
{
   unsigned int data;

   if(!fread(&data, 1, 4, ld->infile))
       return ISO_END_CODE;

   PM_convBE_32(data);

   return data;
}

static inline void skipbytes(int n)
{
   fseek(ld->infile, n, SEEK_CUR);
}

/* parse system layer, ignore everything we don't need */

static int nextpacket(void)
{
  unsigned int code;
  int length;

  while(1) {
    code = getlong();
    switch(code)
    {
    case PACK_START_CODE: /* pack header */
      /* skip pack header (system_clock_reference and mux_rate) */
      code = getword();

      if((code >> 12) == 2) { /* MPEG 1 */
          skipbytes(6);
      } else
      if(code & 0x4000) {
          skipbytes(7);
          skipbytes(getbyte() & 7);
      }
      break;
    case 0x1e0: /* video stream 0 packet */
      length = getword() - 1; /* packet length */
      code = getbyte();

      if((code >> 6) == 0x02) {
           int skip;

           skipbytes(1);
           skip = getbyte();
           skipbytes(skip);
           length -= (skip + 2);

           return length;
      }

      while(code == 0xff) {
         code = getbyte();
         length--;
      }

      if((code & 0x40) == 0x40) { /* skip STD_buffer_scale */
        skipbytes(1);
        code = getbyte();
        length -= 2;
      }

      if(code>=0x30) {
        if(code>=0x40) {
          fprintf(stderr,"Error in packet header\n");
          exit(1);
        }
        /* skip presentation and decoding time stamps */
        skipbytes(9);
        length -= 9;
      }
      else if(code>=0x20) {
        /* skip presentation time stamps */
        skipbytes(4);
        length -= 4;
      }
      else if(code!=0x0f) {
        fprintf(stderr,"Error in packet header\n");
        exit(1);
      }

      if(length < 0) {
        fprintf(stderr,"Bad packet length\n");
        exit(1);
      }

      return length;
    case ISO_END_CODE: /* end */
      /* simulate a buffer full of sequence end codes */
      padbuffer();
      return 0;
    default:
      if(code>=SYSTEM_START_CODE) {
        /* skip system headers and non-video packets*/
        skipbytes(getword());
      } else {
        fprintf(stderr,"Unexpected startcode %08x in system layer\n",code);
        exit(1);
      }
      break;
    }
  }
}

