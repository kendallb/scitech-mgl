/* getpic.c, picture decoding                                               */

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

/* private prototypes*/
static void getMBs _ANSI_ARGS_((int framenum));
static void macroblock_modes _ANSI_ARGS_((int *pmb_type, int *pstwtype,
  int *pstwclass, int *pmotion_type, int *pmv_count, int *pmv_format, int *pdmv,
  int *pmvscale, int *pdct_type));

/* decode one frame or field picture */

static int do_second_field = 0;

int getpicture(framenum)
int framenum;
{
  if (pict_struct==FRAME_PICTURE && secondfield)
  {
    /* recover from illegal number of field pictures */
    printf("odd number of field pictures\n");
    secondfield = 0;
  }

  if(!secondfield)
     assign_surfaces();

  getMBs(framenum);

  if(pict_struct==FRAME_PICTURE) {
      if(bob) {
         do_second_field = 1;
         if(!display_frame(TOP_FIELD))
            return 0;
      } else { 
         if(!display_frame(FRAME_PICTURE))
            return 0;
      }
  } else {
      if(bob) {
          if(secondfield) {
             if(!display_frame(pict_struct))
                return 0;
          } else
             display_field(pict_struct);
      } else if(secondfield) {
          if(!display_frame(FRAME_PICTURE))
              return 0;
      }
  }

  if (pict_struct!=FRAME_PICTURE)
    secondfield = !secondfield;

   return 1;
}

/* decode all macroblocks of the current picture */

