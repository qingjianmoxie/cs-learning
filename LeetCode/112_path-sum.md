给定一个二叉树和一个目标和，判断该树中是否存在根节点到叶子节点的路径，这条路径上所有节点值相加等于目标和。

说明: 叶子节点是指没有子节点的节点。

示例: 
给定如下二叉树，以及目标和 sum = 22，

                  5
                 / \
                4   8
               /   / \
              11  13  4
             /  \      \
            7    2      1

返回 true, 因为存在目标和为 22 的根节点到叶子节点的路径 5->4->11->2。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/path-sum
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

首先想到的是dfs算法, 但是写出来有点怪怪的.

```c++
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    bool hasPathSum(TreeNode* root, int sum) {
        bool res = false;
        int total = 0;
        dfs(root, sum, total, res);
        return res;
    }

    void dfs(TreeNode* root, int sum, int total, bool& res) {
        if (NULL == root) return;
        total += root->val;
        if (NULL == root->left && NULL == root->right && total == sum) {
            res = true;
            return;
        }
        dfs(root->left, sum, total, res);
        if (res) return;
        dfs(root->right, sum, total, res);
    }
};
```

看了一下题解中的写法，发现其想法是一直向下找到叶子节点，如果到叶子节点时sum == 0，说明找到了一条符合要求的路径, 用减法而不是加法, 就不需要total变量.

```py
# Definition for a binary tree node.
# class TreeNode(object):
#     def __init__(self, x):
#         self.val = x
#         self.left = None
#         self.right = None

class Solution(object):
    def hasPathSum(self, root, sum):
        """
        :type root: TreeNode
        :type sum: int
        :rtype: bool
        """
        if not root: return False
        if not root.left and not root.right:
            return sum == root.val
        return self.hasPathSum(root.left, sum - root.val) or self.hasPathSum(root.right, sum - root.val)
```

```c++
class Solution {
public:
    bool hasPathSum(TreeNode* root, int sum) {
        if (NULL == root) return false;
        if (NULL == root->left && NULL == root->right) {
            return root->val == sum;
        }
        bool ret = hasPathSum(root->left, sum - root->val) ||
                   hasPathSum(root->right, sum - root->val);
        return ret;
    }
};
```
