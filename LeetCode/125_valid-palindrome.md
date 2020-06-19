给定一个字符串，验证它是否是回文串，只考虑字母和数字字符，可以忽略字母的大小写。

说明：本题中，我们将空字符串定义为有效的回文串。

示例 1:

    输入: "A man, a plan, a canal: Panama"
    输出: true

示例 2:

    输入: "race a car"
    输出: false

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/valid-palindrome
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

c++判断数字字母的几个函数:

+ islower(char c) 是否为小写字母
+ isupper(char c) 是否为大写字母
+ isdigit(char c) 是否为数字
+ isalpha(char c) 是否为字母
+ isalnum(char c) 是否为字母或者数字
+ toupper(char c) 字母小转大
+ tolower(char c) 字母大转小


自己写的算法

```c++
class Solution {
public:
    bool isPalindrome(string s) {
        int size = int(s.size());
        int i = 0;
        int j = size - 1;
        while (i < j) {
            while (i < j && !isalnum(s[i])) { i++; }
            while (i < j &&!isalnum(s[j])) { j--; }
            if (tolower(s[i++]) != tolower(s[j--])) {
                return false;
            }
        }
        return true;
    }
};
```

高赞题解的算法

```c++
class Solution {
public:
    bool isPalindrome(string s) {
        string tmp;
        for (auto c : s) {
            if (islower(c) || isdigit(c))  tmp += c;
            else if (isupper(c)) tmp += (c + 32);
        }
        int i = 0, j = tmp.size() - 1;
        while (i < j) {
            if (tmp[i] != tmp[j]) return false;
            i++;
            j--;
        }
        return true;
    }
};
```

看似我的代码有三个循环, 高赞的代码只有两个循环. 实际上我的代码只遍历了一次, 高赞遍历了两次. 所以我的代码耗时比较短.