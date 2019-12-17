typedef int ElementType;

void Shell_sort(ElementType A[], int N)
{
    for (int D = N / 2; D > 0; D /= 2)
    { /* 希尔增量序列 */
        for (int P = D; P < N; P++)
        { /* 插入排序 */
            ElementType Tmp = A[P];
            int i;
            for (i = P; i >= D && A[i - D] > Tmp; i -= D)
                A[i] = A[i - D];
            A[i] = Tmp;
        }
    }
}