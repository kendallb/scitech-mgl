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
* Description:  Header file for the set of Array clases, implemented using
*               templates. Defines the following classes:
*
*                   TCArray         - Array of T's
*                   TCSArray        - Sortable array of T's
*                   TCIArray        - Indirect array (array of pointers to T)
*                   TCISArray       - Indirect sortable array
*
*                   TCArrayIterator     - Iterator for array's
*                   TCIArrayIterator    - Iterator for indirect array's
*
*               Sortable array's are _not_ maintained in explicit sorted
*               order unless the addSorted() member function is used,
*               or the sort() member function is called. This allows
*               the arrays to be used to get data into sorted order
*               efficiently by filling the array normally and then calling
*               sort().
*
****************************************************************************/

#ifndef __TCL_ARRAY_HPP
#define __TCL_ARRAY_HPP

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __LIMITS_H
#include <limits.h>
#endif

#ifndef __TCL_TECHLIB_HPP
#include "tcl/techlib.hpp"
#endif

/*--------------------------- Class Definition ----------------------------*/

template <class T> class TCArrayIterator;
template <class T> class TCIArrayIterator;

//---------------------------------------------------------------------------
// The following class maintains an array of T's in contiguous memory.
// The array will dynamically adjust it's size depending on how many items
// are stored in it. The class T must have a valid default constructor and
// copy semantics, as well as valid operator == semantics. For efficiency
// reasons, when direct array's are copied, we copy the data with memcpy(),
// rather than invoking the copy constructor for each element. This
// essentially always performs a shallow copy on each of the elements in
// the array, which would have to be manually deepened if a deep copy is
// required.
//
// The resizeable array uses a simple delta technique to expand the size
// of the array to a new possible size. A better technique that guarantees
// amortised O(1) access to array elements no matter how large the array
// needs to be is to use expand the size of the array by a ratio, rather
// than a set delta value.
//---------------------------------------------------------------------------

template <class T> class TCArray {
protected:
    T       *data;              // Pointer to the array of data
    uint    _size,_delta;       // Size and increment values
    uint    count;              // Number of items in array

            // Resize the array to a new size
            void expand(uint newSize);
            void shrink();

            // Member to compute the actual size
            uint computeSize(uint sz)
            {   return (_delta != 0) && (sz % _delta) ?
                    (((sz + _delta) / _delta) * _delta) : sz;
            };

public:
            // Default constructor
            TCArray();

            // Constructor
            TCArray(uint size,uint delta = 0);

            // Destructor
            ~TCArray()  { delete [] data; };

            // Copy constructors
            TCArray(const TCArray<T>& a,uint delta = UINT_MAX);
            const TCArray<T>& operator = (const TCArray<T>& a);

            // Comparision methods for arrays
            ibool operator == (const TCArray<T>& a);
            ibool operator != (const TCArray<T>& a)
                { return !(*this == a); };

            // Indexing operators
            T& operator [] (uint index)
            {   CHECK(data != NULL && index < count);
                return data[index];
            };

            const T& operator [] (uint index) const
            {   CHECK(data != NULL && index < count);
                return data[index];
            };

            // Methods to add elements to the array
            void add(T item);
            void insert(T item,uint index,uint count = 1);

            // Method to replace an element in the array
            void replace(T item,uint index);

            // Method to remove an element from the array
            void remove(uint index);

            // Method to search for an element in the array
            uint search(T item,uint first,uint last,int direction = +1) const;
            uint search(T item,int direction = +1) const
                { return search(item,0,count-1,direction); };

            // Overloaded cast to a const T* and T*
            T* ptr()                { return data; };
            const T* ptr() const    { return data; };

            // Method to set the number of elements in the array (empties it)
            void setCount(uint newCount);

            // Return the number of items in the array
            uint numberOfItems() const  { return count; };

            // Returns true if the array is empty
            ibool isEmpty() const       { return count == 0; };

            // Returns true if the array is full
            ibool isFull() const
                { return (_delta == 0) && (count == _size); };

            // Return the size/delta for the array
            uint size() const           { return _size; };
            uint delta() const          { return _delta; };

            // Modify the delta value for the array
            void setDelta(uint delta);

            // Resize the array to a new size (array is emptied of data)
            void resize(uint newSize);

            // Empties the array of all data
            void empty();

            // Returns true if the array is valid
            ibool valid() const         { return data != NULL; };

private:
    friend  class TCArrayIterator<T>;
    };

