给定一个 **没有重复** 数字的序列，返回其所有可能的全排列。

示例:

    输入: [1,2,3]
    输出:
    [
      [1,2,3],
      [1,3,2],
      [2,1,3],
      [2,3,1],
      [3,1,2],
      [3,2,1]
    ]

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/permutations
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> res;  //全局变量存储答案
        // 记录「路径」
        vector<int> path;  //全局变量存储路径
        backtrack(nums, path, res);
        return res;
    }

private:
    void backtrack(const vector<int>& nums, vector<int>& path, vector<vector<int>>& res) {
        if (path.size() == nums.size()) {
            res.push_back(path);
            return;
        }

        for (int i = 0; i < nums.size(); ++i) {
            // if (path.contains(nums[i]))
            vector<int>::iterator iter = find(path.begin(), path.end(), nums[i]);
            if (iter != path.end()) continue;
            path.push_back(nums[i]);
            backtrack(nums, path, res);
            path.pop_back();
        }
    }
};
```

[回溯算法](../DS&Algo/回溯算法.md)
