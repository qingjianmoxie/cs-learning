/* 归并排序 - 递归实现 */

#include <iostream>
#include <vector>

// 归并排序的合并函数
template <typename T>
void Merge(std::vector<T> &arr, std::vector<T> tmp, int left, int mid, int right) {
    int i = left;     // i是第一段序列的下标
    int j = mid + 1;  // j是第二段序列的下标
    int k = left;     // k是临时存放合并序列的下标

    // 扫描第一段和第二段序列，直到有一个扫描结束
    while (i <= mid && j <= right) {
        // 判断第一段和第二段取出的数哪个更小，将其存入合并序列，并继续向下扫描
        if (arr[i] <= arr[j]) {
            tmp[k++] = arr[i++];
        } else {
            tmp[k++] = arr[j++];
        }
    }
    // 若第一段序列还没扫描完，将其全部复制到合并序列
    while (i <= mid) {
        tmp[k++] = arr[i++];
    }

    // 若第二段序列还没扫描完，将其全部复制到合并序列
    while (j <= right) {
        tmp[k++] = arr[j++];
    }

    // 将合并序列复制到原始序列中
    for (k = left; k <= right; k++)
        arr[k] = tmp[k];
}

// 归并排序
template <typename T>
void Msort(std::vector<T> &arr, std::vector<T> tmp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        Msort(arr, tmp, left, mid);        /* 递归解决左边 */
        Msort(arr, tmp, mid + 1, right);   /* 递归解决右边 */
        Merge(arr, tmp, left, mid, right); /* 合并两段有序序列 */
    }
}

// 归并排序
template <typename T>
void MergeSort(std::vector<T> &arr) {
    // 在排序前，先建好一个长度等于原数组长度的临时数组，避免递归中频繁开辟空间
    std::vector<T> tmp(arr.size());
    Msort(arr, tmp, 0, arr.size() - 1);
}

int main() {
    std::vector<int> test = {6, 4, 8, 9, 2, 3, 1};
    std::cout << "排序前:";
    for (int i = 0; i < test.size(); i++) {
        std::cout << test[i] << " ";
    }
    std::cout << std::endl;

    std::vector<int> result = test;
    MergeSort(result);
    std::cout << "排序后:";
    for (int i = 0; i < result.size(); i++) {
        std::cout << result[i] << " ";
    }
    std::cout << std::endl;
}