static void getMBs(framenum)
int framenum;
{
  int comp;
  int MBA, MBAmax, MBAinc, mb_type, cbp, motion_type, dct_type;
  int slice_vert_pos_ext;
  unsigned int code;
  int dc_dct_pred[3];
  int mv_count, mv_format, mvscale;
  int PMV[2][2][2], mv_field_sel[2][2];
  int dmv, dmvector[2];
  int qs;
  int stwtype, stwclass;

  /* number of macroblocks per picture */
  MBAmax = mb_width*mb_height;

  if (pict_struct!=FRAME_PICTURE)
    MBAmax>>=1; /* field picture has half as mnay macroblocks as frame */

  MBA = 0; /* macroblock address */
  MBAinc = 0;

  fault=0;

  for (;;)
  {
    if(do_second_field && (MBA >= (MBAmax >> 1))) {
       do_second_field = 0;
       display_second_field(BOTTOM_FIELD);
    }

    if (MBAinc==0)
    {
      if (!showbits(23) || fault) /* startcode or fault */
      {
resync: /* if fault: resynchronize to next startcode */
        fault = 0;

        if (MBA>=MBAmax)
          return; /* all macroblocks decoded */

        startcode();
        code = showbits(32);

        if (code<SLICE_MIN_START || code>SLICE_MAX_START)
        {
          /* only slice headers are allowed in picture_data */
          if (!quiet)
            printf("Premature end of picture\n");
          return;
        }

        flushbits32();

        /* decode slice header (may change quant_scale) */
        slice_vert_pos_ext = getslicehdr();

        /* decode macroblock address increment */
        MBAinc = getMBA();

        if (fault) goto resync;

        /* set current location */
        MBA = ((slice_vert_pos_ext<<7) + (code&255) - 1)*mb_width + MBAinc - 1;
        MBAinc = 1; /* first macroblock in slice: not skipped */

        /* reset all DC coefficient and motion vector predictors */
        dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
        PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
      }
      else /* neither startcode nor fault */
      {
        if (MBA>=MBAmax)
        {
          if (!quiet)
            printf("Too many macroblocks in picture\n");
          return;
        }


        /* decode macroblock address increment */
        MBAinc = getMBA();

        if (fault) goto resync;
      }
    }

    if (MBA>=MBAmax)
    {
      /* MBAinc points beyond picture dimensions */
      if (!quiet)
        printf("Too many macroblocks in picture\n");
      return;
    }

    if (MBAinc==1) /* not skipped */
    {
      macroblock_modes(&mb_type, &stwtype, &stwclass,
        &motion_type, &mv_count, &mv_format, &dmv, &mvscale,
        &dct_type);

      if (fault) goto resync;

      if (mb_type & MB_QUANT)
      {
        qs = getbits(5);

        if (ld->mpeg2)
          ld->quant_scale =
            ld->qscale_type ? non_linear_mquant_table[qs] : (qs << 1);
        else
          ld->quant_scale = qs;
      }

      /* motion vectors */

      /* decode forward motion vectors */
      if ((mb_type & MB_FORWARD) || ((mb_type & MB_INTRA) && conceal_mv))
      {
        if (ld->mpeg2)
          motion_vectors(PMV,dmvector,mv_field_sel,
            0,mv_count,mv_format,h_forw_r_size,v_forw_r_size,dmv,mvscale);
        else
          motion_vector(PMV[0][0],dmvector,
            forw_r_size,forw_r_size,0,0,full_forw);
      }

      if (fault) goto resync;

      /* decode backward motion vectors */
      if (mb_type & MB_BACKWARD)
      {
        if (ld->mpeg2)
          motion_vectors(PMV,dmvector,mv_field_sel,
            1,mv_count,mv_format,h_back_r_size,v_back_r_size,0,mvscale);
        else
          motion_vector(PMV[0][1],dmvector,
            back_r_size,back_r_size,0,0,full_back);
      }

      if (fault) goto resync;

      if ((mb_type & MB_INTRA) && conceal_mv)
        flushbits(1); /* remove marker_bit */

      /* macroblock_pattern */
      if (mb_type & MB_PATTERN)
      {
        cbp = getCBP();
        if (chroma_format==CHROMA422)
        {
          cbp = (cbp<<2) | getbits(2); /* coded_block_pattern_1 */
        }
        else if (chroma_format==CHROMA444)
        {
          cbp = (cbp<<6) | getbits(6); /* coded_block_pattern_2 */
        }
      }
      else
        cbp = (mb_type & MB_INTRA) ? (1<<blk_cnt)-1 : 0;

      if (fault) goto resync;

      /* decode blocks */
      for (comp=0; comp<blk_cnt; comp++) {

        if (cbp & (1<<(blk_cnt-1-comp))) {
          get_block();
          if (mb_type & MB_INTRA)
          {
            if (ld->mpeg2)
              getmpg2intrablock(comp,dc_dct_pred);
            else
              getintrablock(comp,dc_dct_pred);
          }
          else
          {
            if (ld->mpeg2)
              getmpg2interblock(comp);
            else
              getinterblock(comp);
          }

          if (fault) goto resync;

          add_block(mb_type & MB_INTRA);

        }
      }

      /* reset intra_dc predictors */
      if (!(mb_type & MB_INTRA))
        dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;

      /* reset motion vector predictors */
      if ((mb_type & MB_INTRA) && !conceal_mv)
      {
        /* intra mb without concealment motion vectors */
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
        PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
      }

      if ((pict_type==P_TYPE) && !(mb_type & (MB_FORWARD|MB_INTRA)))
      {
        /* non-intra mb without forward mv in a P picture */
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;

        /* derive motion_type */
        if (pict_struct==FRAME_PICTURE)
          motion_type = MC_FRAME;
        else
        {
          motion_type = MC_FIELD;
          /* predict from field of same parity */
          mv_field_sel[0][0] = (pict_struct==BOTTOM_FIELD);
        }
      }

      if (stwclass==4)
      {
        /* purely spatially predicted macroblock */
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
        PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
      }
    }
    else /* MBAinc!=1: skipped macroblock */
    {
      /* reset intra_dc predictors */
      dc_dct_pred[0]=dc_dct_pred[1]=dc_dct_pred[2]=0;

      /* reset motion vector predictors */
      if (pict_type==P_TYPE)
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;

      /* derive motion_type */
      if (pict_struct==FRAME_PICTURE)
        motion_type = MC_FRAME;
      else
      {
        motion_type = MC_FIELD;
        /* predict from field of same parity */
        mv_field_sel[0][0]=mv_field_sel[0][1] = (pict_struct==BOTTOM_FIELD);
      }

      /* skipped I are spatial-only predicted, */
      /* skipped P and B are temporal-only predicted */
      stwtype = (pict_type==I_TYPE) ? 8 : 0;

      /* clear MB_INTRA */
      mb_type&= ~MB_INTRA;

      cbp = 0; /* no block data */
    }

    /* motion compensation */
    add_macroblock(MBA%mb_width, MBA/mb_width, mb_type, motion_type,
                   PMV, mv_field_sel, dmvector, cbp, dct_type);

    /* advance to next macroblock */
    MBA++;
    MBAinc--;

  }
}


