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
* Description:  Member functions for the TCDynStr class, a class for
*               representing dynamically sized strings.
*
****************************************************************************/

#include <limits.h>
#include "tcl/str.hpp"
#include "tcl/techlib.hpp"

//------------------------- Global initialisations ------------------------//

uint    TCDynStr::granularity = 8;          // Default granularity

//----------------------------- Member functions --------------------------//

void TCDynStr::shrink()
/****************************************************************************
*
* Function:     TCDynStr::shrink
*
* Description:  Shrinks the size of the allocated memory down to the
*               smallest possible amount.
*
****************************************************************************/
{
    char    *temp;

    CHECK(valid());
    if ((size - len) > granularity) {

        // Determine the smallest possible size (in increments of
        // granularity and allocate the new memory

        size = computeSize(len);
        temp = new char[size];

        // Copy the text and delete the old text

        if (temp)
            memcpy(temp,text,len);
        delete [] text;
        text = temp;
        }
}

TCDynStr::TCDynStr()
/****************************************************************************
*
* Function:     TCDynStr::TCDynStr
*
* Description:  Constructor given no parameters. All we do here is allocate
*               the smallest amount of space possible, which is equal to
*               the current setting of the granularity, and set the value
*               of the string to empty.
*
****************************************************************************/
{
    len = 1;                    // Length includes the null terminator!
    size = granularity;
    if ((text = new char[size]) != NULL)
        text[0] = '\0';         // Null terminate the string
}

TCDynStr::TCDynStr(const TCDynStr& str)
/****************************************************************************
*
* Function:     TCDynStr::TCDynStr
* Parameters:   str     - TCString to copy from
*
* Description:  Standard copy constructor. This is required for some
*               routines to work (the compiler gets confused about the next
*               constructor).
*
****************************************************************************/
{
    CHECK(str.valid());
    len = str.len;
    size = str.size;
    if ((text = new char[size]) != NULL)
        memcpy(text,str.text,len);
}

TCDynStr::TCDynStr(const TCString& str,uint pos,uint count)
/****************************************************************************
*
* Function:     TCDynStr::TCDynStr
* Parameters:   str     - TCString to copy from
*               pos     - Starting position in the string
*               count   - Number of characters to copy
*
* Description:  Constructs a string from another string, starting at the
*               position 'pos' and including 'count' characters.
*
****************************************************************************/
{
    CHECK(str.valid());
    if (pos > str.length())
        pos = str.length();
    if (count > str.length() - pos)
        count = str.length() - pos;
    len = count+1;
    size = computeSize(len);
    if ((text = new char[size]) != NULL) {
        memcpy(text,(const char *)str+pos,len);
        text[count] = '\0';             // Null terminate the string
        }
}

TCDynStr::TCDynStr(const char *cstr,uint pos,uint count)
/****************************************************************************
*
* Function:     TCDynStr::TCDynStr
* Parameters:   cstr    - C style string to copy from
*               pos     - Starting position in the string
*               count   - Number of characters to copy
*
* Description:  Constructs a string from a C string, starting at the
*               position 'pos' and including 'count' characters.
*
****************************************************************************/
{
    CHECK(cstr != NULL);
    len = strlen(cstr) + 1;
    if (pos >= len)
        pos = len-1;
    if (count >= len - pos)
        count = len-1 - pos;
    len = count+1;
    size = computeSize(len);
    if ((text = new char[size]) != NULL) {
        memcpy(text,cstr+pos,count);
        text[count] = '\0';             // Null terminate the string
        }
}

TCDynStr::TCDynStr(char ch,uint count)
/****************************************************************************
*
* Function:     TCDynStr::TCDynStr
* Parameters:   ch      - Character to fill with
*               count   - Number of characters to fill
*
* Description:  Constructs the string with 'count' replications of 'ch'.
*
****************************************************************************/
{
    len = count + 1;
    size = computeSize(len);
    if ((text = new char[size]) != NULL) {
        memset(text,ch,count);
        text[len-1] = '\0';
        }
}

TCDynStr::~TCDynStr()
/****************************************************************************
*
* Function:     TCDynStr::~TCDynStr
*
* Description:  Destructor for the TCString class. Simply delete the memory
*               allocated to the string.
*
****************************************************************************/
{
    delete [] text;
}

