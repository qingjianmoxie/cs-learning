/*
** Interface for a binary search tree module
*/

/* Type of data in the tree */
typedef int ElementType;

// The TreeNode structure holds the data and pointers for one tree node.
typedef struct TreeNode
{
    ElementType data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef TreeNode *BinTree;

/*
**	The pointer to the root node in the tree.
*/
static TreeNode *tree;

/*
** insert
**	Add a new data to the tree.  The argument is the data
**	to be added and must not already exist in the tree.
*/
void insert(ElementType data);

/*
** find
**	Searches for a specific data, which is passed as the first
**	argument.
*/
ElementType *find(ElementType data);

/*
** pre_order_traverse
**	Does a pre-order traversal of the tree.  The argument is a
**	pointer to a callback function that will be called for
**	each node in the tree, with the data passed as an argument.
*/
void pre_order_traverse(void (*callback)(ElementType data));
