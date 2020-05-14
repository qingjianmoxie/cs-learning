给定一个按照升序排列的整数数组 nums，和一个目标值 target。找出给定目标值在数组中的开始位置和结束位置。

你的算法时间复杂度必须是 O(log n) 级别。

如果数组中不存在目标值，返回 [-1, -1]。

示例 1:

    输入: nums = [5,7,7,8,8,10], target = 8
    输出: [3,4]

示例 2:

    输入: nums = [5,7,7,8,8,10], target = 6
    输出: [-1,-1]

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/find-first-and-last-position-of-element-in-sorted-array
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Solution {
private:
    int findFirstPosition(const vector<int>& nums, int target) {
        int left = 0;
        int right = nums.size() - 1;
        int mid = 0;
        while (left < right) {
            // 取左中位数
            mid = (left + right) >> 1;
            if (nums[mid] > target)
                right = mid - 1;
            else if (nums[mid] < target)
                left = mid + 1;
            else
                right = mid;
        }
        if (nums[left] != target) return -1;
        return left;
    }

    int findLastPosition(const vector<int>& nums, int target) {
        int left = 0;
        int right = nums.size() - 1;
        int mid = 0;
        while (left < right) {
            // 取右中位数, 否则会出现死循环
            mid = (left + right + 1) >> 1;
            if (nums[mid] > target)
                right = mid - 1;
            else if (nums[mid] < target)
                left = mid + 1;
            else
                left = mid;
        }
        if (nums[left] != target) return -1;
        return left;
    }

public:
    vector<int> searchRange(vector<int>& nums, int target) {
        if (nums.empty()) return {-1, -1};
        int left = findFirstPosition(nums, target);
        if (-1 == left) return {-1, -1};
        int right = findLastPosition(nums, target);
        return {left, right};
    }
};
```

复杂度分析：

时间复杂度：O(logN)，这里 N 是数组的长度，两个子问题都是二分查找，因此时间复杂度为对数级别。
空间复杂度：O(1)，只使用了常数个数的辅助变量、指针。
