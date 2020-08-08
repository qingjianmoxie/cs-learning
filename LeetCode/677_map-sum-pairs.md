实现一个 MapSum 类里的两个方法，insert 和 sum。

对于方法 insert，你将得到一对（字符串，整数）的键值对。字符串表示键，整数表示值。如果键已经存在，那么原来的键值对将被替代成新的键值对。

对于方法 sum，你将得到一个表示前缀的字符串，你需要返回所有以该前缀开头的键的值的总和。

示例 1:

    输入: insert("apple", 3), 输出: Null
    输入: sum("ap"), 输出: 3
    输入: insert("app", 2), 输出: Null
    输入: sum("ap"), 输出: 5

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/map-sum-pairs
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class MapSum {
public:
    /** Initialize your data structure here. */
    MapSum() {

    }
    
    void insert(string key, int val) {

    }
    
    int sum(string prefix) {

    }
};

/**
 * Your MapSum object will be instantiated and called as such:
 * MapSum* obj = new MapSum();
 * obj->insert(key,val);
 * int param_2 = obj->sum(prefix);
 */
```

## 题解

使用Trie

```c++
struct TrieNode {
    TrieNode *children[26];
    int value;
    TrieNode() {
        for (int i = 0; i < 26; ++i) {
            children[i] = NULL;
        }
        value = 0;
    }
};
class MapSum {
public:
    /** Initialize your data structure here. */
    MapSum() { root = new TrieNode(); }

    void insert(string key, int val) {
        TrieNode *cur = root;
        for (int i = 0; i < key.size(); ++i) {
            int tmp = key[i] - 'a';
            if (NULL == cur->children[tmp]) {
                cur->children[tmp] = new TrieNode();
            }
            cur = cur->children[tmp];
        }
        cur->value = val;
    }

    int sum(string prefix) {
        TrieNode *cur = root;
        for (int i = 0; i < prefix.size(); ++i) {
            int tmp = prefix[i] - 'a';
            if (NULL == cur->children[tmp]) {
                return 0;
            }
            cur = cur->children[tmp];
        }
        return helper(cur);
    }

private:
    int helper(TrieNode *root) {
        int sum = root->value;
        int i = 0;
        for (; i < 26; ++i) {
            if (NULL != root->children[i]) {
                break;
            }
        }
        if (26 == i) {
            return sum;
        }
        for (i = 0; i < 26; ++i) {
            if (NULL != root->children[i]) {
                sum += helper(root->children[i]);
            }
        }
        return sum;
    }

    TrieNode *root;
};
```

看了一下别人的题解, 将指针数组初始化为NULL可以使用memset;
在写helper函数时一定是脑子进水了, 一根筋, 当时想的是prefix后的第一个节点一定不是NULL, 所以判断节点是不是最后一个节点写得好麻烦, 实际上判断节点是不是NULL节点会简单很多.

```c++
struct TrieNode {
    TrieNode* children[26];
    int value;
    TrieNode() {
        memset(children, 0, sizeof(children));
        value = 0;
    }
};
class MapSum {
public:
    /** Initialize your data structure here. */
    MapSum() { root = new TrieNode(); }

    void insert(string key, int val) {
        TrieNode* cur = root;
        for (char ch : key) {
            if (NULL == cur->children[ch - 'a']) {
                cur->children[ch - 'a'] = new TrieNode();
            }
            cur = cur->children[ch - 'a'];
        }
        cur->value = val;
    }

    int sum(string prefix) {
        TrieNode* cur = root;
        for (char ch : prefix) {
            if (NULL == cur->children[ch - 'a']) {
                return 0;
            }
            cur = cur->children[ch - 'a'];
        }
        return helper(cur);
    }

private:
    int helper(TrieNode* root) {
        if (NULL == root) {
            return 0;
        }
        int sum = root->value;
        for (TrieNode* cur : root->children) {
            sum += helper(cur);
        }
        return sum;
    }

    TrieNode* root;
};
```