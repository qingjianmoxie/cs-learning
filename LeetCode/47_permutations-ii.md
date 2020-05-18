给定一个可包含重复数字的序列，返回所有不重复的全排列。

示例:

    输入: [1,1,2]
    输出:
    [
      [1,1,2],
      [1,2,1],
      [2,1,1]
    ]

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/permutations-ii
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    void backtrace(map<int, int>& m, int k, int n, vector<int>& path, vector<vector<int> >& res) {
        if (k == n) {
            res.push_back(path);
            return;
        }
        for (auto& p : m) {
            if (p.second == 0) continue;
            --p.second;
            path.push_back(p.first);
            backtrace(m, k + 1, n, path, res);
            ++p.second;
            path.pop_back();
        }
    }
    vector<vector<int>> permuteUnique(vector<int>& nums) {
        map<int, int> m;
        for (auto i : nums) ++m[i];
        vector<vector<int> > res;
        vector<int> path;
        backtrace(m, 0, nums.size(), path, res);
        return res;
    }
};
```
