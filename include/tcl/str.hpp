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
* Description:  Header file for a the TCString, TCDynStr and TCSubStr classes.
*               The TCDynStr class is a class for representing dynamically
*               sized strings, and the TCSubStr class is a class for
*               representing finite sub strings that index directly into
*               an allocated dynamic string or a normal C style string.
*
*               To avoid lots of data copying you can set the granularity
*               for a TCDynStr to a large value, say 1k rather than the
*               default 8 bytes. Since the granularity is global to all
*               string's, you should reset the granularity to the previous
*               value when you are finished. ie:
*
*               myfunc()
*               {
*                   uint    old_granularity = TCString::setGranularity(1024);
*
*                   ... do stuff with 1k strings ...
*
*                   TCString::setGranularity(old_granularity);
*               }
*
*               Remember that if you convert a TCSubStr to a (const char *)
*               the string may not necessarily be null terminated, so
*               you need to only look at 'length()' number of characters.
*
*
****************************************************************************/

#ifndef __TCL_STR_HPP
#define __TCL_STR_HPP

#include "scitech.h"
#include <iostream.h>
#include <string.h>
#include <limits.h>
#undef   toupper
#undef   tolower

/*------------------------ Compatability Definitions ----------------------*/

void __tcl_strupr(char *s);
void __tcl_strlwr(char *s);

/*--------------------------- Class Definitions ---------------------------*/

class TCSubStr;
class TCDynStr;

//---------------------------------------------------------------------------
// The TCString class is an abstract base class for the dynamically allocated
// string class and substring class.
//
// The only methods and operations available on the base string class
// are ones that do not modify the internal representation.
//---------------------------------------------------------------------------

class TCString {
protected:
    uint        len;                // Length of the string
    char        *text;              // Pointer to the string data

public:
            // Returns the length of the string
            uint length() const     { return len-1; };

            // Overloaded cast to a (const char *)
            operator const char * () const  { return text; };

            // Return a const char *
            const char * str() const    { return text; };

            // Retrieve the ith character from the string
            char operator [] (uint i) const
                { return i > len ? '\0' : text[i]; };

            // Retrieve the ith character from the string
            char operator [] (int i)
                { return i > (int)len ? '\0' : text[i]; };

            // Equality operator
    friend  int operator == (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) == 0; };
    friend  int operator == (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) == 0; };
    friend  int operator == (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) == 0; };

            // Inequality operator
    friend  int operator != (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) != 0; };
    friend  int operator != (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) != 0; };
    friend  int operator != (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) != 0; };

            // Less than operator
    friend  int operator < (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) < 0; };
    friend  int operator < (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) < 0; };
    friend  int operator < (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) < 0; };

            // Greater than operator
    friend  int operator > (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) > 0; };
    friend  int operator > (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) > 0; };
    friend  int operator > (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) > 0; };

            // Less than or equal to operator
    friend  int operator <= (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) <= 0; };
    friend  int operator <= (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) <= 0; };
    friend  int operator <= (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) <= 0; };

            // Greater than or equal to operator
    friend  int operator >= (const TCString& s1,const TCString& s2)
                { return strncmp(s1,s2,MIN(s1.length(),s2.length())) >= 0; };
    friend  int operator >= (const TCString& s1,const char* s2)
                { return strncmp(s1,s2,s1.length()) >= 0; };
    friend  int operator >= (const char *s1,const TCString& s2)
                { return strncmp(s1,s2,s2.length()) >= 0; };

            // Convert routines for strings
            int asInt() const;
            short asShort() const   { return (short)asInt(); };
            long asLong() const;
            uint asUInt() const;
            ushort asUShort() const { return (ushort)asUInt(); };
            ulong asULong() const;
            float asFloat() const;
            double asDouble() const;

            // Split the string into an array of substrings
            uint split(TCSubStr*& array,const char *fs) const;

            // Compute a hash value for the string
            uint hash() const;

            // Returns true if the string is valid
            ibool valid() const     { return text != NULL; };
    };

//---------------------------------------------------------------------------
// The DynString class is a class for maintaining dynamically sized strings.
// Storage for the string is maintained dynamically on the heap.
//
// Note that the string class is maintained as a 'C' style string
// internally, with an integer count.
//---------------------------------------------------------------------------

class TCDynStr : public TCString {
protected:
    uint        size;               // Amount of memory allocated

    static uint granularity;        // Granularity of memory allocation

            uint computeSize(uint len)
            {
                return ((len + granularity) / granularity) * granularity;
            };

public:
            // Enumerations for the string class
    enum    { left, center, right };

