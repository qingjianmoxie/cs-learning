给定一个链表，旋转链表，将链表每个节点向右移动 k 个位置，其中 k 是非负数。

示例 1:

    输入: 1->2->3->4->5->NULL, k = 2
    输出: 4->5->1->2->3->NULL
    解释:
    向右旋转 1 步: 5->1->2->3->4->NULL
    向右旋转 2 步: 4->5->1->2->3->NULL

示例 2:

    输入: 0->1->2->NULL, k = 4
    输出: 2->0->1->NULL
    解释:
    向右旋转 1 步: 2->0->1->NULL
    向右旋转 2 步: 1->2->0->NULL
    向右旋转 3 步: 0->1->2->NULL
    向右旋转 4 步: 2->0->1->NULL

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/rotate-list
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。


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
    ListNode* rotateRight(ListNode* head, int k) {
        ListNode *dummy = new ListNode(-1);
        dummy->next = head;
        int count = 0;
        ListNode *p = head;
        ListNode *tail = head;
        while (p)
        {
            count++;
            tail = p;
            p = p->next;
        }
        if (0 == count)
            return NULL;
        int i = k % count;
        if (0 == i)
            return head;
        int id = count - i;
        p = head;
        for (int tmp = 1; tmp < id; tmp++)
        {
            p = p->next;
        }
        dummy->next = p->next;
        p->next = NULL;
        tail->next = head;

        ListNode *retNode = dummy->next;
        delete dummy;
        return retNode;
    }
};
```