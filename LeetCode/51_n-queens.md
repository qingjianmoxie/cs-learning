n 皇后问题研究的是如何将 n 个皇后放置在 nxn 的棋盘上，并且使皇后彼此之间不能相互攻击。

给定一个整数 n，返回所有不同的 n 皇后问题的解决方案。

每一种解法包含一个明确的 n 皇后问题的棋子放置方案，该方案中 'Q' 和 '.' 分别代表了皇后和空位。

示例:

    输入: 4
    输出: [
     [".Q..",  // 解法 1
      "...Q",
      "Q...",
      "..Q."],

     ["..Q.",  // 解法 2
      "Q...",
      "...Q",
      ".Q.."]
    ]
    解释: 4 皇后问题存在两个不同的解法。

提示：

+ 皇后，是国际象棋中的棋子，意味着国王的妻子。皇后只做一件事，那就是“吃子”。当她遇见可以吃的棋子时，就迅速冲上去吃掉棋子。当然，她横、竖、斜都可走一到七步，可进可退。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/n-queens
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
    vector<vector<string>> res;  //存结果
    vector<string> path;         //存棋盘
public:
    vector<vector<string>> solveNQueens(int n) {
        string line(n, '.');    //既然一行一行试，那就一行一行存
        backtrack(line, 0, n);  //从第0行开始
        return res;
    }

private:
    //试某一行
    void backtrack(string& line, int row, int n) {
        if (n == path.size())  //棋盘绘制够n行存进结果，不用继续试了
        {
            res.push_back(path);
            return;
        }
        for (int i = 0; i < n; ++i)  //一格一格，每格都要试
        {
            if (checkAll(row, i, n))  //符合条件了
            {
                line[i] = 'Q';         //就把当前试的这一格放皇后
                path.push_back(line);  //然后把这一行绘制进棋盘
                line[i] = '.';  //棋盘的下一行应该是没有皇后的
                backtrack(line, row + 1, n);  //去试下一行
                path.pop_back();  //接下来要去试下一格，刚才绘制进去的那一行删掉
            }
        }
    }

    //暴力检查条件
    bool checkAll(int row, int col, int n) {
        for (int i = 1; i <= row; ++i) {
            // 左上方
            if (col - i >= 0 && path[row - i][col - i] == 'Q') return false;
            // 右上方
            if (col + i < n && path[row - i][col + i] == 'Q') return false;
            if (path[row - i][col] == 'Q') return false;
        }
        return true;
    }
};
```

```c++
class Solution {
    vector<vector<string>> res;

public:
    /* 输入棋盘边长 n，返回所有合法的放置 */
    vector<vector<string>> solveNQueens(int n) {
        // '.' 表示空，'Q' 表示皇后，初始化空棋盘。
        vector<string> board(n, string(n, '.'));
        backtrack(board, 0);
        return res;
    }

private:
    // 路径：board 中小于 row 的那些行都已经成功放置了皇后
    // 选择列表：第 row 行的所有列都是放置皇后的选择
    // 结束条件：row 超过 board 的最后一行
    void backtrack(vector<string>& board, int row) {
        // 触发结束条件
        if (row == board.size()) {
            res.push_back(board);
            return;
        }

        int n = board[row].size();
        for (int col = 0; col < n; col++) {
            // 排除不合法选择
            if (!isValid(board, row, col)) continue;
            // 做选择
            board[row][col] = 'Q';
            // 进入下一行决策
            backtrack(board, row + 1);
            // 撤销选择
            board[row][col] = '.';
        }
    }

    /* 是否可以在 board[row][col] 放置皇后？ */
    bool isValid(vector<string>& board, int row, int col) {
        int n = board.size();
        // 检查列是否有皇后互相冲突
        for (int i = 0; i < n; i++) {
            if (board[i][col] == 'Q') return false;
        }
        // 检查右上方是否有皇后互相冲突
        for (int i = row - 1, j = col + 1; i >= 0 && j < n; i--, j++) {
            if (board[i][j] == 'Q') return false;
        }
        // 检查左上方是否有皇后互相冲突
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
            if (board[i][j] == 'Q') return false;
        }
        return true;
    }
};
```
