设计一个支持 push ，pop ，top 操作，并能在常数时间内检索到最小元素的栈。

+ push(x) —— 将元素 x 推入栈中。
+ pop() —— 删除栈顶的元素。
+ top() —— 获取栈顶元素。
+ getMin() —— 检索栈中的最小元素。
 

示例:

    输入：
    ["MinStack","push","push","push","getMin","pop","top","getMin"]
    [[],[-2],[0],[-3],[],[],[],[]]

    输出：
    [null,null,null,null,-3,null,0,-2]

    解释：
    MinStack minStack = new MinStack();
    minStack.push(-2);
    minStack.push(0);
    minStack.push(-3);
    minStack.getMin();   --> 返回 -3.
    minStack.pop();
    minStack.top();      --> 返回 0.
    minStack.getMin();   --> 返回 -2.

提示：

pop、top 和 getMin 操作总是在 **非空栈** 上调用。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/min-stack
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。


## 题解

这道题的思想很简单：“以空间换时间”，使用**辅助栈**是常见的做法。

**思路分析：**

在代码实现的时候有两种方式：

1、辅助栈和数据栈同步

特点：编码简单，不用考虑一些边界情况，就有一点不好：辅助栈可能会存一些“不必要”的元素。

2、辅助栈和数据栈不同步

特点：由“辅助栈和数据栈同步”的思想，我们知道，当数据栈进来的数越来越大的时候，我们要在辅助栈顶放置和当前辅助栈顶一样的元素，这样做有点“浪费”。基于这一点，我们做一些“优化”，但是在编码上就要注意一些边界条件。

（1）辅助栈为空的时候，必须放入新进来的数；

（2）新来的数小于或者等于辅助栈栈顶元素的时候，才放入，特别注意**这里“等于”要考虑进去，因为出栈的时候，连续的、相等的并且是最小值的元素要同步出栈**；

（3）出栈的时候，辅助栈的栈顶元素**等于**数据栈的栈顶元素，才出栈。

总结一下：**出栈时，最小值出栈才同步；入栈时，最小值入栈才同步。**

对比：个人觉得“同步栈”的方式更好一些，因为思路清楚，因为所有操作都同步进行，所以调试代码、定位问题也简单。“不同步栈”，虽然减少了一些空间，但是在“出栈”、“入栈”的时候还要做判断，也有性能上的消耗。

**方法一：辅助栈和数据栈同步**

```py
class MinStack:

    # 辅助栈和数据栈同步
    # 思路简单不容易出错

    def __init__(self):
        """
        initialize your data structure here.
        """
        # 数据栈
        self.data = []
        # 辅助栈
        self.helper = []

    def push(self, x: int) -> None:
        self.data.append(x)
        if len(self.helper) == 0 or x <= self.helper[-1]:
            self.helper.append(x)
        else:
            self.helper.append(self.helper[-1])

    def pop(self) -> None:
        if self.data:
            self.data.pop()
            self.helper.pop()

    def top(self) -> int:
        if self.data:
            return self.data[-1]
        else:
            return None

    def getMin(self) -> int:
        if self.helper:
            return self.helper[-1]
        else:
            return None

# Your MinStack object will be instantiated and called as such:
# obj = MinStack()
# obj.push(x)
# obj.pop()
# param_3 = obj.top()
# param_4 = obj.getMin()
```

**复杂度分析：**

+ 时间复杂度：O(1)，“出栈”、“入栈”、“查看栈顶元素”的操作不论数据规模多大，都只是有限个步骤，因此时间复杂度是：O(1)。
+ 空间复杂度：O(N)，这里 N 是读出的数据的个数。

**方法二：辅助栈和数据栈不同步**

```py
class MinStack:

    # 辅助栈和数据栈不同步
    # 关键 1：辅助栈的元素空的时候，必须放入新进来的数
    # 关键 2：新来的数小于或者等于辅助栈栈顶元素的时候，才放入（特别注意这里等于要考虑进去）
    # 关键 3：出栈的时候，辅助栈的栈顶元素等于数据栈的栈顶元素，才出栈，即"出栈保持同步"就可以了

    def __init__(self):
        """
        initialize your data structure here.
        """
        # 数据栈
        self.data = []
        # 辅助栈
        self.helper = []

    def push(self, x: int) -> None:
        self.data.append(x)
        # 关键 1 和关键 2
        if len(self.helper) == 0 or x <= self.helper[-1]:
            self.helper.append(x)

    def pop(self) -> None:
        if self.data:
            top = self.data.pop()
        # 关键 3
        if top == self.helper[-1]:
            self.helper.pop()

    def top(self) -> int:
        if self.data:
            return self.data[-1]
        else:
            return None

    def getMin(self) -> int:
        if self.helper:
            return self.helper[-1]
        else:
            return None

# Your MinStack object will be instantiated and called as such:
# obj = MinStack()
# obj.push(x)
# obj.pop()
# param_3 = obj.top()
# param_4 = obj.getMin()
```

**复杂度分析：**

+ 时间复杂度：O(1)，“出栈”、“入栈”、“查看栈顶元素”的操作不论数据规模多大，都只有有限个步骤，因此时间复杂度是：O(1)。
+ 空间复杂度：O(N)，这里 N 是读出的数据的个数。

c++版本

```c++
class MinStack {
public:
    stack<int> s;   //数据栈
    stack<int> min; //辅助栈
    /** initialize your data structure here. */
    MinStack() {
        
    }
    
    void push(int x) {
        s.push(x);
        if (min.empty() || x <= min.top())
        {
            min.push(x);
        }
    }
    
    void pop() {
        if (s.top() == min.top())
            min.pop();
        s.pop();
    }
    
    int top() {
        return s.top();
    }
    
    int getMin() {
        return min.top();
    }
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(x);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */
 ```
