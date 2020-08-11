#include <iostream>

using std::cout;
using std::endl;

template <typename T>
void Insertion_Sort(T A[], int N) {
    // 第一个元素相当于有序, 所以P从1开始, 而不用从0开始
    for (int i = 1; i < N; i++) {
        T tmp = A[i]; /* 摸下一张牌 */
        int j;
        for (j = i; j > 0 && A[j - 1] > tmp; j--)
            A[j] = A[j - 1]; /* 移出空位 */
        A[j] = tmp;          /* 新牌落位 */
    }
}

int main() {
    const int N = 4;
    int arr[N] = {9, 5, 2, 7};
    Insertion_Sort(arr, N);
    for (int i = 0; i < N; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}
