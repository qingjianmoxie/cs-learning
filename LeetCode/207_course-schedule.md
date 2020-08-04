你这个学期必须选修 numCourse 门课程，记为 0 到 numCourse-1 。

在选修某些课程之前需要一些先修课程。 例如，想要学习课程 0 ，你需要先完成课程 1 ，我们用一个匹配来表示他们：[0,1]

给定课程总量以及它们的先决条件，请你判断是否可能完成所有课程的学习？

示例 1:

    输入: 2, [[1,0]] 
    输出: true
    解释: 总共有 2 门课程。学习课程 1 之前，你需要完成课程 0。所以这是可能的。

示例 2:

    输入: 2, [[1,0],[0,1]]
    输出: false
    解释: 总共有 2 门课程。学习课程 1 之前，你需要先完成​课程 0；并且学习课程 0 之前，你还应先完成课程 1。这是不可能的。

提示：

1. 输入的先决条件是由 **边缘列表** 表示的图形，而不是 邻接矩阵 。详情请参见[图的表示法](https://www.cnblogs.com/liushang0419/archive/2011/05/06/2039386.html)。
2. 你可以假定输入的先决条件中没有重复的边。
3. 1 <= numCourses <= 10^5

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/course-schedule
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

[优质题解](https://leetcode-cn.com/problems/course-schedule/solution/course-schedule-tuo-bu-pai-xu-bfsdfsliang-chong-fa/)

解题思路：

+ 本题可约化为： 课程安排图是否是 **有向无环图(DAG)**。即课程间规定了前置条件，但不能构成任何环路，否则课程前置条件将不成立。
+ 思路是通过 **拓扑排序** 判断此课程安排图是否是 **有向无环图(DAG)** 。 **拓扑排序原理**： 对 DAG 的顶点进行排序，使得对每一条有向边 (u,v)，均有 u（在排序记录中）比 v 先出现。亦可理解为对某点 v 而言，只有当 v 的所有源点均出现了，v 才能出现。
+ 通过课程前置条件列表 prerequisites 可以得到课程安排图的 **邻接表** adjacency，以降低算法时间复杂度，以下两种方法都会用到邻接表。

### 方法一：入度表（广度优先遍历）

算法流程：
1. 统计课程安排图中每个节点的入度，生成 入度表 indegrees。
2. 借助一个队列 queue，将所有入度为 0 的节点入队。
3. 当 queue 非空时，依次将队首节点出队，在课程安排图中删除此节点 pre：
    + 并不是真正从邻接表中删除此节点 pre，而是将此节点对应所有邻接节点 cur 的入度 −1，即 indegrees[cur] -= 1。
    + 当入度 -1 后邻接节点 cur 的入度为 0，说明 cur 所有的前驱节点已经被 “删除”，此时将 cur 入队。
4. 在每次 pre 出队时，执行 numCourses--；
    + 若整个课程安排图是有向无环图（即可以安排），则所有节点一定都入队并出队过，即完成拓扑排序。换个角度说，若课程安排图中存在环，一定有节点的入度始终不为 0。
    + 因此，拓扑排序出队次数等于课程个数，返回 numCourses == 0 判断课程是否可以成功安排。

复杂度分析：
时间复杂度 O(N + M)： 遍历一个图需要访问所有节点和所有临边，N 和 M 分别为节点数量和临边数量；
空间复杂度 O(N + M)： 为建立邻接表所需额外空间，adjacency 长度为 N ，并存储 M 条临边的数据。

c++代码
```c++
class Solution {
public:
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> degrees(numCourses);  //记录所有顶点的入度,未初始化的为0
        vector<vector<int>> adjacency(numCourses);  //邻接表
        queue<int> zero;                            //零入度的顶点
        int num = numCourses;
        for (int i = 0; i < prerequisites.size(); i++) {
            // 该顶点的入度+1
            degrees[prerequisites[i][0]]++;
            adjacency[prerequisites[i][1]].push_back(prerequisites[i][0]);
        }
        for (int i = 0; i < numCourses; i++) {
            if (degrees[i] == 0) {
                zero.push(i);  //入度为0的先入队列
                num--;
            }
        }
        while (!zero.empty()) {
            int tmp = zero.front();
            zero.pop();
            for (int j = 0; j < adjacency[tmp].size(); j++) {
                if (--degrees[adjacency[tmp][j]] == 0) {
                    zero.push(adjacency[tmp][j]);
                    num--;
                }
            }
        }
        if (num == 0) return true;
        return false;
    }
};
```

### 方法二：深度优先遍历

原理是通过 DFS 判断图中是否有环。

算法流程：
1. 借助一个标志列表 flags，用于判断每个节点 i （课程）的状态：
    1. 未被 DFS 访问：i == 0；
    2. 已被其他节点启动的 DFS 访问：i == -1；
    3. 已被当前节点启动的 DFS 访问：i == 1。
2. 对 numCourses 个节点依次执行 DFS，判断每个节点起步 DFS 是否存在环，若存在环直接返回 False。DFS 流程:
    1. 终止条件：
        + 当 flag[i] == -1，说明当前访问节点已被其他节点启动的 DFS 访问，无需再重复搜索，直接返回 True。
        + 当 flag[i] == 1，说明在本轮 DFS 搜索中节点 i 被第 2 次访问，即 课程安排图有环 ，直接返回 False。
    2. 将当前访问节点 i 对应 flag[i] 置 1，即标记其被本轮 DFS 访问过；
    3. 递归访问当前节点 i 的所有邻接节点 j，当发现环直接返回 False；
    4. 当前节点所有邻接节点已被遍历，并没有发现环，则将当前节点 flag 置为 -1 并返回 True。
3. 若整个图 DFS 结束并未发现环，返回 True。

复杂度分析：
时间复杂度 O(N + M)： 遍历一个图需要访问所有节点和所有临边，N 和 M 分别为节点数量和临边数量；
空间复杂度 O(N + M)： 为建立邻接表所需额外空间，adjacency 长度为 N ，并存储 M 条临边的数据。
