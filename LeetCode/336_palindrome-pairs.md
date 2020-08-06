给定一组**唯一**的单词， 找出所有**不同** 的索引对(i, j)，使得列表中的两个单词， words[i] + words[j] ，可拼接成回文串。

示例 1:

    输入: ["abcd","dcba","lls","s","sssll"]
    输出: [[0,1],[1,0],[3,2],[2,4]] 
    解释: 可拼接成的回文串为 ["dcbaabcd","abcddcba","slls","llssssll"]

示例 2:

    输入: ["bat","tab","cat"]
    输出: [[0,1],[1,0]] 
    解释: 可拼接成的回文串为 ["battab","tabbat"]

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/palindrome-pairs
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

暴力解法, 可以预见到, 超时了.

```c++
class Solution {
public:
    vector<vector<int>> palindromePairs(vector<string>& words) {
        vector<vector<int>> res;
        int size = words.size();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (i == j) continue;
                string tmp = words[i] + words[j];
                if (isPalindrome(tmp)) {
                    res.push_back({i, j});
                }
            }
        }
        return res;
    }

    bool isPalindrome(const string& str) {
        int i = 0;
        int j = str.size() - 1;
        while (i < j) {
            if (str[i++] != str[j--]) {
                return false;
            }
        }
        return true;
    }
};
```

时间复杂度为`O(N*N*K)`, N 为 words 列表的长度, K 为字符串的平均长度.

[优质题解](https://leetcode-cn.com/problems/palindrome-pairs/solution/hui-wen-dui-by-leetcode-solution/)

### 方法一：枚举前缀和后缀

思路及算法

假设存在两个字符串 s1 和 s2, s1+s2 是一个回文串，记这两个字符串的长度分别为 len1 和 len2，我们分三种情况进行讨论：

1. len1 == len2，这种情况下 s1是 s2 的翻转。
2. len1 > len2，这种情况下我们可以将 s1 拆成左右两部分：t1 和 t2，其中 t1 是 s2 的翻转，t2 是一个回文串。
3. len1 < len2，这种情况下我们可以将 s2 拆成左右两部分：t1 和 t2，其中 t2 是 s1 的翻转，t1 是一个回文串。

这样，对于每一个字符串，我们令其为 s1 和 s2 中较长的那一个，然后找到可能和它构成回文串的字符串即可。

具体地说，我们枚举每一个字符串 k，令其为 s1 和 s2 中较长的那一个，那么 k 可以被拆分为两部分，t1 和 t2 。

1. 当 t1 是回文串时，符合情况 3，我们只需要查询给定的字符串序列中是否包含 t2 的翻转。
2. 当 t2 是回文串时，符合情况 2，我们只需要查询给定的字符串序列中是否包含 t1 的翻转。

也就是说，我们要枚举字符串 k 的每一个前缀和后缀，判断其是否为回文串。如果是回文串，我们就查询其剩余部分的翻转是否在给定的字符串序列中出现即可。

注意到空串也是回文串，所以我们可以将 k 拆解为 k+∅ 或 ∅+k，这样我们就能将情况 1 也解释为特殊的情况 2 或情况 3。

而要实现这些操作，我们只需要设计一个能够在一系列字符串中查询「某个字符串的子串的翻转」是否存在的数据结构，有两种实现方法：

+ 我们可以使用字典树存储所有的字符串。在进行查询时，我们将待查询串的子串逆序地在字典树上进行遍历，即可判断其是否存在。

+ 我们可以使用哈希表存储所有字符串的翻转串。在进行查询时，我们判断带查询串的子串是否在哈希表中出现，就等价于判断了其翻转是否存在。

下面给出使用字典树的代码:
```c++
class Solution {
    public List<List<Integer>> palindromePairs(String[] words) {
        List<List<Integer>> ans = new ArrayList<>();
        int n = words.length;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) continue;
                if (!check(words[i]+words[j])) continue;
                List<Integer> temp = new ArrayList<>();
                temp.add(i); temp.add(j);
                ans.add(temp);
            }
        }
        return ans;
    }
    private boolean check(String s) {
        int i = 0, j = s.length()-1;
        while (i < j) {
            if (s.charAt(i) != s.charAt(j)) return false;
            i++; j--;
        }
        return true;
    }
}
```