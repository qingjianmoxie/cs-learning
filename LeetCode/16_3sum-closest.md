给定一个包括 n 个整数的数组 nums 和 一个目标值 target。找出 nums 中的三个整数，使得它们的和与 target 最接近。返回这三个数的和。假定每组输入只存在唯一答案。

    例如，给定数组 nums = [-1，2，1，-4], 和 target = 1.
    
    与 target 最接近的三个数的和为 2. (-1 + 2 + 1 = 2).

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/3sum-closest
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        sort(nums.begin(), nums.end());
        int res = nums[0] + nums[1] + nums[2] - target;
        int size = int(nums.size());
        for (int i = 0; i < size - 2; ++i)
        {
            int left = i + 1;
            int right = size - 1;
            while (left < right)
            {
                int tmp = nums[i] + nums[left] + nums[right] - target;
                res = abs(res) > abs(tmp) ? tmp : res;
                if (tmp > 0)
                    while (left < right && nums[right] == nums[--right])
                        ;
                else if (tmp < 0)
                    while (left < right && nums[left] == nums[++left])
                        ;
                else
                    return target;
            }
        }
        return target + res;
    }
};
```
