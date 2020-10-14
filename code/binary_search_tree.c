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
void insert(ElementType value, BinTree root) {
    if (NULL == root) {
        /* 若原树为空， 生成并返回一个结点的二叉搜索树 */
        root = (BinTree)malloc(sizeof(TreeNode));
        root->val = value;
        root->left = NULL;
        root->right = NULL;
    } else {
        /* 开始找要插入元素的位置 */
        if (value < root->val) {
            /* 递归插入左子树 */
            root->left = insert(value, root->left);
        } else if (value > root->val) {
            /* 递归插入右子树 */
            root->right = insert(value, root->right);
        } else {
            /* value已经存在, 什么都不做 */
            return;
        }
    }
}

/***********
 * 递归find
 **********/
TreeNode *find_recursion(ElementType value, BinTree root) {
    if (NULL == root) {
        // 没有找到
        return NULL;
    }
    if (value > root->val) {
        // 在右子树中继续查找
        return find_recursion(value, root->right);
    } else if (value < root->val) {
        // 在左子树中继续查找
        return find_recursion(value, root->left);
    } else {
        // 查找成功, 返回找到的节点
        return root;
    }
}

/***********
 * 迭代find
 **********/
TreeNode *find_iteration(ElementType value, BinTree root) {
    TreeNode *current = root;

    while (NULL != current) {
        if (value < current->val) {
            current = current->left;
        } else if (value > current->val) {
            current = current->right;
        } else {
            return current;
        }
    }
    return NULL;
}

TreeNode *FindMin(BinTree root) {
    if (!root)
        return NULL; /*空的二叉搜索树，返回NULL*/
    else if (!root->left)
        return root; /*找到最左叶结点并返回*/
    else
        return FindMin(root->left); /*沿左分支继续查找*/
}

TreeNode *FindMax(BinTree root) {
    if (root)
        while (root->right)
            root = root->right;
    /*沿右分支继续查找，直到最右叶结点*/
    return root;
}

TreeNode *Delete(ElementType value, BinTree root) {
    TreeNode *current;
    if (NULL == root) {
        printf("要删除的元素未找到");
    } else if (value < root->val) {
        root->left = Delete(value, root->left); /* 左子树递归删除 */
    } else if (value > root->val) {
        root->right = Delete(value, root->right); /* 右子树递归删除 */
    } else {
        /*找到要删除的结点 */
        if (root->left && root->right) {
            /*被删除结点有左右两个子结点 */
            current = FindMin(root->right);
            /*在右子树中找最小的元素填充删除结点*/
            root->val = current->val;
            /*在删除结点的右子树中删除最小元素*/
            root->right = Delete(root->val, root->right);
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
