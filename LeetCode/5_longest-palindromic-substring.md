给定一个字符串 s，找到 s 中最长的回文子串。你可以假设 s 的最大长度为 1000。

示例 1：

输入: "babad"
输出: "bab"
注意: "aba" 也是一个有效答案。
示例 2：

输入: "cbbd"
输出: "bb"

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/longest-palindromic-substring
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    string longestPalindrome(string s) {
        
    }
};
```

## 题解

### 方法一：暴力匹配 (Brute Force)

+ 根据回文子串的定义，枚举所有长度大于等于 2 的子串，依次判断它们是否是回文；
+ 在具体实现时，可以只针对大于“当前得到的最长回文子串长度”的子串进行“回文验证”；
+ 在记录最长回文子串的时候，可以只记录“当前子串的起始位置”和“子串长度”，不必做截取。

说明：暴力解法时间复杂度高，但是思路清晰、编写简单。由于编写正确性的可能性很大，**可以使用暴力匹配算法检验我们编写的其它算法是否正确**。优化的解法在很多时候，是基于“暴力解法”，以空间换时间得到的，因此思考清楚暴力解法，分析其缺点，很多时候能为我们打开思路。

```c++
class Solution {
private:
    bool valid(string s, int left, int right) {
        // 验证子串 s[left, right] 是否为回文串
        while (left < right) {
            if (s[left] != s[right]) {
                return false;
            }
            left++;
            right--;
        }
        return true;
    }

public:
    string longestPalindrome(string s) {
        int size = s.size();
        if (size < 2) {
            return s;
        }

        int maxLen = 1;
        string res = s.substr(0, 1);

        // 枚举所有长度大于等于 2 的子串
        for (int i = 0; i < size - 1; i++) {
            for (int j = i + 1; j < size; j++) {
                if (j - i + 1 > maxLen && valid(s, i, j)) {
                    maxLen = j - i + 1;
                    res = s.substr(i, maxLen);
                }
            }
        }
        return res;
    }
};
```

复杂度分析：

+ 时间复杂度：O(N^3)，这里 N 是字符串的长度，枚举字符串的左边界、右边界，然后继续验证子串是否是回文子串，这三种操作都与 N 相关；
+ 空间复杂度：O(1)，只使用到常数个临时变量，与字符串长度无关。

### 方法二：动态规划

这道题比较烦人的是判断回文子串。因此需要一种能够快速判断原字符串的所有子串是否是回文子串的方法，于是想到了「动态规划」。

「动态规划」的一个关键的步骤是想清楚「状态如何转移」。事实上，「回文」天然具有「状态转移」性质。

+ 一个回文去掉两头以后，剩下的部分依然是回文（这里暂不讨论边界情况）；

依然从回文串的定义展开讨论：

+ 如果一个字符串的头尾两个字符都不相等，那么这个字符串一定不是回文串；
+ 如果一个字符串的头尾两个字符相等，才有必要继续判断下去。
  + 如果里面的子串是回文，整体就是回文串；
  + 如果里面的子串不是回文串，整体就不是回文串。

即：**在头尾字符相等的情况下，里面子串的回文性质据定了整个子串的回文性质**，这就是状态转移。因此可以把「状态」定义为原字符串的一个子串是否为回文子串。

**第 1 步：定义状态**

dp[i][j] 表示子串 s[i..j] 是否为回文子串，这里子串 s[i..j] 定义为左闭右闭区间，可以取到 s[i] 和 s[j]。

**第 2 步：思考状态转移方程**

在这一步分类讨论（根据头尾字符是否相等），根据上面的分析得到：

    dp[i][j] = (s[i] == s[j]) and dp[i + 1][j - 1]

说明：

+ 「动态规划」事实上是在填一张二维表格，由于构成子串，因此 i 和 j 的关系是 i <= j ，因此，只需要填这张表格对角线以上的部分。

+ 看到 dp[i + 1][j - 1] 就得考虑边界情况。

  + 如果子串 s[i + 1..j - 1] 只有 1 个字符，即去掉两头，剩下中间部分只有 1 个字符，显然是回文；
  + 如果子串 s[i + 1..j - 1] 为空串，那么子串 s[i, j] 一定是回文子串。

因此，在 s[i] == s[j] 成立和 j - i < 3 的前提下，直接可以下结论，dp[i][j] = true，否则才执行状态转移。

**第 3 步：考虑初始化**

初始化的时候，单个字符一定是回文串，因此把对角线先初始化为 true，即 dp[i][i] = true 。

事实上，初始化的部分都可以省去。因为只有一个字符的时候一定是回文，dp[i][i] 根本不会被其它状态值所参考。

**第 4 步：考虑输出**

只要一得到 dp[i][j] = true，就记录子串的长度和起始位置，没有必要截取，这是因为截取字符串也要消耗性能，记录此时的回文子串的「起始位置」和「回文长度」即可。

**第 5 步：考虑优化空间**

因为在填表的过程中，只参考了左下方的数值。事实上可以优化，但是增加了代码编写和理解的难度，丢失可读和可解释性。在这里不优化空间。

注意事项：总是先得到小子串的回文判定，然后大子串才能参考小子串的判断结果，即填表顺序很重要。

大家能够可以自己动手，画一下表格，相信会对「动态规划」作为一种「表格法」有一个更好的理解。

```c++
class Solution {
   public:
    string longestPalindrome(string s) {
        int size = int(s.size());
        if (size < 2) {
            return s;
        }

        bool dp[size][size];
        int max_len = 1;
        int start = 0;
        for (int j = 1; j < size; j++) {
            for (int i = 0; i < j; i++) {
                if (s[i] == s[j]) {
                    if (j - i < 3) {
                        dp[i][j] = true;
                    } else {
                        dp[i][j] = dp[i + 1][j - 1];
                    }
                } else {
                    dp[i][j] = false;
                }
                if (dp[i][j]) {
                    int cur_len = j - i + 1;
                    if (cur_len > max_len) {
                        max_len = cur_len;
                        start = i;
                    }
                }
            }
        }

        return s.substr(start, max_len);
    }
};
```

```py
class Solution:
    def longestPalindrome(self, s: str) -> str:
        size = len(s)
        if size < 2:
            return s

        dp = [[False for _ in range(size)] for _ in range(size)]

        max_len = 1
        start = 0

        for i in range(size):
            dp[i][i] = True

        for j in range(1, size):
            for i in range(0, j):
                if s[i] == s[j]:
                    if j - i < 3:
                        dp[i][j] = True
                    else:
                        dp[i][j] = dp[i + 1][j - 1]
                else:
                    dp[i][j] = False

                if dp[i][j]:
                    cur_len = j - i + 1
                    if cur_len > max_len:
                        max_len = cur_len
                        start = i
        return s[start:start + max_len]
