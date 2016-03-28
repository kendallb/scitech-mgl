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
* Description:	Test program for the Array classes.
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

	Array<int>	a(10),b(a),c;

	cout << "Memory after creating array: " << coreleft() << " bytes\n";

	dumpArray(a);
	dumpArray(b);
	c = a;
	dumpArray(c);

	for (i = 0; i < 10; i++)
		a.add(i);

	// Insert a number of elements into the middle of the array

	a.setDelta(10);
	for (i = 100; i < 120; i++)
		a.insert(i,5);

	dumpArray(a);

	// Insert an element at the start and at the end

	a.insert(-1,0);
	a.insert(-2,a.numberOfItems());

	dumpArray(a);

	// Now replace some elements in the array

	a.replace(-3,4);
	a.replace(-3,5);

	a[6] = -4;
	a[7] = -5;

	dumpArray(a);

	// Now remove some elements from the array

	for (i = 0; i < 7; i++)
		a.remove(4);
	dumpArray(a);

	a.remove(10);
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

	a.empty();
	dumpArray(a);

	cout << "Memory at end: " << coreleft() << " bytes\n\n";
}