TCDynStr& TCDynStr::operator = (const TCString& str)
/****************************************************************************
*
* Function:     TCDynStr::operator =
* Parameters:   str - TCString to assign from
* Returns:      Reference to the newly created string.
*
* Description:  Assignment operator given a string. We delete the old text
*               and then create a new string.
*
****************************************************************************/
{
    // If the amount of memory required for the new string is the same as
    // for the old string, we do not need to delete the memory allocated
    // to the text!

    CHECK(valid() && str.valid());
    uint newsize = computeSize(str.length()+1);
    char *temp = text;
    if (size != newsize)
        temp = new char[newsize];

    if (temp) {
        len = str.length()+1;
        memcpy(temp,str,len);
        if (size != newsize) {
            size = newsize;
            delete [] text;
            text = temp;
            }
        text[len-1] = '\0';                 // Null Terminate string
        }

    return *this;
}

TCDynStr& TCDynStr::operator = (const char *cstr)
/****************************************************************************
*
* Function:     TCDynStr::operator =
* Parameters:   cstr    - C style string to assign
* Returns:      Reference to the newly allocated string.
*
* Description:  Assignment operator given a C style string. We delete the
*               old text and copy the C string into a new string.
*
****************************************************************************/
{
    CHECK(valid() && cstr != NULL);
    return *this = TCSubStr(cstr);
}

TCDynStr& TCDynStr::operator += (const TCString& str)
/****************************************************************************
*
* Function:     TCDynStr::operator +=
* Parameters:   str - TCString to concatenate
* Returns:      Reference to the concatenated string.
*
* Description:  Concatenation operator. Here we copy the second string onto
*               the end of the first string, so no new string is created.
*               We may need to reallocate the memory though.
*
****************************************************************************/
{
    uint    newsize,newlen;
    char    *temp;

    CHECK(valid() && str.valid());
    CHECK((ulong)(len + str.length()) < UINT_MAX);

    newlen = length() + str.length() + 1;
    newsize = computeSize(newlen);

    // If the amount of memory already allocated for the string is enough
    // we do not need to re-allocate the memory!

    if (newsize != size) {
        size = newsize;
        if ((temp = new char[size]) != NULL) {
            memcpy(temp,text,len-1);
            memcpy(&temp[len-1],str,str.length()+1);
            }
        delete [] text;
        text = temp;
        }
    else
        memcpy(&text[len-1],str,str.length()+1);

    if (valid()) {
        len = newlen;
        text[len-1] = '\0';             // Null terminate the string
        }
    return *this;
}

TCDynStr& TCDynStr::operator += (const char* cstr)
/****************************************************************************
*
* Function:     TCDynStr::operator +=
* Parameters:   cstr    - TCString to concatenate
* Returns:      Reference to the concatenated string.
*
* Description:  Concatenation operator. Here we copy the second string onto
*               the end of the first string, so no new string is created.
*               We may need to reallocate the memory though.
*
****************************************************************************/
{
    CHECK(valid() && cstr != NULL);
    return *this += TCSubStr(cstr);
}

TCDynStr& TCDynStr::operator += (const char ch)
/****************************************************************************
*
* Function:     TCDynStr::operator +=
* Parameters:   ch  - Character to concatenate.
* Returns:      Reference to the concatenated string.
*
****************************************************************************/
{
    char    temp[2];

    temp[0] = ch;
    temp[1] = '\0';
    CHECK(valid());
    return *this += TCSubStr(temp,0,1);
}

TCDynStr& TCDynStr::operator *= (uint count)
/****************************************************************************
*
* Function:     TCDynStr::operator *=
* Parameters:   count   - Number of times to replicate string
* Returns:      Reference to the replicated string.
*
* Description:  Replicates a string 'count' times.
*
****************************************************************************/
{
    uint    newsize,newlen;
    char    *temp,*p;

    CHECK((ulong)(length() + length()*(count-1) + 1) < UINT_MAX);

    newlen = length() + length()*(count-1) + 1;
    newsize = computeSize(newlen);

    // If the amount of memory already allocated for the string is enough
    // we do not need to re-allocate the memory!

    if (newsize != size) {
        if ((temp = new char[newsize]) != NULL)
            memcpy(temp,text,length());
        }
    else
        temp = text;

    if (temp) {
        p = temp + length();
        for (uint i = 1; i < count; i++,p += length())
            memcpy(p,temp,length());
        temp[newlen-1] = '\0';
        }

    if (newsize != size) {
        size = newsize;
        delete [] text;
        text = temp;
        }

    len = newlen;
    return *this;
}

// Slower concatenation/replication routines in terms of the faster ones

TCDynStr operator + (const TCString& s1,const TCString& s2)
{
    CHECK(s1.valid() && s2.valid());
    TCDynStr    s(s1);
    return s += s2;
}

