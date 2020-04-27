给定一个字符串，请你找出其中不含有重复字符的 **最长子串** 的长度。

示例 1:

    输入: "abcabcbb"
    输出: 3 
    解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。

示例 2:

    输入: "bbbbb"
    输出: 1
    解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。

示例 3:

    输入: "pwwkew"
    输出: 3
    解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
         请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/longest-substring-without-repeating-characters
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

方法1: 滑动窗口

    设立左指针a和右指针b
    b指针向右伸缩
    {
        对每个A[b]判断是否在之前的数组出现过;
        如果出现, 指针a指向出现过的位置的下一个位置;
        更新右指针和最大长度;
    }

该方法的时间复杂度O(n^2), 空间复杂度O(1).

```c++
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        //s[start,end) 前面包含 后面不包含
        int start(0), end(0), length(0), result(0);
        int sSize = int(s.size());
        while (end < sSize)
        {
            char tmpChar = s[end];
            for (int index = start; index < end; index++)
            {
                if (tmpChar == s[index])
                {
                    start = index + 1;
                    length = end - start;
                    break;
                }
            }

            end++;
            length++;
            result = result > length ? result : length;
        }
        return result;
    }
};
```

方法2: 利用hashmap优化

判断是否出现过时, 利用hashmap优化时间

该方法的时间复杂度O(n), 空间复杂度O(n)

```c++
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        //s[start,end) 前面包含 后面不包含
        int start(0), end(0), length(0), result(0);
        int sSize = int(s.size());
        unordered_map<char, int> hash;
        while (end < sSize)
        {
            char tmpChar = s[end];
            //仅当s[start,end) 中存在s[end]时更新start
            if (hash.find(tmpChar) != hash.end() && hash[tmpChar] >= start)
            {
                start = hash[tmpChar] + 1;
                length = end - start;
            }
            hash[tmpChar] = end;

            end++;
            length++;
            result = result > length ? result : length;
        }
        return result;
    }
};
```

方法3: 利用数组(桶)代替hashmap

判断是否出现过时, 利用桶来代替hashmap以优化时间.  
int[26]用于字母'a'-'z'或者'A'-'Z';  
int[128]用于ASCII码;  
int[256]用于扩展ASCII码.  
该方法的时间复杂度O(n), 空间复杂度O(n).

```c++
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        //s[start,end) 前面包含 后面不包含
        int start(0), end(0), length(0), result(0);
        int sSize = int(s.size());
        vector<int> vec(128, -1);
        while (end < sSize)
        {
            char tmpChar = s[end];
            //仅当s[start,end) 中存在s[end]时更新start
            if (vec[int(tmpChar)] >= start)
            {
                start = vec[int(tmpChar)] + 1;
                length = end - start;
            }
            vec[int(tmpChar)] = end;

            end++;
            length++;
            result = result > length ? result : length;
        }
        return result;
    }
};
```
