给定一个二叉树，找出其最小深度。

最小深度是从根节点到最近叶子节点的最短路径上的节点数量。

说明: 叶子节点是指没有子节点的节点。

示例:

给定二叉树 [3,9,20,null,null,15,7],

        3
       / \
      9  20
        /  \
       15   7

返回它的最小深度  2.

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/minimum-depth-of-binary-tree
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

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
    int minDepth(TreeNode* root) {
        
    }
};
```

## 题解

这题和 [104_二叉树的最大深度(easy)](./104_maximum-depth-of-binary-tree.md) 有关系, 但是用递归的时候一直写不对, 错误的测试用例为 [1, 2].

这道题的关键是搞清楚递归结束条件:

+ 当 root 节点左右孩子都为空时，返回 1
+ **当 root 节点左右孩子有一个为空时，返回不为空的孩子节点的深度**
+ 当 root 节点左右孩子都不为空时，返回左右孩子较小深度的节点值

```c++
class Solution {
public:
    int minDepth(TreeNode *root) {
        if (NULL == root) {
            return 0;
        }
        // if (NULL == root->left && NULL == root->right) {
        //     return 1;
        // }
        if (NULL == root->left || NULL == root->right) {
            return minDepth(root->left) + minDepth(root->right) + 1;
        } else {
            return min(minDepth(root->left), minDepth(root->right)) + 1;
        }
    }
};
```

在代码中, 情况1 和 2 可以写成同一种形式.

解法二: BFS

```c++
class Solution {
public:
    int minDepth(TreeNode *root) {
        if (root == NULL) return 0;
        deque<TreeNode *> q;
        q.push_back(root);
        int deep = 0;
        while (!q.empty()) {
            deep++;
            int num = q.size();
            for (int i = 1; i <= num; i++) {
                TreeNode *p = q.front();
                q.pop_front();
                if (NULL == p->left && NULL == p->right) {
                    return deep;
                }
                if (p->left) q.push_back(p->left);
                if (p->right) q.push_back(p->right);
            }
        }
        return deep;
    }
};
```
