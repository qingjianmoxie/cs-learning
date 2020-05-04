给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串，判断字符串是否有效。

有效字符串需满足：

1. 左括号必须用相同类型的右括号闭合。
2. 左括号必须以正确的顺序闭合。

注意空字符串可被认为是有效字符串。

示例 1:

    输入: "()"
    输出: true

示例 2:

    输入: "()[]{}"
    输出: true

示例 3:

    输入: "(]"
    输出: false

示例 4:

    输入: "([)]"
    输出: false

示例 5:

    输入: "{[]}"
    输出: true

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/valid-parentheses
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    bool isValid(string s) {
        if (s.length() % 2 != 0)
            return false;         //一但是奇数说明不是有效的括号
        map<char, char> wordbook; //建立哈希表
        wordbook.insert(map<char, char>::value_type(')', '('));
        wordbook.insert(map<char, char>::value_type(']', '['));
        wordbook.insert(map<char, char>::value_type('}', '{'));
        stack<char> mystack; //建立栈
        for (int i = 0; i < s.length(); i++)
        {
            if (s[i] == '[' || s[i] == '{' || s[i] == '(')      //匹配到左括号
                mystack.push(s[i]);                             //放入栈中
            else if (s[i] == ']' || s[i] == '}' || s[i] == ')') //匹配到右括号
            {
                if (mystack.empty())
                    return false;
                //匹配到右括号，栈中应该存在左括号。否则就是无效的括号
                if (wordbook[s[i]] == mystack.top()) //与栈顶元素进行匹配
                    mystack.pop(); //匹配成功删除栈顶元素
                else
                    return false;
            }
        }
        if (mystack.empty())
            return true; //有效的括号到最后检测结束栈中应没有元素
        else
            return false;
    }
};
```
