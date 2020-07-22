假设按照升序排序的数组在预先未知的某个点上进行了旋转。

( 例如，数组 [0,1,2,4,5,6,7] 可能变为 [4,5,6,7,0,1,2] )。

搜索一个给定的目标值，如果数组中存在这个目标值，则返回它的索引，否则返回 -1 。

你可以假设数组中不存在重复的元素。

你的算法时间复杂度必须是 O(log n) 级别。

示例 1:

    输入: nums = [4,5,6,7,0,1,2], target = 0
    输出: 4

示例 2:

    输入: nums = [4,5,6,7,0,1,2], target = 3
    输出: -1

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/search-in-rotated-sorted-array
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

以二分搜索为基本思路

简要来说：

+ nums[0] <= nums[mid]（0 - mid不包含旋转）且nums[0] <= target <= nums[mid] 时 high 向前规约；

+ nums[mid] < nums[0]（0 - mid包含旋转），target <= nums[mid] < nums[0] 时向前规约（target 在旋转位置到 mid 之间）

+ nums[mid] < nums[0]，nums[mid] < nums[0] <= target 时向前规约（target 在 0 到旋转位置之间）

+ 其他情况向后规约

也就是说nums[mid] < nums[0]，nums[0] > target，target > nums[mid] 三项均为真或者只有一项为真时向后规约。

上述三种情况可以总结如下：

    nums[0] <= target <= nums[i]
               target <= nums[i] < nums[0]
                         nums[i] < nums[0] <= target
所以我们进行三项判断：

(nums[0] <= target)， (target <= nums[i]) ，(nums[i] < nums[0])，现在我们想知道这三项中有哪两项为真（明显这三项不可能均为真或均为假（因为这三项可能已经包含了所有情况））

所以我们现在只需要区别出这三项中有两项为真还是只有一项为真。

使用 “异或” 操作可以轻松的得到上述结果（两项为真时异或结果为假，一项为真时异或结果为真，可以画真值表进行验证）

之后我们通过二分查找不断做小 target 可能位于的区间直到 left == right，此时如果 nums[left] == target 则找到了，如果不等则说明该数组里没有此项。

```c++
class Solution {
public:
    int search(vector<int> &nums, int target) {
        int left = 0, right = nums.size() - 1;
        while (left < right) {
            int mid = (left + right) >> 1;
            if ((nums[left] > target) ^ (nums[left] > nums[mid]) ^ (target > nums[mid]))
                left = mid + 1;
            else
                right = mid;
        }
        return left == right && nums[left] == target ? left : -1;
    }
};
```