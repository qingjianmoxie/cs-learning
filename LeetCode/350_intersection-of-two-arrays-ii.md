给定两个数组，编写一个函数来计算它们的交集。

示例 1：

    输入：nums1 = [1,2,2,1], nums2 = [2,2]
    输出：[2,2]

示例 2:

    输入：nums1 = [4,9,5], nums2 = [9,4,9,8,4]
    输出：[4,9]

说明：

+ 输出结果中每个元素出现的次数，应与元素在两个数组中出现次数的最小值一致。
+ 我们可以不考虑输出结果的顺序。

进阶：

+ 如果给定的数组已经排好序呢？你将如何优化你的算法？
+ 如果 nums1 的大小比 nums2 小很多，哪种方法更优？
+ 如果 nums2 的元素存储在磁盘上，磁盘内存是有限的，并且你不能一次加载所有的元素到内存中，你该怎么办？

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/intersection-of-two-arrays-ii
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

## 题解

### 解法一：排序双指针

将两个数组进行排序，随后用双指针顺序查找相同的元素
时间复杂度O(max(nlogn, mlogm, n+m))，空间复杂度O(1) (n,m分别为两个数组的长度)

如果是进阶问题一中已排序的数组，则只需O(n)的时间复杂度

c++代码:
```c++
class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        sort(nums1.begin(), nums1.end());
        sort(nums2.begin(), nums2.end());
        vector<int> res;
        int left(0), right(0);
        while (left < nums1.size() && right < nums2.size()) {
            if (nums1[left] == nums2[right]) {
                res.push_back(nums1[left]);
                left++;
                right++;
            } else if (nums1[left] < nums2[right]) {
                left++;
            } else {
                right++;
            }
        }
        return res;
    }
};
```

python代码:
```py
class Solution:
    def intersect(self, nums1: [int], nums2: [int]) -> [int]:
        nums1.sort()
        nums2.sort()
        r = []
        left, right = 0, 0
        while left < len(nums1) and right < len(nums2):
            if nums1[left] < nums2[right]:
                left += 1
            elif nums1[left] == nums2[right]:
                r.append(nums1[left])
                left += 1
                right += 1    
            else:
                right += 1
        return r
```

### 解法二: 哈希计数

将较小的数组哈希计数，随后在另一个数组中根据哈希来寻找。
时间复杂度O(max(n, m)) 空间复杂度O(min(n, m))

适用于进阶问题二

```c++
class Solution {
public:
    vector<int> intersect(vector<int>& nums1, vector<int>& nums2) {
        unordered_map<int, int> hashtb;
        vector<int> res;
        for (int i = 0; i < nums1.size(); i++) {
            hashtb[nums1[i]] += 1;
        }
        for (int j = 0; j < nums2.size(); j++) {
            if (hashtb[nums2[j]] > 0) {
                res.push_back(nums2[j]);
                hashtb[nums2[j]] -= 1;
            }
        }
        return res;
    }
};
```

### 解法三：通过归并外排将两个数组排序后再使用排序双指针查找

对应进阶问题三，如果内存十分小，不足以将数组全部载入内存，那么必然也不能使用哈希这类费空间的算法，只能选用空间复杂度最小的算法，即解法一。

但是解法一中需要改造，一般说排序算法都是针对于内部排序，一旦涉及到跟磁盘打交道（外部排序），则需要特殊的考虑。归并排序是天然适合外部排序的算法，可以将分割后的子数组写到单个文件中，归并时将小文件合并为更大的文件。当两个数组均排序完成生成两个大文件后，即可使用双指针遍历两个文件，如此可以使空间复杂度最低。

关于外部排序与JOIN，强烈推荐大家看一下 [数据库内核杂谈（六）：表的 JOIN（连接）](https://www.infoq.cn/article/6XGx92FyQ45cMXpj2mgZ) 这一系列数据库相关的文章