static void macroblock_modes(pmb_type,pstwtype,pstwclass,
  pmotion_type,pmv_count,pmv_format,pdmv,pmvscale,pdct_type)
  int *pmb_type, *pstwtype, *pstwclass;
  int *pmotion_type, *pmv_count, *pmv_format, *pdmv, *pmvscale;
  int *pdct_type;
{
  int mb_type;
  int stwtype, stwcode, stwclass;
  int motion_type = 0, mv_count, mv_format, dmv, mvscale;
  int dct_type;
  static unsigned char stwc_table[3][4]
    = { {6,3,7,4}, {2,1,5,4}, {2,5,7,4} };
  static unsigned char stwclass_table[9]
    = {0, 1, 2, 1, 1, 2, 3, 3, 4};

  /* get macroblock_type */
  mb_type = getMBtype();

  if (fault) return;

  /* get spatial_temporal_weight_code */
  if (mb_type & MB_WEIGHT)
  {
    if (stwc_table_index==0)
      stwtype = 4;
    else
    {
      stwcode = getbits(2);
      stwtype = stwc_table[stwc_table_index-1][stwcode];
    }
  }
  else
    stwtype = (mb_type & MB_CLASS4) ? 8 : 0;

  /* derive spatial_temporal_weight_class (Table 7-18) */
  stwclass = stwclass_table[stwtype];

  /* get frame/field motion type */
  if (mb_type & (MB_FORWARD|MB_BACKWARD))
  {
    if (pict_struct==FRAME_PICTURE) /* frame_motion_type */
    {
      motion_type = frame_pred_dct ? MC_FRAME : getbits(2);
    }
    else /* field_motion_type */
    {
      motion_type = getbits(2);
    }
  }
  else if ((mb_type & MB_INTRA) && conceal_mv)
  {
    /* concealment motion vectors */
    motion_type = (pict_struct==FRAME_PICTURE) ? MC_FRAME : MC_FIELD;
  }

  /* derive mv_count, mv_format and dmv, (table 6-17, 6-18) */
  if (pict_struct==FRAME_PICTURE)
  {
    mv_count = (motion_type==MC_FIELD && stwclass<2) ? 2 : 1;
    mv_format = (motion_type==MC_FRAME) ? MV_FRAME : MV_FIELD;
  }
  else
  {
    mv_count = (motion_type==MC_16X8) ? 2 : 1;
    mv_format = MV_FIELD;
  }

  dmv = (motion_type==MC_DMV); /* dual prime */

  /* field mv predictions in frame pictures have to be scaled */
  mvscale = ((mv_format==MV_FIELD) && (pict_struct==FRAME_PICTURE));

  /* get dct_type (frame DCT / field DCT) */
  dct_type = (pict_struct==FRAME_PICTURE)
             && (!frame_pred_dct)
             && (mb_type & (MB_PATTERN|MB_INTRA))
             ? getbits(1)
             : 0;

  /* return values */
  *pmb_type = mb_type;
  *pstwtype = stwtype;
  *pstwclass = stwclass;
  *pmotion_type = motion_type;
  *pmv_count = mv_count;
  *pmv_format = mv_format;
  *pdmv = dmv;
  *pmvscale = mvscale;
  *pdct_type = dct_type;
}


