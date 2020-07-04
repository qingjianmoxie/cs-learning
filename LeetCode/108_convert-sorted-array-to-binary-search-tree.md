将一个按照升序排列的有序数组，转换为一棵高度平衡二叉搜索树。

本题中，一个高度平衡二叉树是指一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过 1。

示例:

    给定有序数组: [-10,-3,0,5,9],

    一个可能的答案是：[0,-3,9,-10,null,5]，它可以表示下面这个高度平衡二叉搜索树：

          0
         / \
       -3   9
       /   /
     -10  5

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/convert-sorted-array-to-binary-search-tree
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
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        // helper中有判断, 此处不需要判断
        // if (nums.empty()) return NULL;
        int size = nums.size();
        int left = 0;
        int right = size - 1;
        return helper(nums, left, right);
    }

    TreeNode* helper(const vector<int>& nums, int left, int right) {
        if (left > right) return NULL;
        // 总是选择中间位置左边的数字作为根节点
        int mid = (left + right) >> 1;
        TreeNode* root = new TreeNode(nums[mid]);
        // if (left == right) {
        //     return root;
        // }
        root->left = helper(nums, left, mid - 1);
        root->right = helper(nums, mid + 1, right);
        return root;
    }
};
```

复杂度分析

+ 时间复杂度：O(n)，其中 n 是数组的长度。每个数字只访问一次。

+ 空间复杂度：O(logn)，其中 n 是数组的长度。空间复杂度不考虑返回值，因此空间复杂度主要取决于递归栈的深度，递归栈的深度是 O(logn)。
