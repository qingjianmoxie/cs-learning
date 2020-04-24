将两个升序链表合并为一个新的升序链表并返回。新链表是通过拼接给定的两个链表的所有节点组成的。 

示例：

    输入：1->2->4, 1->3->4
    输出：1->1->2->3->4->4

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/merge-two-sorted-lists
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

我看到题目的思路就是用迭代的方法.

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
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode *dummy = new ListNode(-1);
        ListNode *move = dummy;
        while (l1 && l2)
        {
            if (l1->val < l2->val)
            {
                move->next = l1;
                l1 = l1->next;
            }
            else
            {
                move->next = l2;
                l2 = l2->next;
            }
            move = move->next;
        }
        move->next = NULL != l1 ? l1 : l2;
        ListNode *retNode = dummy->next;
        delete dummy;
        return retNode;
    }
};
```

自己写的时候, 结尾的处理用了两个if判断
```c++
if (l1)
    move->next = l1;
if (l2)
    move->next = l2;
```
题解中的三元运算符更加简洁明了.

c++ new创建的对象需要手动delete释放空间, 可以改为在栈上创建对象.
```c++
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode dummy = ListNode(-1); //  创建栈空间对象
        ListNode *move = &dummy;       // 注意这里需要取地址&
        while (l1 && l2)
        {
            if (l1->val < l2->val)
            {
                move->next = l1;
                l1 = l1->next;
            }
            else
            {
                move->next = l2;
                l2 = l2->next;
            }
            move = move->next;
        }
        move->next = NULL != l1 ? l1 : l2;
        return dummy.next; // 此处是.
    }
};
```

题解还给出了递归方法, 非常巧妙

```c++
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        if (l1 == NULL)
            return l2;
        else if (l2 == NULL)
            return l1;
        else if (l1->val < l2->val)
        {
            l1->next = mergeTwoLists(l1->next, l2);
            return l1;
        }
        else
        {
            l2->next = mergeTwoLists(l1, l2->next);
            return l2;
        }
    }
};
```
