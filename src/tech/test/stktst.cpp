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
* Description:	Test program for the stack class.
*
****************************************************************************/

#include "tcl\stack.hpp"
#include <iostream.h>
#include <malloc.h>
#include <stdlib.h>

class MyNode : public TCListNode {
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

TCStack<MyNode>	s;

void dumpStack(TCStack<MyNode>& s)
{
	TCListIterator<MyNode>	i;

	if (!s.isEmpty()) {

		// Dump some details about the stack

		cout << "Top Item: " << s.top()->id() << endl;

		cout << "Contents of stack (" << s.numberOfItems() << " items):";
		cout << endl << "    ";

		for (i = (TCList<MyNode>&)s; i; i++)
			cout << i.node()->id() << " ";
		}
	else
		cout << "Empty stack (" << s.numberOfItems() << ")";

	cout << endl << endl;
}

void main(void)
{
	int		i;
	MyNode	*n;

	cout << "Memory at start: " << coreleft() << " bytes\n";

	// Create a stack of 10 nodes.

	for (i = 0; i < 10; i++) {
		n = new MyNode(i);
		s.push(n);
		}

	cout << "Memory after creating stack: " << coreleft() << " bytes\n";

	dumpStack(s);

	// Kill the stack and display it

	s.empty();
	dumpStack(s);

	// Create a stack of 10 nodes.

	for (i = 0; i < 10; i++) {
		n = new MyNode(i);
		s.push(n);
		}

	dumpStack(s);

	// Remove all the nodes from the stack with pop()

	while ((n = s.pop()) != NULL) {
		cout << "Popped " << n->id() << endl;
		delete n;
		}

	dumpStack(s);

	cout << "Memory at end: " << coreleft() << " bytes\n";
}
