/* global.h, global variables                                               */

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

#define MAXDWORDS   512
#define MAXBYTES    (MAXDWORDS * 4)
#define MAXBITS     (MAXDWORDS * 32)

#include "mpeg2dec.h"

/* choose between declaration (GLOBAL undefined)
 * and definition (GLOBAL defined)
 * GLOBAL is defined in exactly one file (mpeg2dec.c)
 */

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

/* prototypes of global functions */

/* getbits.c */
void initbits _ANSI_ARGS_((void));
void fillbfr _ANSI_ARGS_((void));
void startcode _ANSI_ARGS_((void));
void flushend(int n);

/* getblk.c */
void SetupTabs _ANSI_ARGS_((void));
void getintrablock _ANSI_ARGS_((int comp, int dc_dct_pred[]));
void getinterblock _ANSI_ARGS_((int comp));
void getmpg2intrablock _ANSI_ARGS_((int comp, int dc_dct_pred[]));
void getmpg2interblock _ANSI_ARGS_((int comp));

/* gethdr.c */
int getheader _ANSI_ARGS_((void));
int getslicehdr _ANSI_ARGS_((void));

/* getpic.c */
int getpicture _ANSI_ARGS_((int framenum));
void putlast _ANSI_ARGS_((int framenum));

/* getvlc.c */
int getMBtype _ANSI_ARGS_((void));
int getMV _ANSI_ARGS_((void));
int getDMV _ANSI_ARGS_((void));
int getCBP _ANSI_ARGS_((void));
int getMBA _ANSI_ARGS_((void));
int getDClum _ANSI_ARGS_((void));
int getDCchrom _ANSI_ARGS_((void));

/* idct.c */
void idct _ANSI_ARGS_((short *block));

/* idctref.c */
void init_idctref _ANSI_ARGS_((void));
void idctref _ANSI_ARGS_((short *block));

/* motion.c */
void motion_vectors _ANSI_ARGS_((int PMV[2][2][2], int dmvector[2],
  int mv_field_sel[2][2], int s, int mv_count, int mv_format,
  int h_r_size, int v_r_size, int dmv, int mvscale));
void motion_vector _ANSI_ARGS_((int *PMV, int *dmvector,
  int h_r_size, int v_r_size, int dmv, int mvscale, int full_pel_vector));
void calc_DMV _ANSI_ARGS_((int DMV[][2], int *dmvector, int mvx, int mvy));

/* mpeg2dec.c */
void error _ANSI_ARGS_((char *text));
void warning _ANSI_ARGS_((char *text));
void printbits _ANSI_ARGS_((int code, int bits, int len));

/* store.c */
void storeframe _ANSI_ARGS_((unsigned char *src[], int frame));

/* display.c */
void init_display (void);
void exit_display (void);
int display_frame (int);
void display_second_field (int);
void display_field (int);
void assign_surfaces (void);
void add_block (int);
void get_block(void);
void add_macroblock (
   int x,
   int y,
   int mb_type,
   int motion_type,
   int (*PMV)[2][2],
   int (*mv_field_sel)[2],
   int *dmvector,
   int cbp,
   int dct_type
);

/* global variables */

EXTERN char version[]
#ifdef GLOBAL
  ="mpeg2play V1.1a, 94/07/04"
#endif
;

EXTERN char author[]
#ifdef GLOBAL
  ="(C) 1994, Stefan Eckart & MPEG Software Simulation Group"
#endif
;

/* zig-zag scan */
EXTERN unsigned char zig_zag_scan[64]
#ifdef GLOBAL
=
{
  0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,
  12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
  35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,
  58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
}
#endif
;

/* alternate scan */
EXTERN unsigned char alternate_scan[64]
#ifdef GLOBAL
=
{
  0,8,16,24,1,9,2,10,17,25,32,40,48,56,57,49,
  41,33,26,18,3,11,4,12,19,27,34,42,50,58,35,43,
  51,59,20,28,5,13,6,14,21,29,36,44,52,60,37,45,
  53,61,22,30,7,15,23,31,38,46,54,62,39,47,55,63
}
#endif
;

