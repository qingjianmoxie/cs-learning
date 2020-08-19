给定一个单链表，其中的元素按升序排序，将其转换为高度平衡的二叉搜索树。

本题中，一个高度平衡二叉树是指一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过 1。

示例:

    给定的有序链表： [-10, -3, 0, 5, 9],

    一个可能的答案是：[0, -3, 9, -10, null, 5], 它可以表示下面这个高度平衡二叉搜索树：

          0
         / \
       -3   9
       /   /
     -10  5

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/convert-sorted-list-to-binary-search-tree
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
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
    TreeNode* sortedListToBST(ListNode* head) {
        
    }
};
```

## 题解

首先想到的是用[108_将有序数组转换为二叉搜索树(easy)](./108_convert-sorted-array-to-binary-search-tree.md)的方法, 先将链表转为数组, 然后做, 效果竟然非常好.

```c++
class Solution {
public:
    TreeNode *sortedListToBST(ListNode *head) {
        vector<int> vec;
        while (NULL != head) {
            vec.push_back(head->val);
            head = head->next;
        }
        int left = 0;
        int right = vec.size() - 1;
        return helper(vec, left, right);
    }

    TreeNode *helper(const vector<int> &vec, int left, int right) {
        if (left > right) return NULL;
        int center = (left + right) >> 1;
        TreeNode *root = new TreeNode(vec[center]);
        root->left = helper(vec, left, center - 1);
        root->right = helper(vec, center + 1, right);
        return root;
    }
};
```

**快慢指针**

既然是找链表的中间点, 可以使用快慢指针.

```c++
class Solution {
public:
    TreeNode *sortedListToBST(ListNode *head) {
        if (NULL == head) return NULL;
        ListNode *fast = head;
        ListNode *slow = head;
        ListNode *pre_slow = NULL;
        while (fast && fast->next) {
            fast = fast->next->next;
            pre_slow = slow;
            slow = slow->next;
        }
        TreeNode *root = new TreeNode(slow->val);
        if (pre_slow) {
            pre_slow->next = NULL;
            root->left = sortedListToBST(head);
        }
        root->right = sortedListToBST(slow->next);
        return root;
    }
};
```

或者

```c++
class Solution {
public:
    TreeNode *sortedListToBST(ListNode *head) { return helper(head, NULL); }

    TreeNode *helper(ListNode *head, ListNode *tail) {
        if (head == tail) {
            return NULL;
        }
        ListNode *slow = head;
        ListNode *fast = head;
        while (fast != tail && fast->next != tail) {
            slow = slow->next;
            fast = fast->next->next;
        }
        TreeNode *root = new TreeNode(slow->val);
        root->left = helper(head, slow);
        root->right = helper(slow->next, tail);
        return root;
    }
};
```
