给定一个三角形，找出自顶向下的最小路径和。每一步只能移动到下一行中相邻的结点上。

**相邻的结点** 在这里指的是 下标 与 上一层结点下标 相同或者等于 上一层结点下标 + 1 的两个结点。

例如，给定三角形：

    [
         [2],
        [3,4],
       [6,5,7],
      [4,1,8,3]
    ]

自顶向下的最小路径和为 11（即，2 + 3 + 5 + 1 = 11）。 

说明：

如果你可以只使用 O(n) 的额外空间（n 为三角形的总行数）来解决这个问题，那么你的算法会很加分。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/triangle
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

首先想到的是dfs, 但是超时了, 需要加备忘录.

```c++
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        if (triangle.empty()) return 0;
        int res = dfs(triangle, 0, 0);
        return res;
    }

    int dfs(const vector<vector<int>>& triangle, int i, int j) {
        int res = INT_MAX;
        if (i == triangle.size() - 1) {
            res = triangle[i][j];
        } else {
            res = min(triangle[i][j] + dfs(triangle, i + 1, j),
                      triangle[i][j] + dfs(triangle, i + 1, j + 1));
        }
        return res;
    }
};
```

我想了一下, 结合这道题的具体描述, 可以从下往上计算. 比如最后一行的第1列和第二列, 一定是其中的较小者所在的路径路径和较小.

```c++
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        if (triangle.empty()) return 0;
        // 行数
        int n = triangle.size();
        vector<int> res(n, 0);
        for (int i = n - 1; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                res[j] += triangle[i][j];
            }
            for (int j = 0; j < i; j++) {
                res[j] = min(res[j], res[j+1]);
            }
        }
        return res[0];
    }
};
```
