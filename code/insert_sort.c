typedef int ElementType;

void Insertion_Sort(ElementType A[], int N)
{
    // 第一个元素相当于有序, 所以P从1开始, 而不用从0开始
    for (int P = 1; P < N; P++)
    {
        ElementType Tmp = A[P]; /* 摸下一张牌 */
        int i;
        for (i = P; i > 0 && A[i - 1] > Tmp; i--)
            A[i] = A[i - 1]; /* 移出空位 */
        A[i] = Tmp;          /* 新牌落位 */
    }
}