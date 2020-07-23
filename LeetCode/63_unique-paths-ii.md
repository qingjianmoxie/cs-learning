一个机器人位于一个 m x n 网格的左上角 （起始点在下图中标记为“Start” ）。

机器人每次只能向下或者向右移动一步。机器人试图达到网格的右下角（在下图中标记为“Finish”）。

现在考虑网格中有障碍物。那么从左上角到右下角将会有多少条不同的路径？

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2018/10/22/robot_maze.png)

网格中的障碍物和空位置分别用 1 和 0 来表示。

说明：m 和 n 的值均不超过 100。

示例 1:

    输入:
    [
      [0,0,0],
      [0,1,0],
      [0,0,0]
    ]
    输出: 2
    解释:
    3x3 网格的正中间有一个障碍物。
    从左上角到右下角一共有 2 条不同的路径：
    1. 向右 -> 向右 -> 向下 -> 向下
    2. 向下 -> 向下 -> 向右 -> 向右

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/unique-paths-ii
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

我第一个想到的是回溯算法, 然而超时了.

```c++
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        if (obstacleGrid.empty()) return -1;
        // 测试用例[[1]], 结果应为0
        // 测试用例[[1,0]], 结果应为0
        if (obstacleGrid[0][0] == 1) return 0;
        int row_num = obstacleGrid.size();
        int column_num = obstacleGrid[0].size();
        int count = 0;
        int i = 0;
        int j = 0;
        backtrack(obstacleGrid, i, j, count);
        return count;
    }

    void backtrack(const vector<vector<int>>& obstacleGrid, int& i, int& j, int& count) {
        int row_num = obstacleGrid.size();
        int column_num = obstacleGrid[0].size();
        if (i == row_num - 1 && j == column_num - 1) {
            count++;
            return;
        }
        if (i < row_num - 1 && obstacleGrid[i + 1][j] != 1) {
            i++;
            backtrack(obstacleGrid, i, j, count);
            i--;
        }
        if (j < column_num - 1 && obstacleGrid[i][j + 1] != 1) {
            j++;
            backtrack(obstacleGrid, i, j, count);
            j--;
        }
        return;
    }
};
```

查找了一下题解, 都是 dfs + 备忘录 实现的.

```c++
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        if (obstacleGrid.empty()) return -1;
        if (obstacleGrid[0][0] == 1) return 0;
        int row_num = obstacleGrid.size();
        int column_num = obstacleGrid[0].size();
        int i = 0;
        int j = 0;
        vector<int> row_mem(column_num, -1);
        vector<vector<int>> mem(row_num, row_mem);
        return dfs(obstacleGrid, i, j, mem);
    }

    int dfs(const vector<vector<int>>& obstacleGrid, int i, int j, vector<vector<int>>& mem) {
        int row_num = obstacleGrid.size();
        int column_num = obstacleGrid[0].size();
        if (i == row_num - 1 && j == column_num - 1) {
            return 1;
        }
        if (-1 != mem[i][j]) {
            return mem[i][j];
        }
        int total = 0;
        if (i < row_num - 1 && obstacleGrid[i + 1][j] != 1) {
            total += dfs(obstacleGrid, i + 1, j, mem);
        }
        if (j < column_num - 1 && obstacleGrid[i][j + 1] != 1) {
            total += dfs(obstacleGrid, i, j + 1, mem);
        }
        mem[i][j] = total;
        return total;
    }
};
```

从 dfs + 备忘录 的方法就能感觉到, 这题用动态规划更好.

```c++
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        if (obstacleGrid.empty()) return -1;
        int row_num = obstacleGrid.size();
        int column_num = obstacleGrid[0].size();
        vector<int> row_dp(column_num, 0);
        vector<vector<int>> dp(row_num, row_dp);
        dp[0][0] = !obstacleGrid[0][0];
        for (int j = 1; j < column_num; j++) {
            dp[0][j] = dp[0][j - 1] && (!obstacleGrid[0][j]);
        }
        for (int i = 1; i < row_num; i++) {
            dp[i][0] = dp[i - 1][0] && (!obstacleGrid[i][0]);
        }
        for (int i = 1; i < row_num; i++) {
            for (int j = 1; j < column_num; j++) {
                dp[i][j] = obstacleGrid[i][j] ? 0 : dp[i - 1][j] + dp[i][j - 1];
            }
        }
        return dp[row_num - 1][column_num - 1];
    }
};
```
注意是使用`非!`, 我原本想使用取反运算符`~`的, 但是 `~0` 的结果是 -1.

相似题:
[64_最小路径和(medium)](./64_minimum-path-sum.md)
