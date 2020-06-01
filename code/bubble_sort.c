typedef int ElementType;

void Bubble_Sort(ElementType A[], int N) {
    for (int P = N - 1; P > 0; P--) {
        int flag = 0;
        for (int i = 0; i < P; i++) { /* 一趟冒泡 */
            if (A[i] > A[i + 1]) {
                Swap(A[i], A[i + 1]);
                flag = 1; /* 标识发生了交换 */
            }
        }
        if (flag == 0) break; /* 全程无交换 */
    }
}
