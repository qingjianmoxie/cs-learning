实现获取下一个排列的函数，算法需要将给定数字序列重新排列成字典序中下一个更大的排列。

如果不存在下一个更大的排列，则将数字重新排列成最小的排列（即升序排列）。

必须原地修改，只允许使用额外常数空间。

以下是一些例子，输入位于左侧列，其相应输出位于右侧列。
1,2,3 → 1,3,2
3,2,1 → 1,2,3
1,1,5 → 1,5,1

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/next-permutation
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

本题重要的是题目的理解与思路转换。
思路：
从最末位寻找第一个破坏升序的数nums[pos - 1], 然后在遍历过的数里寻找比该数大的最小的一个数。
如遍历过的数为[7,6,4,3], nums[pos - 1]为5, 则5需要与6进行交换, 在将[7,5,4,3]改为升序(这里使用reverse)。

遍历过的数从后往前一定是升序, 故改为从前往后升序只需要反转该部分即可。
寻找第一个比nums[pos - 1]大的数, 只需改为从前往后升序之后, 从升序的第一个数开始遍历比较大小即可。

```c++
class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        int pos = nums.size() - 1;
        while (pos > 0 && nums[pos - 1] >= nums[pos]) {
            --pos;
        }
        reverse(nums.begin() + pos, nums.end());
        if (pos > 0) {
            for (int i = pos; i < nums.size(); ++i) {
                if (nums[i] > nums[pos - 1]) {
                    swap(nums[pos - 1], nums[i]);
                    break;
                }
            }
        }
    }
};
```

v2:稍微改进下, 寻找第一个大于nums[pos - 1]的数原来为线性时间复杂度, 可以改用二分查找, 这里使用STL标准库upper_bound()

```c++
class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        int pos = nums.size() - 1;
        while (pos > 0 && nums[pos - 1] >= nums[pos]) {
            --pos;
        }
        reverse(nums.begin() + pos, nums.end());
        if (pos > 0) {
            auto iter = upper_bound(nums.begin() + pos, nums.end(), nums[pos - 1]);
            swap(*iter, nums[pos - 1]);
        }
    }
};
```
