给两个整数数组 A 和 B ，返回两个数组中公共的、长度最长的子数组的长度。

示例 1:

    输入:
    A: [1,2,3,2,1]
    B: [3,2,1,4,7]
    输出: 3
    解释: 
    长度最长的公共子数组是 [3, 2, 1]。

说明:

1. 1 <= len(A), len(B) <= 1000
2. 0 <= A[i], B[i] < 100

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/maximum-length-of-repeated-subarray
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 提示

Use dynamic programming. dp[i][j] will be the answer for inputs A[i:], B[j:].

## 题解

dp[i][j]表示以A[i]开头的数组, 和以B[j]开头的数组, 二者的最大公共前缀序列长度(公共序列必须以A[i]和B[j]为开头).

状态转移方程为:
如果 A[i] != B[j] ， dp[i][j] = 0
如果 A[i] == B[j] ， dp[i][j] = dp[i+1][j+1] + 1
base case：如果 i==A.size() || j==B.size() ，即其中一个长度为0，空子数组，没有公共长度，dp[i][j] = 0

有了状态转移方程，从 base case 可递推出 二维数组 dp 的每一项

最长公共子序列，以哪一项为开头都有可能，所以每个 dp[i][j] 都可能是最大值

```c++
class Solution {
public:
    int findLength(vector<int> &A, vector<int> &B) {
        int size_a = A.size();
        int size_b = B.size();
        if (0 == size_a || 0 == size_b) return 0;
        int dp[size_a + 1][size_b + 1];
        int res = 0;
        for (int i = 0; i <= size_a; i++) {
            dp[i][size_b] = 0;
        }
        for (int j = 0; j <= size_b; j++) {
            dp[size_a][j] = 0;
        }
        for (int i = size_a - 1; i >= 0; i--) {
            for (int j = size_b - 1; j >= 0; j--) {
                if (A[i] != B[j])
                    dp[i][j] = 0;
                else
                    dp[i][j] = dp[i + 1][j + 1] + 1;
                res = max(res, dp[i][j]);
            }
        }
        return res;
    }
};
```
