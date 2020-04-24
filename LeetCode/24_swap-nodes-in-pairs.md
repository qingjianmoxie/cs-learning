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

看到题解说迭代也可以做, 我写的迭代很垃圾:

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
        if (NULL == head || NULL == head->next)
            return head;
        else if (NULL == head->next->next)
        {
            ListNode dummy = ListNode(-1);
            dummy.next = head;
            ListNode *tmp1 = &dummy;
            ListNode *tmp2 = head->next;
            tmp1->next->next = tmp2->next;
            tmp2->next = tmp1->next;
            tmp1->next = tmp2;
            return dummy.next;
        }
        else
        {
            ListNode dummy = ListNode(-1);
            dummy.next = head;
            ListNode *tmp1 = &dummy;
            ListNode *tmp2 = head->next;
            tmp1->next->next = tmp2->next;
            tmp2->next = tmp1->next;
            tmp1->next = tmp2;
            head = head->next;
            dummy.next->next->next = swapPairs(head);
            return dummy.next;
        }
    }
};
```

实际上看题解, 不用这么多if判断
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
        ///终止条件为所有都交换完了的链表
        if (head == NULL || head->next == NULL)
            return head;
        ///递归单元内两个节点做交换
        //定义一个p结点为head->next(该递归单元内第二个节点)
        ListNode *p = head->next;
        //记录第三个节点同时也是下一次递归的head
        ListNode *temp = p->next;
        //使此单元内第二个节点指向第一个节点，即交换位置
        p->next = head;
        //交换完成后的head应指向下一次单元返回的头节点
        head->next = swapPairs(temp);
        //返回该单元的头节点
        return p;
    }
};
```
