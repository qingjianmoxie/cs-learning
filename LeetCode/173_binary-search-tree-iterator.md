实现一个二叉搜索树迭代器。你将使用二叉搜索树的根节点初始化迭代器。

调用 next() 将返回二叉搜索树中的下一个最小的数。

示例：

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2018/12/25/bst-tree.png)

    BSTIterator iterator = new BSTIterator(root);
    iterator.next();    // 返回 3
    iterator.next();    // 返回 7
    iterator.hasNext(); // 返回 true
    iterator.next();    // 返回 9
    iterator.hasNext(); // 返回 true
    iterator.next();    // 返回 15
    iterator.hasNext(); // 返回 true
    iterator.next();    // 返回 20
    iterator.hasNext(); // 返回 false

提示：

+ next() 和 hasNext() 操作的时间复杂度是 O(1)，并使用 O(h) 内存，其中 h 是树的高度。
+ 你可以假设 next() 调用总是有效的，也就是说，当调用 next() 时，BST 中至少存在一个下一个最小的数。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/binary-search-tree-iterator
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

**方法一：扁平化二叉搜索树**

在计算机程序设计中，迭代器是使程序员能够遍历容器的对象。这是维基百科对迭代器的定义。当前，实现迭代器的最简单的方法是在类似数组的容器接口上。如果我们有一个数组，则我们只需要一个指针或者索引，就可以轻松的实现函数 next() 和 hasNext()。

因此，我们要研究的第一种方法就是基于这种思想。我们将使用额外的数组，并将二叉搜索树展开存放到里面。我们想要数组的元素按升序排序，则我们应该对二叉搜索树进行中序遍历，然后我们在数组中构建迭代器函数。

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
class BSTIterator {
    vector<int> values;

public:
    BSTIterator(TreeNode* root) {
        stack<TreeNode *> s;
        TreeNode *tmp = root;
        while (tmp || !s.empty())
        {
            while (tmp)
            {
                s.push(tmp);
                tmp = tmp->left;
            }
            tmp = s.top();
            s.pop();
            values.push_back(tmp->val);
            tmp = tmp->right;
        }
        reverse(values.begin(), values.end());
    }
    
    /** @return the next smallest number */
    int next() {
        int res = values.back();
        values.pop_back();
        return res;
    }
    
    /** @return whether we have a next smallest number */
    bool hasNext() {
        if (values.empty())
            return false;
        else
            return true;
    }
};

/**
 * Your BSTIterator object will be instantiated and called as such:
 * BSTIterator* obj = new BSTIterator(root);
 * int param_1 = obj->next();
 * bool param_2 = obj->hasNext();
 */
 ```

**复杂度分析**

+ 时间复杂度：构造迭代器花费的时间为 O(N)，问题陈述只要求我们分析两个函数的复杂性，但是在实现类时，还要注意初始化类对象所需的时间；在这种情况下，时间复杂度与二叉搜索树中的节点数成线性关系。
    + next()：O(1)
    + hasNext()：O(1)
+ 空间复杂度：O(N)，由于我们创建了一个数组来包含二叉搜索树中的所有节点值，这不符合问题陈述中的要求，任一函数的最大空间复杂度应为 O(h)，其中 h 指的是树的高度，对于平衡的二叉搜索树，高度通常为 logN。

**方法二：受控递归**

我们前面使用的方法在空间上与二叉搜索树中的节点数呈线性关系。然而，我们不得不使用这种方法的原因是我们在数组上迭代，且我们不能够暂停递归，然后在某个时候启动它。

但是，如果我们可以模拟中序遍历的受控递归，那么除了堆栈用于模拟递归的空间之外，实际上不需要使用任何其他空间。

让我们更加具体的看一下这个算法。

算法：

1. 初始化一个空栈 S，用于模拟二叉搜索树的中序遍历。中序遍历我们采用与之前相同的方法，只是我们现在使用的是自己的栈而不是系统的堆栈。由于我们使用自定义的数据结构，因此可以随时暂停和恢复递归。
2. 我们还要实现一个帮助函数，在实现中将一次又一次的调用它。这个函数叫 `inorderLeft`，它将给定节点中的所有左子节点添加到栈中，直到节点没有左子节点为止。
3. 第一次调用 next() 函数时，必须返回二叉搜索树的最小元素，然后我们模拟递归必须向前移动一步，即移动到二叉搜索树的下一个最小元素上。栈的顶部始终包含 next() 函数返回的元素。hasNext() 很容易实现，因为我们只需要检查栈是否为空。
4. 首先，给定二叉搜索树的根节点，我们调用函数 `inorderLeft`，这确保了栈顶部始终包含了 next() 函数返回的元素。
5. 假设我们调用 next()，我们需要返回二叉搜索树中的下一个最小元素，即栈的顶部元素。有两种可能性：
    + 一个是栈顶部的节点是一个叶节点。这是最好的情况，因为我们什么都不用做，只需将节点从栈中弹出并返回其值。所以这是个常数时间的操作。
    + 另一个情况是栈顶部的节点拥有右节点。我们不需要检查左节点，因为左节点已经添加到栈中了。栈顶元素要么没有左节点，要么左节点已经被处理了。如果栈顶部拥有右节点，那么我们需要对右节点上调用帮助函数。该时间复杂度取决于树的结构。
6. next() 函数调用中，获取下一个最小的元素不需要花太多时间，但是要保证栈顶元素是 next() 函数返回的元素方面花费了一些时间。

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
class BSTIterator {
    stack<TreeNode *> s;

public:
    BSTIterator(TreeNode* root) {
        inorderLeft(root);
    }

    void inorderLeft(TreeNode *root)
    {
        TreeNode *tmp = root;
        while (tmp)
        {
            s.push(tmp);
            tmp = tmp->left;
        }
    }

    /** @return the next smallest number */
    int next() {
        TreeNode *target = s.top();
        s.pop();
        if (target->right)
            inorderLeft(target->right);
        return target->val;
    }
    
    /** @return whether we have a next smallest number */
    bool hasNext() {
        return s.size() > 0;
    }
};

/**
 * Your BSTIterator object will be instantiated and called as such:
 * BSTIterator* obj = new BSTIterator(root);
 * int param_1 = obj->next();
 * bool param_2 = obj->hasNext();
 */
 ```

**复杂度分析**

+ 时间复杂度：
    + hasNext()：若栈中还有元素，则返回 true，反之返回 false。所以这是一个 O(1) 的操作。
    + next()：包含了两个主要步骤。一个是从栈中弹出一个元素，它是下一个最小的元素。这是一个 O(1) 的操作。然而，随后我们要调用帮助函数 `inorderLeft` ，它需要递归的，将左节点添加到栈上，是线性时间的操作，最坏的情况下为 O(N)。但是我们只对含有右节点的节点进行调用，它也不会总是处理 N 个节点。只有当我们有一个倾斜的树，才会有 N 个节点。因此该操作的平均时间复杂度仍然是 O(1)，符合问题中所要求的。
+ 空间复杂度：O(h)，使用了一个栈来模拟递归。
