给定两个字符串形式的非负整数 num1 和num2 ，计算它们的和。

注意：

1. num1 和num2 的长度都小于 5100.
2. num1 和num2 都只包含数字 0-9.
3. num1 和num2 都不包含任何前导零。
4. 你不能使用任何內建 BigInteger 库， 也不能直接将输入的字符串转换为整数形式。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/add-strings
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

自己写的真的是又臭又长

```c++
class Solution {
   public:
    string addStrings(string num1, string num2) {
        int size1 = num1.size();
        int size2 = num2.size();
        reverse(num1.begin(), num1.end());
        reverse(num2.begin(), num2.end());
        int i = 0;
        bool carry = false;
        string res = "";
        while (i < size1 && i < size2) {
            int tmp = num1[i] - '0' + num2[i] - '0' + carry;
            if (tmp <= 9) {
                carry = false;
            } else {
                carry = true;
                tmp %= 10;
            }
            char c = '0' + tmp;
            res += c;
            i++;
        }
        for (; i < size1; i++) {
            int tmp = num1[i] - '0' + carry;
            if (tmp <= 9) {
                carry = false;
            } else {
                carry = true;
                tmp %= 10;
            }
            char c = '0' + tmp;
            res += c;
        }
        for (; i < size2; i++) {
            int tmp = num2[i] - '0' + carry;
            if (tmp <= 9) {
                carry = false;
            } else {
                carry = true;
                tmp %= 10;
            }
            char c = '0' + tmp;
            res += c;
        }
        if (carry) {
            char c = '1';
            res += c;
        }
        reverse(res.begin(), res.end());
        return res;
    }
};
```

官方题解, 学习一个

```c++
class Solution {
public:
    string addStrings(string num1, string num2) {
        int i = num1.length() - 1, j = num2.length() - 1, add = 0;
        string ans = "";
        while (i >= 0 || j >= 0 || add != 0) {
            int x = i >= 0 ? num1[i] - '0' : 0;
            int y = j >= 0 ? num2[j] - '0' : 0;
            int result = x + y + add;
            ans.push_back('0' + result % 10);
            add = result / 10;
            i -= 1;
            j -= 1;
        }
        // 计算完以后的答案需要翻转过来
        reverse(ans.begin(), ans.end());
        return ans;
    }
};
```

这题和[2_两数相加](./2_add-two-numbers.md)神似.