//---------------------------------------------------------------------------
// The following class maintains an array of T's in contiguous memory, and
// is derived from the Array class. The items in the array are assumed to
// be sortable, and have valid <, == and > operators. The items are _not_
// maintained in explicit sorted order unless the addSorted() member
// function is used to add items, or the sort() member function is called.
//---------------------------------------------------------------------------

#ifdef  __IBMCPP__
typedef int (_Optlink *_compare_func_t)(const void *t1,const void *t2);
#else
typedef int (*_compare_func_t)(const void *t1,const void *t2);
#endif

template <class T> class TCSArray : public TCArray<T> {
protected:
            // Comparison function for calling qsort().
    static  int cmp(const void *t1,const void *t2);

public:
            // Default constructor
            TCSArray() : TCArray<T>() {};

            // Constructor
            TCSArray(uint size,uint delta = 0)
                : TCArray<T>(size,delta) {};

            // Method to add elements to the array in sorted order
            void addSorted(T item);

            // Method to do a binary search for an item
            uint binarySearch(T item,uint first,uint last) const;
            uint binarySearch(T item) const
                { return binarySearch(item,0,count-1); };

            // Method to sort the elements in the array
            void sort()
                { qsort(data,count,sizeof(T),(_compare_func_t)TCSArray<T>::cmp); };
    };

//---------------------------------------------------------------------------
// The following class maintains an array of pointers to T in contiguous
// memory. The array will dynamically adjust it's size depending on how
// many items are stored in it.
//
// If a copy is made of an array, shouldDelete is set to false so that
// we do not attempt to delete the data twice. You should still however be
// careful about copying indirect array's and freeing the memory used.
//---------------------------------------------------------------------------

template <class T> class TCIArray : public TCArray<T*> {
protected:
    ibool   shouldDelete;           // Flags if elements should be deleted

public:
            // Default constructor
            TCIArray() : TCArray<T*>()
                { shouldDelete = true; };

            // Constructor
            TCIArray(uint size,uint delta = 0,ibool shouldDelete = true)
                : TCArray<T*>(size,delta), shouldDelete(shouldDelete) {};

            // Destructor
            ~TCIArray() { empty(); };

            // Copy constructors
            TCIArray(const TCIArray<T>& a,uint delta = UINT_MAX)
                : TCArray<T*>(a,delta) { shouldDelete = false; };
            const TCIArray<T>& operator = (const TCIArray<T>& a);

            // Comparision methods for indirect arrays
            ibool operator == (const TCIArray<T>& a);
            ibool operator != (const TCIArray<T>& a)
                { return !(*this == a); };

            // Method to replace an element in the array
            void replace(T* item,uint index);

            // Method to destroy an element in the array
            void destroy(uint index);

            // Methods to search for an element in the array
            uint search(const T* item,uint first,uint last,int direction = +1) const;
            uint search(const T* item,int direction = +1) const
                { return search(item,0,count-1,direction); };

            // Method to set the number of elements in the array (empties it)
            void setCount(uint newCount);

            // Resize the array to a new size (array is emptied of data)
            void resize(uint newSize);

            // Empties the array of all data
            void empty();

            // Method to set the shouldDelete value
            void setShouldDelete(ibool s)   { shouldDelete = s; };

private:
    friend  class TCIArrayIterator<T>;
    };

//---------------------------------------------------------------------------
// The following class maintains an array of pointers to T in contiguous
// memory, and is derived from the TCIArray class. The items in the array are
// assumed to be sortable, and have valid <, == and > operators. The items
// are _not_ maintained in explicit sorted order unless the addSorted()
// member function is used to add items, or the sort() member function is
// called.
//---------------------------------------------------------------------------

