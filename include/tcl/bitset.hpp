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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Header file for the BitSet class, a class designed
*               for representing bit valued sets of arbitrary size.
*
****************************************************************************/

#ifndef __TCL_BITSET_HPP
#define __TCL_BITSET_HPP

#ifndef __TCL_TECHLIB_HPP
#include "tcl/techlib.hpp"
#endif

#ifndef __MEM_H
#include <mem.h>
#endif

/*--------------------- Macros and type definitions -----------------------*/

// The following defines the smallest element in the set. By default this
// is an unsigned short, but you may change it to be an unsigned long for
// better performance on 32 bit machines.

#ifdef  _LONG_ELEMS

typedef unsigned long       _SETTYPE;
#define _BITS_IN_WORD       32
#define _BYTES_IN_ARRAY(x)  ((x) << 2)      // # of bytes in bit map
#define _DIV_WSIZE(x)       ((uint)(x) >> 5)
#define _MOD_WSIZE(x)       ((x) & 0x1F)

#else

typedef unsigned short      _SETTYPE;       // one cell in bit map
#define _BITS_IN_WORD       16
#define _BYTES_IN_ARRAY(x)  ((x) << 1)      // # of bytes in bit map
#define _DIV_WSIZE(x)       ((uint)(x) >> 4)
#define _MOD_WSIZE(x)       ((x) & 0x0F)

#endif

// Macro to modify the appropriate bit in the bitmap

#define _GBIT(m,b,op)   ( ((m)[_DIV_WSIZE(b)] op (1 << _MOD_WSIZE(b)) )

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The following template class defines a fixed bit set class, which is
// used for manipulating small sets of numbers that are not required to
// change in size over time.
//---------------------------------------------------------------------------

template <int nbits = 128> class TCFixedBitSet {
protected:
    ibool       compl;          // True if set is complemented
    _SETTYPE    defmap[nbits / _BITS_IN_WORD];

            uint inline defBits()   { return nbits; };
            uint inline defWords()  { return nbits / _BITS_IN_WORD; };
            uint inline round(uint bit)
                { return (((_DIV_WSIZE(bit) + 8) >> 3) << 3); };

    enum { UNION, INTERSECT, DIFFERENCE };

            // Method to perform above operations on sets
            void _set_op(const TCFixedBitSet<nbits>& bs1,
                const TCFixedBitSet<nbits>& bs2,int op);

public:
    enum { Equivalent, Disjoint, Intersect };

            // Constructor
            TCFixedBitSet();

            // Methods to add and remove elements
            void add(int bit);
            void remove(int bit);

            // Methods to test for membership in the set
            ibool member(int bit);
            ibool test(int bit) { return member(bit) ? !compl : compl; };

            // Methods to determine equivalence of sets
            int testSet(const TCFixedBitSet<nbits>& bs);
            ibool isDisjoint(const TCFixedBitSet<nbits>& bs)
                { return testSet(bs) == Disjoint; };
            ibool isIntersecting(const TCFixedBitSet<nbits>& bs)
                { return testSet(bs) == Intersect; };
            ibool isEquivalent(const TCFixedBitSet<nbits>& bs)
                { return testSet(bs) == Equivalent; };
            ibool isSubset(const TCFixedBitSet<nbits>& bs);

            // Methods to compute set union, intersection and difference
            void setUnion(const TCFixedBitSet<nbits>& bs1,
                const TCFixedBitSet<nbits>& bs2) { _set_op(bs1,bs2,UNION); }
            void setIntersect(const TCFixedBitSet<nbits>& bs1,
                const TCFixedBitSet<nbits>& bs2) { _set_op(bs1,bs2,INTERSECT); };
            void setDifference(const TCFixedBitSet<nbits>& bs1,
                const TCFixedBitSet<nbits>& bs2) { _set_op(bs1,bs2,DIFFERENCE); };

            // Method to compare two sets
            int cmp(const TCFixedBitSet<nbits>& bs);

            // Return the cardinality of the set
            int numElements();

            // Miscellaneous methods
            void clear();
            void fill();
            void complement()   { compl = !compl; };
            void invert();
            ibool isEmpty() { return numElements() == 0; };

            // Method to print the set!!

            // operator <
            // operator <=
            // operator ==  for calls to set comparision function
            // operator >=
            // operator >

            // operator +=  add a member to the set
            // operator -=  remove a member from the set
            // operator |   test for set membership
            // operator ~   invert a set
            // operator +   union of two sets
            // operator -   difference of two sets
            // operator &   intersection of two sets

    };

// Need to include iterators for the sets to iterate over the elements in
// the set!!!

//---------------------------------------------------------------------------
// The following defines a general bit set class, for representing bit
// set of arbitrary size that expands and shrinks to fit the required
// allocation.
//---------------------------------------------------------------------------

class TCBitSet : public TCFixedBitSet<128> {
protected:
public:
    };

//---------------------------------------------------------------------------
// The following defines a character set class, for reprensenting sets of
// available characters.
//---------------------------------------------------------------------------

class TCCharSet : public TCFixedBitSet<255> {
protected:
public:
    };

/*------------------------ Inline member functions ------------------------*/

template <int nbits> inline TCFixedBitSet<nbits>::TCFixedBitSet()
/****************************************************************************
*
* Function:     TCFixedBitSet<nbits>::TCFixedBitSet
*
* Description:  Default constructor for the TCFixedBitSet class. This sets
*               up an empty set.
*
****************************************************************************/
{
    memset(defmap,0,sizeof(defmap));    // Clear out the set
    compl = false;
}

template <int nbits> void TCFixedBitSet<nbits>::add(int bit)
/****************************************************************************
*
* Function:     TCFixedBitSet<nbits>::add
* Parameters:   bit - Bit to add to the set
*
* Description:  Adds a bit to the set.
*
****************************************************************************/
{
    CHECK(bit < nbits);
    _GBIT(defmap,bit,|=);
}

template <int nbits> void TCFixedBitSet<nbits>::remove(int bit)
/****************************************************************************
*
* Function:     TCFixedBitSet<nbits>::remove
* Parameters:   bit - Bit to remove from the set if present
*
* Description:  Adds a bit to the set.
*
****************************************************************************/
{
    CHECK(bit < nbits);
    _GBIT(defmap,bit,&= ~);
}

template <int nbits> ibool TCFixedBitSet<nbits>::member(int bit)
/****************************************************************************
*
* Function:     TCFixedBitSet<nbits>::member
* Parameters:   bit - Bit to test for membership in set
* Returns:      True if bit is a member, false if not.
*
****************************************************************************/
{
    CHECK(bit < nbits);
    return _GBIT(defmap,bit,&);
}

#endif  // __TCL_BITSET_HPP
