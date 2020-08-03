# 2_两数相加

给出两个 **非空** 的链表用来表示两个非负的整数。其中，它们各自的位数是按照 **逆序** 的方式存储的，并且它们的每个节点只能存储 **一位** 数字。

如果，我们将这两个数相加起来，则会返回一个新的链表来表示它们的和。

您可以假设除了数字 0 之外，这两个数都不会以 0 开头。

示例：

    输入：(2 -> 4 -> 3) + (5 -> 6 -> 4)
    输出：7 -> 0 -> 8
    原因：342 + 465 = 807

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/add-two-numbers
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

一开始写的时候, new一直报错, 看了题解才发现之前写的`new(ListNode(0))`, 正确的写法是`new ListNode(0)`.

需要思考两个特殊情况:
1. 两个数的位数不同时怎么处理, 比如 1 + 123
2. 最高位进位时怎么处理, 比如 5 + 5

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
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode *head = new ListNode(-1); // 存放结果的链表, 头结点为哨兵
        ListNode *tail = head;             // 结果链表的尾节点
        ListNode *p = NULL;                // 供生成新的节点使用
        ListNode *tmp1 = l1;
        ListNode *tmp2 = l2;
        bool isNeedCarry = false; // 是否需要进位的标识符
        while (tmp1 && tmp2)
        {
            int sum = 0; // 每个位相加的和
            sum = tmp1->val + tmp2->val + isNeedCarry;
            if (sum <= 9)
            {
                p = new ListNode(sum);
                isNeedCarry = false;
            }
            else
            {
                p = new ListNode(sum % 10);
                isNeedCarry = true;
            }
            tail->next = p;
            tail = tail->next;
            tmp1 = tmp1->next;
            tmp2 = tmp2->next;
            if (NULL != tmp1 && NULL == tmp2)
                tmp2 = new ListNode(0);
            if (NULL == tmp1 && NULL != tmp2)
                tmp1 = new ListNode(0);
        }
        if (isNeedCarry)
            tail->next = new ListNode(1);
        /* 返回结果前将哨兵释放, 防止内存泄漏 */
        ListNode *pDelete = head;
        head = head->next;
        delete pDelete;
        return head;
    }
};
```

这题和[415_字符串相加(easy)](./415_add-strings.md)神似.

由415题题解想到的另一种写法:

```c++
class Solution {
   public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* head = new ListNode(-1);  // 存放结果的链表, 头结点为哨兵
        ListNode* tail = head;              // 结果链表的尾节点
        ListNode* p = NULL;                 // 供生成新的节点使用
        ListNode* tmp1 = l1;
        ListNode* tmp2 = l2;
        int carry = 0;  // 是否需要进位的标识符
        while (tmp1 || tmp2 || 0 != carry) {
            int x = tmp1 ? tmp1->val : 0;
            int y = tmp2 ? tmp2->val : 0;
            int sum = x + y + carry;
            p = new ListNode(sum % 10);
            carry = sum / 10;
            tail->next = p;
            tail = tail->next;
            if (tmp1) tmp1 = tmp1->next;
            if (tmp2) tmp2 = tmp2->next;
        }
        /* 返回结果前将哨兵释放, 防止内存泄漏 */
        ListNode* pDelete = head;
        head = head->next;
        delete pDelete;
        return head;
    }
};
```

又看出来上面代码中tmp1和tmp2判断了两次是否为NULL, 可以改为1次

```c++
class Solution {
   public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* head = new ListNode(-1);  // 存放结果的链表, 头结点为哨兵
        ListNode* tail = head;              // 结果链表的尾节点
        ListNode* p = NULL;                 // 供生成新的节点使用
        ListNode* tmp1 = l1;
        ListNode* tmp2 = l2;
        int carry = 0;  // 是否需要进位的标识符
        while (tmp1 || tmp2 || 0 != carry) {
            int sum = 0;
            if (tmp1) {
                sum += tmp1->val;
                tmp1 = tmp1->next;
            }
            if (tmp2) {
                sum += tmp2->val;
                tmp2 = tmp2->next;
            }
            sum += carry;
            p = new ListNode(sum % 10);
            carry = sum / 10;
            tail->next = p;
            tail = tail->next;
        }
        /* 返回结果前将哨兵释放, 防止内存泄漏 */
        ListNode* pDelete = head;
        head = head->next;
        delete pDelete;
        return head;
    }
};
```

提交耗时变长了一丢丢, 不知道为啥...