template <class T> class TCISArray : public TCIArray<T> {
protected:
            // Comparison function for calling qsort().
    static  int cmp(const void *t1,const void *t2);

public:
            // Default constructor
            TCISArray() : TCIArray<T>() {};

            // Constructor
            TCISArray(uint size,uint delta = 0,ibool shouldDelete = true)
                : TCIArray<T>(size,delta,shouldDelete) {};

            // Method to add elements to the array in sorted order
            void addSorted(T* item);

            // Method to do a binary search for an item
            uint binarySearch(const T* item,uint first,uint last) const;
            uint binarySearch(const T* item) const
                { return binarySearch(item,0,count-1); };

            // Method to sort the elements in the array
            void sort()
                { qsort(data,count,sizeof(T*),(_compare_func_t)TCISArray<T>::cmp); };
    };

//---------------------------------------------------------------------------
// The following classes are used to iterate through the elements in the
// array. Generally you can simply do normal array indexing rather than
// using an iterator, but iterators are provided for all the fundamental
// data structures such as linked lists and thus arrays and lists can be
// treated the same through the use of iterators.
//---------------------------------------------------------------------------

template <class T> class TCArrayIterator {
protected:
    const TCArray<T>    *a;
    uint                cursor,lower,upper;

public:
            // Default constructor
            TCArrayIterator()       { a = NULL; cursor = lower = upper = 0; };

            // Constructor given an array reference
            TCArrayIterator(const TCArray<T>& arr)
                { a = &arr; restart(0,arr.numberOfItems()); };

            // Constructor given an array reference and range
            TCArrayIterator(const TCArray<T>& arr,uint start,uint stop)
                { a = &arr; restart(start,stop); };

            // Initialise an array iterator from an array
            const TCArrayIterator<T>& operator = (const TCArray<T>& arr)
            {   a = &arr; restart(0,arr.numberOfItems());
                return *this;
            };

            // Initialise an array iterator from another iterator
            const TCArrayIterator<T>& operator = (const TCArrayIterator<T>& i)
            {   a = i.a; restart(i.lower,i.upper);
                return *this;
            };

            // Overloaded cast to an integer
            operator int ()     { return cursor < upper; };

            // Convert the iterator to the corresponding node
            T node()
            {   CHECK(cursor < upper);
                return (*a)[cursor];
            };

            // Pre-increment operator
            T operator ++ ()
            {   if (++cursor < upper)
                    return (*a)[cursor];
                else
                    return (*a)[upper-1];
            };

            // Post-increment operator
            T operator ++ (int)
            {   if (cursor < upper)
                    return (*a)[cursor++];
                else
                    return (*a)[upper-1];
            };

            // Method to restart the array iterator
            void restart()  { restart(lower,upper); };

            // Method to restart with a new range
            void restart(uint start,uint stop)
                {  cursor = lower = start; upper = stop; };
    };

template <class T> class TCIArrayIterator : public TCArrayIterator<T*> {
public:
            // Default constructor
            TCIArrayIterator() : TCArrayIterator<T*>() {};

            // Constructor given an array reference
            TCIArrayIterator(const TCIArray<T>& arr) : TCArrayIterator<T*>(arr) {};

            // Constructor given an array reference and range
            TCIArrayIterator(const TCIArray<T>& arr,uint start,uint stop)
                : TCArrayIterator<T*>(arr,start,stop) {};

            // Initialise an array iterator from an array
            const TCIArrayIterator<T>& operator = (const TCIArray<T>& arr)
                { return (TCIArrayIterator<T>&)TCArrayIterator<T*>::operator=(arr); };

            // Initialise an array iterator from another iterator
            const TCIArrayIterator<T>& operator = (const TCIArrayIterator<T>& i)
                { return (TCIArrayIterator<T>&)TCArrayIterator<T*>::operator=(i); };
    };

/*------------------------ Inline member functions ------------------------*/

template <class T> inline void TCArray<T>::add(T item)
/****************************************************************************
*
* Function:     TCArray<T>::add
* Parameters:   item    - Item to add to the array
*
* Description:  Resizes the array if the count exceeds the size of the
*               array, and copies the item into the last position.
*
****************************************************************************/
{
    CHECK(valid());
    if (count == _size)
        expand(count+1);
    CHECK(count < _size);
    if (valid())
        data[count++] = item;
}

