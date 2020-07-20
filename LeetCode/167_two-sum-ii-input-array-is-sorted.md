给定一个已按照**升序排列** 的有序数组，找到两个数使得它们相加之和等于目标数。

函数应该返回这两个下标值 index1 和 index2，其中 index1 必须小于 index2。

说明:

+ 返回的下标值（index1 和 index2）不是从零开始的。
+ 你可以假设每个输入只对应唯一的答案，而且你不可以重复使用相同的元素。

示例:

    输入: numbers = [2, 7, 11, 15], target = 9
    输出: [1,2]
    解释: 2 与 7 之和等于目标数 9 。因此 index1 = 1, index2 = 2 。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/two-sum-ii-input-array-is-sorted
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

看到题目就想到用双指针, 但是写的代码执行用时排名较低, 代码如下:

```c++
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        vector<int> res;
        int size = numbers.size();
        if (0 == size) return res;
        int left(0), right(size - 1);
        while (left < right) {
            if (numbers[left] + numbers[right] > target) {
                right--;
            } else if (numbers[left] + numbers[right] < target) {
                left++;
            } else {
                res.push_back(left + 1);
                res.push_back(right + 1);
                break;
            }
        }
        return res;
    }
};
```

可以优化的点大致有3条:

1. 异常情况空数组可以不用单独判断, 之后代码已经cover了
2. 两个数的和可以存为变量, 不然每判断一个分支都要计算一遍
3. vector使用push_back会增加耗时

修改的代码如下:

```c++
class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        int left(0), right(numbers.size() - 1);
        while (left < right) {
            int sum = numbers[left] + numbers[right];
            if (sum > target) {
                right--;
            } else if (sum < target) {
                left++;
            } else {
                return { left + 1, right + 1 };
            }
        }
        return {};
    }
};
```
