给定一个**非空**字符串 s 和一个包含**非空**单词列表的字典 wordDict，判定 s 是否可以被空格拆分为一个或多个在字典中出现的单词。

说明：

+ 拆分时可以重复使用字典中的单词。
+ 你可以假设字典中没有重复的单词。

示例 1：

    输入: s = "leetcode", wordDict = ["leet", "code"]
    输出: true
    解释: 返回 true 因为 "leetcode" 可以被拆分成 "leet code"。

示例 2：

    输入: s = "applepenapple", wordDict = ["apple", "pen"]
    输出: true
    解释: 返回 true 因为 "applepenapple" 可以被拆分成 "apple pen apple"。
         注意你可以重复使用字典中的单词。

示例 3：

    输入: s = "catsandog", wordDict = ["cats", "dog", "sand", "and", "cat"]
    输出: false

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/word-break
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    vector<int> store;
    bool wordBreak(string s, vector<string> &wordDict) {
        if (s.empty()) return true;
        int size_s = s.size();
        for (auto x : wordDict) {
            int size_x = x.size();
            if (size_s < size_x) continue;
            if (s.substr(0, size_x) == x) {
                // 剪枝, 如果没有会超时
                if (find(store.begin(), store.end(), size_s - size_x) != store.end())
                    continue;
                string tmp_s = s.substr(size_x, size_s - size_x);
                store.push_back(size_s - size_x);
                if (wordBreak(tmp_s, wordDict))
                    return true;
                else
                    continue;
            }
        }
        return false;
    }
};
```