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
* Description:  Member functions for the TCBinaryTree class, a class designed
*               for storing ordered information in a simple binary tree.
*               This class does nothing to ensure the binary tree is
*               balanced, so it may well become a degenerate linked list.
*               Use the AVLTree or RedBlackTree class if you require the
*               tree to be balanced.
*
****************************************************************************/

#include "tcl/bintree.hpp"

/*--------------------------- Member functions ----------------------------*/

// Save a number of variabels globally when executing the recursive
// routines to save stack space and speed execution.

PRIVATE TCBinaryTreeNode    *o_z;
PRIVATE TCBinaryTreeNode    *o_low,*o_high;
PRIVATE void                (*o_visit)(TCBinaryTreeNode*);

TCGenBinaryTree::TCGenBinaryTree()
/****************************************************************************
*
* Function:     TCGenBinaryTree::TCGenBinaryTree
*
* Description:  Constructor for the TCGenBinaryTree class. We build an
*               empty binary tree. Note that we use special head and tail
*               nodes to simplify some of the tree manipulation routines.
*
****************************************************************************/
{
    count = 0;
    root = &hz[0];              // Setup root and tail pointers
    z = &hz[1];
    root->left = root->right = z->left = z->right = z;
    root->parent = z->parent = root;
}

TCGenBinaryTree::~TCGenBinaryTree()
/****************************************************************************
*
* Function:     TCGenBinaryTree::~TCGenBinaryTree
*
* Description:  Destructor for the TCGenBinaryTree class. We simply empty
*               the tree of all nodes currently owned by the tree.
*
****************************************************************************/
{
    empty();
}

TCBinaryTreeNode* TCGenBinaryTree::add(TCBinaryTreeNode *node)
/****************************************************************************
*
* Function:     TCGenBinaryTree::add
* Parameters:   node    - Node to add to the tree
* Returns:      NULL on success, or pointer to conflicting node.
*
* Description:  Adds a new node to the binary tree. If a node with the same
*               key already exists in the tree, we bomb out and return
*               a pointer to the existing node.
*
****************************************************************************/
{
    TCBinaryTreeNode    *t = root->right,*p = root;
    int             val = 1;        // Insert into right tree for first node

    // Search for the correct place to insert the node into the tree

    while (t != z) {
        p = t;
        if ((val = node->cmp(t)) < 0)
            t = t->left;
        else if (val > 0)
            t = t->right;
        else return t;              // Node already exists in tree
        }
    node->parent = p;
    if (val < 0)
        p->left = node;             // Insert into left subtree
    else p->right = node;           // Insert into right subtree
    node->left = node->right = z;
    count++;
    return NULL;                    // Insertion was successful
}

TCBinaryTreeNode* TCGenBinaryTree::remove(TCBinaryTreeNode *key)
/****************************************************************************
*
* Function:     TCGenBinaryTree::remove
* Parameters:   key - Key to use to find the node to remove
* Returns:      Pointer to the removed node, NULL if node was not found.
*
* Description:
*
****************************************************************************/
{
    TCBinaryTreeNode *node,*p;

    if ((node = find(key)) == NULL) // Find the node to delete
        return NULL;                // Node was not found

    if (node->left == z || node->right == z)
        p = node;
    else p = next(node);

    count--;
    return node;                    // Deletion was successful
}

void TCGenBinaryTree::_empty(TCBinaryTreeNode* t)
/****************************************************************************
*
* Function:     TCGenBinaryTree::_empty
* Parameters:   t   - Root of subtree to empty
*
* Description:  Recursive routine to delete all the nodes in a specified
*               subtree. This routine is declared as static to reduce
*               the runtime stack overheads required.
*
****************************************************************************/
{
    if (t != o_z) {
        _empty(t->left);
        _empty(t->right);
        delete t;
        }
}

void TCGenBinaryTree::empty()
/****************************************************************************
*
* Function:     TCGenBinaryTree::empty
*
* Description:  Empties the tree of all nodes currently installed in the
*               tree.
*
****************************************************************************/
{
    o_z = z;
    _empty(root->right);
    root->left = root->right = z->left = z->right = z;
    root->parent = z->parent = root;
}

TCBinaryTreeNode* TCGenBinaryTree::find(TCBinaryTreeNode* key) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::find
* Parameters:   key - Key used to determine if we have found the node
* Returns:      Pointer to the node found, NULL if not present.
*
* Description:  Looks up a specified node in the tree, return a pointer
*               to the actual node in the tree if found. We use a fast
*               iterative routine to perform this operation.
*
****************************************************************************/
{
    TCBinaryTreeNode* node = root->right;
    int             val;

    while (node != z) {
        if ((val = node->cmp(key)) == 0)
            break;
        if (val < 0)
            node = node->left;
        else node = node->right;
        }
    return (node == z) ? NULL : node;
}

TCBinaryTreeNode* TCGenBinaryTree::_findMin(TCBinaryTreeNode *t) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::_findMin
* Parameters:   t   - Root of subtree to search for minimum
* Returns:      Pointer to the smallest node in the tree, NULL if tree empty
*
****************************************************************************/
{
    while (t->left != z)
        t = t->left;
    return (t == z) ? NULL : t;
}

