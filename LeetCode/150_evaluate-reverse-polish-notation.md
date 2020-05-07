根据逆波兰表示法，求表达式的值。

有效的运算符包括 +, -, *, / 。每个运算对象可以是整数，也可以是另一个逆波兰表达式。

说明：

+ 整数除法只保留整数部分。
+ 给定逆波兰表达式总是有效的。换句话说，表达式总会得出有效数值且不存在除数为 0 的情况。

示例 1：

    输入: ["2", "1", "+", "3", "*"]
    输出: 9
    解释: ((2 + 1) * 3) = 9

示例 2：

    输入: ["4", "13", "5", "/", "+"]
    输出: 6
    解释: (4 + (13 / 5)) = 6

示例 3：

    输入: ["10", "6", "9", "3", "+", "-11", "*", "/", "*", "17", "+", "5", "+"]
    输出: 22
    解释: 
      ((10 * (6 / ((9 + 3) * -11))) + 17) + 5
    = ((10 * (6 / (12 * -11))) + 17) + 5
    = ((10 * (6 / -132)) + 17) + 5
    = ((10 * 0) + 17) + 5
    = (0 + 17) + 5
    = 17 + 5
    = 22

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/evaluate-reverse-polish-notation
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        stack<int> numStack;
        int res = 0;
        for (auto str : tokens)
        {
            if ("+" == str || "-" == str || "*" == str || "/" == str)
            {
                int second = numStack.top();
                numStack.pop();
                int first = numStack.top();
                numStack.pop();
                if ("+" == str)
                {
                    res = first + second;
                }
                else if ("-" == str)
                {
                    res = first - second;
                }
                else if ("*" == str)
                {
                    res = first * second;
                }
                else
                {
                    res = first / second;
                }
                // NOTE: 计算后的值还要入栈, 我第一次写就忘了
                numStack.push(res);
            }
            else
            {
                stringstream ss(str);
                ss >> res;
                numStack.push(res);
            }
        }
        // NOTE: 不能return res, 否则只输入一个数字的情况结果不正确
        return numStack.top();
    }
};
```
