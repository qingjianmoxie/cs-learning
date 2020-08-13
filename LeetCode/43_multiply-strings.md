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

写代码的时候写蒙了, 看了题解才反应过来哪里写的不对, 第一个版本:

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
            string each_res = "";
            for (int i = 0; i < size2 - 1 - j; ++i) {
                each_res += '0';
            }
            // 个位数字
            int one_figure = 0;
            // 十位数字
            int two_figure = 0;
            int right = num2[j] - '0';
            for (int i = size1 - 1; i >= 0; --i) {
                int left = num1[i] - '0';
                // 当前的乘积 + 上一位乘积的十位
                int sum = left * right + two_figure;
                one_figure = sum % 10;
                each_res += '0' + one_figure;
                two_figure = sum / 10;
            }
            if (0 != two_figure) {
                each_res += '0' + two_figure;
            }
            // 将该列结果与res求和
            res = add(res, each_res);
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

[优质题解](https://leetcode-cn.com/problems/multiply-strings/solution/you-hua-ban-shu-shi-da-bai-994-by-breezean/)

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
        for (int i = 0; i < ans.size(); ++i) {
            if (0 == i && 0 == ans[i]) continue;
            res += '0' + ans[i];
        }
        return res;
    }
};
```
