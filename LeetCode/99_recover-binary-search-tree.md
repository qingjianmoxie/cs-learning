二叉搜索树中的两个节点被错误地交换。

请在不改变其结构的情况下，恢复这棵树。

示例 1:

    输入: [1,3,null,null,2]

       1
      /
     3
      \
       2

    输出: [3,1,null,null,2]

       3
      /
     1
      \
       2

示例 2:

    输入: [3,1,4,null,null,2]

      3
     / \
    1   4
       /
      2

    输出: [2,1,4,null,null,3]

      2
     / \
    1   4
       /
      3

进阶:

+ 使用 O(n) 空间复杂度的解法很容易实现。
+ 你能想出一个只使用常数空间的解决方案吗？

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/recover-binary-search-tree
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

### 解题思路

本题使用树的**中序遍历**，利用**Morris遍历**方法，实现常数空间复杂度

**中序遍历 + 双指针找逆序对**

二叉搜索树的中序遍历结果可以等效于一个升序数组
因此先用数组举例，如果原始的二叉搜索树为
1, 2, 3, 4, 5
如果将其中2,4两个元素进行交换，变成
1, 4, 3, 2, 5
那么我们可以使用双指针的方法，检查这个数组里的逆序对，将逆序对找出来就可以解决问题

观察数组

+ 第一对逆序对4, 3，是索引小的那个是被交换元素
+ 第二对逆序对3, 2，是索引大的那个是被交换元素

所以我们在遇到逆序对的时候，如果是第一次遇见，则存储索引小的那个，如果不是，则存储索引大的那个
```c++
if(pre != NULL && cur->val < pre->val){
    if(s1 == NULL) s1 = pre;  // 索引小的已经找到，就不再改变
    s2 = cur;
}
```
为什么这里s2用的不是else？
因为存在两个相邻元素交换的情况，所以我们在第一次遇见的把索引大的那个也存下来

+ 如果后面没有逆序对了，那就是这一对
+ 如果后面还有逆序对，会覆盖s2(因为有if判断，所以s1不会被覆盖)

### Morris遍历

通过前面的分析，使用递归的方法可以很容易地完成本题, 下面讲讲Morris遍历算法。

Morris代码框架

```c++
while(cur){
    if(cur->left == NULL){// 左子树为空时，直接比较，然后进入右子树
        /*************
        /*  进行你的操作
        *************/
        pre = cur;
        cur = cur->right;
    }else{// 进入左子树
        /*************
        /* 找cur的前驱结点，找到后分两种情况
        /*   1、cur的左子结点没有右子结点，那cur的左子结点就是前驱
        /*   2、cur的左子结点有右子结点，就一路向右下，走到底就是cur的前驱
        *************/
        TreeNode* preceesor = cur->left;
        while(preceesor->right && preceesor->right != cur){
            preceesor = preceesor->right;
        }

        // 前驱已经指向自己了，直接比较，然后进入右子树
        if(preceesor->right == cur){
            /*************
            /*  进行你的操作
            *************/
            preceesor->right = NULL; // 断开连接，恢复原树
            pre = cur;
            cur = cur->right;
        }else{ // 前驱还没有指向自己，说明左边还没有遍历，将前驱的右指针指向自己，后进入前驱
            preceesor->right = cur;
            cur = cur->left;
        }
    }
}
```

### 复杂度分析

+ 时间复杂度：每个结点访问了2次，因此时间复杂度为O(n)
+ 空间复杂度：只使用了常数空间，因此空间复杂度为O(1)

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
    // s1 存小索引那个结点，s2存大索引那个结点，pre存前驱结点
    TreeNode *s1 = NULL, *s2 = NULL, *pre = NULL;
    void recoverTree(TreeNode* root) {
        TreeNode* cur = root;  // 游标
        while (cur != NULL) {
            if (cur->left != NULL) {  // 进入左子树
                // 找到cur的前驱结点，分两种情况
                // 1、cur的左子结点没有右子结点，那cur的左子结点就是前驱
                // 2、cur的左子结点有右子结点，就一路向右下，走到底就是cur的前驱
                TreeNode* predecessor = cur->left;
                while (predecessor->right != NULL && predecessor->right != cur) {
                    predecessor = predecessor->right;
                }

                // 前驱还没有指向自己，说明左边还没有遍历，将前驱的右指针指向自己，后进入前驱
                if (predecessor->right == NULL) {
                    predecessor->right = cur;
                    cur = cur->left;
                } else {
                    // 前驱已经指向自己了，直接比较是否有逆序对，然后进入右子树
                    if (pre != NULL && cur->val < pre->val) {
                        if (s1 == NULL) s1 = pre;
                        s2 = cur;
                    }
                    pre = cur;
                    predecessor->right = NULL;
                    cur = cur->right;
                }
            } else {  // 左子树为空时，检查是否有逆序对，然后进入右子树
                if (pre != NULL && cur->val < pre->val) {
                    if (s1 == NULL) s1 = pre;
                    s2 = cur;
                }
                pre = cur;
                cur = cur->right;
            }
        }
        // 进行交换
        int t = s1->val;
        s1->val = s2->val;
        s2->val = t;
        return;
    }
};
```