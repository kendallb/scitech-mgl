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
* Description:	Test program for the deque class.
*
****************************************************************************/

#include "tcl\deque.hpp"
#include <iostream.h>
#include <malloc.h>
#include <stdlib.h>

class MyNode : public TCDListNode {
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


TCDeque<MyNode>	q;

void dumpQueue(TCDeque<MyNode>& q)
{
	TCDListIterator<MyNode>	i;

	if (!q.isEmpty()) {

		// Dump some details about the queue

		cout << "First Item: " << q.peek()->id() << endl;
		cout << "Left Item: " << q.peekLeft()->id() << endl;
		cout << "Right Item: " << q.peekRight()->id() << endl;

		cout << "Contents of queue (" << q.numberOfItems() << " items):";
		cout << endl << "    ";

		for (i = (TCDList<MyNode>&)q; i; i++)
			cout << i.node()->id() << " ";
		}
	else
		cout << "Empty queue (" << q.numberOfItems() << ")";

	cout << endl << endl;
}

void main(void)
{
	int		i;
	MyNode	*n;

	cout << "Memory at start: " << coreleft() << " bytes\n";

	// Create a queue of 30 nodes.

	for (i = 0; i < 10; i++) {
		n = new MyNode(i);
		q.put(n);
		}

	for (i = 10; i < 20; i++) {
		n = new MyNode(i);
		q.putLeft(n);
		}

	for (i = 20; i < 30; i++) {
		n = new MyNode(i);
		q.putRight(n);
		}

	cout << "Memory after creating queue: " << coreleft() << " bytes\n";

	dumpQueue(q);

	// Kill the queue and display it

	q.empty();
	dumpQueue(q);

	// Create a queue of 10 nodes.

	for (i = 0; i < 10; i++) {
		n = new MyNode(i);
		q.put(n);
		}

	dumpQueue(q);

	// Remove all the nodes from the queue with get()

	while ((n = q.get()) != NULL) {
		cout << "Removed " << n->id() << endl;
		delete n;
		}

	dumpQueue(q);

	cout << "Memory at end: " << coreleft() << " bytes\n";
}
