#include <iostream>

using std::cout;
using std::endl;

template <typename T>
void Shell_sort(T A[], int N) {
    for (int D = N / 2; D > 0; D /= 2) { /* 希尔增量序列 */
        // 第一个元素相当于有序, 所以i从D开始, 而不用从0开始
        for (int i = D; i < N; i++) { /* 插入排序 */
            T tmp = A[i];
            int j;
            // 需要保证j-D>=0, 所以中间的条件为j >= D
            for (j = i; j >= D && A[j - D] > tmp; j -= D) {
                A[j] = A[j - D];
            }
            A[j] = tmp;
        }
    }
}

int main() {
    const int N = 4;
    int arr[N] = {9, 5, 2, 7};
    Shell_sort(arr, N);
    for (int i = 0; i < N; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}
