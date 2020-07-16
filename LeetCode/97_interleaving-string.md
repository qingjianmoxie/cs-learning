给定三个字符串 s1, s2, s3, 验证 s3 是否是由 s1 和 s2 交错组成的。

示例 1:

    输入: s1 = "aabcc", s2 = "dbbca", s3 = "aadbbcbcac"
    输出: true

示例 2:

    输入: s1 = "aabcc", s2 = "dbbca", s3 = "aadbbbaccc"
    输出: false

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/interleaving-string
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

根据之前学习的技巧, 一般涉及两个字符串的都可以用动态规划, 这题一开始用动态规划去想想不出来, 还是dfs比较容易想.

我形成的思路是先用dfs, 超时了就会加备忘录, 加上备忘录后, 动态规划的方法差不多也就出来了.

dfs+备忘录代码如下:

```c++
class Solution {
   private:
    bool valid;
    vector<vector<int>> dp;

   public:
    bool isInterleave(string s1, string s2, string s3) {
        int s1_size(s1.size()), s2_size(s2.size()), s3_size(s3.size());
        if (0 == s1_size && 0 == s2_size && 0 == s3_size) return true;
        if (s1_size + s2_size != s3_size) return false;
        dp.assign(s1_size + 1, vector<int>(s2_size + 1, -1));
        valid = false;
        dfs(s1, 0, s2, 0, s3, 0);
        return valid;
    }
    void dfs(const string& s1, int s1_i, const string& s2, int s2_i,
             const string& s3, int s3_i) {
        if (-1 != dp[s1_i][s2_i]) {
            return;
        }
        if (s1_i < s1.size() && s1[s1_i] == s3[s3_i]) {
            if (s1.size() - 1 == s1_i && s2.size() == s2_i &&
                s3.size() - 1 == s3_i) {
                valid = true;
                return;
            }
            if (s3_i < s3.size() - 1) {
                dfs(s1, s1_i + 1, s2, s2_i, s3, s3_i + 1);
            }
            if (valid) {
                dp[s1_i][s2_i] = 1;
                return;
            }
        }
        if (s2_i < s2.size() && s2[s2_i] == s3[s3_i]) {
            if (s1.size() == s1_i && s2.size() - 1 == s2_i &&
                s3.size() - 1 == s3_i) {
                valid = true;
                return;
            }
            if (s3_i < s3.size() - 1) {
                dfs(s1, s1_i, s2, s2_i + 1, s3, s3_i + 1);
            }
            if (valid) {
                dp[s1_i][s2_i] = 1;
                return;
            }
        }
        dp[s1_i][s2_i] = 0;
    }
};
```

动态规划的方法与[63_不同路径II(hard)](./63_unique-paths-ii.md)有异曲同工之妙.

```c++
class Solution {
public:
    bool isInterleave(string s1, string s2, string s3) {
        int s1_size(s1.size()), s2_size(s2.size()), s3_size(s3.size());
        if (0 == s1_size && 0 == s2_size && 0 == s3_size) return true;
        if (s1_size + s2_size != s3_size) return false;
        vector<vector<bool>> dp(s1_size + 1, vector<bool>(s2_size + 1));
        dp[0][0] = true;
        for (int i = 1; i <= s1_size; i++) {
            dp[i][0] = dp[i - 1][0] && s1[i - 1] == s3[i - 1];
        }
        for (int j = 1; j <= s2_size; j++) {
            dp[0][j] = dp[0][j - 1] && s2[j - 1] == s3[j - 1];
        }
        for (int i = 1; i <= s1_size; i++) {
            for (int j = 1; j <= s2_size; j++) {
                dp[i][j] = (dp[i - 1][j] && s1[i - 1] == s3[i + j - 1]) ||
                           (dp[i][j - 1] && s2[j - 1] == s3[i + j - 1]);
            }
        }
        return dp[s1_size][s2_size];
    }
};
```
