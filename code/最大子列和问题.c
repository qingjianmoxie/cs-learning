// 输入第1行给出正整数K(≤100000)；第2行给出K个整数，其间以空格分隔。
// 输入样例:
// 6
// -2 11 -4 13 -5 -2
// 输出样例:
// 20
#include <stdio.h>

int MaxSubseqSum(int A[], int N)
{
    int ThisSum = 0;
    int MaxSum = 0;
    for (int i = 0; i < N; i++)
    {
        ThisSum += A[i];
        if (ThisSum > MaxSum)
        {
            MaxSum = ThisSum;
        }
        else if (ThisSum < 0)
        {
            ThisSum = 0;
        }
    }
    return MaxSum;
}

int main()
{
    int num;
    scanf("%d\n", &num);
    int a[num];
    for (int i = 0; i < num; ++i)
    {
        scanf("%d\n", &a[i]);
    }
    int sum = 0;
    sum = MaxSubseqSum(a, num);
    printf("%d\n", sum);
    return 0;
}