TCDynStr operator + (const TCString& s1,const char *s2)
{
    CHECK(s1.valid() && s2 != NULL);
    TCDynStr    s(s1);
    return s += s2;
}

TCDynStr operator + (const char *s1,const TCString& s2)
{
    CHECK(s1 != NULL && s2.valid());
    TCDynStr    s(s1);
    return s += s2;
}

TCDynStr operator * (const TCString& s1,uint count)
{
    CHECK(s1.valid());
    TCDynStr    s(s1);
    return s *= count;
}

TCDynStr& TCDynStr::del(uint pos,uint count,ibool shrinkIt)
/****************************************************************************
*
* Function:     TCDynStr::del
* Parameters:   pos         - Position to delete from
*               count       - Number of characters to delete
*               shrinkIt    - True if string should be shrunk to minimum
* Returns:      Reference to the new string.
*
* Description:  Deletes a substring from the string, given the position
*               and number of characters to delete.
*
****************************************************************************/
{
    ulong   copyfrom;

    CHECK(valid());
    if (pos >= length() || count == 0)
        return *this;

    copyfrom = (ulong)pos + (ulong)count;

    if (copyfrom >= length()) {
        text[pos] = '\0';           // Just null terminate the string
        len = pos+1;
        }
    else {
        memmove(&text[pos],&text[copyfrom],(size_t)(len-copyfrom));
        len -= count;
        }

    if (shrinkIt)
        shrink();                   // Shrink the size of the string

    return *this;
}

TCDynStr& TCDynStr::insert(uint pos,const char ch)
/****************************************************************************
*
* Function:     TCDynStr::insert
* Parameters:   pos - Character position to insert before
*               ch  - Character to insert
* Returns:      Reference to the new string.
*
* Description:  Inserts a character into the string before the character at
*               position 'pos'. If 'pos' is set to the length of the string,
*               the string is simply concatenated onto the end.
*
****************************************************************************/
{
    char    *temp;

    CHECK(valid());
    if (pos > length())
        return *this;

    // Expand the size of the string if necessary

    if (len == size) {
        size += granularity;
        if ((temp = new char[size]) != NULL)
            memcpy(temp,text,len);
        delete [] text;
        text = temp;
        }

    // Move the text and insert the character

    if (valid()) {
        memmove(&text[pos+1],&text[pos],len-pos);
        text[pos] = ch;
        len++;
        text[len-1] = '\0';         // Null terminate the expanded string
        }

    return *this;
}

TCDynStr& TCDynStr::insert(uint pos,const TCString& str)
/****************************************************************************
*
* Function:     TCDynStr::insert
* Parameters:   pos - Character position to insert before
*               str - TCString to insert
* Returns:      Reference to the new string.
*
* Description:  Inserts a string into the string before the character at
*               position 'pos'. If 'pos' is set to the length of the string,
*               the string is simply concatenated onto the end.
*
****************************************************************************/
{
    char    *temp;
    uint    newlen,newsize;

    CHECK(valid());
    if (pos > length())
        return *this;

    // Expand the size of the string if necessary

    newlen = length() + str.length() + 1;
    newsize = ((newlen + granularity) / granularity) * granularity;

    if (newsize != size) {
        size = newsize;
        if ((temp = new char[size]) != NULL)
            memcpy(temp,text,len);
        delete [] text;
        text = temp;
        }

    // Move the text and insert the new string

    if (valid()) {
        memmove(&text[pos+str.length()],&text[pos],len-pos);
        memcpy(&text[pos],str,str.length());
        len = newlen;
        text[len-1] = '\0';         // Null terminate the expanded string
        }

    return *this;
}

TCDynStr& TCDynStr::insert(uint pos,const char *cstr)
/****************************************************************************
*
* Function:     TCDynStr::insert
* Parameters:   pos     - Character position to insert before
*               cstr    - C style string to insert
* Returns:      Reference to the new string.
*
* Description:  Inserts a string into the string before the character at
*               position 'pos'.
*
****************************************************************************/
{
    return insert(pos,TCSubStr(cstr));
}

TCDynStr& TCDynStr::replace(uint pos,const char ch)
/****************************************************************************
*
* Function:     TCDynStr::replace
* Parameters:   pos - Position to replace character at
*               ch  - Character to replace with
* Returns:      Reference to the new string
*
* Description:
*
****************************************************************************/
{
    char    *temp;

    CHECK(valid());
    if (pos < length())
        text[pos] = ch;
    else if (pos == length()) {

        // Expand the size of the string if necessary

        if (len == size) {
            size += granularity;
            if ((temp = new char[size]) != NULL)
                memcpy(temp,text,len);
            delete [] text;
            text = temp;
            }

        // Insert the character

        if (valid()) {
            text[pos] = ch;
            len++;
            text[len-1] = '\0';     // Null terminate the expanded string
            }
        }
    return *this;
}