/* default intra quantization matrix */
EXTERN unsigned char default_intra_quantizer_matrix[64]
#ifdef GLOBAL
=
{
  8, 16, 19, 22, 26, 27, 29, 34,
  16, 16, 22, 24, 27, 29, 34, 37,
  19, 22, 26, 27, 29, 34, 34, 38,
  22, 22, 26, 27, 29, 34, 37, 40,
  22, 26, 27, 29, 32, 35, 40, 48,
  26, 27, 29, 32, 35, 40, 48, 58,
  26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83
}
#endif
;

/* non-linear quantization coefficient table */
EXTERN unsigned char non_linear_mquant_table[32]
#ifdef GLOBAL
=
{
   0, 1, 2, 3, 4, 5, 6, 7,
   8,10,12,14,16,18,20,22,
  24,28,32,36,40,44,48,52,
  56,64,72,80,88,96,104,112
}
#endif
;

EXTERN int quiet;
EXTERN char errortext[256];
EXTERN unsigned char *clp;
EXTERN int horizontal_size,vertical_size,mb_width,mb_height;
EXTERN int coded_picture_width, coded_picture_height;
EXTERN int chroma_format,chrom_width,chrom_height,blk_cnt;
EXTERN int pict_type;
EXTERN int forw_r_size,back_r_size;
EXTERN int full_forw,full_back;
EXTERN int fault;
EXTERN int verbose;
EXTERN int mode_xres;
EXTERN int mode_yres;
EXTERN int mode_bits;
EXTERN int prog_seq;
EXTERN int h_forw_r_size,v_forw_r_size,h_back_r_size,v_back_r_size;
EXTERN int dc_prec,pict_struct,topfirst,frame_pred_dct,conceal_mv;
EXTERN int intravlc,repeatfirst,prog_frame;
EXTERN int secondfield;
EXTERN int stwc_table_index,llw,llh,hm,hn,vm,vn;
EXTERN int lltempref,llx0,lly0,llprog_frame,llfieldsel;
EXTERN unsigned char *llframe0[3],*llframe1[3];
EXTERN short *lltmp;
EXTERN char *llinputname;
EXTERN int sflag;
EXTERN int matrix_coefficients;
EXTERN int sysstream;
/* output */
EXTERN int slices;
EXTERN int use_idct;
EXTERN int bob;
EXTERN int num_frames;
EXTERN int alt_block_format;

/* layer specific variables (needed for SNR and DP scalability) */
EXTERN struct layer_data {
  /* bit input */
  FILE *infile;
  unsigned int rdBfr[MAXDWORDS];
  unsigned int Bfr;
  int outcnt;
  int bytesInPacket;
  /* sequence header */
  int intra_quantizer_matrix[64],non_intra_quantizer_matrix[64];
  int chroma_intra_quantizer_matrix[64],chroma_non_intra_quantizer_matrix[64];
  int mpeg2;
  /* sequence scalable extension */
  int scalable_mode;
  /* picture coding extension */
  int qscale_type,altscan;
  /* picture spatial scalable extension */
  int pict_scal;
  /* slice/macroblock */
  int pri_brk;
  int quant_scale;
  short *block;
} base,  *ld;

#define showbits(n) (ld->Bfr >> (32-(n)))

static inline void flushbits (int n)
{
   ld->Bfr <<= n;
   if((MAXBITS - ld->outcnt) >= n) {
      int shift = ld->outcnt & 31;
      int index = ld->outcnt >> 5;
      ld->Bfr |= (ld->rdBfr[index] << shift) >> (32 - n);
      if((32 - shift) < n)
         ld->Bfr |= ld->rdBfr[index + 1] >> (64 - n - shift);
      ld->outcnt += n;
   } else flushend(n);
}

static inline void flushbits32()
{
   if((MAXBITS - ld->outcnt) >= 32) {
      int shift = ld->outcnt & 31;
      int index = ld->outcnt >> 5;
      if(shift) {
         ld->Bfr = (ld->rdBfr[index] << shift) |
                   (ld->rdBfr[index + 1] >> (32 - shift));
      } else
         ld->Bfr = ld->rdBfr[index];
      ld->outcnt += 32;
   } else flushend(32);
}

static inline unsigned int getbits(int n)
{
  unsigned int l;

  l = showbits(n);
  flushbits(n);

  return l;
}