```

复杂度分析：

+ 时间复杂度：O(N^2)。
+ 空间复杂度：O(N^2)，二维 dp 问题，一个状态得用二维有序数对表示，因此空间复杂度是 O(N^2)。

### 方法三：中心扩散法

暴力法采用双指针两边夹，验证是否是回文子串。

除了枚举字符串的左右边界以外，比较容易想到的是枚举可能出现的回文子串的“中心位置”，从“中心位置”尝试尽可能扩散出去，得到一个回文串。

因此中心扩散法的思路是：遍历每一个索引，以这个索引为中心，利用“回文串”中心对称的特点，往两边扩散，看最多能扩散多远。

枚举“中心位置”时间复杂度为 O(N)，从“中心位置”扩散得到“回文子串”的时间复杂度为 O(N)，因此时间复杂度可以降到 O(N^2)。空间复杂度为O(1), 这道题是少有的动态规划非最优解法的问题。

在这里要注意一个细节：回文串在长度为奇数和偶数的时候，“回文中心”的形式是不一样的。

奇数回文串的“中心”是一个具体的字符，例如：回文串 "aba" 的中心是字符 "b"；
偶数回文串的“中心”是位于中间的两个字符之间，例如：回文串 "abba" 的中心是两个 "b" 之间。

我们可以设计一个方法，兼容以上两种情况：

1、如果传入重合的索引编码，进行中心扩散，此时得到的回文子串的长度是奇数；

2、如果传入相邻的索引编码，进行中心扩散，此时得到的回文子串的长度是偶数。

具体编码细节在以下的代码的注释中体现。

```c++
class Solution {
private:
    string centerSpread(string s, int left, int right) {
        // left = right的时候，此时回文中心是一个字符，向两边扩散得到的回文子串的长度是奇数
        // right = left + 1的时候，此时回文中心在两字符之间，向两边扩散得到的回文子串的长度是偶数
        int size = s.size();
        int i = left;
        int j = right;
        while (i >= 0 && j < size) {
            if (s[i] == s[j]) {
                i--;
                j++;
            } else {
                break;
            }
        }
        // 这里要小心，跳出 while 循环时，恰好满足 s.charAt(i) !=
        // s.charAt(j)，因此不能取 i，不能取 j
        return s.substr(i + 1, j - i - 1);
    }

public:
    string longestPalindrome(string s) {
        int size = s.size();
        if (size < 2) {
            return s;
        }

        int maxLen = 1;
        string res = s.substr(0, 1);

        // 中心位置枚举到 len - 2 即可
        for (int i = 0; i < size - 1; i++) {
            string oddStr = centerSpread(s, i, i);
            string evenStr = centerSpread(s, i, i + 1);
            string maxLenStr = oddStr.size() > evenStr.size() ? oddStr : evenStr;
            if (maxLenStr.length() > maxLen) {
                maxLen = maxLenStr.size();
                res = maxLenStr;
            }
        }
        return res;
    }
};
```

实际上这是代码合并后的样子, 而且效率也不佳, 因为每个循环中都有substr操作. 面试时没有时间封装函数, 按最容易理解的方式来写.

```c++
class Solution {
public:
    string longestPalindrome(string s) {
        int size = s.size();
        int max_len(1), res_left(0), res_right(0);
        for (int i = 1; i < size - 1; ++i) {
            int left(i), right(i);
            while (left >= 0 && right < size) {
                if (s[left] == s[right]) {
                    --left;
                    ++right;
                } else {
                    break;
                }
            }
            ++left;
            --right;
            if (right - left > res_right - res_left) {
                res_left = left;
                res_right = right;
            }
        }
        for (int i = 0; i < size - 1; ++i) {
            int left(i), right(i + 1);
            while (left >= 0 && right < size) {
                if (s[left] == s[right]) {
                    --left;
                    ++right;
                } else {
                    break;
                }
            }
            ++left;
            --right;
            if (right - left > res_right - res_left) {
                res_left = left;
                res_right = right;
            }
        }
        return s.substr(res_left, res_right - res_left + 1);
    }
};
```
