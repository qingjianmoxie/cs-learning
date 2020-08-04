给定一个二叉树，原地将它展开为一个单链表。

例如，给定二叉树

        1
       / \
      2   5
     / \   \
    3   4   6

将其展开为：

    1
     \
      2
       \
        3
         \
          4
           \
            5
             \
              6

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/flatten-binary-tree-to-linked-list
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

自己写的时候使用了栈, 用来存储右子树, 所以不是原地算法.

```c++
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    void flatten(TreeNode* root) {
        stack<TreeNode*> mystack;
        TreeNode* tmp = root;
        while (1) {
            while (tmp) {
                if (tmp->right) {
                    mystack.push(tmp->right);
                }
                if (tmp->left) {
                    tmp->right = tmp->left;
                    tmp->left = nullptr;
                    tmp = tmp->right;
                    continue;
                }
                break;
            }
            if (!mystack.empty()) {
                tmp->right = mystack.top();
                mystack.pop();
                tmp = tmp->right;
            } else {
                break;
            }
        }
    }
};
```

高赞算法:

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
    void flatten(TreeNode* root) {
        while (root != nullptr) {
            if (root->left != nullptr) {
                auto most_right = root->left; // 如果左子树不为空, 那么就先找到左子树的最右节点
                while (most_right->right != nullptr) most_right = most_right->right; // 找最右节点
                most_right->right = root->right; // 然后将跟的右孩子放到最右节点的右子树上
                root->right = root->left; // 这时候根的右孩子可以释放, 因此将左孩子放到右孩子上
                root->left = nullptr; // 将左孩子置为空
            }
            root = root->right; // 继续下一个节点
        }
        return;
    }
};
```
