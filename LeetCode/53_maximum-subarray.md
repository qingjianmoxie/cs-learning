给定一个整数数组 nums ，找到一个具有最大和的连续子数组（子数组最少包含一个元素），返回其最大和。

示例:

    输入: [-2,1,-3,4,-1,2,1,-5,4],
    输出: 6
    解释: 连续子数组 [4,-1,2,1] 的和最大，为 6。

进阶:

如果你已经实现复杂度为 O(n) 的解法，尝试使用更为精妙的分治法求解。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/maximum-subarray
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

方法1: 动态规划

dp[i]表示nums中以nums[i]结尾的最大子序和:
dp[i] = max(db[i-1]+nums[i], nums[i]);
时间复杂度O(n), 空间复杂度O(n), 可优化至O(1).

```c++
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        //类似寻找最大最小值的题目，初始值要定义成理论上的最小最大值
        int result = INT_MIN;
        int numsSize = int(nums.size());
        //dp[i]表示nums中以nums[i]结尾的最大子序和
        vector<int> dp(numsSize);
        dp[0] = nums[0];
        result = dp[0];
        for (int i = 1; i < numsSize; i++)
        {
            dp[i] = max(dp[i - 1] + nums[i], nums[i]);
            result = max(result, dp[i]);
        }
        return result;
    }
};
```

在计算dp[i]的时候, 我们只用到了前一项的值, 所以可以用int代替一维数组, 时间复杂度为O(1).

```c++
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        //类似寻找最大最小值的题目，初始值要定义成理论上的最小最大值
        int result = INT_MIN;
        int numsSize = int(nums.size());
        //因为只需要知道dp的前一项，我们用int代替一维数组
        int dp = nums[0];
        result = dp;
        for (int i = 1; i < numsSize; i++)
        {
            dp = max(dp + nums[i], nums[i]);
            result = max(result, dp);
        }
        return result;
    }
};
```

方法2: 贪心算法
时间复杂度O(n), 空间复杂度O(1)

```c++
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        //类似寻找最大最小值的题目，初始值要定义成理论上的最小最大值
        int result = INT_MIN;
        int numsSize = int(nums.size());
        int sum = 0;
        for (int i = 0; i < numsSize; i++)
        {
            sum += nums[i];
            result = result > sum ? result : sum;
            //如果sum < 0，重新开始找子序串
            if (sum < 0)
                sum = 0;
        }
        return result;
    }
};
```
