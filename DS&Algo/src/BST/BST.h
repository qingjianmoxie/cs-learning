/*
** Interface for a binary search tree module
*/

#include <iostream>

/* Type of data in the tree */
typedef int ElementType;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

/*********
 * insert
 * 给定二叉搜索树（BST）的根节点和要插入树中的值，将值插入二叉搜索树。 
 * 返回插入后二叉搜索树的根节点
 *********/
TreeNode *insert(TreeNode *root, int val);

/*
** find
**  Searches for a specific data, which is passed as the first argument.
*/
TreeNode *find_recursion(TreeNode *root, int val);
TreeNode *find_iteration(TreeNode *root, int val);

TreeNode *FindMin(TreeNode *root);
TreeNode *FindMax(TreeNode *root);

/********
 * Delete
 ********/
TreeNode *Delete(TreeNode *root, int key);
