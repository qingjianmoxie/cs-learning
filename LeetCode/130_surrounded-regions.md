给定一个二维的矩阵，包含 'X' 和 'O'（字母 O）。

找到所有被 'X' 围绕的区域，并将这些区域里所有的 'O' 用 'X' 填充。

示例:

    X X X X
    X O O X
    X X O X
    X O X X

运行你的函数后，矩阵变为：

    X X X X
    X X X X
    X X X X
    X O X X

解释:

被围绕的区间不会存在于边界上，换句话说，任何边界上的 'O' 都不会被填充为 'X'。 任何不在边界上，或不与边界上的 'O' 相连的 'O' 最终都会被填充为 'X'。如果两个元素在水平或垂直方向相邻，则称它们是“相连”的。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/surrounded-regions
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    void solve(vector<vector<char>>& board) {
        
    }
};
```

## 题解

这道题我们拿到基本就可以确定是图的 dfs、bfs 遍历的题目了。题目中解释说被包围的区间不会存在于边界上，所以我们会想到边界上的 O 要特殊处理，只要把边界上的 O 特殊处理了，那么剩下的 O 替换成 X 就可以了。问题转化为，**如何寻找和边界联通的 O**，我们需要考虑如下情况。

    X X X X
    X O O X
    X X O X
    X O O X

这时候的 O 是不做替换的。因为和边界是连通的。为了记录这种状态，我们把这种情况下的 O 换成 # 作为占位符，待搜索结束之后，遇到 O 替换为 X（和边界不连通的 O）；遇到 #，替换回 O(和边界连通的 O)。

如何寻找和边界联通的O? 从边界出发，对图进行 dfs 和 bfs 即可。这里简单总结下 bfs 和 dfs。

+ bfs 递归。可以想想二叉树中如何递归的进行层序遍历。
+ bfs 非递归。一般用队列存储。
+ dfs 递归。最常用，如二叉树的先序遍历。
+ dfs 非递归。一般用 stack。

那么基于上面这种想法，我们有四种方式实现。

**dfs递归**
```c++
class Solution {
public:
    void solve(vector<vector<char>>& board) {
        int width = board.size();
        if (0 == width) return;
        int length = board[0].size();
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                bool isEdge = 0 == i || 0 == j || width - 1 == i || length - 1 == j;
                if (isEdge && 'O' == board[i][j]) {
                    dfs(board, i, j);
                }
            }
        }
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                if ('O' == board[i][j]) {
                    board[i][j] = 'X';
                } else if ('#' == board[i][j]) {
                    board[i][j] = 'O';
                }
            }
        }
    }

    void dfs(vector<vector<char>>& board, int i, int j) {
        if (i < 0 || j < 0 || i >= board.size() || j >= board[0].size() ||
            'X' == board[i][j] || '#' == board[i][j]) {
            // board[i][j] == '#' 说明已经搜索过了
            return;
        }
        board[i][j] = '#';
        dfs(board, i - 1, j);
        dfs(board, i + 1, j);
        dfs(board, i, j - 1);
        dfs(board, i, j + 1);
    }
};
```

**dfs 非递归**
非递归的方式，我们需要记录每一次遍历过的位置，我们用 stack 来记录，因为它先进后出的特点。而位置我们定义一个内部结构体 Pos 来标记横坐标和纵坐标。注意的是，在写非递归的时候，我们每次查看 stack 顶，但是并不出 stack，直到这个位置上下左右都搜索不到的时候出 Stack

```c++
class Solution {
public:
    struct Pos {
        int i;
        int j;
        Pos(int x, int y) : i(x), j(y) {}
    };
    void solve(vector<vector<char>>& board) {
        int width = board.size();
        if (0 == width) return;
        int length = board[0].size();
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                bool isEdge =
                    0 == i || 0 == j || width - 1 == i || length - 1 == j;
                if (isEdge && 'O' == board[i][j]) {
                    dfs(board, i, j);
                }
            }
        }
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                if ('O' == board[i][j]) {
                    board[i][j] = 'X';
                } else if ('#' == board[i][j]) {
                    board[i][j] = 'O';
                }
            }
        }
    }
    void dfs(vector<vector<char>>& board, int i, int j) {
        stack<Pos> s;
        s.push(Pos(i, j));
        board[i][j] = '#';
        while (!s.empty()) {
            // 取出当前stack 顶, 不弹出
            Pos cur = s.top();
            // 上
            if (cur.i - 1 >= 0 && 'O' == board[cur.i - 1][cur.j]) {
                s.push(Pos(cur.i - 1, cur.j));
                board[cur.i - 1][cur.j] = '#';
                continue;
            }
            // 下
            if (cur.i + 1 < board.size() && 'O' == board[cur.i + 1][cur.j]) {
                s.push(Pos(cur.i + 1, cur.j));
                board[cur.i + 1][cur.j] = '#';
                continue;
            }
            // 左
            if (cur.j - 1 >= 0 && 'O' == board[cur.i][cur.j - 1]) {
                s.push(Pos(cur.i, cur.j - 1));
                board[cur.i][cur.j - 1] = '#';
                continue;
            }
            // 右
            if (cur.j + 1 < board[0].size() && 'O' == board[cur.i][cur.j + 1]) {
                s.push(Pos(cur.i, cur.j + 1));
                board[cur.i][cur.j + 1] = '#';
                continue;
            }
            // 如果上下左右都搜索不到,本次搜索结束，弹出stack
            s.pop();
        }
    }
};
```

**bfs 非递归**
dfs 非递归的时候我们用 stack 来记录状态，而 bfs 非递归，我们则用队列来记录状态。和 dfs 不同的是，dfs 中搜索上下左右，只要搜索到一个满足条件，我们就顺着该方向继续搜索，所以你可以看到 dfs 代码中，只要满足条件，就入 Stack，然后 continue 本次搜索，进行下一次搜索，直到搜索到没有满足条件的时候出 stack。而 dfs 中，我们要把上下左右满足条件的都入队，所以搜索的时候就不能 continue。大家可以对比下两者的代码，体会 bfs 和 dfs 的差异。

```c++
class Solution {
public:
    struct Pos {
        int i;
        int j;
        Pos(int x, int y) : i(x), j(y) {}
    };
    void solve(vector<vector<char>>& board) {
        int width = board.size();
        if (0 == width) return;
        int length = board[0].size();
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                bool isEdge =
                    0 == i || 0 == j || width - 1 == i || length - 1 == j;
                if (isEdge && 'O' == board[i][j]) {
                    bfs(board, i, j);
                }
            }
        }
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < length; ++j) {
                if ('O' == board[i][j]) {
                    board[i][j] = 'X';
                } else if ('#' == board[i][j]) {
                    board[i][j] = 'O';
                }
            }
        }
    }
    void bfs(vector<vector<char>>& board, int i, int j) {
        queue<Pos> q;
        q.push(Pos(i, j));
        board[i][j] = '#';
        while (!q.empty()) {
            Pos cur = q.front();
            q.pop();
            // 上
            if (cur.i - 1 >= 0 && 'O' == board[cur.i - 1][cur.j]) {
                q.push(Pos(cur.i - 1, cur.j));
                board[cur.i - 1][cur.j] = '#';
            }
            // 下
            if (cur.i + 1 < board.size() && 'O' == board[cur.i + 1][cur.j]) {
                q.push(Pos(cur.i + 1, cur.j));
                board[cur.i + 1][cur.j] = '#';
            }
            // 左
            if (cur.j - 1 >= 0 && 'O' == board[cur.i][cur.j - 1]) {
                q.push(Pos(cur.i, cur.j - 1));
                board[cur.i][cur.j - 1] = '#';
            }
            // 右
            if (cur.j + 1 < board[0].size() && 'O' == board[cur.i][cur.j + 1]) {
                q.push(Pos(cur.i, cur.j + 1));
                board[cur.i][cur.j + 1] = '#';
            }
        }
    }
};
```

**bfs 递归**
bfs 一般我们不会去涉及，而且比较绕，之前我们唯一 A 过的用 bfs 递归的方式是层序遍历二叉树的时候可以用递归的方式。

**并查集**
[题解](https://leetcode-cn.com/problems/surrounded-regions/solution/bfsdi-gui-dfsfei-di-gui-dfsbing-cha-ji-by-ac_pipe/)
