;/* getblk.c, DCT block decoding                                             */

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

#include "config.h"
#include "global.h"


/* defined in getvlc.h */
typedef struct {
  char run, level, len;
} DCTtab;

extern DCTtab DCTtabfirst[],DCTtabnext[],DCTtab0[],DCTtab1[];
extern DCTtab DCTtab2[],DCTtab3[],DCTtab4[],DCTtab5[],DCTtab6[];
extern DCTtab DCTtab0a[],DCTtab1a[];


/* decode one intra coded MPEG-1 block */

void getintrablock(comp, dc_dct_pred)
  int comp;
  int dc_dct_pred[];
{
  int val, i, j, sign;
  int k, run;
  unsigned int code, bits;
  DCTtab *tab;
  short *bp;

  bp = ld->block;
  /* decode DC coefficients */
  if (comp<4)
    val = (dc_dct_pred[0]+=getDClum()) << 3;
  else if (comp==4)
    val = (dc_dct_pred[1]+=getDCchrom()) << 3;
  else
    val = (dc_dct_pred[2]+=getDCchrom()) << 3;

  if (alt_block_format) {
    bp[0] = 0;
    bp[1] = val;
    k = 2;
  } else
    bp[0] = val;

  if (fault) return;

  /* decode AC coefficients */
  for (i=1; ; i++) {
    bits = showbits(32);
    code = bits >> 16;
    if (code>=16384)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else {
      if (!quiet)
        fprintf(stderr,"invalid Huffman code in getintrablock()\n");
      fault = 1;
      return;
    }

    bits <<= tab->len;

    if(tab->run < 64) {
      run = tab->run;
      i+= run;
      val = tab->level;
      sign = bits >> (32 - 1);
      flushbits(tab->len + 1);
    } else if (tab->run == 64) { /* end_of_block */
      if (alt_block_format) {
        for (j = 0; j < 4 && k < 129; j++,k += 2)
          bp[k] = 0xFF;
      }          
      flushbits(tab->len);
      return;
    } else {   /* 65, escape */
      run = bits >> (32 - 6);
      i+= run;

      bits <<= 6;

      val = bits >> (32 - 8);

      flushbits(tab->len + 14);

      if (val==0)
        val = getbits(8);
      else if (val==128)
        val = getbits(8) - 256;
      else if (val>128)
        val -= 256;

      if ((sign = (val<0)))
        val = -val;
    }

    j = zig_zag_scan[i];
    val = (val*ld->quant_scale*ld->intra_quantizer_matrix[j]) >> 3;
    val = (val-1) | 1;
    if (alt_block_format) {
      bp[k++] = run;
      bp[k++] = sign ? -val : val;
    } else {
      bp[j] = sign ? -val : val;
    }
  }
}


/* decode one non-intra coded MPEG-1 block */

void getinterblock(comp)
int comp;
{
  int val, i, j, sign;
  int k = 0, run;
  unsigned int code, bits;
  DCTtab *tab;
  short *bp;

  bp = ld->block;

  /* decode AC coefficients */
  for (i=0; ; i++) {
    bits = showbits(32);
    code = bits >> 16;
    if (code>=16384) {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    } else if (code>=1024)
      tab = &DCTtab0[(code>>8)-4];
    else if (code>=512)
      tab = &DCTtab1[(code>>6)-8];
    else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else {
      if (!quiet)
        fprintf(stderr,"invalid Huffman code in getinterblock()\n");
      fault = 1;
      return;
    }

    bits <<= tab->len;

    if (tab->run < 64) {
      run = tab->run;
      i+= run;
      val = tab->level;
      sign = bits >> (32 - 1);
      flushbits(tab->len + 1);
    } else if(tab->run == 64) {  /* end_of_block */
      if (alt_block_format) {
        for (j = 0; j < 4 && k < 129; j++,k += 2)
          bp[k] = 0xFF;
      }          
      flushbits(tab->len);
      return;
    } else {  /* 65, escape */
      run = bits >> (32 - 6);
      i+= run;

      bits <<= 6;

      val = bits >> (32 - 8);

      flushbits(tab->len + 14);
      if (val==0)
        val = getbits(8);
      else if (val==128)
        val = getbits(8) - 256;
      else if (val>128)
        val -= 256;

      if ((sign = (val<0)))
        val = -val;
    }

    j = zig_zag_scan[i];
    val = (((val<<1)+1)*ld->quant_scale*ld->non_intra_quantizer_matrix[j]) >> 4;
    val = (val-1) | 1;
    if (alt_block_format) {
      bp[k++] = run;
      bp[k++] = sign ? -val : val;
    } else {
      bp[j] = sign ? -val : val;
    }
  }
}

/* decode one intra coded MPEG-2 block */

