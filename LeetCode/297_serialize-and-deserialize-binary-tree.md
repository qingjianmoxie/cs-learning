序列化是将一个数据结构或者对象转换为连续的比特位的操作，进而可以将转换后的数据存储在一个文件或者内存中，同时也可以通过网络传输到另一个计算机环境，采取相反方式重构得到原数据。

请设计一个算法来实现二叉树的序列化与反序列化。这里不限定你的序列 / 反序列化算法执行逻辑，你只需要保证一个二叉树可以被序列化为一个字符串并且将这个字符串反序列化为原始的树结构。

示例: 

    你可以将以下二叉树：

        1
       / \
      2   3
         / \
        4   5

    序列化为 "[1,2,3,null,null,4,5]"

提示: 这与 LeetCode 目前使用的方式一致，详情请参阅 LeetCode 序列化二叉树的格式。你并非必须采取这种方式，你也可以采用其他的方法解决这个问题。

说明: 不要使用类的成员 / 全局 / 静态变量来存储状态，你的序列化和反序列化算法应该是无状态的。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/serialize-and-deserialize-binary-tree
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

序列化改的之前写过的前序遍历, 反序列化看的评论区中的算法.

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
class Codec {
public:

    // Encodes a tree to a single string.
    string serialize(TreeNode *root) {
        string res;
        stack<TreeNode *> s;
        TreeNode *tmp = root;
        while (tmp || !s.empty()) {
            while (tmp) {
                res = res + to_string(tmp->val) + " ";
                s.push(tmp);
                tmp = tmp->left;
            }
            res += "null ";
            tmp = s.top();
            s.pop();
            tmp = tmp->right;
        }
        res += "null";
        return res;
    }

    // Decodes your encoded data to tree.
    TreeNode *deserialize(string data) {
        stringstream is(data);
        queue<string> strs;
        string strtmp;
        while (getline(is, strtmp, ' ')) {
            strs.push(strtmp);
        }
        TreeNode *root = buildTree(strs);
        return root;
    }

    TreeNode *buildTree(queue<string> &strs) {
        TreeNode *node;
        if (strs.front() == "null") {
            strs.pop();
            return NULL;
        } else {
            stringstream ss;
            int i;
            ss << strs.front();
            ss >> i;
            node = new TreeNode(i);
            strs.pop();
            node->left = buildTree(strs);
            node->right = buildTree(strs);
        }
        return node;
    }
};

// Your Codec object will be instantiated and called as such:
// Codec codec;
// codec.deserialize(codec.serialize(root));
```

结果超时了, 尝试过各种方法, 比如将反序列化的递归改为迭代, 将分割字符串换种方法, 字符串转数字使用stoi函数, 均超时. 最后没想到把前序遍历的迭代改为递归竟然过了. 递归的耗时竟然比迭代少, 好神奇, 完全是思路盲区.

```c++
class Codec {
   public:
    // Encodes a tree to a single string.
    string serialize(TreeNode *root) {
        string ret;
        dfs(root, ret);
        return ret;
    }

    void dfs(TreeNode *root, string &ret) {
        if (root == nullptr) {
            ret.append("null,");
            return;
        }

        ret.append(to_string(root->val)).append(",");
        dfs(root->left, ret);
        dfs(root->right, ret);
    }

    // Decodes your encoded data to tree.
    TreeNode *deserialize(string data) {
        vector<string> strs = split(data);
        int i = 0;
        TreeNode *root = buildTree(strs, i);
        return root;
    }

    vector<string> split(string &data) {
        int start = 0;
        vector<string> res;
        while (1) {
            auto end = data.find(',', start);
            if (end == string::npos) break;
            res.push_back(data.substr(start, end - start));
            start = end + 1;
        }
        return move(res);
    }

    TreeNode *buildTree(vector<string> &strs, int &i) {
        if (i == strs.size() || strs[i] == "null") {
            return NULL;
        }
        TreeNode *root = new TreeNode(stoi(strs[i]));
        ++i;
        root->left = buildTree(strs, i);
        ++i;
        root->right = buildTree(strs, i);
        return root;
    }
};
```

再贴一个题解中不分割字符串的方法:

```c++
class Codec {
   public:
    // Encodes a tree to a single string.
    string serialize(TreeNode *root) {
        string ret;
        dfs(root, ret);
        return ret;
    }

    void dfs(TreeNode *root, string &ret) {
        if (root == nullptr) {
            ret.append("null,");
            return;
        }

        ret.append(to_string(root->val)).append(",");
        dfs(root->left, ret);
        dfs(root->right, ret);
    }

    // Decodes your encoded data to tree.
    TreeNode *deserialize(string data) {
        int i = 0;
        return rdfs(data, i);
    }

    TreeNode *rdfs(const string &data, int &i) {
        if (data[i] == '#') {
            i += 2;
            return nullptr;
        }

        int next = i;
        while (data[next] != ',') next++;
        int val = stoi(data.substr(i, next - i));
        auto ret = new TreeNode(val);
        i = next + 1;

        ret->left = rdfs(data, i);
        ret->right = rdfs(data, i);

        return ret;
    }
};
```

此题与 [1028_从先序遍历还原二叉树(hard)](./1028_recover-a-tree-from-preorder-traversal.md) 有异曲同工之妙.
