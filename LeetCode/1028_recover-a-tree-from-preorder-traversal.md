我们从二叉树的根节点 root 开始进行深度优先搜索。

在遍历中的每个节点处，我们输出 D 条短划线（其中 D 是该节点的深度），然后输出该节点的值。（如果节点的深度为 D，则其直接子节点的深度为 D + 1。根节点的深度为 0）。

如果节点只有一个子节点，那么保证该子节点为左子节点。

给出遍历输出 S，还原树并返回其根节点 root。

示例 1：

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2019/04/12/recover-a-tree-from-preorder-traversal.png)

    输入："1-2--3--4-5--6--7"
    输出：[1,2,5,3,4,6,7]

示例 2：

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2019/04/12/screen-shot-2019-04-10-at-114101-pm.png)

    输入："1-2--3---4-5--6---7"
    输出：[1,2,5,3,null,6,null,4,null,7]

示例 3：

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2019/04/12/screen-shot-2019-04-10-at-114955-pm.png)

    输入："1-401--349---90--88"
    输出：[1,401,null,349,88,90]

提示：

+ 原始树中的节点数介于 1 和 1000 之间。
+ 每个节点的值介于 1 和 10 ^ 9 之间。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/recover-a-tree-from-preorder-traversal
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

我们每次从字符串 s 中取出一个节点的值以及它的深度信息。具体地：

+ 我们首先读取若干个字符 -，直到遇到非 - 字符位置。通过 - 的个数，我们就可以知道当前节点的深度信息；

+ 我们再读取若干个数字，直到遇到非数字或者字符串的结尾。通过这些数字，我们就可以知道当前节点的值。

得到这些信息之后，我们就需要考虑将当前的节点放在何处。记当前节点为 T，上一个节点为 S，那么实际上只有两种情况：

+ T 是 S 的左子节点；

+ T 是根节点到 S 这一条路径上（不包括 S）某一个节点的右子节点。

为什么不包括 S？因为题目中规定了如果节点只有一个子节点，那么保证该子节点为左子节点。在 T 出现之前，S 仍然还是一个叶节点，没有左子节点，因此 T 如果是 S 的子节点，一定是优先变成 S 的左子节点。
这是因为先序遍历本身的性质。在先序遍历中，我们是通过「根 — 左 — 右」的顺序访问每一个节点的。想一想先序遍历的递归 + 回溯方法，对于在先序遍历中任意的两个相邻的节点 S 和 T，要么 T 就是 S 的左子节点（对应上面的第一种情况），要么在遍历到 S 之后发现 S 是个叶节点，于是回溯到之前的某个节点，并开始递归地遍历其右子节点（对应上面的第二种情况）。这样以来，我们按照顺序维护从根节点到当前节点的路径上的所有节点，就可以方便地处理这两种情况。仔细想一想，这实际上就是使用递归 + 回溯的方法遍历一棵树时，栈中的所有节点，也就是可以回溯到的节点。因此我们只需要使用一个栈来模拟递归 + 回溯即可。

回到上面的分析，当我们得到当前节点的值以及深度信息之后，我们可以发现：如果 T 是 S 的左子节点，那么 T 的深度恰好比 S 的深度大 1；在其它的情况下，T 是栈中某个节点（不包括 S）的右子节点，那么我们将栈顶的节点不断地出栈，直到 T 的深度恰好比栈顶节点的深度大 1，此时我们就找到了 T 的双亲节点。

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
    TreeNode *recoverFromPreorder(string S) {
        int index = 0;
        int depth = 0;
        stack<TreeNode *> mystack;
        int next = index;
        // 注意判断next < S.size(), 否则只有一个节点时, 会越界
        while (next < S.size() && S[next] != '-') next++;
        int val = stoi(S.substr(index, next - index));
        TreeNode *root = new TreeNode(val);
        mystack.push(root);
        index = next;
        while (index < S.size()) {
            next = index;
            while (S[next] == '-') next++;
            int curdepth = next - index;
            index = next;
            // 注意判断next < S.size(), 否则会越界
            while (next < S.size() && S[next] != '-') next++;
            val = stoi(S.substr(index, next - index));
            TreeNode *res = new TreeNode(val);
            index = next;
            if (curdepth == depth + 1) {
                mystack.top()->left = res;
            } else {
                int delta = depth - curdepth + 1;
                for (int i = 0; i < delta; i++) {
                    mystack.pop();
                }
                mystack.top()->right = res;
            }
            mystack.push(res);
            depth = curdepth;
        }
        return root;
    }
};
```

也可以将第一个节点写进循环内, 此题与 [297_二叉树的序列化与反序列化(hard)](./297_serialize-and-deserialize-binary-tree.md) 有异曲同工之妙.
