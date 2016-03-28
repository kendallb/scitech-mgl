/****************************************************************************
*
*                         Techniques Class Library
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
* Description:  C implementation for the pre-defined hashing routines
*               for the class library.
*
*
****************************************************************************/

#include "tcl/hashtab.hpp"
#include "tcl/error.hpp"

/*---------------------------- Implementation -----------------------------*/

uint TCL_hashAdd(uchar *name)
/****************************************************************************
*
* Function:     TCL_hashAdd
* Parameters:   name    - String to hash
* Returns:      hash value of the string
*
* Description:  This hash function simply adds together the characters
*               in the name.
*
****************************************************************************/
{
    uint    h;

    for (h = 0; *name; h += *name++)
        ;
    return h;
}

#if     defined(__16BIT__)
#define NBITS_IN_UNSIGNED       16
#else
#define NBITS_IN_UNSIGNED       32
#endif
#define SEVENTY_FIVE_PERCENT    ((int)(NBITS_IN_UNSIGNED * .75))
#define TWELVE_PERCENT          ((int)(NBITS_IN_UNSIGNED * .125))
#define HIGH_BITS               ( ~( (unsigned)(~0) >> TWELVE_PERCENT) )

uint TCL_hashPJW(uchar *name)
/****************************************************************************
*
* Function:     TCL_hashPJW
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  This hash function uses a shift-and-XOR strategy to
*               randomise the input key. The main iteration of the loop
*               shifts the accumulated hash value to the left by a few
*               bits and adds in the current character. When the number
*               gets too large, it is randomised by XORing it with a
*               shifted version of itself.
*
****************************************************************************/
{
    uint    h = 0;          // The hash value
    uint    g;

    for (; *name; name++) {
        h = (h << TWELVE_PERCENT) + *name;
        if ((g = (h & HIGH_BITS)) != 0)
            h = (h ^ (g >> SEVENTY_FIVE_PERCENT)) ^ g;
        }

    return h;
}

uint TCL_hashSA(uchar *name)
/****************************************************************************
*
* Function:     TCL_hashSA
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  This hash function was taken from Sedgewick's Algorithms
*               textbook.
*
****************************************************************************/
{
    uint    h = *name++;

    while (*name)
        h = (h << 7) + *name++;
    return h;
}

uint TCL_hashGE(uchar *name)
/****************************************************************************
*
* Function:     TCL_hashGE
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  This hash function was taken from Gosling's Emac's.
*
****************************************************************************/
{
    uint    h = 0;

    while (*name)
        h = (h << 5) - h + *name++;
    return h;
}

static uchar permutation[256] = {
     51, 140, 233,  27, 118, 125, 170, 138,
    119, 132, 174,  97,  25, 110,   1,  14,
     65,  36,  40, 188,  73, 173,   7,  30,
     68,  56, 169, 234, 107, 177, 197,  87,
     28, 210, 186,  67,   2,  15, 115,  48,
    223, 148, 211,  57, 190, 104, 213,  49,
    144, 172, 147, 124, 157, 238, 167, 183,
     78,  75,  58,  22,  70, 103, 181,  12,
    254,  41, 198, 168,  46,  79, 241, 156,
     83, 128,  66,  60,  86, 141, 161, 176,
    221,  54, 192, 252, 116,  95, 206,  35,
     88, 133, 154, 250, 237, 253,  85, 178,
     93, 159, 155,  42,   9,  89,   3,  61,
    201, 158, 106,  82, 240, 255, 218, 102,
    189,   8,  33,   4, 145,  16, 150,  26,
     99, 100, 195, 175,  34,  50,  80, 166,
    194, 195, 164,  29, 134, 105,  55, 143,
    122, 130, 245, 208,  72,  77,  64, 121,
    139, 232, 191, 108, 228, 137,  59,  74,
     11, 126, 171,   5, 242, 101, 239, 193,
    112, 113,  98,  21, 207, 225, 151, 251,
     92,  91,  17, 127,  20,  81,  24,   6,
     43, 196, 204, 247, 212, 224, 220,  94,
     32,  13, 187, 199, 214,  18, 226,  84,
     71, 231, 165,  19, 202, 217,  90, 129,
    136, 153, 182, 111, 244,  45, 236, 249,
    109,  47, 180, 205, 215, 160,  53, 162,
    114, 246, 179,  62, 227,  96, 142, 230,
    184, 146, 117,  39,  69,  37,  23,  63,
     52, 216,   0, 135, 149,  31,  38,  44,
    209, 120,  76, 203, 229, 123, 131, 152,
     10, 219, 243, 248, 235, 222, 200, 163,
    };

uchar TCL_hash8(uchar *name)
/****************************************************************************
*
* Function:     TCL_hash8
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  Returns a randomised hash value distributed over an 8 bit
*               number.
*
****************************************************************************/
{
    uchar h;

    if (!*name) return 0;
    h = permutation[*name];
    while (*++name)
        h ^= permutation[h ^ *name];
    return h;
}

ushort TCL_hash16(uchar *name)
/****************************************************************************
*
* Function:     TCL_hash16
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  Returns a randomised hash value distributed over a 16 bit
*               number.
*
****************************************************************************/
{
    uchar   h1,h2;

    if (!*name) return 0;
    h1 = permutation[*name];
    h2 = permutation[*name + 1];
    while (*++name) {
        h1 ^= permutation[h1 ^ *name];
        h2 ^= permutation[h2 ^ *name];
        }
    return (ushort)(((ushort)h1 << 8) | h2);
}

ulong TCL_hash32(uchar *name)
/****************************************************************************
*
* Function:     TCL_hash32
* Parameters:   name    - String to hash
* Returns:      hash value for the string
*
* Description:  Returns a randomised hash value distributed over a 32 bit
*               number.
*
****************************************************************************/
{
    uchar   h1,h2,h3,h4;

    if (!*name) return 0;
    h1 = permutation[*name];
    h2 = permutation[*name + 1];
    h3 = permutation[*name + 2];
    h4 = permutation[*name + 3];
    while (*++name) {
        h1 ^= permutation[h1 ^ *name];
        h2 ^= permutation[h2 ^ *name];
        h3 ^= permutation[h3 ^ *name];
        h4 ^= permutation[h4 ^ *name];
        }
    return (((ulong)h1 << 24) | ((ulong)h2 << 16) | ((ulong)h3 << 8) | h4);
}
