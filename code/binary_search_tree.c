/******************************************************************************
 * A binary search tree implemented by linking dynamically allocated structures.
 *****************************************************************************/
#include "binary_search_tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*****************
 * insert
 *****************/
BinTree Insert(ElementType X, BinTree BST)
{
    if (!BST)
    {
        /* 若原树为空， 生成并返回一个结点的二叉搜索树 */
        BST = (BinTree)malloc(sizeof(TreeNode));
        BST->data = X;
        BST->left = BST->right = NULL;
    }
    else /* 开始找要插入元素的位置 */
    {
        if (X < BST->data)
            /* 递归插入左子树 */
            BST->left = Insert(X, BST->left);
        else if (X > BST->data)
            /* 递归插入右子树 */
            BST->right = Insert(X, BST->right);
        else /* X已经存在, 什么都不做 */
            ;
    }
    return BST;
}

void insert(ElementType value)
{
    TreeNode *current;
    TreeNode **link;

	// Start with the root node.
    link = &tree;

    // As long as we keep finding values, go to the proper subtree.
    while ((current = *link) != NULL)
    {
        /*
		** Go to the left or right subtree, as appropriate.
		** (And make sure we don't have a duplicate value!)
		*/
        if (value < current->data)
            link = &current->left;
        else
        {
            assert(value != current->data);
            link = &current->right;
        }
    }

    // Allocate a new node; make the proper link field point to it.
    current = malloc(sizeof(TreeNode));
    assert(current != NULL);
    current->data = value;
    current->left = NULL;
    current->right = NULL;
    *link = current;
}


/*
**	find
*/
ElementType *find(ElementType value)
{
    TreeNode *current;

    /*
	** Start with the root node.  Until we find the value,
	** go to the proper subtree.
	*/
    current = tree;

    while (current != NULL && current->value != value)
    {
        /*
		** Go to the left or right subtree, as appropriate.
		*/
        if (value < current->value)
            current = current->left;
        else
            current = current->right;
    }

    if (current != NULL)
        return &current->value;
    else
        return NULL;
}

/*
** do_pre_order_traverse
**	Do one level of a pre-order traverse.  This helper function
**	is needed to save the information of which node we're
**	currently processing; this is not a part of the
**	client's interface.
*/
static void do_pre_order_traverse(TreeNode *current,
                      void (*callback)(ElementType value))
{
    if (current != NULL)
    {
        callback(current->value);
        do_pre_order_traverse(current->left, callback);
        do_pre_order_traverse(current->right, callback);
    }
}

/*
**	pre_order_traverse
*/
void pre_order_traverse(void (*callback)(ElementType value))
{
    do_pre_order_traverse(tree, callback);
}
