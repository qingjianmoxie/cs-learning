# 19_删除链表的倒数第N个节点

给定一个链表，删除链表的倒数第 n 个节点，并且返回链表的头结点。

示例：

    给定一个链表: 1->2->3->4->5, 和 n = 2.

    当删除了倒数第二个节点后，链表变为 1->2->3->5.

说明：

给定的 n 保证是有效的。

进阶：

你能尝试使用一趟扫描实现吗？

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/remove-nth-node-from-end-of-list
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

自己写的时候想的是用vector存入指针, 然后用下标访问, 也算是一种方法吧...

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
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        vector<ListNode *> array;
        ListNode *p = head;
        while (p)
        {
            array.push_back(p);
            p = p->next;
        }
        int size = array.size();
        int id = size - n;

        if (size > 1 && 0 != id)
            array[id - 1]->next = array[id]->next;
        else
            head = head->next;

        return head;
    }
};
```

普遍的方法是用双指针

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
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode *dummy = new ListNode(-1);
        dummy->next = head;
        ListNode *tmp1 = dummy;
        ListNode *tmp2 = dummy;
        for (int distance = 0; distance <= n; distance++)
        {
            tmp2 = tmp2->next;
        }
        while (tmp2)
        {
            tmp1 = tmp1->next;
            tmp2 = tmp2->next;
        }

        ListNode *pDelete = tmp1->next;
        tmp1->next = pDelete->next;
        delete pDelete;

        ListNode *retNode = dummy->next;
        delete dummy;
        return retNode;
    }
};
```

我使用过while循环, 也通过了. while循环是用来判断移除头节点的情形. 适合于 n 不保证有效的情形.

```c++
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode *tmp1 = head;
        ListNode *tmp2 = head;
        int distance = 0;
        while (tmp2 && distance <= n)
        {
            tmp2 = tmp2->next;
            distance++;
        }
        if (n == distance)
        {
            head = head->next;
            return head;
        }
        while (tmp2)
        {
            tmp1 = tmp1->next;
            tmp2 = tmp2->next;
        }
        ListNode *pDelete = tmp1->next;
        tmp1->next = pDelete->next;
        delete pDelete;
        return head;
    }
};
```