将十进制的整数转换为字符串. int10_to_str()函数是int2str()函数针对基数为10/-10的情况进行的优化.

参数:
    val   - 待转换的数字
    dst   - 指向 用来存储字符串的buffer 的指针
    radix - 基数

返回值:
    指向结尾'\0'字符的指针

```c++
char *int10_to_str(long int val,char *dst,int radix)
{
  char buffer[65];
  char *p;
  long int new_val;
  unsigned long int uval = (unsigned long int) val;

  if (radix < 0)				/* -10 */
  {
    if (val < 0)
    {
      *dst++ = '-';
      /* Avoid integer overflow in (-val) for LLONG_MIN (BUG#31799). */
      uval = (unsigned long int)0 - uval;
    }
  }

  p = &buffer[sizeof(buffer)-1];
  *p = '\0';
  new_val= (long) (uval / 10);
  *--p = '0'+ (char) (uval - (unsigned long) new_val * 10);
  val = new_val;

  while (val != 0)
  {
    new_val=val/10;
    *--p = '0' + (char) (val-new_val*10);
    val= new_val;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}
```

调用该函数时, 会修改buffer中的内容, 但是不会影响调用者的buffer指针. 举个例子

```c
#include <stdio.h>

void test(char *buff) {
    *buff++ = '1';
    *buff = '2';
    printf("test %s\n", buff);
}

int main() {
    char buff[3] = "ab";
    printf("begin %s\n", buff);
    test(buff);
    printf("end %s\n", buff);
    return 0;
}
```

输出结果为:

    begin ab
    test 2
    end 12
