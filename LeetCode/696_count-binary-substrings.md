给定一个字符串 s，计算具有相同数量0和1的非空(连续)子字符串的数量，并且这些子字符串中的所有0和所有1都是组合在一起的。

重复出现的子串要计算它们出现的次数。

示例 1 :

    输入: "00110011"
    输出: 6
    解释: 有6个子串具有相同数量的连续1和0：“0011”，“01”，“1100”，“10”，“0011” 和 “01”。

    请注意，一些重复出现的子串要计算它们出现的次数。

    另外，“00110011”不是有效的子串，因为所有的0（和1）没有组合在一起。

示例 2 :

    输入: "10101"
    输出: 4
    解释: 有4个子串：“10”，“01”，“10”，“01”，它们具有相同数量的连续1和0。

注意：

+ s.length 在1到50,000之间。
+ s 只包含“0”或“1”字符。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/count-binary-substrings
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

暴力解法是遍历所有的子串, 判断是否合格, 优化后依然超时. 时间复杂度为O(N^3).

```c++
class Solution {
public:
    int countBinarySubstrings(string s) {
        int size = s.size();
        int count = 0;
        for (int i = 0; i < size; ++i) {
            for (int j = i + 1; j < size; j = j + 2) {
                if (isValid(s, i, j)) {
                    ++count;
                    // 如果s[i,j]是的话, 之后的子串就不可能是了
                    break;
                }
            }
        }
        return count;
    }

    bool isValid(const string& s, int left, int right) {
        if (s[left] == s[right]) {
            return false;
        }
        while (left + 1 < right - 1) {
            if (s[left] == s[left + 1] && s[right] == s[right - 1]) {
                ++left;
                --right;
            } else {
                return false;
            }
        }
        return true;
    }
};
```

尝试先判断2个字符的子串是否合格, 合格的向两边扩展, 再次判断是否合格.

```c++
class Solution {
public:
    int countBinarySubstrings(string s) {
        int size = s.size();
        int count = 0;
        for (int i = 0; i < size - 1; ++i) {
            int left = i;
            int right = i + 1;
            if (s[left] != s[right]) {
                ++count;
                while (left > 0 && right < size - 1) {
                    if (s[left] == s[left - 1] && s[right] == s[right + 1]) {
                        ++count;
                        --left;
                        ++right;
                    } else {
                        break;
                    }
                }
            }
        }
        return count;
    }
};
```
时间复杂度为O(N^2).

看了一下官方题解, 可以按字符分组

我们可以将字符串 s 按照 0 和 1 的连续段分组，存在 counts 数组中，例如 s = 00111011，可以得到这样的 counts 数组：counts={2,3,1,2}。

这里 counts 数组中两个相邻的数一定代表的是两种不同的字符。假设 counts 数组中两个相邻的数字为 u 或者 v，它们对应着 u 个 0 和 v 个 1，或者 u 个 1 和 v 个 0。它们能组成的满足条件的子串数目为 min{u,v}，即一对相邻的数字对答案的贡献。

我们只要遍历所有相邻的数对，求它们的贡献总和，即可得到答案。

实际上我们每次只使用了数组中的两个元素, 所以可以用两个变量维护当前位置和前一个位置, 进一步降低空间复杂度.

```c++
class Solution {
public:
    int countBinarySubstrings(string s) {
        int size = s.size();
        int count = 0;
        int pre = 0;
        int cur = 1;
        for (int i = 1; i < size; ++i) {
            if (s[i - 1] == s[i]) {
                ++cur;
            } else {
                count += min(pre, cur);
                pre = cur;
                cur = 1;
            }
        }
        count += min(pre, cur);
        return count;
    }
};
```

时间复杂度为O(N), 空间复杂度为O(1).
