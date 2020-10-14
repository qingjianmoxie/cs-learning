给定两个以字符串形式表示的非负整数 num1 和 num2，返回 num1 和 num2 的乘积，它们的乘积也表示为字符串形式。

示例 1:

    输入: num1 = "2", num2 = "3"
    输出: "6"

示例 2:

    输入: num1 = "123", num2 = "456"
    输出: "56088"

说明：

1. num1 和 num2 的长度小于110。
2. num1 和 num2 只包含数字 0-9。
3. num1 和 num2 均不以零开头，除非是数字 0 本身。
4. **不能使用任何标准库的大数类型（比如 BigInteger）或直接将输入转换为整数来处理。**

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/multiply-strings
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    string multiply(string num1, string num2) {
        
    }
};
```

## 题解

看到题目, 首先想到的是将竖式乘法写为代码的形式.
这个过程看似简单，但是其中涉及乘法进位，涉及错位相加，还涉及加法进位；而且还有一些不易察觉的问题，比如说两位数乘以两位数，结果可能是四位数，也可能是三位数，你怎么想出一个标准化的处理方式？没有一定的编码能力, 这些都在困扰着我.

```c++
class Solution {
public:
    string multiply(string num1, string num2) {
        int size1 = num1.size();
        int size2 = num2.size();
        if (0 == size1 || 0 == size2) {
            return "";
        }
        if ("0" == num1 || "0" == num2) {
            return "0";
        }
        string res = "0";
        for (int j = size2 - 1; j >= 0; --j) {
            // 每次算出的值, 倒序
            string tmp = "";
            for (int i = 0; i < size2 - 1 - j; ++i) {
                tmp += '0';
            }
            int carry = 0;
            int n2 = num2[j] - '0';
            for (int i = size1 - 1; i >= 0; --i) {
                int n1 = num1[i] - '0';
                // 当前的乘积 + 上一位乘积的十位
                int sum = n1 * n2 + carry;
                tmp += '0' + sum % 10;
                carry = sum / 10;
            }
            if (0 != carry) {
                tmp += '0' + carry;
            }
            // 将该列结果与res求和
            res = add(res, tmp);
        }
        reverse(res.begin(), res.end());
        return res;
    }
    string add(const string& str1, const string& str2) {
        string res = "";
        int carry = 0;
        int i(0), j(0);
        while (i < str1.size() || j < str2.size() || 0 != carry) {
            int x = i < str1.size() ? str1[i] - '0' : 0;
            int y = j < str2.size() ? str2[j] - '0' : 0;
            int sum = x + y + carry;
            res += '0' + sum % 10;
            carry = sum / 10;
            ++i;
            ++j;
        }
        return res;
    }
};
```

复杂度分析

+ 时间复杂度：O(M N)。M,N 分别为 num1 和 num2 的长度。
+ 空间复杂度：O(M+N)。用于存储计算结果。

### 优化的竖式乘法

在竖式乘法运算过程中, 我们可以用一个数组在底下接收相加结果. 而这个数组多长呢? 不难想出, M位数字 与 N位数字 的乘积最大为 M+N.

整个计算过程大概是这样，有两个指针 `i，j` 在 `num1` 和 `num2` 上游走，计算乘积，同时将乘积叠加到 `res` 的正确位置.

现在还有一个关键问题，如何将乘积叠加到 `res` 的正确位置，或者说，如何通过 `i，j` 计算 `res` 的对应索引呢？

其实，细心观察之后就发现，`num1[i]` 和 `num2[j]` 的乘积对应的就是 `res[i+j]` 和 `res[i+j+1]` 这两个位置。

明白了这一点，就可以用代码模仿出这个计算过程了：

```c++
class Solution {
public:
    string multiply(string num1, string num2) {
        int size1 = num1.size();
        int size2 = num2.size();
        if (0 == size1 || 0 == size2) {
            return "";
        }
        if ("0" == num1 || "0" == num2) {
            return "0";
        }
        vector<int> ans(size1 + size2, 0);
        for (int i = size1 - 1; i >= 0; --i) {
            int n1 = num1[i] - '0';
            for (int j = size2 - 1; j >= 0; --j) {
                int n2 = num2[j] - '0';
                int sum = ans[i + j + 1] + n1 * n2;
                ans[i + j + 1] = sum % 10;
                ans[i + j] += sum / 10;
            }
        }

        string res = "";
        // 结果前缀可能存的 0（未使用的位）
        int i = 0;
        if (0 == ans[i]) i++;
        for (; i < ans.size(); ++i) {
            res += '0' + ans[i];
        }
        return res;
    }
};
```

复杂度分析

+ 时间复杂度：O(M N)。M,N 分别为 num1 和 num2 的长度。
+ 空间复杂度：O(M+N)。用于存储计算结果。

虽然两者时间复杂度和空间复杂度相同，但优化竖式执行速度提高很明显，普通竖式耗时主要还是对每步计算的字符串相加这个过程。