void getmpg2intrablock(comp,dc_dct_pred)
int comp;
int dc_dct_pred[];
{
  int val, i, j, sign;
  int k, run;
  unsigned int code, bits;
  DCTtab *tab;
  short *bp;
  int *qmat;
  unsigned char *scan;

  /* with data partitioning, data always goes to base layer */
  bp = ld->block;

  qmat = (comp<4 || chroma_format==CHROMA420)
         ? ld->intra_quantizer_matrix
         : ld->chroma_intra_quantizer_matrix;

  scan = ld->altscan ? alternate_scan : zig_zag_scan;

  /* decode DC coefficients */
  if (comp<4)
    val = (dc_dct_pred[0]+= getDClum());
  else if ((comp&1)==0)
    val = (dc_dct_pred[1]+= getDCchrom());
  else
    val = (dc_dct_pred[2]+= getDCchrom());

  if (fault) return;

  if (alt_block_format) {
    bp[0] = 0;
    bp[1] = val << (3-dc_prec);
    k = 2;
  } else
    bp[0] = val << (3-dc_prec);

  /* decode AC coefficients */
  for (i=1; ; i++) {
    bits = showbits(32);
    code = bits >> 16;
    if (code>=16384 && !intravlc)
      tab = &DCTtabnext[(code>>12)-4];
    else if (code>=1024) {
      if (intravlc)
        tab = &DCTtab0a[(code>>8)-4];
      else
        tab = &DCTtab0[(code>>8)-4];
    } else if (code>=512) {
      if (intravlc)
        tab = &DCTtab1a[(code>>6)-8];
      else
        tab = &DCTtab1[(code>>6)-8];
    } else if (code>=256)
      tab = &DCTtab2[(code>>4)-16];
    else if (code>=128)
      tab = &DCTtab3[(code>>3)-16];
    else if (code>=64)
      tab = &DCTtab4[(code>>2)-16];
    else if (code>=32)
      tab = &DCTtab5[(code>>1)-16];
    else if (code>=16)
      tab = &DCTtab6[code-16];
    else {
      if (!quiet)
        fprintf(stderr,"invalid Huffman code in getmpg2intrablock()\n");
      fault = 1;
      return;
    }


    if(tab->run < 64) {
      run = tab->run;
      i+= run;
      val = tab->level;
      sign = (bits << tab->len) >> (32 - 1);
      flushbits(tab->len + 1);
    } else if(tab->run == 64) { /* end_of_block */
      if (alt_block_format) {
        for (j = 0; j < 4 && k < 129; j++,k += 2)
          bp[k] = 0xFF;
      }          
      flushbits(tab->len);
      return;
    } else {  /* 65, escape */
      run = (bits << tab->len) >> (32 - 6);
      i+= run;

      flushbits(tab->len + 6);

      val = getbits(12);
      if ((val&2047)==0) {
        if (!quiet)
          fprintf(stderr,
                  "invalid signed_level (escape) in getmpg2intrablock()\n");
        fault = 1;
        return;
      }
      if ((sign = (val>=2048)))
        val = 4096 - val;
    }

    if (i>=64) {
      if (!quiet)
        fprintf(stderr,"DCT coeff index (i) out of bounds (intra2)\n");
      fault = 1;
      return;
    }

    j = scan[i];
    val = (val * ld->quant_scale * qmat[j]) >> 4;
    if (alt_block_format) {
      bp[k++] = run;
      bp[k++] = sign ? -val : val;
    } else {
      bp[j] = sign ? -val : val;
    }
  }
}

/* decode one non-intra coded MPEG-2 block */

void getmpg2interblock(comp)
int comp;
{
  int val, i, j, sign;
  int k = 0, run;
  unsigned int code, bits;
  unsigned char *scan;
  DCTtab *tab;
  short *bp;
  int *qmat;

  bp = ld->block;

  qmat = (comp<4 || chroma_format==CHROMA420)
         ? ld->non_intra_quantizer_matrix
         : ld->chroma_non_intra_quantizer_matrix;

  scan = ld->altscan ? alternate_scan : zig_zag_scan;

  /* decode AC coefficients */
  for (i=0; ; i++) {
    bits = showbits(32);
    code = bits >> 16;
    if (code>=16384) {
      if (i==0)
        tab = &DCTtabfirst[(code>>12)-4];
      else
        tab = &DCTtabnext[(code>>12)-4];
    }
    else if (code>=1024) {
      tab = &DCTtab0[(code>>8)-4];
    } else if (code>=512) {
      tab = &DCTtab1[(code>>6)-8];
    } else if (code>=256) {
      tab = &DCTtab2[(code>>4)-16];
    } else if (code>=128) {
      tab = &DCTtab3[(code>>3)-16];
    } else if (code>=64) {
      tab = &DCTtab4[(code>>2)-16];
    } else if (code>=32) {
      tab = &DCTtab5[(code>>1)-16];
    } else if (code>=16) {
      tab = &DCTtab6[code-16];
    } else {
      if (!quiet)
        fprintf(stderr,"invalid Huffman code in getmpg2interblock()\n");
      fault = 1;
      return;
    }

    if(tab->run < 64) {
      run = tab->run;
      i+= run;
      val = tab->level;
      sign = (bits << tab->len) >> (32 - 1);
      flushbits(tab->len + 1);
    } else if(tab->run == 64) { /* end_of_block */
      if (alt_block_format) {
        for (j = 0; j < 4 && k < 129; j++,k += 2)
          bp[k] = 0xFF;
      }          
      flushbits(tab->len);
      return;
    } else { /* 65, escape */
      run = (bits << tab->len) >> (32 - 6);
      i+= run;

      flushbits(tab->len + 6);

      val = getbits(12);
      if ((val&2047)==0) {
        if (!quiet)
          fprintf(stderr,
                  "invalid signed_level (escape) in getmpg2intrablock()\n");
        fault = 1;
        return;
      }
      if ((sign = (val>=2048)))
         val = 4096 - val;
    }

    if (i>=64) {
      if (!quiet)
        fprintf(stderr,"DCT coeff index (i) out of bounds (inter2)\n");
      fault = 1;
      return;
    }

    j = scan[i];
    val = (((val<<1)+1) * ld->quant_scale * qmat[j]) >> 5;
    if (alt_block_format) {
      bp[k++] = run;
      bp[k++] = sign ? -val : val;
    } else {
      bp[j] = sign ? -val : val;
    }
  }
}
