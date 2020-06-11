请根据每日 气温 列表，重新生成一个列表。对应位置的输出为：要想观测到更高的气温，至少需要等待的天数。如果气温在这之后都不会升高，请在该位置用 0 来代替。

例如，给定一个列表 temperatures = [73, 74, 75, 71, 69, 72, 76, 73]，你的输出应该是 [1, 1, 4, 2, 1, 1, 0, 0]。

提示：气温 列表长度的范围是 [1, 30000]。每个气温的值的均为华氏度，都是在 [30, 100] 范围内的整数。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/daily-temperatures
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

暴力解法

```c++
class Solution {
public:
    vector<int> dailyTemperatures(vector<int> &T) {
        int size = int(T.size());
        vector<int> res;
        for (int i = 0; i < size; i++) {
            int j = i + 1;
            for (; j < size; j++) {
                if (T[j] > T[i]) {
                    res.push_back(j - i);
                    break;
                }
            }
            if (size == j) {
                res.push_back(0);
            }
        }
        return res;
    }
};
```

但是超时了.

解法二: 使用递减栈

```c++
class Solution {
public:
    vector<int> dailyTemperatures(vector<int> &T) {
        int size = int(T.size());
        vector<int> res(size, 0);
        stack<int> mystack;
        for (int i = 0; i < size; i++) {
            while (!mystack.empty() && T[i] > T[mystack.top()]) {
                int j = mystack.top();
                mystack.pop();
                res[j] = i - j;
            }
            mystack.push(i);
        }
        return res;
    }
};
```

复杂度分析
该方法只需要对数组进行一次遍历，每个元素最多被压入和弹出堆栈一次，算法复杂度是 O(n)。