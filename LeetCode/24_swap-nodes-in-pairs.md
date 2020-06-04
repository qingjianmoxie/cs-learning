给定一个链表，两两交换其中相邻的节点，并返回交换后的链表。

**你不能只是单纯的改变节点内部的值**，而是需要实际的进行节点交换。

    示例:

    给定 1->2->3->4, 你应该返回 2->1->4->3.

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/swap-nodes-in-pairs
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

我想到的方法是迭代.

```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (NULL == head)
            return head;
        ListNode dummy = ListNode(-1);
        dummy.next = head;
        ListNode *tmp1 = &dummy;
        ListNode *tmp2 = head->next;
        while (tmp1 && tmp2)
        {
            tmp1->next->next = tmp2->next;
            tmp2->next = tmp1->next;
            tmp1->next = tmp2;
            if (NULL == tmp2->next->next || NULL == tmp2->next->next->next)
                break;
            tmp1 = tmp1->next->next;
            tmp2 = tmp2->next->next->next;
        }
        return dummy.next;
    }
};
```

也可以使用递归:
```c++
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode *swapPairs(ListNode *head) {
        if (NULL == head || NULL == head->next) {
            return head;
        } else {
            ListNode *tmp1 = head;
            ListNode *tmp2 = head->next;
            tmp1->next = swapPairs(tmp2->next);
            tmp2->next = tmp1;
            return tmp2;
        }
    }
};
```