template <class T> inline void TCArray<T>::setCount(uint newCount)
/****************************************************************************
*
* Function:     TCArray<T>::setCount
* Parameters:   newCount    - New count for the array
*
* Description:  Sets the count value for the array, expanding the size
*               if necessary. The array will be filled with the default
*               value.
*
****************************************************************************/
{
    resize(newCount);
    count = newCount;
}

template <class T> inline void TCArray<T>::replace(T item,uint index)
/****************************************************************************
*
* Function:     TCArray<T>::replace
* Parameters:   item    - Item to replace in the array
*               index   - Index of the item in the array to replace
*
* Description:  Replaces the item in the array with the new item. The index
*               MUST fall within the current bounds of the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(index < count);
    data[index] = item;
}

template <class T> inline void TCIArray<T>::setCount(uint newCount)
/****************************************************************************
*
* Function:     TCIArray<T>::setCount
* Parameters:   newCount    - New count for the array
*
* Description:  Sets the count value for the array, expanding the size
*               if necessary. The array will be filled with the default
*               value.
*
****************************************************************************/
{
    resize(newCount);
    count = newCount;
}

/*---------------------------- Implementation -----------------------------*/

template <class T> TCArray<T>::TCArray()
    : _size(1), _delta(1), count(0)
/****************************************************************************
*
* Function:     TCArray<T>::TCArray
*
* Description:  Default constructor for the array template. This creates
*               an empty array of size 1, with an increment of 1.
*
****************************************************************************/
{
    data = new T[_size];
    CHECK(data != NULL);
}

template <class T> TCArray<T>::TCArray(uint size,uint delta)
    : _delta(delta), count(0)
/****************************************************************************
*
* Function:     TCArray<T>::TCArray
* Parameters:   size    - Initial size of the array
*               delta   - Initial increment value for the array
*
* Description:  Constructor for the array template, given the size of the
*               array to create, and the expansion ratio. If the increment
*               is zero, the array is of a fixed size and will not be
*               expanded.
*
*               The array is initially empty containing no valid data.
*
****************************************************************************/
{
    _size = computeSize(size);
    data = new T[_size];
    CHECK(data != NULL);
}

template <class T> TCArray<T>::TCArray(const TCArray<T>& a,uint delta)
    : count(a.count)
/****************************************************************************
*
* Function:     TCArray<T>::TCArray
* Parameters:   a   - TCArray to copy
*
* Description:  Copy constructor for array's. We simply allocate space for
*               the new array, and copy each item individually. We copy
*               the data with a fast memcpy.
*
****************************************************************************/
{
    CHECK(a.valid());
    _delta = (delta == UINT_MAX) ? a._delta : delta;
    _size = computeSize(a._size);
    data = new T[_size];
    if (valid())
        memcpy(data,a.data,count * sizeof(T));
}

template <class T> const TCArray<T>& TCArray<T>::operator = (const TCArray<T>& a)
/****************************************************************************
*
* Function:     TCArray<T>::operator =
* Parameters:   a   - Array to copy
*
* Description:  Assignment operator for array's. Allocates space for the
*               new array and copies the data with a fast memcpy call.
*
****************************************************************************/
{
    CHECK(valid() && a.valid());
    if (data != a.data) {
        _size = a._size;
        _delta = a._delta;
        count = a.count;
        delete [] data;
        data = new T[_size];
        if (valid())
            memcpy(data,a.data,count * sizeof(T));
        }
    return *this;
}

template <class T> ibool TCArray<T>::operator == (const TCArray<T>& a)
/****************************************************************************
*
* Function:     TCArray<T>::operator ==
* Parameters:   a   - Array to compare to this array
* Returns:      True if the array elements are all equal, false if not.
*
****************************************************************************/
{
    CHECK(valid() && a.valid());
    if (count != a.count)
        return false;
    T *p = data, *pa = a.data;
    for (uint i = 0; i < count; i++)
        if (!(*p++ == *pa++))
            return false;
    return true;
}

