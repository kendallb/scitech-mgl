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
* Description:	Test program for the hash table class.
*
****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <iostream.h>
#include "tcl/hashtab.hpp"

#ifdef __UNIX__

unsigned long coreleft(void) {
	return 1024;
}

#endif

// Define the nodes to be placed into the hash table

class entry : public TCHashTableNode {
protected:
	char	*name;			// Key of node
	int		other_stuff;
public:
			// Constructor
			entry(char *n,int stuff)
			{
				name = n;
				other_stuff = stuff;
			}

			// Destructor to delete a hash table node
			~entry();

			// Member to compute the hash value of a symbol
			uint hash(void) const;

			// Equality operator for a hash table node and a key
			ibool operator == (const TCHashTableNode& key) const;

			// Member to display a hash table node
			void printOn(ostream& o) const;
	};

entry::~entry()
{
//	cout << "Deleting " << name << endl;
}

uint entry::hash(void) const
{
	return TCL_hashAdd(name);
}

ibool entry::operator == (const TCHashTableNode& key) const
{
	return (strcmp(name,((entry&)key).name) == 0);
}

void entry::printOn(ostream& o) const
{
	o << name << " : " << other_stuff << endl;
}

void main(int argc,char *argv[])
{
	entry	*p,*q,*r;

	cout << "\nMemory at start: " << (unsigned long)coreleft() << endl;

	// Create the hash table

	TCHashTable<entry>* symtab = new TCHashTable<entry>(31);

	// For each element on the command line, put it into the table

	for (++argv, --argc; --argc>=0; argv++)	{
		p = new entry(*argv,argc);
		symtab->add(p);
		}

	// Display the contents of the table

	cout << *symtab << endl;

	// Look up a node in the hash table

	p = new entry("in",0);
	q = symtab->find(p);

	if (q == NULL)
		cout << "Node not found.\n";
	else {
		cout << *q;
		}

	r = q;

	while ((q = symtab->next(q)) != NULL)
		cout << " and " << *q;
	cout << endl;

	// Attempt to remove the first node found

	if (r)
		delete symtab->remove(r);
	cout << *symtab << endl;

	cout << "\nMemory before deleting hash table: " << (unsigned long)coreleft() << endl;

	delete symtab;
	delete p;

	cout << "\nMemory after deleting hash table: " << (unsigned long)coreleft() << endl;
}
