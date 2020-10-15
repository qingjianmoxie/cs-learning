#include "BST.h"

/**************
 * insert
 **************/
TreeNode *insert(TreeNode *root, int val) {
    if (NULL == root) {
        /* 若原树为空， 生成并返回一个结点的二叉搜索树 */
        root = new TreeNode(val);
    } else {
        /* 开始找要插入元素的位置 */
        if (val < root->val) {
            /* 递归插入左子树 */
            root->left = insert(root->left, val);
        } else if (val > root->val) {
            /* 递归插入右子树 */
            root->right = insert(root->right, val);
        } else {
            /* value已经存在, 什么都不做 */
        }
    }
    return root;
}

/***********
 * 递归find
 **********/
TreeNode *find_recursion(TreeNode *root, int val) {
    if (NULL == root) {
        // 没有找到
        return NULL;
    }
    if (val > root->val) {
        // 在右子树中继续查找
        return find_recursion(root->right, val);
    } else if (val < root->val) {
        // 在左子树中继续查找
        return find_recursion(root->left, val);
    } else {
        // 查找成功, 返回找到的节点
        return root;
    }
}

/***********
 * 迭代find
 **********/
TreeNode *find_iteration(TreeNode *root, int val) {
    TreeNode *current = root;

    while (NULL != current) {
        if (val < current->val) {
            current = current->left;
        } else if (val > current->val) {
            current = current->right;
        } else {
            return current;
        }
    }
    return NULL;
}

TreeNode *FindMin(TreeNode *root) {
    if (!root)
        return NULL; /*空的二叉搜索树，返回NULL*/
    else if (!root->left)
        return root; /*找到最左叶结点并返回*/
    else
        return FindMin(root->left); /*沿左分支继续查找*/
}

TreeNode *FindMax(TreeNode *root) {
    if (root)
        while (root->right)
            root = root->right;
    /*沿右分支继续查找，直到最右叶结点*/
    return root;
}

TreeNode *Delete(TreeNode *root, int key) {
    TreeNode *current;
    if (NULL == root) {
        return NULL;
    }
    if (key < root->val) {
        root->left = Delete(root->left, key); /* 左子树递归删除 */
    } else if (key > root->val) {
        root->right = Delete(root->right, key); /* 右子树递归删除 */
    } else {
        /*找到要删除的结点 */
        if (root->left && root->right) {
            /*被删除结点有左右两个子结点 */
            current = FindMin(root->right);
            /*在右子树中找最小的元素填充删除结点*/
            root->val = current->val;
            /*在删除结点的右子树中删除最小元素*/
            root->right = Delete(root->right, root->val);
        } else {
            /*被删除结点有一个或无子结点*/
            current = root;
            if (NULL == root->left) /* 有右孩子或无子结点*/
                root = root->right;
            else if (NULL == root->right) /*有左孩子或无子结点*/
                root = root->left;
        }
    }
    return root;
}