template <class T> void TCArray<T>::insert(T item,uint index,uint aCount)
/****************************************************************************
*
* Function:     TCArray<T>::insert
* Parameters:   item    - Item to insert into the array
*               index   - Index to insert the item in front of
*               aCount  - Count of elements to insert
*
* Description:  Inserts the specified item into the array at the index'th
*               position. All the elements from [index,count] are moved
*               up one position to make room. The index must be in the
*               range [0,count], and if the value is count it is simply
*               tacked onto the end of the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(index <= count);
    if (count+aCount > _size)   expand(count+aCount);

    // Move the items up one position in the array to make room, and insert

    if (valid()) {
        memmove(&data[index+aCount],&data[index],(count-index) * sizeof(T));
        for (uint i = 0; i < aCount; i++)
            data[index+i] = item;
        count += aCount;
        }
}

template <class T> void TCArray<T>::remove(uint index)
/****************************************************************************
*
* Function:     TCArray<T>::remove
* Parameters:   index   - Index of element to remove
*
* Description:  Removes an indexed element from the array, by copying all
*               the data down one position. The index must be in the
*               range [0,count).
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(index < count);

    // Move the items down one position, and shrink the allocated memory

    count--;
    memmove(&data[index],&data[index+1],(count-index) * sizeof(T));
    shrink();
}

template <class T> uint TCArray<T>::search(T item,uint first,uint last,
    int direction) const
/****************************************************************************
*
* Function:     TCArray<T>::search
* Parameters:   item        - Item to search for
*               first       - Index of first element to search
*               last        - Index of last element to search
*               direction   - End to search array from (+1 = start, -1 = end)
* Returns:      Index of the item in the array, UINT_MAX if not found
*
* Description:  Performs a simple linear search for the item from the
*               specified end of the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(first < count && last < count);
    CHECK(direction == +1 || direction == -1);
    if (direction == +1) {
        T *p = &data[first];
        for (uint i = first; i <= last; i++)
            if (*p++ == item)
                return i;
        }
    else {
        T *p = &data[last];
        for (uint i = last; i >= first; i--)
            if (*p-- == item)
                return i;
        }
    return UINT_MAX;
}

template <class T> void TCArray<T>::expand(uint newSize)
/****************************************************************************
*
* Function:     TCArray<T>::expand
* Parameters:   newSize - New size of the array
*
* Description:  Expands the array to be a multiple of 'delta' that includes
*               the specified newSize for the array. The array data is
*               re-allocated and copied to the resized array.
*
*               Note that 'count' must contain the actual number of elements
*               in the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(_delta != 0); // Array is not resizeable when _delta == 0
    CHECK(newSize >= count);

    T *temp = new T[_size = computeSize(newSize)];

    // Now copy the data from the old array into the newly resized array.
    // Note that we use a fast memcpy to do this.

    if (temp)
        memcpy(temp,data,count * sizeof(T));
    delete [] data;
    data = temp;
}

template <class T> void TCArray<T>::shrink()
/****************************************************************************
*
* Function:     TCArray<T>::shrink
*
* Description:  Shrinks the allocated space for the array, if the threshold
*               point has been reached.
*
****************************************************************************/
{
    CHECK(valid());
    if (_delta == 0)        // Array is not resizeable when _delta == 0
        return;

    // Only shrink the array when the amount of free space gets smaller
    // than half of the delta value, and it is at least delta in size

    if ((_size - count) > (_delta + _delta/2) && (_size > _delta)) {
        T *temp = new T[_size = computeSize(_size - _delta)];
        if (temp)
            memcpy(temp,data,count * sizeof(T));
        delete [] data;
        data = temp;
        }
}

template <class T> void TCArray<T>::setDelta(uint delta)
/****************************************************************************
*
* Function:     TCArray<T>::setDelta
* Parameters:   delta   - New delta value for the array
*
* Description:  Sets the delta value for the array, expanding or shrinking
*               the array as need be.
*
****************************************************************************/
{
    if (delta >= _delta) {
        _delta = delta;
        expand(_size);
        }
    else {
        _delta = delta;
        shrink();
        }
}

