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
* Description:	Test program for the CList list class.
*
****************************************************************************/

#include "clist.hpp"
#include <iostream.h>
#include <malloc.h>
#include <stdlib.h>

class MyNode : public ListNode {
	int		_id;
public:
			MyNode(int id) 	{ _id = id; };
			int id(void)	{ return _id; };
	virtual	~MyNode();
	};

MyNode::~MyNode()
{
//	cout << "Deleting " << _id << endl;
}

List<MyNode>	l;

void dumpList(List<MyNode>& l)
{
	MyNode	*n;

	if (!l.isEmpty()) {

		// Dump some details about the list

		cout << "Head Node: " << l.peekHead()->id() << endl;

		cout << "Contents of list (" << l.numberOfItems() << " items):";
		cout << endl << "    ";

		for (n = l.peekHead(); n; n = l.next(n))
			cout << n->id() << " ";
		}
	else
		cout << "Empty list (" << l.numberOfItems() << ")";

	cout << endl << endl;
}

int myCmp(MyNode *n1,MyNode *n2)
{
	return n1->id() - n2->id();
}

void main(void)
{
	int		i,count;
	MyNode	*n1,*middle,*beforeMiddle,*last;

	cout << "Memory at start: " << coreleft() << " bytes\n";

	// Create a linked list of 10 nodes adding at the head of the list.

	for (i = 0; i < 10; i++) {
		n1 = new MyNode(i);
		l.addToHead(n1);
		if (i == 0)
			last = n1;
		if (i == 5)
			beforeMiddle = n1;
		if (i == 4)
			middle = n1;
		}

	cout << "Memory after creating list: " << coreleft() << " bytes\n";

	dumpList(l);

	// Add a new node after the head of the list and after a middle node

	n1 = new MyNode(20);
	l.addAfter(n1,l.peekHead());
	n1 = new MyNode(21);
	l.addAfter(n1,last);
	n1 = new MyNode(22);
	l.addAfter(n1,middle);

	dumpList(l);

	// Remove the middle object from the list using beforeMiddle as the
	// previous node in the list.

	delete l.removeNext(beforeMiddle);
	dumpList(l);

	// Remove all remaining nodes with removeFromHead().

	count = l.numberOfItems();
	for (i = 0; i < count+5; i++)
		delete l.removeFromHead();

	dumpList(l);

	cout << "Memory: " << coreleft() << " bytes\n";

	for (i = 0; i < 10; i++) {
		n1 = new MyNode(i);
		l.addToHead(n1);
		}

	// Display the list using iterators to move through the list.

	ListIterator<MyNode> it1;

	for (it1 = l; it1; it1++)
		cout << it1.node()->id() << " ";
	cout << endl;

	for (it1.restart(); it1;)
		cout << it1++->id() << " ";
	cout << endl;

	l.empty();

	l.empty();

	// Create a list of 100 random numbers, and sort them

	srand(time(NULL));
	for (i = 0; i < 100; i++) {
		n1 = new MyNode(rand());
		l.addToHead(n1);
		}

	dumpList(l);

	l.sort(myCmp);

	dumpList(l);

	l.empty();

	cout << "Memory at end: " << coreleft() << " bytes\n\n";
}
