很多读者反应，就算看了[动态规划](../DS&Algo/动态规划.md)，了解了动态规划的套路，也不会写状态转移方程，没有思路，怎么办？本文就借助「最长上升子序列」来讲一种设计动态规划的通用技巧：数学归纳法。

最长上升子序列（Longest Increasing Subsequence，简写 LIS）是比较经典的一个问题，比较容易想到的是动态规划解法，时间复杂度 O(N^2)，我们借这个问题来由浅入深讲解如何写动态规划。比较难想到的是利用二分查找，时间复杂度是 O(NlogN)，我们通过一种简单的纸牌游戏来辅助理解这种巧妙的解法。

先看一下题目，很容易理解：

## 题目

给定一个无序的整数数组，找到其中最长上升子序列的长度。

示例:

    输入: [10,9,2,5,3,7,101,18]
    输出: 4 
    解释: 最长的上升子序列是 [2,3,7,101]，它的长度是 4。

说明:

+ 可能会有多种最长上升子序列的组合，你只需要输出对应的长度即可。
+ 你算法的时间复杂度应该为 O(n^2) 。

进阶: 你能将算法的时间复杂度降低到 O(n log n) 吗?

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/longest-increasing-subsequence
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

注意「子序列」和「子串」这两个名词的区别，子串一定是连续的，而子序列不一定是连续的。下面先来一步一步设计动态规划算法解决这个问题。

### 一、动态规划解法

动态规划的核心设计思想是数学归纳法。

相信大家对数学归纳法都不陌生，高中就学过，而且思路很简单。我们设计动态规划算法，不是需要一个 dp 数组吗？我们可以假设 dp[0]...dp[i-1] 都已经被算出来了，然后问自己：怎么通过这些结果算出 dp[i]？

直接拿最长上升子序列这个问题举例你就明白了。不过，首先要定义清楚 dp 数组的含义，即 dp[i] 的值到底代表着什么？

**我们的定义是这样的：dp[i] 表示以 nums[i] 这个数结尾的最长上升子序列的长度。**

根据这个定义，我们的最终结果（子序列的最大长度）应该是 dp 数组中的最大值。

```c++
int ret = 1;
for (int i = 0; i < dp.size(); i++) {
    ret = max(ret, dp[i]);
}
return ret;
```

我们应该怎么设计算法逻辑来正确计算每个 dp[i] 呢？这就是动态规划的重头戏了，要思考如何进行状态转移，这里就可以使用数学归纳的思想：

根据题目, 假设我们已经知道了 dp[0]...dp[6] 的所有结果，我们如何通过这些已知结果推出 dp[7] 呢？

    index   0   1  2  3  4  5   6   7 
    nums   [10, 9, 2, 5, 3, 7, 101, 18]
    dp      1   1  1  2  2  3   4   ?

根据刚才我们对 dp 数组的定义，现在想求 dp[7] 的值，也就是想求以 nums[7] 为结尾的最长上升子序列。

nums[7] = 18，既然是上升子序列，我们只要找到前面那些结尾比 18 小的子序列，然后把 18 接到最后，就可以形成一个新的递增子序列，而且这个新的子序列长度加一。

当然，可能形成很多种新的子序列，但是我们只要最长的，把最长子序列的长度作为 dp[7] 的值即可。

```c++
for (int j = 0; j < i; j++) {
    if (nums[j] < nums[i]) {
        dp[i] = max(dp[i], dp[j] + 1);
    }
}
```

这段代码的逻辑就可以算出 dp[7]。到这里，这道算法题我们就基本做完了。读者也许会问，我们刚才只是算了 dp[7] 呀，dp[6], dp[5] 这些怎么算呢？

类似数学归纳法，你已经可以算出 dp[7] 了，其他的就都可以算出来：

```c++
for (int i = 1; i < nums.size(); i++) {
    for (int j = 0; j < i; j++) {
        if (nums[j] < nums[i]) {
            dp[i] = max(dp[i], dp[j] + 1);
        }
    }
}
```

还有一个细节问题，dp 数组应该全部初始化为 1，因为子序列最少也要包含自己，所以长度最小为 1。下面我们看一下完整代码：

```c++
class Solution {
public:
    int lengthOfLIS(vector<int> &nums) {
        // 特列, 空数组size为0, 不能创建dp[0]的数组
        if (nums.empty()) return 0;
        int size = nums.size();
        int dp[size];
        for (auto &x : dp) {
            x = 1;
        }
        for (int i = 1; i < size; i++) {
            for (int j = 0; j < i; j++) {
                if (nums[j] < nums[i]) {
                    dp[i] = max(dp[i], dp[j] + 1);
                }
            }
        }
        int ret = 1;
        for (auto x : dp) {
            ret = max(ret, x);
        }
        return ret;
    }
};
```

python3代码如下:

``` py3
def lengthOfLIS(self, nums: List[int]) -> int:
    n = len(nums)
    ## dp 数组全部初始化为1
    dp = [1 for _ in range(n)]
    for i in range(n):
        for j in range(i):
            if nums[i] > nums[j]:
                dp[i] = max(dp[i],dp[j]+1)
    res = 0
    for temp in dp:
        res = max(temp,res)
    return res
```

至此，这道题就解决了，时间复杂度 O(N^2)。总结一下动态规划的设计流程：

1. 首先明确 dp 数组所存数据的含义。这步很重要，如果不得当或者不够清晰，会阻碍之后的步骤。

2. 然后根据 dp 数组的定义，运用数学归纳法的思想，假设 dp[0]...dp[i-1] 都已知，想办法求出 dp[i]，一旦这一步完成，整个题目基本就解决了。

3. 但如果无法完成这一步，很可能就是 dp 数组的定义不够恰当，需要重新定义 dp 数组的含义；或者可能是 dp 数组存储的信息还不够，不足以推出下一步的答案，需要把 dp 数组扩大成二维数组甚至三维数组。

4. 最后想一想问题的 base case 是什么，以此来初始化 dp 数组，以保证算法正确运行。

二分查找法详见[这里](https://github.com/labuladong/fucking-algorithm/blob/master/%E5%8A%A8%E6%80%81%E8%A7%84%E5%88%92%E7%B3%BB%E5%88%97/%E5%8A%A8%E6%80%81%E8%A7%84%E5%88%92%E8%AE%BE%E8%AE%A1%EF%BC%9A%E6%9C%80%E9%95%BF%E9%80%92%E5%A2%9E%E5%AD%90%E5%BA%8F%E5%88%97.md)