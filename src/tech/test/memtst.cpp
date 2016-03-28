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
* Description:	Test program for the TCMemStack class.
*
****************************************************************************/

#include "tcl\memmgr.hpp"
#include <malloc.h>
#include <iostream.h>

class MyNode {
	int		_id;
	char	stuff[255];

public:
			MyNode(int id) 	{ _id = id; };
			int id(void)	{ return _id; };
	};

void DoLocal(TCMemStack& mem)
{
	MyNode	*n;

	TCMemMarker marker(mem);

	for (int i = 0; i < 100; i++)
		n = new(mem) MyNode(i);

	cout << "Memory at end of locals: " << coreleft() << " bytes\n";
}

void main(void)
{
	MyNode	*n;

	cout << "Memory at start: " << coreleft() << " bytes\n";

	TCMemStack* mem = new TCMemStack;	// Allocate a memory stack

	cout << "Memory after creating stack: " << coreleft() << " bytes\n";

	for (int i = 0; i < 10; i++)
		n = new(*mem) MyNode(i);

	cout << "Memory after doing allocation: " << coreleft() << " bytes\n";

	DoLocal(*mem);

	cout << "Memory after doing locals: " << coreleft() << " bytes\n";

	for (i = 0; i < 20; i++)
		n = new(*mem) MyNode(i);

	cout << "Memory before end: " << coreleft() << " bytes\n";

	delete mem;

	cout << "Memory at end: " << coreleft() << " bytes\n";
}
