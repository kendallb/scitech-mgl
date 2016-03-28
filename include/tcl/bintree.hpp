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
* Description:  Header file for the BinaryTree class, a class designed
*               for storing ordered information in a simple binary tree.
*               This class does nothing to ensure the binary tree is
*               balanced, so it may well become a degenerate linked list.
*               Use the AVLTree or RedBlackTree class if you require the
*               tree to be balanced.
*
****************************************************************************/

#ifndef __TCL_BINTREE_HPP
#define __TCL_BINTREE_HPP

#ifndef __SCITECH_H
#include "scitech.h"
#endif

/*--------------------------- Class Definition ----------------------------*/

//---------------------------------------------------------------------------
// The TCBinaryTreeNode class is a simple class used to link the objects in
// the binary tree together. To put anything useful into the tree, you must
// derive the object placed into the tree from TCBinaryTreeNode.
//---------------------------------------------------------------------------

class TCBinaryTreeNode {
protected:
    TCBinaryTreeNode    *left,*right,*parent;

    friend  class TCGenBinaryTree;
public:
            // Constructor to statisfy some compilers :-)
            TCBinaryTreeNode() {};

            // Virtual destructor to delete a binary tree node
    virtual ~TCBinaryTreeNode();

            // Virtual comparision function between nodes
    virtual int cmp(const TCBinaryTreeNode* node) const = 0;

            // Overload comparision operators
            ibool operator < (const TCBinaryTreeNode& n)    { return cmp(&n) < 0; };
            ibool operator <= (const TCBinaryTreeNode& n)   { return cmp(&n) <= 0; };
            ibool operator == (const TCBinaryTreeNode& n)   { return cmp(&n) == 0; };
            ibool operator != (const TCBinaryTreeNode& n)   { return cmp(&n) != 0; };
            ibool operator >= (const TCBinaryTreeNode& n)   { return cmp(&n) >= 0; };
            ibool operator > (const TCBinaryTreeNode& n)    { return cmp(&n) > 0; };
    };

//---------------------------------------------------------------------------
// The TCGenBinaryTree class is designed to manipulate a binary tree of
// TCBinaryTreeNode objects. In the simple form, BinartTreeNode objects
// contain nothing special. To add an arbitrary class to the tree, you must
// derive the class from TCBinaryTreeNode (either through single or multiple
// inheritance).
//---------------------------------------------------------------------------

class TCGenBinaryTree {
protected:
    ulong           count;      // Number of objects in tree
    TCBinaryTreeNode    *root;      // Pointer to first node in list
    TCBinaryTreeNode    *z;         // Pointer to last node in list
    TCBinaryTreeNode    hz[2];      // Space for head and z nodes

            // Internal methods
    static  void _empty(TCBinaryTreeNode *t);
    static  void _preOrder(TCBinaryTreeNode *t);
    static  void _inOrder(TCBinaryTreeNode *t);
    static  void _postOrder(TCBinaryTreeNode *t);
    static  void _range(TCBinaryTreeNode *t);
            TCBinaryTreeNode* _findMin(TCBinaryTreeNode *t) const;
            TCBinaryTreeNode* _findMax(TCBinaryTreeNode *t) const;

public:
            // Constructor
            TCGenBinaryTree();

            // Destructor
            ~TCGenBinaryTree();

            // Method to add a node to the tree
            TCBinaryTreeNode* add(TCBinaryTreeNode* node);

            // Methods to remove a node from the tree
            TCBinaryTreeNode* remove(TCBinaryTreeNode *key);
            TCBinaryTreeNode* removeMin();
            TCBinaryTreeNode* removeMax();

            // Methods to find a node in the tree
            TCBinaryTreeNode* find(TCBinaryTreeNode *key) const;
            TCBinaryTreeNode* findMin() const
                { return _findMin(root->right); };
            TCBinaryTreeNode* findMax() const
                { return _findMax(root->right); };

            // Methods to iterate through the tree
            TCBinaryTreeNode* next(TCBinaryTreeNode *node) const;
            TCBinaryTreeNode* prev(TCBinaryTreeNode *node) const;

            // Methods to traverse the binary tree
            void preOrder(void (*visit)(TCBinaryTreeNode*)) const;
            void inOrder(void (*visit)(TCBinaryTreeNode*)) const;
            void postOrder(void (*visit)(TCBinaryTreeNode*)) const;

            // Method to perform a range search on the tree
            void range(TCBinaryTreeNode *low,TCBinaryTreeNode *high,
                void (*visit)(TCBinaryTreeNode*)) const;

            // Empties the entire tree by destroying all nodes
            void empty();

            // Returns the number of items in the tree
            ulong numberOfItems(void) const { return count; };

            // Returns true if the tree is empty
            ibool isEmpty(void) const       { return count == 0; };
    };

//---------------------------------------------------------------------------
// Set of template wrapper classes for declaring Type Safe binary trees.
// Note that the elements of the binary tree must still be derived from
// TCBinaryTreeNode.
//---------------------------------------------------------------------------

typedef void (*_TCGenBinaryTreeVisit)(TCBinaryTreeNode*);

template <class T> class TCBinaryTree : public TCGenBinaryTree {
public:
            T* remove(T* key)   { return (T*)TCGenBinaryTree::remove(key); };
            T* removeMin()      { return (T*)TCGenBinaryTree::removeMin(); };
            T* removeMax()      { return (T*)TCGenBinaryTree::removeMax(); };
            T* find(T* key)     { return (T*)TCGenBinaryTree::find(key); };
            T* findMin()        { return (T*)TCGenBinaryTree::findMin(); };
            T* findMax()        { return (T*)TCGenBinaryTree::findMax(); };
            T* next(T* node)    { return (T*)TCGenBinaryTree::next(node); };
            T* prev(T* node)    { return (T*)TCGenBinaryTree::prev(node); };
            void preOrder(void (*visit)(T*))
                { TCGenBinaryTree::preOrder((_TCGenBinaryTreeVisit)visit); };
            void inOrder(void (*visit)(T*))
                { TCGenBinaryTree::inOrder((_TCGenBinaryTreeVisit)visit); };
            void postOrder(void (*visit)(T*))
                { TCGenBinaryTree::postOrder((_TCGenBinaryTreeVisit)visit); };
            void range(T* low,T* high,void (*visit)(T*))
                { TCGenBinaryTree::range(low,high,(_TCGenBinaryTreeVisit)visit); };
    };

/*------------------------ Inline member functions ------------------------*/

#endif  // __TCL_BINTREE_HPP
