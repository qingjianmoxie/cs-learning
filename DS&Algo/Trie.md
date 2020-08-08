# Trie

## 概述

前缀树 ，又称 字典树 ，是 N 叉树 的特殊形式。

我们将深入讨论前缀树的实现方法以及如何将这个数据结构应用到实际问题中。

完成后，你将能够：

+ 理解前缀树的 基本概念 ；
+ 掌握前缀树中的 插入 和 搜索操作 ；
+ 了解前缀树如何帮助解决 实际应用问题 ；
+ 运用前缀树解题。

## 什么是前缀树?

前缀树 是 N叉树 的一种特殊形式。通常来说，一个前缀树是用来 **存储字符串** 的。前缀树的每一个节点代表一个 字符串（前缀）。每一个节点会有多个子节点，通往不同子节点的路径上有着不同的字符。子节点代表的字符串是由节点本身的 原始字符串 ，以及 通往该子节点路径上所有的字符 组成的。

下面是前缀树的一个例子：

![](https://aliyun-lc-upload.oss-cn-hangzhou.aliyuncs.com/aliyun-lc-upload/uploads/2018/02/07/screen-shot-2018-01-31-at-163403.png)

在上图示例中，我们在节点中标记的值是该节点对应表示的字符串。例如，我们从根节点开始，选择第二条路径 'b'，然后选择它的第一个子节点 'a'，接下来继续选择子节点 'd'，我们最终会到达叶节点 "bad"。节点的值是由从根节点开始，与其经过的路径中的字符按顺序形成的。

值得注意的是，**根节点表示空字符串**。

前缀树的一个重要的特性是，节点所有的后代都与该节点相关的字符串有着共同的前缀。这就是 前缀树 名称的由来。

我们再来看这个例子。例如，以节点 "b" 为根的子树中的节点表示的字符串，都具有共同的前缀 "b"。反之亦然，具有公共前缀 "b" 的字符串，全部位于以 "b" 为根的子树中，并且具有不同前缀的字符串来自不同的分支。

前缀树有着广泛的应用，例如自动补全，拼写检查等等。我们将在后面的章节中介绍实际应用场景。

## 如何表示一个前缀树？

在前面的文章中，我们介绍了前缀树的概念。在这篇文章中，我们将讨论如何用代码表示这个数据结构。

在阅读一下内容前，请简要回顾 N 叉树的节点结构。

前缀树的特别之处在于字符和子节点之间的对应关系。有许多不同的表示前缀树节点的方法，这里我们只介绍其中的两种方法。

### 方法一 - 数组

第一种方法是用 数组 存储子节点。

例如，如果我们只存储含有字母 a 到 z 的字符串，我们可以在每个节点中声明一个大小为 26 的数组来存储其子节点。对于特定字符 c ，我们可以使用 c - 'a' 作为索引来查找数组中相应的子节点。

```c++
// change this value to adapt to different cases
#define N 26

struct TrieNode {
    TrieNode* children[N];
    
    // you might need some extra values according to different cases
};

/** Usage:
 *  Initialization: TrieNode root = new TrieNode();
 *  Return a specific child node with char c: (root->children)[c - 'a']
 */
```

访问子节点十分 快捷 。访问一个特定的子节点比较 容易 ，因为在大多数情况下，我们很容易将一个字符转换为索引。但并非所有的子节点都需要这样的操作，所以这可能会导致 空间的浪费 。

### 方法二 - Map

第二种方法是使用 Hashmap 来存储子节点。

我们可以在每个节点中声明一个 Hashmap 。Hashmap 的键是字符，值是相对应的子节点。

```c++
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    
    // you might need some extra values according to different cases
};

/** Usage:
 *  Initialization: TrieNode root = new TrieNode();
 *  Return a specific child node with char c: (root->children)[c]
 */
```

通过相应的字符来访问特定的子节点 更为容易 。但它可能比使用数组 稍慢一些 。但是，由于我们只存储我们需要的子节点，因此 节省了空间 。这个方法也更加 灵活 ，因为我们不受到固定长度和固定范围的限制。

### 补充

我们已经提到过如何表示前缀树中的子节点。除此之外，我们也需要用到一些其他的值。

例如，我们知道，前缀树的每个节点表示一个字符串，但并不是所有由前缀树表示的字符串都是有意义的。如果我们只想在前缀树中存储单词，那么我们可能需要在每个节点中声明一个布尔值（Boolean）作为标志，来表明该节点所表示的字符串是否为一个单词。

## 基本操作

通过阅读前一章节的文章，相信你已经对前缀树以及如何用代码实现这个数据结构有了基本的了解。

本章将讨论前缀树中的一些基本操作。相信在学习完本章后，你可以独立实现一个前缀树。

### 前缀树中的插入操作

我们已经在另一本 LeetBook 中讨论了 [如何在二叉搜索树中实现插入操作](https://leetcode-cn.com/leetbook/read/introduction-to-data-structure-binary-search-tree/xp4tij/)。

> 提问：
> 你还记得如何在二叉搜索树中插入一个新的节点吗？

当我们在二叉搜索树中插入目标值时，在每个节点中，我们都需要根据 节点值 和 目标值 之间的关系，来确定目标值需要去往哪个子节点。同样地，当我们向前缀树中插入一个目标值时，我们也需要根据插入的 目标值 来决定我们的路径。

更具体地说，如果我们在前缀树中插入一个字符串 S，我们要从根节点开始。 我们将根据 S[0]（S中的第一个字符），选择一个子节点或添加一个新的子节点。然后到达第二个节点，并根据 S[1] 做出选择。 再到第三个节点，以此类推。 最后，我们依次遍历 S 中的所有字符并到达末尾。 末端节点将是表示字符串 S 的节点。

我们来用伪代码总结一下以上策略：

    Initialize: cur = root
    for each char c in target string S:
        if cur does not have a child c:
            cur.children[c] = new Trie node
        cur = cur.children[c]
    cur is the node which represents the string S

通常情况情况下，你需要自己构建前缀树。构建前缀树实际上就是多次调用插入函数。但请记住在插入字符串之前要 初始化根节点 。

### 前缀树中的搜索操作

**搜索前缀**

正如我们在前缀树的简介中提到的，所有节点的后代都与该节点相对应字符串的有着共同前缀。因此，很容易搜索以特定前缀开头的任何单词。

同样地，我们可以根据给定的前缀沿着树形结构搜索下去。一旦我们找不到我们想要的子节点，搜索就以失败终止。否则，搜索成功。

我们来用伪代码总结一下以上策略：

    Initialize: cur = root
    for each char c in target string S:
        if cur does not have a child c:
            search fails
        cur = cur.children[c]
    search successes

**搜索单词**

你可能还想知道如何搜索特定的单词，而不是前缀。我们可以将这个词作为前缀，并同样按照上述同样的方法进行搜索。

1. 如果搜索失败，那么意味着没有单词以目标单词开头，那么目标单词绝对不会存在于前缀树中。

2. 如果搜索成功，我们需要检查目标单词是否是前缀树中单词的前缀，或者它本身就是一个单词。为了进一步解决这个问题，你可能需要稍对节点的结构做出修改。

> 提示：往每个节点中加入布尔值可能会有效地帮助你解决这个问题。

### 实现 Trie (前缀树)

实现一个 Trie (前缀树)，包含 insert, search, 和 startsWith 这三个操作。

示例:

    Trie trie = new Trie();

    trie.insert("apple");
    trie.search("apple");   // 返回 true
    trie.search("app");     // 返回 false
    trie.startsWith("app"); // 返回 true
    trie.insert("app");   
    trie.search("app");     // 返回 true

说明:

+ 你可以假设所有的输入都是由小写字母 a-z 构成的。
+ 保证所有输入均为非空字符串。

```c++
class Trie {
public:
    /** Initialize your data structure here. */
    Trie() {

    }
    
    /** Inserts a word into the trie. */
    void insert(string word) {

    }
    
    /** Returns if the word is in the trie. */
    bool search(string word) {

    }
    
    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix) {

    }
};

/**
 * Your Trie object will be instantiated and called as such:
 * Trie* obj = new Trie();
 * obj->insert(word);
 * bool param_2 = obj->search(word);
 * bool param_3 = obj->startsWith(prefix);
 */
```

### Solution

The key to this problem is to design the Trie node structure. In order to know if the string represented by the node is a word or not, we need an extra boolean flag.

When we insert a new word, we will set the flag in the end node of the word to be true. When we implement the startsWith method, we return true if we successfully find the path. However, when we implement the search method, we return true only if we successfully find the path and the flag of the end node is true.

We implement Trie using the map.

```c++
struct TrieNode {
    bool flag;
    map<char, TrieNode*> next; 
};
class Trie {
private:
    TrieNode* root;
    
public:
    /** Initialize your data structure here. */
    Trie() {
        root = new TrieNode();
    }
    
    /** Inserts a word into the trie. */
    void insert(string word) {
        TrieNode* p = root;
        for (int i = 0; i < word.length(); ++i) {
            if ((p->next).count(word[i]) <= 0) {
                // insert a new node if the path does not exist
                (p->next).insert(make_pair(word[i], new TrieNode()));
            }
            p = (p->next)[word[i]];
        }
        p->flag = true;
    }
    
    /** Returns if the word is in the trie. */
    bool search(string word) {
        TrieNode* p = root;
        for (int i = 0; i < word.length(); ++i) {
            if ((p->next).count(word[i]) <= 0) {
                return false;
            }
            p = (p->next)[word[i]];
        }
        return p->flag;
    }
    
    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix) {
        TrieNode* p = root;
        for (int i = 0; i < prefix.length(); ++i) {
            if ((p->next).count(prefix[i]) <= 0) {
                return false;
            }
            p = (p->next)[prefix[i]];
        }
        return true;
    }
};
```

或者用数组:

```c++
class Trie {
public:
    /** Initialize your data structure here. */
    Trie() { root = new TrieNode(); }

    /** Inserts a word into the trie. */
    void insert(string word) {
        TrieNode *cur = root;
        for (int i = 0; i < word.size(); ++i) {
            int tmp = word[i] - 'a';
            if (NULL == cur->children[tmp]) {
                cur->children[tmp] = new TrieNode();
            }
            cur = cur->children[tmp];
        }
        cur->isWord = true;
    }

    /** Returns if the word is in the trie. */
    bool search(string word) {
        TrieNode *cur = root;
        for (int i = 0; i < word.size(); ++i) {
            int tmp = word[i] - 'a';
            if (NULL == cur->children[tmp]) {
                return false;
            }
            cur = cur->children[tmp];
        }
        return cur->isWord;
    }

    /** Returns if there is any word in the trie that starts with the given
     * prefix. */
    bool startsWith(string prefix) {
        TrieNode *cur = root;
        for (int i = 0; i < prefix.size(); ++i) {
            int tmp = prefix[i] - 'a';
            if (NULL == cur->children[tmp]) {
                return false;
            }
            cur = cur->children[tmp];
        }
        return true;
    }

private:
    struct TrieNode {
        TrieNode *children[26];
        bool isWord;
        TrieNode() {
            for (int i = 0; i < 26; ++i) {
                children[i] = NULL;
            }
            isWord = false;
        }
    };
    TrieNode *root;
};
```

**Complexity Analysis**

Let's discuss the complexity of this algorithm.

If the longest length of the word is N, the height of Trie will be N + 1. Therefore, the time complexity of all insert, search and startsWith methods will be O(N).

How about space complexity?

If we have M words to insert in total and the length of words is at most N, there will be at most M*N nodes in the worst case (any two words don't have a common prefix).

Let's assume that there are maximum K different characters (K is equal to 26 in this problem, but might differs in different cases). So each node will maintain a map whose size is at most K.

Therefore, the space complexity will be `O(M*N*K)`.

It seems that Trie is really space consuming, however, the real space complexity of Trie is much smaller than our estimation(估计), especially when the distribution of words is dense(密集的).

You can also implement it by the array which will achieve a slightly better time performance but a slightly lower space performance.

**Comparison with Hash Table**

You might wonder why not use a hash table to store strings. Let's do a brief comparison between these two data structures. We assume there are N keys and the maximum length of a key is M.

1. Time Complexity

The time complexity to search in hash table is typically O(1), but will be O(logN) in the worst time if there are too many collisions and we solve collisions using height-balanced BST.

The time complexity to search in Trie is O(M).

The hash table wins in most cases.

2. Space Complexity

The space complexity of hash table is `O(M * N)`. If you want hash table to have the same function with Trie, you might need to store several copies of the key. For instance, you might want to store "a", "ap", "app", "appl" and also "apple" for a keyword "apple" in order to search by prefix. The space complexity can be even much larger in that case.

The space complexity of Trie is `O(M * N)` as we estimated above. But actually far smaller than the estimation since there will be a lot of words have the similar prefix in real cases.

Trie wins in most cases.

## 实际应用 I

前缀树广泛应用于 存储 字符串和 检索 关键字，特别是和 前缀 相关的关键字。

前缀树的实际应用场景非常简单。通常，你只需要提供插入的方法和一些与单词前缀相关的搜索操作。我们会在本章节后，为你提供一些配套习题。

当然，前缀树还会涉及到一些稍微复杂的实际问题：

1. [自动补全](https://en.wikipedia.org/wiki/Autocomplete)

![](https://leetcode.com/media/original_images/208_GoogleSuggest.png)

一个简单的实现自动补全的方法是在前缀树中存储ngram，并根据词频进行搜索推荐。请仔细思考，什么是解决这个问题的理想的节点结构。

2. [拼写检查](https://en.wikipedia.org/wiki/Spell_checker)（[添加与搜索单词 - 数据结构设计](https://leetcode-cn.com/leetbook/read/trie/x0jtri/)）

![](https://leetcode.com/media/original_images/208_SpellCheck.png)

在前缀树中找到具有相同前缀的单词很容易。但是怎么找到相似的单词呢？你可能需要运用一些搜索算法来解决这个问题。

[键值映射](../LeetCode/677_map-sum-pairs.md)