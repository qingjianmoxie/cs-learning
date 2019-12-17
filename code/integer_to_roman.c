#include <stdio.h>

int main()
{
    const char *const c[4][10] = {
        {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"},
        {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"},
        {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
        {"", "M", "MM", "MMM"}}; // 建立对照表
    int num, m, i;
    printf("please enter number:");
    scanf("%d", &num);
    printf("%d = ", num);
    for (m = 0, i = 1000; m < 4; m++, i /= 10)
    {
        //int t = (num % i) / (i / 10); 两个式子都可以用，这个式子中i初始值为10000
        int t = num / i % 10;
        printf("%s", c[3 - m][t]);
    }
    printf("\n");
    return 0;
}