            // Default constructor
            TCDynStr();

            // Copy constructor
            TCDynStr(const TCDynStr& str);

            // Constructor given another string and counts etc
            TCDynStr(const TCString& str,uint pos = 0,uint count = (uint)UINT_MAX);

            // Constructor using a normal C type string
            TCDynStr(const char *cstr,uint pos = 0,uint count = (uint)UINT_MAX);

            // Constructor given a character and a count value
            TCDynStr(char c,uint count = 1U);

            // Assignment operator given a TCString
            TCDynStr& operator = (const TCString& str);

            // Assignment operator given a C type string
            TCDynStr& operator = (const char *cstr);

            // Destructor
            ~TCDynStr();

            // Convert the string to all upper case
            TCDynStr& toupper()     { __tcl_strupr(text); return *this; };

            // Convert the string to all lower case
            TCDynStr& tolower()     { __tcl_strlwr(text); return *this; };

            // Substring deletion method
            TCDynStr& del(uint pos,uint count = (uint)UINT_MAX,
                ibool shrink = true);

            // Substring Insertion method given another string
            TCDynStr& insert(uint pos,const TCString& str);

            // Substring Insertion method given a C type string
            TCDynStr& insert(uint pos,const char *cstr);

            // Character Insertion method
            TCDynStr& insert(uint pos,const char ch);

            // Substring replacement method given another string
            TCDynStr& replace(uint pos,const TCString& str);

            // Substring replacement method given a C type string
            TCDynStr& replace(uint pos,const char *cstr);

            // Character replacement method
            TCDynStr& replace(uint pos,const char ch);

            // Justify the string according to the mode
            TCDynStr& justify(int mode,uint len,ibool clip = false,
                ibool shrink = true);

            // Trim the string of leading/trailing characters
            TCDynStr& trim(int mode = center,const char ch = 0,
                ibool shrink = true);

            //---------------------------------------------------------------
            // Fast efficient operators (mimimum copying).
            //---------------------------------------------------------------

            // Concatenation operators
            TCDynStr& operator += (const TCString& str);
            TCDynStr& operator += (const char *str);
            TCDynStr& operator += (const char);

            // Replicate a string a number of times
            TCDynStr& operator *= (uint count);

            //---------------------------------------------------------------
            // Slower but more flexible operators (lots of construction etc).
            //---------------------------------------------------------------

            // Concatenation operators
    friend  TCDynStr operator + (const TCString& s1,const TCString& s2);
    friend  TCDynStr operator + (const TCString& s1,const char *s2);
    friend  TCDynStr operator + (const char *s1,const TCString& s2);

            // Replicate a string a number of times
    friend  TCDynStr operator * (const TCString& s1,uint count);

            // Shrink the string to the minimum memory allocation
            void shrink();

            // Member functions to set and obtain the granularity
    static  uint setGranularity(uint gran)
                { uint old = granularity; granularity = gran; return old; };
    static  uint getGranularity()
                { return granularity; };

            // Method to dump a TCDynStr to a stream
    friend  ostream& operator << (ostream& o,const TCDynStr& s)
                { return o << (const char *)s; };
    };

//---------------------------------------------------------------------------
// The TCSubStr class is a class for maintaining sub strings that are indexed
// into the storage of an allocated dynamic string, or a c style string.
// No storage is owned by the class, so none is deleted when it is killed.
//
// Thus the class does not allow you to modify the string it represents.
//---------------------------------------------------------------------------

class TCSubStr : public TCString {
public:
            // Default constructor
            TCSubStr();

            // Constructor given another string
            TCSubStr(const TCString& str,uint pos = 0,uint count = (uint)UINT_MAX);

            // Constructor using a normal C type string
            TCSubStr(const char *cstr,uint pos = 0,uint count = (uint)UINT_MAX);

            // Assignment operator given a TCString
            TCSubStr& operator = (const TCString& str);

            // Assignment operator given a C type string
            TCSubStr& operator = (const char *cstr);

            // Overloaded cast to a DynString
            operator TCDynStr () const;

            // Convert to leftmost count characters
            TCSubStr& left(uint count);

            // Convert to middle count characters starting at pos
            TCSubStr& mid(uint pos,uint count);

            // Convert to rightmost count characters
            TCSubStr& right(uint count);

            // Method to dump a TCSubStr to a stream
    friend  ostream& operator << (ostream& o,const TCSubStr& s);
    };

#endif  // __TCL_STR_HPP
