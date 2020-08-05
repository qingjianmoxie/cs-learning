在上次打劫完一条街道之后和一圈房屋后，小偷又发现了一个新的可行窃的地区。这个地区只有一个入口，我们称之为“根”。 除了“根”之外，每栋房子有且只有一个“父“房子与之相连。一番侦察之后，聪明的小偷意识到“这个地方的所有房屋的排列类似于一棵二叉树”。 如果两个直接相连的房子在同一天晚上被打劫，房屋将自动报警。

计算在不触动警报的情况下，小偷一晚能够盗取的最高金额。

示例 1:

    输入: [3,2,3,null,3,null,1]

         3
        / \
       2   3
        \   \ 
         3   1

    输出: 7 
    解释: 小偷一晚能够盗取的最高金额 = 3 + 3 + 1 = 7.

示例 2:

    输入: [3,4,5,1,3,null,1]

         3
        / \
       4   5
      / \   \ 
     1   3   1

    输出: 9
    解释: 小偷一晚能够盗取的最高金额 = 4 + 5 = 9.

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/house-robber-iii
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

[优质题解](https://leetcode-cn.com/problems/house-robber-iii/solution/san-chong-fang-fa-jie-jue-shu-xing-dong-tai-gui-hu/)

### 说明

本题目本身就是动态规划的树形版本，通过此题解，可以了解一下树形问题在动态规划问题解法
我们通过三个方法不断递进解决问题

+ 解法一通过递归实现，虽然解决了问题，但是复杂度太高
+ 解法二通过解决方法一中的重复子问题，实现了性能的百倍提升
+ 解法三直接省去了重复子问题，性能又提升了一步

### 解法一、暴力递归 - 最优子结构

在解法一和解法二中，我们使用爷爷、两个孩子、4 个孙子来说明问题
首先来定义这个问题的状态
爷爷节点获取到最大的偷取的钱数呢

1. 首先要明确相邻的节点不能偷，也就是爷爷选择偷，儿子就不能偷了，但是孙子可以偷
2. 二叉树只有左右两个孩子，一个爷爷最多 2 个儿子，4 个孙子

根据以上条件，我们可以得出单个节点的钱该怎么算

4 个孙子偷的钱 + 爷爷的钱 VS 两个儿子偷的钱 哪个组合钱多，就当做**当前节点能偷的最大钱数**。这就是动态规划里面的最优子结构

由于是二叉树，这里可以选择计算所有子节点

4 个孙子投的钱加上爷爷的钱如下
```c++
int money1 = root->val + rob(root->left->left) + rob(root->left->right) 
                       + rob(root->right->left) + rob(root->right->right)
```

两个儿子偷的钱如下
```c++
int money2 = rob(root->left) + rob(root->right)
```

挑选一个钱数多的方案则
```c++
int result = max(money1, money2)
```

将上述方案写成代码如下

```c++
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    int rob(TreeNode *root) {
        if (NULL == root) {
            return 0;
        }
        int money = root->val;
        if (NULL != root->left) {
            money += rob(root->left->left) + rob(root->left->right);
        }
        if (NULL != root->right) {
            money += rob(root->right->left) + rob(root->right->right);
        }
        return max(money, rob(root->left) + rob(root->right));
    }
};
```

结果超时了, 下一个解法进行优化.

### 解法二、记忆化 - 解决重复子问题

针对解法一速度太慢的问题，经过分析其实现，我们发现爷爷在计算自己能偷多少钱的时候，同时计算了 4 个孙子能偷多少钱，也计算了 2 个儿子能偷多少钱。这样在儿子当爷爷时，就会产生重复计算一遍孙子节点。

于是乎我们发现了一个动态规划的关键优化点

**重复子问题**

我们这一步针对重复子问题进行优化，我们在做斐波那契数列时，使用的优化方案是记忆化，但是之前的问题都是使用数组解决的，把每次计算的结果都存起来，下次如果再来计算，就从缓存中取，不再计算了，这样就保证每个数字只计算一次。
由于二叉树不适合拿数组当缓存，我们这次使用哈希表来存储结果，TreeNode 当做 key，能偷的钱当做 value

解法一加上记忆化优化后代码如下：

```c++
class Solution {
public:
    int rob(TreeNode *root) {
        unordered_map<TreeNode *, int> memo;
        return helper(root, memo);
    }

    int helper(TreeNode *root, unordered_map<TreeNode *, int> &memo) {
        if (NULL == root) {
            return 0;
        }
        if (memo.find(root) != memo.end()) {
            return memo[root];
        }
        int money = root->val;
        if (NULL != root->left) {
            money += helper(root->left->left, memo) + helper(root->left->right, memo);
        }
        if (NULL != root->right) {
            money += helper(root->right->left, memo) + helper(root->right->right, memo);
        }
        int res = max(money, helper(root->left, memo) + helper(root->right, memo));
        memo.insert(make_pair(root, res));
        return res;
    }
};
```

### 解法三、终极解法

上面两种解法用到了孙子节点，计算爷爷节点能偷的钱还要同时去计算孙子节点偷的钱，虽然有了记忆化，但是还是有性能损耗。

我们换一种办法来定义此问题

每个节点可选择偷或者不偷两种状态，根据题目意思，相连节点不能一起偷

+ 当前节点选择偷时，那么两个孩子节点就不能选择偷了
+ 当前节点选择不偷时，两个孩子节点只需要拿最多的钱出来就行(两个孩子节点偷不偷没关系)

我们使用一个大小为 2 的数组来表示, 0 代表不偷，1 代表偷
**任何一个节点能偷到的最大钱的状态可以定义为**

1. 当前节点选择不偷：当前节点能偷到的最大钱数 = 左孩子能偷到的钱 + 右孩子能偷到的钱
2. 当前节点选择偷：当前节点能偷到的最大钱数 = 左孩子选择自己不偷时能得到的钱 + 右孩子选择不偷时能得到的钱 + 当前节点的钱数

表示为公式如下

root[0] = max(rob(root->left)[0], rob(root->left)[1]) + max(rob(root->right)[0], rob(root->right)[1])
root[1] = rob(root->left)[0] + rob(root->right)[0] + root.val;

将公式做个变换就是代码啦

```c++
class Solution {
public:
    int rob(TreeNode *root) {
        vector<int> res = helper(root);
        return max(res[0], res[1]);
    }

    vector<int> helper(TreeNode *root) {
        if (NULL == root) {
            return {0, 0};
        }
        vector<int> res(2);
        vector<int> left = helper(root->left);
        vector<int> right = helper(root->right);
        res[0] = max(left[0], left[1]) + max(right[0], right[1]);
        res[1] = left[0] + right[0] + root->val;
        return res;
    }
};
```