TCBinaryTreeNode* TCGenBinaryTree::_findMax(TCBinaryTreeNode *t) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::_findMax
* Parameters:   t   - Root of subtree to search for maximum
* Returns:      Pointer to the largest node in the tree, NULL if tree empty
*
****************************************************************************/
{
    while (t->right != z)
        t = t->right;
    return (t == z) ? NULL : t;
}

TCBinaryTreeNode* TCGenBinaryTree::next(TCBinaryTreeNode* node) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::next
* Parameters:   node    - Current node to find successor of
* Returns:      Successor node to the specified node, NULL if none.
*
* Description:  Finds the next node in the binary tree defined by an
*               inorder traversal of the tree. If there is no successor,
*               this routine returns NULL.
*
****************************************************************************/
{
    if (node->right != z)
        return _findMin(node->right);
    TCBinaryTreeNode *p = node->parent;
    while (p != root && p->right == node) {
        node = p;
        p = p->parent;
        }
    return (p == root) ? NULL : p;
}

TCBinaryTreeNode* TCGenBinaryTree::prev(TCBinaryTreeNode* node) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::prev
* Parameters:   node    - Current node to find predecessor of
* Returns:      Predecessor node to the specified node, NULL if none.
*
* Description:  Finds the previous node in the binary tree defined by an
*               inorder traversal of the tree. If there is no predecessor,
*               this routine returns NULL.
*
****************************************************************************/
{
    if (node->left != z)
        return _findMax(node->left);
    TCBinaryTreeNode *p = node->parent;
    while (p != root && p->left == node) {
        node = p;
        p = p->parent;
        }
    return (p == root) ? NULL : p;
}

void TCGenBinaryTree::_preOrder(TCBinaryTreeNode* t)
/****************************************************************************
*
* Function:     TCGenBinaryTree::_preOrder
* Parameters:   t   - Root of subtree to perform traversal on
*
* Description:  Recursive routine to perform a pre-order traversal on the
*               subtree.
*
****************************************************************************/
{
    if (t != o_z) {
        o_visit(t);
        _preOrder(t->left);
        _preOrder(t->right);
        }
}

void TCGenBinaryTree::_inOrder(TCBinaryTreeNode* t)
/****************************************************************************
*
* Function:     TCGenBinaryTree::_inOrder
* Parameters:   t   - Root of subtree to perform traversal on
*
* Description:  Recursive routine to perform an in-order traversal on the
*               subtree.
*
****************************************************************************/
{
    if (t != o_z) {
        _inOrder(t->left);
        o_visit(t);
        _inOrder(t->right);
        }
}

void TCGenBinaryTree::_postOrder(TCBinaryTreeNode* t)
/****************************************************************************
*
* Function:     TCGenBinaryTree::_postOrder
* Parameters:   t   - Root of subtree to perform traversal on
*
* Description:  Recursive routine to perform a post-order traversal on the
*               subtree.
*
****************************************************************************/
{
    if (t != o_z) {
        _postOrder(t->left);
        _postOrder(t->right);
        o_visit(t);
        }
}

void TCGenBinaryTree::preOrder(void (*visit)(TCBinaryTreeNode*)) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::preOrder
* Parameters:   visit   - Function to call for each node visited
*
****************************************************************************/
{
    o_z = z;
    o_visit = visit;
    _preOrder(root->right);
}

void TCGenBinaryTree::inOrder(void (*visit)(TCBinaryTreeNode*)) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::inOrder
* Parameters:   visit   - Function to call for each node visited
*
****************************************************************************/
{
    o_z = z;
    o_visit = visit;
    _inOrder(root->right);
}

void TCGenBinaryTree::postOrder(void (*visit)(TCBinaryTreeNode*)) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::postOrder
* Parameters:   visit   - Function to call for each node visited
*
****************************************************************************/
{
    o_z = z;
    o_visit = visit;
    _postOrder(root->right);
}

void TCGenBinaryTree::_range(TCBinaryTreeNode* t)
/****************************************************************************
*
* Function:     TCGenBinaryTree::_range
* Parameters:   t   - Root of subtree to perform range search on
*
* Description:  Recursive routine to perform a range search on the specified
*               subtree.
*
****************************************************************************/
{
    if (t != o_z) {
        if (*t < *o_low)
            _range(t->left);
        else if (*t > *o_high)
            _range(t->right);
        else {
            _range(t->left);
            o_visit(t);
            _range(t->right);
            }
        }
}

void TCGenBinaryTree::range(TCBinaryTreeNode* low,TCBinaryTreeNode *high,
    void (*visit)(TCBinaryTreeNode*)) const
/****************************************************************************
*
* Function:     TCGenBinaryTree::range
* Parameters:   low     - Low value to be used in range search
*               high    - High value to be used in range search
*               visit   - Function to call for each node with the range
*
* Description:  Visits all the nodes with the specified range, calling the
*               visit routine for each node within the range in sorted
*               order.
*
****************************************************************************/
{
    o_z = z;
    o_visit = visit;
    o_low = low;
    o_high = high;
    _range(root->right);
}

TCBinaryTreeNode::~TCBinaryTreeNode()
{
}
