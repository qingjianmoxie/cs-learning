给定一个只包含 '(' 和 ')' 的字符串，找出最长的包含有效括号的子串的长度。

示例 1:

    输入: "(()"
    输出: 2
    解释: 最长有效括号子串为 "()"

示例 2:

    输入: ")()())"
    输出: 4
    解释: 最长有效括号子串为 "()()"

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/longest-valid-parentheses
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

需要注意一下, "()(())"这种也是有效括号, 题目表述不清, 我写的时候被坑了.

**解法一: 使用栈**

```c++
class Solution {
public:
    int longestValidParentheses(string s) {
        stack<pair<char, int> > mystack;
        int len = 0;
        int maxlen = 0;
        for (int index = 0; index < s.size(); index++) {
            if (s[index] == '(') {
                mystack.push(make_pair('(', index));
            } else {
                if (mystack.empty()) {
                    mystack.push(make_pair(')', index));
                } else if ('(' == mystack.top().first) {
                    mystack.pop();
                    if (mystack.empty()) {
                        len = index + 1;
                    } else {
                        len = index - mystack.top().second;
                    }
                    maxlen = max(maxlen, len);
                } else {
                    mystack.push(make_pair(')', index));
                }
            }
        }
        return maxlen;
    }
};
```

时间复杂度： O（n）。

空间复杂度：O（n）。

**解法二: 神奇解法**

保持时间复杂度是 O（n），将空间复杂度优化到了 O（1），它的动机是怎么想到的没有理出来，就介绍下它的想法吧。

从左到右扫描，用两个变量 left 和 right 保存的当前的左括号和右括号的个数，都初始化为 0 。

+ 如果左括号个数等于右括号个数了，那么就更新合法序列的最长长度。
+ 如果左括号个数大于右括号个数了，那么就接着向右边扫描。
+ 如果左括号数目小于右括号个数了，那么后边无论是什么，此时都不可能是合法序列了，此时 left 和 right 归 0，然后接着扫描。

从左到右扫描完毕后，同样的方法从右到左再来一次，因为类似这样的情况 ( ( ( ) ) ，如果从左到右扫描到最后，left = 3，right = 2，期间不会出现 left == right。但是如果从右向左扫描，扫描到倒数第二个位置的时候，就会出现 left = 2，right = 2 ，就会得到一种合法序列。

```c++
class Solution {
public:
    int longestValidParentheses(string s) {
        int left(0), right(0), maxlen(0);
        for (auto x : s) {
            if ('(' == x) {
                left++;
            } else {
                right++;
            }
            if (left == right) {
                maxlen = max(maxlen, 2 * left);
            } else if (right > left) {
                left = 0;
                right = 0;
            }
        }
        left = 0;
        right = 0;
        for (int i = s.size() - 1; i >= 0; i--) {
            if ('(' == s[i]) {
                left++;
            } else {
                right++;
            }
            if (left == right) {
                maxlen = max(maxlen, 2 * left);
            } else if (left > right) {
                left = 0;
                right = 0;
            }
        }
        return maxlen;
    }
};
```