template <class T> void TCArray<T>::resize(uint newSize)
/****************************************************************************
*
* Function:     TCArray<T>::resize
* Parameters:   newSize - New size for the array
*
* Description:  Resizes the array to the new size. If the array is non-
*               resizeable, we bomb out. Note that the array will be empty
*               after this operation.
*
****************************************************************************/
{
    CHECK(_delta != 0);
    empty();
    expand(newSize);
}

template <class T> void TCArray<T>::empty()
/****************************************************************************
*
* Function:     TCArray<T>::empty
*
* Description:  Empties the array of all elements.
*
****************************************************************************/
{
    count = 0;
    shrink();
}

template <class T> int TCSArray<T>::cmp(const void *t1,const void *t2)
/****************************************************************************
*
* Function:     TCSArray<T>::cmp
* Parameters:   t1,t2   - Elements to compare
* Returns:      Result of comparision:
*
*                   t1 < t2,  -1
*                   t1 == t2, 0
*                   t1 > t2,  1
*
****************************************************************************/
{
    if (*((T*)t1) < *((T*)t2))
        return -1;
    else if (*((T*)t1) > *((T*)t2))
        return 1;
    return 0;
}

template <class T> void TCSArray<T>::addSorted(T item)
/****************************************************************************
*
* Function:     TCSArray<T>::addSorted
* Parameters:   item    - Item to add to the array
*
* Description:  Adds the element to the array in sorted order. This
*               function will only work if the elements are already in
*               sorted order, which can be achieved by calling sort().
*
****************************************************************************/
{
    // Search for the spot to put the new item, and insert it into the
    // array.
    uint i;

    CHECK(valid());
    T *p = data;
    for (i = 0; i < count; i++)
        if (*p++ > item)
            break;
    insert(item,i);
}

template <class T> uint TCSArray<T>::binarySearch(T item,uint L,uint R) const
/****************************************************************************
*
* Function:     TCSArray<T>::binarySearch
* Parameters:   item    - Item to search for in the array
*               L       - Index of first element to search
*               R       - Index of last element to search
* Returns:      Index of the item in the array, UINT_MAX if not found
*
* Description:  Performs a standard binary search on the array looking
*               for the specified item. The elements in the array _must_
*               be in sorted order for this function to work (by calling
*               the sort() member function).
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(L < count && R < count);

    while (L < R) {
        uint M = (L+R)/2;
        if (data[M] == item)
            return M;
        if (data[M] < item)
            L = M+1;
        else R = M-1;
        }
    if (data[L] == item)
        return L;
    return UINT_MAX;
}

template <class T> const TCIArray<T>& TCIArray<T>::operator = (const TCIArray<T>& a)
/****************************************************************************
*
* Function:     TCIArray<T>::operator =
* Parameters:   a   - Array to copy
*
* Description:  Assignment operator for TCIArray's. First empties the array
*               the copies it.
*
****************************************************************************/
{
    // Check to make sure we are not being assigned to ourselves :-)

    CHECK(valid() && a.valid());
    if (data != a.data) {
        empty();
        shouldDelete = false;
        TCArray<T*>::operator=(a);
        }
    return *this;
}

template <class T> ibool TCIArray<T>::operator == (const TCIArray<T>& a)
/****************************************************************************
*
* Function:     TCIArray<T>::operator ==
* Parameters:   a   - Array to compare to this array
* Returns:      True if the array elements are all equal, false if not.
*
****************************************************************************/
{
    CHECK(valid() && a.valid());
    if (count != a.count)
        return false;
    T **p = data, **pa = a.data;
    for (uint i = 0; i < count; i++)
        if (!(*(*p++) == *(*pa++)))
            return false;
    return true;
}

