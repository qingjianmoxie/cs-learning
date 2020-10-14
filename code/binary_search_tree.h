/*
** Interface for a binary search tree module
*/

/* Type of data in the tree */
typedef int ElementType;

// The TreeNode structure holds the data and pointers for one tree node.
typedef struct TreeNode {
    ElementType val;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef TreeNode *BinTree;

/*
** insert
**  Add a new data to the tree.  The argument is the data
**  to be added and must not already exist in the tree.
*/
void insert(ElementType value, BinTree root);

/*
** find
**  Searches for a specific data, which is passed as the first argument.
*/
TreeNode *find_recursion(ElementType value, BinTree root);
TreeNode *find_iteration(ElementType value, BinTree root);

TreeNode *FindMin(BinTree root);
TreeNode *FindMax(BinTree root);

/********
 * Delete
 ********/
TreeNode *Delete(ElementType value, BinTree root);
