/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  any
*
* Description:  Routines to generate the lookup tables used by the fast
*               fixed point math routines. This is here as a reference
*               in case the tables need to be generated again (and to
*               understand what the values in the tables mean ;-).
*
*               Note also that these routines are written specifically
*               for little endian routines, so will need to be changed
*               for big endian machines (but you only need to generate
*               the tables once!).
*
****************************************************************************/

#include "debug.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void dump_sqrt(FILE *f)
{
    float   flt;
    ulong   i,v,*fi = (ulong*)&flt;

    fprintf(f,"\n/* Table of normalised fixed point square root values */\n\n");
    fprintf(f,"unsigned long FXsqrt_table[] = {\n\t");
    for (i = 0; i < 128; i++) {
        /* Build a float with the bit pattern i as the mantissa, and
         * an exponent of 0, stored as 127, and then square root it.
         */
        *fi = (i << 16) | (127L << 23);
        flt = sqrt(flt);

        /* Strip the 23 bits of the mantissa into the table */
        v = (*fi & 0x7FFFFFL) | 0x800000L;
        if ((i != 0) && ((i & 3) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%08lXL, ", v);
        }
    fprintf(f,"\n\n\t");
    for (i = 0; i < 128; i++) {
        /* Build a float with the bit pattern i as the mantissa, and
         * an exponent of 1, stored as 128, and then square root it.
         */
        *fi = (i << 16) | (128L << 23);
        flt = sqrt(flt);

        /* Strip the 23 bits of the mantissa into the table */
        v = (*fi & 0x7FFFFFL) | 0x800000L;
        if ((i != 0) && ((i & 3) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%08lXL, ", v);
        }
    fprintf(f,"\n\t0x00FFFFFF};\n");
}

void dump_log(FILE *f)
{
    ulong   i,v;

    fprintf(f,"\n/* Table of normalised fixed point common logarithms */\n\n");
    fprintf(f,"unsigned short FXlog10_table[] = {\n\t");
    for (i = 0; i <= 128; i++) {
        v = log10(1.0 + i / 128.0) * 65536.0;

        if ((i != 0) && ((i & 7) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%04X, ", v);
        }
    fprintf(f,"\n\t};\n");

    fprintf(f,"\n/* Table of fixed point common logarithms for the powers of 2 */\n\n");
    fprintf(f,"unsigned long FXlog2_table[] = {\n\t");
    for (i = 0; i < 16; i++) {
        /* Find the log10 of 2^i) */
        v = log10(1L << i) * 65536.0;
        if ((i != 0) && ((i & 3) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%08lXL, ", v);
        }
    fprintf(f,"\n\t};\n");
}

void dump_sine(FILE *f)
{
    int     i;
    uint    v;

    fprintf(f,"\n/* Table of fixed point sines between 0-90 degrees */\n\n");
    fprintf(f,"unsigned long FXsin_table[] = {\n\t");
    for (i = 0; i < 256; i++) {
        v = sin(M_PI_2/256 * i) * 65536.0;
        if ((i != 0) && ((i & 7) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%04X, ", v);
        }
    fprintf(f,"\n\t0x10000L, 0x10000L};\n");
}

void dump_atan(FILE *f)
{
    int     i;
    ulong   v;

    fprintf(f,"\n/* Table of fixed point arc tangents between 0-90 degrees */\n\n");
    fprintf(f,"unsigned long FXatan_table[] = {\n\t");
    for (i = 0; i < 256; i++) {
        v = 180.0/M_PI * 65536.0 * atan(i/256.0);
        if ((i != 0) && ((i & 3) == 0))
            fprintf(f,"\n\t");
        fprintf(f,"0x%08lXL, ", v);
        }
    fprintf(f,"\n\t0x002D0000L, 0x002D0000L};\n");
}

void main(void)
{
    FILE    *f;

    if ((f = fopen("tables.c","w")) == NULL) {
        printf("Unable to open output file!\n");
        exit(1);
        }

    dump_sqrt(f);
    dump_log(f);
    dump_sine(f);
    dump_atan(f);
    fclose(f);
}