template <class T> void TCIArray<T>::replace(T* item,uint index)
/****************************************************************************
*
* Function:     TCIArray<T>::replace
* Parameters:   item    - Item to replace in the array
*               index   - Index of the item in the array to replace
*
* Description:  Replaces the item in the array with the new item. The index
*               MUST fall within the current bounds of the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(index < count);
    if (shouldDelete)
        delete data[index];
    data[index] = item;
}

template <class T> void TCIArray<T>::destroy(uint index)
/****************************************************************************
*
* Function:     TCIArray<T>::destroy
* Parameters:   index   - Index of element to remove
*
* Description:  Removes an indexed element from the array, by copying all
*               the data down one position. The index must be in the
*               range [0,count). If shouldDelete is true, the element is
*               deleted.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(index < count);

    // Move the items down one position, and shrink the allocated memory

    if (shouldDelete)
        delete data[index];
    count--;
    memmove(&data[index],&data[index+1],(count-index) * sizeof(T*));
    shrink();
}

template <class T> uint TCIArray<T>::search(const T* item,uint first,uint last,
    int direction) const
/****************************************************************************
*
* Function:     TCIArray<T>::search
* Parameters:   item        - Item to search for
*               first       - Index of first element to search
*               last        - Index of last element to search
*               direction   - End to search array from (+1 = start, -1 = end)
* Returns:      Index of the item in the array, UINT_MAX if not found
*
* Description:  Performs a simple linear search for the item from the
*               specified end of the array.
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(first < count && last < count);
    CHECK(direction == +1 || direction == -1);
    if (direction == +1) {
        T **p = &data[first];
        for (uint i = first; i <= last; i++)
            if (*(*p++) == *item)
                return i;
        }
    else {
        T **p = &data[last];
        for (uint i = last; i >= first; i--)
            if (*(*p--) == *item)
                return i;
        }
    return UINT_MAX;
}

template <class T> void TCIArray<T>::resize(uint newSize)
/****************************************************************************
*
* Function:     TCIArray<T>::resize
* Parameters:   newSize - New size for the array
*
* Description:  Resizes the array to the new size. If the array is non-
*               resizeable, we bomb out. Note that the array will be empty
*               after this operation.
*
****************************************************************************/
{
    CHECK(_delta != 0);
    empty();
    expand(newSize);
}

template <class T> void TCIArray<T>::empty()
/****************************************************************************
*
* Function:     TCIArray<T>::empty
*
* Description:  Deletes all of the elements if shouldDelete is set to true
*               (the default).
*
****************************************************************************/
{
    if (shouldDelete) {
        for (uint i = 0; i < count; i++)
            delete (T*)data[i];
        }
    TCArray<T*>::empty();
}

template <class T> int TCISArray<T>::cmp(const void *t1,const void *t2)
/****************************************************************************
*
* Function:     TCISArray<T>::cmp
* Parameters:   t1,t2   - Elements to compare
* Returns:      Result of comparision:
*
*                   t1 < t2,  -1
*                   t1 == t2, 0
*                   t1 > t2,  1
*
****************************************************************************/
{
    if (**((T**)t1) < **((T**)t2))
        return -1;
    else if (**((T**)t1) > **((T**)t2))
        return 1;
    return 0;
}

template <class T> void TCISArray<T>::addSorted(T* item)
/****************************************************************************
*
* Function:     TCISArray<T>::addSorted
* Parameters:   item    - Item to add to the array
*
* Description:  Adds the element to the array in sorted order. This
*               function will only work if the elements are already in
*               sorted order, which can be achieved by calling sort().
*
****************************************************************************/
{
    uint    i;

    // Search for the spot to put the new item, and insert it into the
    // array.

    CHECK(valid());
    T **p = data;
    for (i = 0; i < count; i++)
        if (*(*p++) > *item)
            break;
    insert(item,i);
}

template <class T> uint TCISArray<T>::binarySearch(const T* item,uint L,
    uint R) const
/****************************************************************************
*
* Function:     TCISArray<T>::binarySearch
* Parameters:   item    - Item to search for in the array
*               L       - Index of first element to search
*               R       - Index of last element to search
* Returns:      Index of the item in the array, UINT_MAX if not found
*
* Description:  Performs a standard binary search on the array looking
*               for the specified item. The elements in the array _must_
*               be in sorted order for this function to work (by calling
*               the sort() member function).
*
****************************************************************************/
{
    CHECK(valid());
    CHECK(L < count && R < count);

    while (L < R) {
        uint M = (L+R)/2;
        if (*data[M] == *item)
            return M;
        if (*data[M] < *item)
            L = M+1;
        else R = M-1;
        }
    if (*data[L] == *item)
        return L;
    return UINT_MAX;
}

#endif  // __TCL_ARRAY_HPP
