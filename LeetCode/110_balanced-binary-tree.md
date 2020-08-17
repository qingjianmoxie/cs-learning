给定一个二叉树，判断它是否是高度平衡的二叉树。

本题中，一棵高度平衡二叉树定义为：

    一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过1。

示例 1:

给定二叉树 [3,9,20,null,null,15,7]

        3
       / \
      9  20
        /  \
       15   7

返回 true 。

示例 2:

给定二叉树 [1,2,2,3,3,null,null,4,4]

           1
          / \
         2   2
        / \
       3   3
      / \
     4   4

返回 false 。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/balanced-binary-tree
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
    bool isBalanced(TreeNode* root) {
        
    }
};
```

## 题解

### 从底至顶（提前阻断）

此方法为本题的最优解法，思路是对二叉树做先序遍历，从底至顶返回子树最大高度，若判定某子树不是平衡树则 “剪枝” ，直接向上返回。

```py
class Solution:
    def isBalanced(self, root: TreeNode) -> bool:
        return self.recur(root) != -1

    def recur(self, root):
        if not root: return 0
        left = self.recur(root.left)
        if left == -1: return -1
        right = self.recur(root.right)
        if right == -1: return -1
        return max(left, right) + 1 if abs(left - right) < 2 else -1
```

```c++
class Solution {
   public:
    bool isBalanced(TreeNode* root) { return -1 != helper(root); }

    int helper(TreeNode* root) {
        if (NULL == root) {
            return 0;
        }
        int left = helper(root->left);
        if (-1 == left) {
            return -1;
        }
        int right = helper(root->right);
        if (-1 == right) {
            return -1;
        }
        return abs(left - right) < 2 ? max(left, right) + 1 : -1;
    }
};
```

复杂度分析：
时间复杂度 O(N)： N 为树的节点数；最差情况下，需要递归遍历树的所有节点。
空间复杂度 O(N)： 最差情况下（树退化为链表时），系统递归需要使用 O(N) 的栈空间。

### 从顶至底（暴力法）

此方法容易想到，但会产生大量重复计算，时间复杂度较高。

思路是构造一个获取当前节点最大深度的方法 depth(root) ，通过比较此子树的左右子树的最大高度差abs(depth(root.left) - depth(root.right))，来判断此子树是否是二叉平衡树。若树的所有子树都平衡时，此树才平衡。

```py
class Solution:
    def isBalanced(self, root: TreeNode) -> bool:
        if not root: return True
        return abs(self.depth(root.left) - self.depth(root.right)) <= 1 and \
            self.isBalanced(root.left) and self.isBalanced(root.right)

    def depth(self, root):
        if not root: return 0
        return max(self.depth(root.left), self.depth(root.right)) + 1
```

```c++
class Solution {
   public:
    bool isBalanced(TreeNode* root) {
        if (NULL == root) return true;
        return abs(depth(root->left) - depth(root->right)) < 2 &&
               isBalanced(root->left) && isBalanced(root->right);
    }

    int depth(TreeNode* root) {
        if (NULL == root) {
            return 0;
        }
        return max(depth(root->left), depth(root->right)) + 1;
    }
};
```

复杂度分析：
时间复杂度 O(NlogN)： 最差情况下， isBalanced(root) 遍历树所有节点，占用 O(N) ；判断每个节点的最大高度 depth(root) 需要遍历 各子树的所有节点 ，子树的节点数的复杂度为 O(logN)。
空间复杂度 O(N)： 最差情况下（树退化为链表时），系统递归需要使用 O(N) 的栈空间。

该方法中求二叉树的深度 与 [104_二叉树的最大深度(easy)](./104_maximum-depth-of-binary-tree.md)类似.
