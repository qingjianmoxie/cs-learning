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

    }
};
```

## 题解

```c++
class Solution {
public:
    bool isValid(string s) {
        if (s.size() % 2 != 0) {
            //一但是奇数说明不是有效的括号
            return false;
        }
        stack<char> mystack;
        map<char, char> map;
        map.insert(make_pair(')', '('));
        map.insert(make_pair(']', '['));
        map.insert(make_pair('}', '{'));
        for (int i = 0; i < s.size(); i++) {
            if ('(' == s[i] || '[' == s[i] || '{' == s[i]) {
                mystack.push(s[i]);
            } else if (')' == s[i] || ']' == s[i] || '}' == s[i]) {
                if (mystack.empty()) {
                    return false;
                } else {
                    if (mystack.top() == map[s[i]]) {
                        mystack.pop();
                    } else {
                        return false;
                    }
                }
            }
        }
        if (mystack.empty()) {
            return true;
        } else {
            return false;
        }
    }
};
```

```py
def isValid(self, s: str):
        left = [] # 定义一个左栈，记录所有的左括号
        match = {'}':'{', ']':'[', ')':'('} # 定义一个字典，检查当前str是否是右括号
        right = {'}', ']', ')'} # 定义一个右括号集合，方便快速检查
        
        # 进行循环，如果当前str是左括号，则入栈；如果是右括号，则检查左栈的最后一个元素是不是
        # 与其对应。
        for x in s:
            if x in right:
                if len(left) == 0 or match[x] != left[-1]: 
                    return(False) # 如果对应的左栈元素不符（括号种类不同或左栈为空），返回False
                else:
                    left.pop() # 移除左栈最顶端的元素
            else:
                left.append(x) # 当前str是左括号，入左栈
        
        if len(left) == 0:
            return(True) # 如果左栈为空（左右括号数相等），返回True
        else:
            return(False)
```
