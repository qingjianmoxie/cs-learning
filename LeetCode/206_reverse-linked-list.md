反转一个单链表。

示例:

    输入: 1->2->3->4->5->NULL
    输出: 5->4->3->2->1->NULL

进阶:
你可以迭代或递归地反转链表。你能否用两种方法解决这道题？

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/reverse-linked-list
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
    ListNode* reverseList(ListNode* head) {

    }
};
```

## 题解

方法1: 好理解的双指针

+ 定义两个指针： pre 和 curr; pre 在前 curr 在后。
+ 每次让 pre 的 next 指向 curr ，实现一次局部反转
+ 局部反转完成之后， pre 和 curr 同时往前移动一个位置
+ 循环上述过程，直至 pre 到达链表尾部

    NULL    1 -> 2 -> 3 -> 4 -> 5 -> NULL
      |     |
    curr   pre

```c++
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        ListNode *curr = NULL;
        ListNode *pre = head;
        while (pre) {
            ListNode *tmp = pre->next;
            pre->next = curr;
            curr = pre;
            pre = tmp;
        }
        return curr;
    }
};
```

方法2: 递归

**对于递归算法，最重要的就是明确递归函数的定义**。具体来说，我们的 `reverse` 函数定义是这样的：

**输入一个节点 `head`，将「以 `head` 为起点」的链表反转，并返回反转之后的头结点**。

明确了函数定义, 就容易看懂代码了.

```c++
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (NULL == head || NULL == head->next) {
            return head;
        }
        ListNode *ret = reverseList(head->next);
        head->next->next = head;
        head->next = NULL;
        return ret;
    }
};
```
