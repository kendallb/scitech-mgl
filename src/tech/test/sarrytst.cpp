/****************************************************************************
*
*						  Techniques Class Library
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
* Language:		C++ 3.0
* Environment:	any
*
* Description:	Test program for the SArray class.
*
****************************************************************************/

#include "array.hpp"
#include <malloc.h>
#include <iostream.h>

void dumpArray(Array<int>& a)
{
	if (!a.isEmpty()) {

		// Dump some details about the array

		cout << "Contents of array (" << a.numberOfItems() << " elements,"
			 << a.size() << " size, " << a.delta() << " delta):";
		cout << endl << "    ";

		for (int i = 0; i < a.numberOfItems(); i++)
			cout << a[i] << " ";
		if (a.isFull())
			cout << "Array is full!" << endl;
		}
	else
		cout << "Empty array (" << a.numberOfItems() << ")";

	cout << endl << endl;
}

void main(void)
{
	int		i;

	cout << endl << endl;

	cout << "Memory at start: " << coreleft() << " bytes\n";

	SArray<int>	a(10),b(a),c;

	// Create an array of 10 elements

	for (i = 10; i > 1; i--) {
		a.add(i);
		}

	cout << "Memory after creating array: " << coreleft() << " bytes\n";

	dumpArray(a);
	dumpArray(b);

	// Sort the array a and copy it to c

	a.sort();
	c = a;
	dumpArray(c);

	// Insert a number of elements in sorted order

	a.setDelta(10);
	for (i = 100; i < 120; i++) {
		a.addSorted(i);
		}

	dumpArray(a);

	// Display the array using iterators.

	ArrayIterator<int> it1;

	for (it1 = a; it1; it1++)
		cout << it1.node() << " ";
	cout << endl;

	for (it1.restart(); it1;)
		cout << it1++ << " ";
	cout << endl;

	for (it1.restart(10,20); it1;)
		cout << ++it1 << " ";
	cout << endl;

	// Search for an item in the array

	cout << "Index of element 110 (linear search): " << a.search(110) << endl;
	cout << "                     (binary search): " << a.binarySearch(110) << endl;

	cout << "Index of element 200 (linear search): " << a.search(200) << endl;

	cout << "                     (binary search): " << a.binarySearch(200) << endl;

	cout << "Memory at end: " << coreleft() << " bytes\n\n";
}