TCDynStr& TCDynStr::replace(uint pos,const TCString& str)
/****************************************************************************
*
* Function:     TCDynStr::replace
* Parameters:   pos - Position to replace string at
*               str - TCString to replace with
* Returns:      Reference to the new string.
*
* Description:  Overwrites the string 'str' with the new string. If the new
*               string runs off the end of the old string, this string
*               will be re-sized to fit.
*
*               If pos is set to the length of the string, it will simply
*               be concatenated onto the end of the string.
*
****************************************************************************/
{
    char    *temp;
    uint    newlen,newsize;

    CHECK(valid());
    if (pos > length())
        return *this;

    // Expand the size of the string if necessary

    newlen = pos + str.length() + 1;
    newlen = MAX(len,newlen);
    newsize = ((newlen + granularity) / granularity) * granularity;

    if (newsize != size) {
        size = newsize;
        if ((temp = new char[size]) != NULL)
            memcpy(temp,text,len);
        delete [] text;
        text = temp;
        if (valid())
            text[newlen-1] = '\0';      // Null terminate the expanded string
        }

    // Overwrite the new text in the string

    if (valid()) {
        memcpy(&text[pos],str,str.length());
        len = newlen;
        }

    return *this;
}

TCDynStr& TCDynStr::replace(uint pos,const char *cstr)
/****************************************************************************
*
* Function:     TCDynStr::replace
* Parameters:   pos     - Position to replace string at
*               cstr    - TCString to replace with
* Returns:      Reference to the new string.
*
* Description:  Overwrites the string 'str' with the new string. If the new
*               string runs off the end of the old string, this string
*               will be re-sized to fit.
*
*               If pos is set to the length of the string, it will simply
*               be concatenated onto the end of the string.
*
****************************************************************************/
{
    CHECK(valid());
    return replace(pos,TCSubStr(cstr));
}

TCDynStr& TCDynStr::justify(int mode,uint count,ibool clip,ibool shrinkIt)
/****************************************************************************
*
* Function:     TCDynStr::justify
* Parameters:   mode        - Justification mode (left,center,right)
*               len         - Length of space to justify within
*               clip        - Clip to justification space?
*               shrinkIt    - True if the string should be shrunk
* Returns:      Reference to the newly allocated string.
*
* Description:
*
****************************************************************************/
{
    CHECK(valid());
    trim(center,0,false);

    if (!clip && len > count) {             // Check for out of bounds
        if (shrinkIt)
            shrink();
        return *this;
        }

    if (clip && len >= count) {             // Clip the string to bounds
        TCSubStr    s(*this);

        switch (mode) {
            case left:
                *this = s.left(count);
                break;
            case center:
                *this = s.mid((length()-count)/2,count);
                break;
            case right:
                *this = s.right(count);
                break;
            }
        }
    else {
        switch (mode) {
            case left:
                *this += TCDynStr(' ',count - length());
                break;
            case center:
                *this = TCDynStr(' ',(count - length())/2)
                    + *this
                    + TCDynStr(' ',count - (count+length())/2);
                break;
            case right:
                *this = TCDynStr(' ',count - length()) + *this;
                break;
            }
        }

    return *this;
}

TCDynStr& TCDynStr::trim(int mode,const char ch,ibool shrink)
/****************************************************************************
*
* Function:     TCDynStr::trim
* Parameters:   mode    - Trimming mode (left,center,right)
*               ch      - Character's to trim (0 = whitespace).
* Returns:      Reference to the trimmed string.
*
* Description:  Trims the string of leading and/or trailing characters. If
*               ch is set to 0, we trim whitespace as defined by the
*               standard 'isspace' function.
*
****************************************************************************/
{
    int     i;
    char    *whitespace;
    char    p[2];

    CHECK(valid());
    if (ch != 0) {
        whitespace = p;
        p[0] = ch;
        p[1] = '\0';
        }
    else whitespace = " \t\n\v\f\r";

    if (mode == left || mode == center)
        del(0,strspn(text,whitespace),false);

    if (mode == right || mode == center) {
        i = len-1;
        while (strchr(whitespace,text[i]) != NULL)
            i--;
        del(i+1,(len-i-1),false);
        }

    if (shrink)
        this->shrink();

    return *this;
}

