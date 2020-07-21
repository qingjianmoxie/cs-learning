给定一个整数 n，生成所有由 1 ... n 为节点所组成的 **二叉搜索树** 。

示例：

    输入：3
    输出：
    [
      [1,null,3,2],
      [3,2,null,1],
      [3,1,null,null,2],
      [2,1,3],
      [1,null,2,null,3]
    ]
    解释：
    以上的输出对应以下 5 种不同结构的二叉搜索树：

       1         3     3      2      1
        \       /     /      / \      \
         3     2     1      1   3      2
        /     /       \                 \
       2     1         2                 3

提示：

+ 0 <= n <= 8

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/unique-binary-search-trees-ii
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

[优质题解](https://leetcode-cn.com/problems/unique-binary-search-trees-ii/solution/xiang-xi-tong-su-de-si-lu-fen-xi-duo-jie-fa-by-2-7/)

**解法二 递归**

解法一 回溯算法 完全没有用到查找二叉树的性质，暴力尝试了所有可能从而造成了重复。我们可以利用一下查找二叉树的性质。左子树的所有值小于根节点，右子树的所有值大于根节点。

所以如果求 1...n 的所有可能。

我们只需要把 1 作为根节点，[ ] 空作为左子树，[ 2 ... n ] 的所有可能作为右子树。

2 作为根节点，[ 1 ] 作为左子树，[ 3...n ] 的所有可能作为右子树。

3 作为根节点，[ 1 2 ] 的所有可能作为左子树，[ 4 ... n ] 的所有可能作为右子树，然后左子树和右子树两两组合。

4 作为根节点，[ 1 2 3 ] 的所有可能作为左子树，[ 5 ... n ] 的所有可能作为右子树，然后左子树和右子树两两组合。

...

n 作为根节点，[ 1... n ] 的所有可能作为左子树，[ ] 作为右子树。

至于，[ 2 ... n ] 的所有可能以及 [ 4 ... n ] 以及其他情况的所有可能，可以利用上边的方法，把每个数字作为根节点，然后把所有可能的左子树和右子树组合起来即可。

如果只有一个数字，那么所有可能就是一种情况，把该数字作为一棵树。而如果是 [ ]，那就返回 null。

```c++
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<TreeNode *> generateTrees(int n) {
        if (0 == n) return {};
        return helper(1, n);
    }

    vector<TreeNode *> helper(int start, int end) {
        vector<TreeNode *> ans;
        if (start > end) {
            ans.push_back(NULL);
            return ans;
        }

        if (start == end) {
            TreeNode *root = new TreeNode(start);
            ans.push_back(root);
            return ans;
        }

        // 尝试每个数字作为根节点
        for (int i = start; i <= end; i++) {
            // 得到所有可能的左子树
            vector<TreeNode *> left_trees = helper(start, i - 1);
            // 得到所有可能的右子树
            vector<TreeNode *> right_trees = helper(i + 1, end);
            // 左子树右子树两两组合
            for (auto left_tree : left_trees) {
                for (auto right_tree : right_trees) {
                    TreeNode *root = new TreeNode(i);
                    root->left = left_tree;
                    root->right = right_tree;
                    ans.push_back(root);
                }
            }
        }
        return ans;
    }
};
```

**解法三 动态规划**

大多数递归都可以用动态规划的思想重写，这道也不例外。从底部往上走。

举个例子，n = 3

    数字个数是 0 的所有解
    null

    数字个数是 1 的所有解
    1
    2
    3

    数字个数是 2 的所有解，我们只需要考虑连续数字
    [ 1 2 ]
      1  
       \    
        2
    
       2
      /
     1

    [ 2 3 ]
      2  
       \    
        3
    
       3
      /
     2
    
    如果求 3 个数字的所有情况。
    [ 1 2 3 ]
    利用解法二递归的思路，就是分别把每个数字作为根节点，然后考虑左子树和右子树的可能
    1 作为根节点，左子树是 [] 的所有可能，右子树是 [ 2 3 ] 的所有可能，利用之前求出的结果进行组合。
        1
      /   \
    null   2
            \
             3

        1
      /   \
    null   3
          /
         2 

    2 作为根节点，左子树是 [ 1 ] 的所有可能，右子树是  [ 3 ] 的所有可能，利用之前求出的结果进行组合。
        2
      /   \
     1     3

    3 作为根节点，左子树是 [ 1 2 ] 的所有可能，右子树是 [] 的所有可能，利用之前求出的结果进行组合。
         3
       /   \
      1   null
       \
        2

          3
        /   \
       2   null 
      /
     1

然后利用上边的思路基本上可以写代码了，就是求出长度为 1 的所有可能，长度为 2 的所有可能 ... 直到 n。

但是我们注意到，求长度为 2 的所有可能的时候，我们需要求 [ 1 2 ] 的所有可能，[ 2 3 ] 的所有可能，这只是 n = 3 的情况。如果 n 等于 100，我们需要求的更多了 [ 1 2 ] ， [ 2 3 ] ， [ 3 4 ] ... [ 99 100 ] 太多了。能不能优化呢？

仔细观察，我们可以发现长度是为 2 的所有可能其实只有两种结构。

      x  
     /    
    y

    y
     \
      x

看之前推导的 [ 1 2 ] 和 [ 2 3 ]，只是数字不一样，结构是完全一样的。

    [ 1 2 ]
      1  
       \    
        2
       2
      /
     1

    [ 2 3 ]
      2  
       \    
        3
       3
      /
     2

所以我们 n = 100 的时候，求长度是 2 的所有情况的时候，我们没必要把 [ 1 2 ] ， [ 2 3 ] ， [ 3 4 ] ... [ 99 100 ] 所有的情况都求出来，只需要求出 [ 1 2 ] 的所有情况即可。

推广到任意长度 len，我们其实只需要求 [ 1 2 ... len ] 的所有情况就可以了。下一个问题随之而来，这些 [ 2 3 ] ， [ 3 4 ] ... [ 99 100 ] 没求的怎么办呢？

举个例子。n = 100，此时我们求把 98 作为根节点的所有情况，根据之前的推导，我们需要长度是 97 的 [ 1 2 ... 97 ] 的所有情况作为左子树，长度是 2 的 [ 99 100 ] 的所有情况作为右子树。

[ 1 2 ... 97 ] 的所有情况刚好是 [ 1 2 ... len ] ，已经求出来了。但 [ 99 100 ] 怎么办呢？我们只求了 [ 1 2 ] 的所有情况。答案很明显了，在 [ 1 2 ] 的所有情况每个数字加一个偏差 98，即加上根节点的值就可以了。

    [ 1 2 ]
      1  
       \    
        2
       2
      /
     1

    [ 99 100 ]
      1 + 98
       \    
        2 + 98
       2 + 98
      /
     1 + 98

    即
      99  
       \    
        100
       100
      /
     99

所以我们需要一个函数，实现树的复制并且加上偏差。

    ```c++
    TreeNode *clone(TreeNode *root, int offset) {
        if (NULL == root) {
            return NULL;
        }
        TreeNode *node = new TreeNode(root->val + offset);
        node->left = clone(root->left, offset);
        node->right = clone(root->right, offset);
        return node;
    }
    ```

通过上边的所有分析，代码可以写了，总体思想就是求长度为 2 的所有情况，求长度为 3 的所有情况直到 n。而求长度为 len 的所有情况，我们只需要求出一个代表 [ 1 2 ... len ] 的所有情况，其他的话加上一个偏差，加上当前根节点即可。

```c++
class Solution {
private:
    TreeNode *clone(TreeNode *root, int offset) {
        if (NULL == root) {
            return NULL;
        }
        TreeNode *node = new TreeNode(root->val + offset);
        node->left = clone(root->left, offset);
        node->right = clone(root->right, offset);
        return node;
    }

public:
    vector<TreeNode *> generateTrees(int n) {
        if (0 == n) return {};
        vector<vector<TreeNode *>> dp(n + 1);
        dp[0].push_back(NULL);
        for (int len = 1; len <= n; len++) {
            for (int root_val = 1; root_val <= len; root_val++) {
                int left = root_val - 1;    // 左子树的长度
                int right = len - root_val; // 右子树的长度
                for (auto left_tree : dp[left]) {
                    for (auto right_tree : dp[right]) {
                        TreeNode *root = new TreeNode(root_val);
                        root->left = left_tree;
                        root->right = clone(right_tree, root_val);
                        dp[len].push_back(root);
                    }
                }
            }
        }
        return dp[n];
    }
};
```

值得注意的是，所有的左子树我们没有 clone ，也就是很多子树被共享了，在内存中就会是下边的样子。

![](https://pic.leetcode-cn.com/03f7e2b792dd6adc3176a3d475108e8b7592e89d68a8de1cafe29e3bfa43a111-95_2.jpg)

也就是左子树用的都是之前的子树，没有开辟新的空间。

**解法四 动态规划2**

解法三的动态规划完全是模仿了解法二递归的思想，这里再介绍另一种思想，会更好理解一些。参考这里。

    考虑 [] 的所有解
    null

    考虑 [ 1 ] 的所有解
    1

    考虑 [ 1 2 ] 的所有解
      2
     /
    1

     1
      \
       2

    考虑 [ 1 2 3 ] 的所有解
        3
       /
      2
     /
    1

       2
      / \
     1   3

       3
      /
     1
      \
       2

       1
         \
          3
         /
        2

      1
        \
         2
          \
           3

仔细分析，可以发现一个规律。首先我们每次新增加的数字大于之前的所有数字，所以新增加的数字出现的位置只可能是根节点或者是根节点的右孩子，右孩子的右孩子，右孩子的右孩子的右孩子等等，总之一定是右边。其次，新数字所在位置原来的子树，改为当前插入数字的左孩子即可，因为插入数字是最大的。

    对于下边的解 
      2
     /
    1

    然后增加 3
    1.把 3 放到根节点
        3
       /
      2
     /
    1

    1. 把 3 放到根节点的右孩子
       2
      / \
     1   3

    对于下边的解
     1
      \
       2

    然后增加 3
    1.把 3 放到根节点
        3
       /
      1
       \
        2

    2. 把 3 放到根节点的右孩子，原来的子树作为 3 的左孩子       
          1
            \
             3
            /
          2

    3. 把 3 放到根节点的右孩子的右孩子
      1
        \
         2
          \
           3

以上就是根据 [ 1 2 ] 推出 [ 1 2 3 ] 的所有过程，可以写代码了。由于求当前的所有解只需要上一次的解，所有我们只需要两个 vector, pre 保存上一次的所有解， cur 计算当前的所有解。

```c++
class Solution {
   private:
    TreeNode *tree_copy(TreeNode *root) {
        if (NULL == root) {
            return NULL;
        }
        TreeNode *node = new TreeNode(root->val);
        node->left = tree_copy(root->left);
        node->right = tree_copy(root->right);
        return node;
    }

   public:
    vector<TreeNode *> generateTrees(int n) {
        if (0 == n) return {};
        vector<TreeNode *> pre;
        pre.push_back(NULL);
        //每次增加一个数字
        for (int i = 1; i <= n; i++) {
            vector<TreeNode *> cur;
            //遍历之前的所有解
            for (auto root : pre) {
                //插入到根节点
                TreeNode *insert = new TreeNode(i);
                insert->left = root;
                cur.push_back(insert);
                //插入到右孩子，右孩子的右孩子...最多找 n 次孩子
                for (int j = 0; j <= n; j++) {
                    TreeNode *root_copy = tree_copy(root);  // 复制当前的树
                    //找到要插入右孩子的位置
                    TreeNode *right = root_copy;
                    int k = 0;
                    //遍历 j 次找右孩子
                    for (; k < j; k++) {
                        if (NULL == right) {
                            break;
                        }
                        right = right->right;
                    }
                    //到达 null 提前结束
                    if (NULL == right) {
                        break;
                    }
                    //保存当前右孩子的位置的子树作为插入节点的左孩子
                    TreeNode *right_tree = right->right;
                    insert = new TreeNode(i);
                    right->right = insert;  //右孩子是插入的节点
                    insert->left =
                        right_tree;  //插入节点的左孩子更新为插入位置之前的子树
                    cur.push_back(root_copy);
                }
            }
            // 这次循环的cur是下次循环的pre
            pre = cur;
        }
        return pre;
    }
};
```
