# explicit关键字

首先, C++中的`explicit`关键字只能用于修饰只有一个参数的类构造函数, 它的作用是表明该构造函数是显示的, 而非隐式的, 跟它相对应的另一个关键字是`implicit`, 意思是隐藏的,类构造函数默认情况下即声明为implicit(隐式).

那么显示声明的构造函数和隐式声明的有什么区别呢? 我们来看下面的例子:

```c++
class CxString
{
public:
    char *pstr_;
    int size_;
    // 没有使用explicit关键字的类声明, 即默认为隐式声明
    CxString(int size) {
        size_ = size;              // string的预设大小
        pstr_ = malloc(size + 1);  // 分配string的内存
        memset(pstr_, 0, size + 1);
    }
    CxString(const char *p) {
        int size = strlen(p);
        pstr_ = malloc(size + 1);  // 分配string的内存
        strcpy(pstr_, p);          // 复制字符串
        size_ = strlen(pstr_);
    }
    // 析构函数这里不讨论, 省略...
};
// 下面是调用:
CxString string1(24);  // OK, 为CxString预分配24字节大小的内存
CxString string2 = 10;  // OK, 为CxString预分配10字节大小的内存
CxString string3;  // ERR, 因为没有默认构造函数, 错误为: “CxString”: 没有合适的默认构造函数可用
CxString string4("aaaa");  // OK
CxString string5 = "bbb";  // OK, 调用的是CxString(const char *p)
CxString string6 = 'c';  // OK, 其实调用的是CxString(int size), 且size等于'c'的ascii码
string1 = 2;  // OK, 为CxString预分配2字节的大小的内存
string2 = 3;  // OK, 为CxString预分配3字节的大小的内存
string3 = string1;  // OK, 至少编译是没问题的, 但是如果析构函数里用free释放pstr_内存指针的时候可能会报错, 完整的代码必须重载运算符"=", 并在其中处理内存释放
```

上面的代码中, `CxString string2 = 10;` 这句为什么是可以的呢?

在C++中, 如果的构造函数只有一个参数时, 那么在编译的时候就会有一个缺省的转换操作: 将该构造函数对应数据类型的数据转换为该类对象. 也就是说 `CxString string2 = 10;` 这段代码, 编译器自动将整型转换为CxString类对象, 实际上等同于下面的操作:

```c++
CxString string2(10);  
或  
CxString temp(10);  
CxString string2 = temp;
```

但是, 上面的代码中的`size_`代表的是字符串内存分配的大小, 那么调用的第二句 `CxString string2 = 10;` 和第六句 `CxString string6 = 'c';` 就显得不伦不类, 而且容易让人疑惑. 有什么办法阻止这种用法呢? 答案就是使用explicit关键字. 我们把上面的代码修改一下, 如下:

```c++
class CxString
{
public:
    char *pstr_;
    int size_;
    // 使用关键字explicit的类声明, 显示转换
    explicit CxString(int size) {
        size_ = size;  // 代码同上, 省略...
    }
    CxString(const char *p) {
        // 代码同上, 省略...
    }
};
// 下面是调用:
CxString string1(24);  // OK
CxString string2 = 10;  // ERR, 因为explicit关键字取消了隐式转换
CxString string3;          // ERR, 因为没有默认构造函数
CxString string4("aaaa");  // OK
CxString string5 = "bbb";  // OK, 调用的是CxString(const char *p)
CxString string6 = 'c';  // ERR, 其实调用的是CxString(int size), 且size等于'c'的ascii码, 但explicit关键字取消了隐式转换
string1 = 2;  // ERR, 因为取消了隐式转换
string2 = 3;  // ERR, 因为取消了隐式转换
string3 = string1;  // ERR, 因为取消了隐式转换, 除非类实现操作符"="的重载
```

`explicit`关键字的作用就是防止类构造函数的隐式自动转换.

上面也已经说过了, explicit关键字只对有一个参数的类构造函数有效, 如果类构造函数参数大于或等于两个时, 是不会产生隐式转换的, 所以explicit关键字也就无效了. 例如: 

```c++
class CxString
{
   public:
    char *pstr_;
    int age_;
    int size_;
    // explicit关键字在类构造函数参数大于或等于两个时无效
    explicit CxString(int age, int size) {
        age_ = age;
        size_ = size;
        // 代码同上, 省略...
    }
    CxString(const char *p) {
        // 代码同上, 省略...
    }
};  // 这个时候有没有explicit关键字都是一样的
```

但是, 也有一个例外, 就是当除了第一个参数以外的其他参数都有默认值的时候, explicit关键字依然有效, 此时, 当调用构造函数时只传入一个参数, 等效于只有一个参数的类构造函数, 例子如下:

```c++
class CxString
{
   public:
    int age_;
    int size_;
    // 使用关键字explicit声明
    explicit CxString(int age, int size = 0) {
        age_ = age;
        size_ = size;
        // 代码同上, 省略...
    }
    CxString(const char *p) {
        // 代码同上, 省略...
    }
};
// 下面是调用:
CxString string1(24);   // OK
CxString string2 = 10;  // ERR, 因为explicit关键字取消了隐式转换
CxString string3;       // ERR, 因为没有默认构造函数
string1 = 2;            // ERR, 因为取消了隐式转换
string2 = 3;            // ERR, 因为取消了隐式转换
string3 = string1;  // ERR, 因为取消了隐式转换, 除非类实现操作符"="的重载
```

以上即为C++ explicit关键字的详细介绍.

总结：

+ explicit关键字只用于类内的单参数构造函数前面。由于无参数的构造函数和多参数的构造函数总是显示调用，这种情况在构造函数前加explicit无意义。

+ google的c++规范中提到explicit的优点是可以避免不合时宜的类型变换，缺点无。所以google约定所有单参数的构造函数都必须是显示的，只有极少数情况下拷贝构造函数可以不声明称explicit。例如作为其他类的透明包装器的类。

+ effective c++中说：被声明为explicit的构造函数通常比其non-explicit兄弟更受欢迎。因为它们禁止编译器执行非预期（往往也不被期望）的类型转换。