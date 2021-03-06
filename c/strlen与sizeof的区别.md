# strlen与sizeof的区别

C语言中没有字符串，用的是字符数组来模拟字符串。

C风格的字符串时字符数组然后在末尾加'\0'表示结尾。

在C语言中strlen和sizeof都可以求字符串的长度，他俩的区别如下。

strlen是不加的，他表示字符串的长度。

而sizeof求的是字符串在内存中的长度，所以它是加上最后的'\0'的

所以一般而言后者的长度会比前者多1。

strlen 是一个函数，它用来计算指定字符串 str 的长度，但不包括结束字符(即'\0')。其原型如下面的代码所示：
```c
size_t strlen(char const* str);
```

也正因为 strlen 是一个函数，所以需要进行一次函数调用，调用示例如下面的代码所示：

```c
char sArr[] = "ILOVEC";
/*用strlen()求长度*/
printf("sArr的长度=%d\n", strlen(sArr));
```

很显然，上面示例代码的运行结果为6 (因为不包括结束字符'\0')。这里需要特别注意的是，函数 strlen 返回的是一个类型为 size_t 的值，从而有可能让程序导致意想不到的结果，如下面的示例代码所示：

```c
/* 判断一 */
if(strlen(x) >= strlen(y))
{
}
/* 判断二 */
if(strlen(x) - strlen(y) >= 0)
{
}
```

从表面上看，上面的两个判断表达式完全相等，但实际情况并非如此。其中，判断表达式一没什么问题，程序也能够完全按照预想的那样工作；但判断表达式二的结果就不一样了，它将永远是真，这是为什么呢？

原因很简单，因为函数 strlen 的返回结果是 size_t 类型（即无符号整型），而 size_t 类型绝不可能是负的。因此，语句`if(strlen(x)-strlen(y)>=0)`将永远为真。

同样，就算表达式中同时包含了有符号整数和无符号整数，还是有可能产生意想不到的结果，如下面的代码所示：

```c
/*判断一*/
if(strlen(x)>= 5)
{
}
/*判断二*/
if(strlen(x)- 5>=0)
{
}
```

很显然，判断表达式二的结果还是永远是真，其原因与上面相同。

关键字 sizeof 是一个单目运算符，而不是一个函数。与函数 strlen 不同，它的参数可以是数组、指针、类型、对象、函数等，如下面的示例代码所示：

```c
char sArr[] = "ILOVEC";
/*用sizeof求长度*/
printf("sArr的长度=%d\n", sizeof(sArr));
```

相对于函数 strlen，这里的示例代码运行结果为7 (因为它包括结束字符'\0')。同时，对 sizeof 而言，因为缓冲区已经用已知字符串进行了初始化，其长度是固定的，所以 sizeof 在编译时计算缓冲区的长度。也正是由于在编译时计算，因此 sizeof 不能用来返回动态分配的内存空间的大小。
