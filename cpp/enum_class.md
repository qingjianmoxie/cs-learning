# C++11的enum class & enum struct和enum

[C++标准文档——n2347（学习笔记）](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2347.pdf)

# 1. 旧版enum存在的问题

1. 向整形的隐式转换(Implicit conversion to an integer)
2. 无法指定底层所使用的数据类型(Inability to specify underlying type)
3. enum的作用域(Scope)
4. 不同编译器解决该问题的方法不统一

## 1.1 问题1：向整形的隐式转换

**在开始这个问题之前，我们需要知道什么是整形提升**

**查看之前的博文：**[C\C++中的整形提升](http://blog.csdn.net/sanoseiichirou/article/details/50171727)

在看完什么是整形提升之后，我们开始这个问题：

> 旧版enum其实并不具有非常完全的类型安全（当然它也体现了一定的类型安全：1.禁止不同枚举体之间的赋值 2.禁止整形向枚举体的隐式转换等），也就是面对整形提升，旧版enum是没有抗拒力的。

例如：

```c++
#include <iostream>

enum colorA{redA,greenA,grayA};
enum colorB {redB,greenB,yellowB};

void test(int data) {
    std::cout << "test called" << std::endl;
}

int main() {
    colorA ca(redA);
    colorB cb(greenB);

    //ca = cb; ERROR , 无法从“colorB”转换为“colorA”
    //ca = 2;  ERROR , 无法从“int”转换为“colorA”

    bool res(ca < cb); //OK
    std::cout << std::boolalpha << res << std::endl;

    test(ca); //OK

    return 0;
}
```

**运行结果：**

> true
> test called

**就像上面的代码：我们仍然可以比较两个不同枚举体的大小，用枚举体调用参数为int的函数。显然此时的枚举体发生了 整形提升 。**

在无法使用C++11新版enum的情况下，机制的程序员想到了：将enum封装到类的内部的方法。

```c++
#include <iostream>

class Color {
private:
    enum _color { _red, _blue, _yellow, _black };
public:
    explicit Color(const _color & other) {
        value = value;
    }
    explicit Color(const Color & other) {
        value = other.value;
    }
    const Color& operator=(const Color& other) {
        value = other.value;
        return *this;
    }
    static const Color red, blue, yellow, black;
    _color value;
    //operators
    bool operator <(const Color & other) { return value < other.value; }
    bool operator >(const Color & other) { return value > other.value; }
    bool operator <=(const Color & other) { return value <= other.value; }
    bool operator >=(const Color & other) { return value >= other.value; }
    bool operator ==(const Color & other) { return value == other.value; }
    //...

    //conversion
    int toint() { return value; }
};
//init static const Color obj
const Color Color::red(Color::_color::_red);
const Color Color::blue(Color::_color::_blue);
const Color Color::yellow(Color::_color::_yellow);
const Color Color::black(Color::_color::_black);

void test(int data) {
    std::cout << "called" << std::endl;
}

int main() {
    Color ca(Color::blue);
    std::cout << ca.toint() << std::endl;

    //ca = 2; ERROR, 没有找到接受“int”类型的右操作数的运算符(或没有可接受的转换)
    //test(ca); ERROR, 无法将参数 1 从“Color”转换为“int”
    //bool res(ca > 2); ERROR,没有找到接受“int”类型的右操作数的运算符(或没有可接受的转换)

    return 0;
}
```

的确，封装在类中的enum能够抵抗整形提升。但是这种enum不同于POD(plain old data)，无法放入寄存器当中，这会带来额外的开销。

## 1.2 问题2：无法指定底层所使用的数据类型

1. 首先，无法指定数据类型，导致我们无法明确枚举类型所占的内存大小。这种麻烦在结构体当中尤为突出，特别是当我们需要内存对齐和填充处理的时候。

```c++
#include <iostream>

enum Version { Ver1 = 1, Ver2, Ver3 };

struct MyStruct {
    MyStruct(Version ver) { this->Ver = ver; }
    Version Ver;
    //Ohters...
};

int main() {
    MyStruct m(Version::Ver1);
    return 0;
}
```

此时我们的解决办法还是：不使用enum

```c++
#include <iostream>

enum Version { Ver1 = 1, Ver2, Ver3 };

struct MyStruct {
    MyStruct(Version ver) { this->Ver = ver; }
    unsigned char Ver;//将enum Version转为unsigned char类型
    //Ohters...
};

int main() {
    MyStruct m(Version::Ver1);
    return 0;
}
```

2. 其次，当我们使用enum时，我们无法决定编译器底层是如何对待enum的（比如：signed和unsigned）。

```c++
#include <iostream>

enum MyEnum { num1 = 1, num2 = 2, numn = 0xFFFFFF00U };

int main() {
    std::cout << num1 << std::endl;
    std::cout << num2 << std::endl;
    std::cout << numn << std::endl;
    return 0;
}
```

VS2015运行结果：

> 1
> 2
> -256

CodeBlocks运行结果：

> 1
> 2
> 4294967040

在 numn=0xFFFFFF00U中，我们希望0xFFFFFF00表现为unsigned。但是不同的编译器其标准也不同。这就给我们的程序带来了许多的不确定性。

在文档n2347中的例子：不同编译器对0xFFFFFFF0U的表现。

```c++
#include <iostream>
using namespace std;
enum E { E1 = 1, E2 = 2, Ebig = 0xFFFFFFF0U };
int main() {
    cout << sizeof(E) << endl;
    cout << "Ebig = " << Ebig << endl;
    cout << "E1 ? -1 =\t" << (E1 < -1 ? "less" : E1 > -1 ? "greater" : "equal") << endl;
    cout << "Ebig ? -1 =\t" << (Ebig < -1 ? "less" : Ebig > -1 ? "greater" : "equal") << endl;
}
```

![](https://img-blog.csdn.net/20151204130336056)

## 1.3 问题3：enum的作用域

> **enum的中的 " { } " 大括号并没有将枚举成员的可见域限制在大括号内，导致enum成员曝露到了上一级作用域(块语句)中。**

**例如：**

```c++
#include <iostream>
enum color{red,blue};//定义拥有两个成员的enum，red和blue在enum的大括号外部可以直接访问，而不需要使用域运算符。
int main() {
    std::cout << blue << std::endl;
    return 0;
}
```

运行结果：

> 1

> **就如上面的代码，我们可以在blue的大括号之外访问它，color的成员被泄露到了该文件的全局作用域中（虽然它尚不具备外部链接性）。可以直接访问，而不需要域运算符的帮助。**

> **但是这不是关键，有时我们反而觉得非常方便。下面才是问题所在：**

* **问题：无法定义同名的枚举成员**

```c++
enum color { red, blue };
//enum MyEnum { red, yellow }; ERROR, 重定义；以前的定义是“枚举数”
```

> **如上面的代码所示：我们无法重复使用red这个标识符。因为它在color中已经被用过了。但是，它们明明就是不同的枚举类型，如果可以使用相同的成员名称，然后通过域运算符来访问的话，该有多好！就像下面这样：**
> 
> `color::red`
> 
> **但是这是旧版的enum无法做到的。**

* **解决上述问题：利用命名空间**

```c++
#include <iostream>

namespace spaceA {
    enum color { red, blue };
}
namespace spaceB {
    enum colorX { red, blue };
}
int main() {
    std::cout << spaceA::red << std::endl;
    std::cout << spaceB::blue << std::endl;
    std::cout << std::boolalpha << (spaceA::red > spaceB::blue) << std::endl;
    return 0;
}
```

运行结果：

> 0
> 1
> false

> **是的，只要利用命名空间我们就能解决枚举体的成员重定义问题，但是添加了多余的一层命名空间，未免显得麻烦**

## 1.4 不同编译器解决该问题的方法不统一

> **在1.2中展示的图片告诉我们：有些编译器可能提供了相应的扩展来解决这些问题，但是有的编译器却没有，这使得我们的编程非常的不统一，有时候因为enum而削弱了程序的可移植性。**

# 2. enum class 和 enum struct

## 2.1 enum class 和 enum struct 是等价的

## 2.2 声明

如大标题，枚举体的声明和定义使用`enum class`或是`enum struct`， 二者是等价的。使用enum class\enum struct不会与现存的enum关键词冲突。而且enum class\enum struct具有更好的类型安全和类似封装的特性（scoped nature）。

```c++
enum class color { red, green, yellow };
enum class colorx { red, green = 100, yellow };
//....
```

## 2.3 类型转换

与整形之间不会发生隐式类型转换，但是可以强转。

```c++
#include <iostream>

enum class color { red, green, yellow };

int main() {
    //int res(color::red); //ERROR , “初始化”: 无法从“color”转换为“int”
    //color col(2);//ERROR , “初始化”: 无法从“int”转换为“color”

    //强转
    int res(static_cast<int>(color::red));//OK
    color col(static_cast<color>(1));//OK

    return 0;
}
```

## 2.4 指定底层数据类型(underlying type)

默认的底层数据类型是int，用户可以通过：type（冒号+类型）来指定任何整形（除了wchar_t）作为底层数据类型。

```c++
enum class color : unsigned char { red, blue };
enum calss colorb : long long { yellow, black };
```

## 2.5 域

引入了域，要通过域运算符访问，不可以直接通过枚举体成员名来访问（所以我们可以定义相同的枚举体成员而不会发生重定义的错误）

```c++
#include <iostream>

enum class color { red, green, yellow };
enum class colorX { red, green, yellow };

int main() {
    //使用域运算符访问枚举体成员，强转后打印
    std::cout << static_cast<int>(color::red) << std::endl;
    std::cout << static_cast<int>(colorX::red) << std::endl;
    return 0;
}
```

运行结果：

> 0
> 0

# 3. C++11 enum的一些新特点

* 枚举体的定义和声明问题
    ![](https://img-blog.csdn.net/20151204205254639)

* 用enum定义的枚举体是一个不具有封装性(不知道如何翻译是好：unscoped enumeration)的枚举体，他的成员可以在enum的大括号外被直接访问。而用enum class或是enum struct（二者在语法上是等价的）定义的枚举体是具有封装性的（scoped enumeration），他的成员不能通过成员名直接访问，而应通过域运算符来访问。

```c++
#include <iostream>

enum class color { red, black };
enum colorx { green, yellow };

int main() {
    color::red;     //用域运算符访问color的成员
    green;          //直接访问colorx的成员
    colorx::green;  //用域运算符访问colorx的成员
    return 0;
}
```

* 图中的enum-base应该只能是整形的数据（不能是浮点类型或是其他类型），const或是volatile会被忽略。
* enumerator-list中的成员被作为常量使用，与常量的功能等价。
* 使用`=`给成员初始化的时候，`=`右边应该使用常量，这个常量应该为整形或是其他的枚举类型。如果第一个枚举成员没有初始化，那么他默认为0，其他没有初始化的成员是前面一个成员的值加1。

```c++
enum color { red=3, black, gray };//成员的值分别为：3 4 5
enum colorx { green, yellow };//成员的值分别为：0 1
enum colorxx{xred,xyellow,xblack=12,xgray};//成员的值分别为：0 1 12 13
```

* 每种枚举体的类型都不同于其他枚举体。

```c++
enum colora{red};//colora的类型与colorb的类型不同
enum colorb{yellow};
```

* 每种枚举都具有底层数据类型，同过：type（冒号+类型）来指定。**对于指定了数据类型的枚举体，他的数据类型为指定的数据类型。如果没有固定的底层数据类型：**

  1. 对于enum class和enum struct来说，他的底层数据类型是int。
  2. 对于enum来说，他的底层数据类型根据编译器而不同。
  3. 如果有使用数据初始化，那么他的数据类型与用来初始化的数据的类型相同。

* 如果该枚举体没有指定的底层数据类型，而且该枚举体的成员为空，那么这个枚举体相当于只有一个成员0

*   enum（非enum class\enum struct）会发生[整形提升](http://blog.csdn.net/sanoseiichirou/article/details/50171727)

```c++
#include <iostream>

enum color { red, green, yellow };

int main() {
    std::cout << std::boolalpha << (red == green) << std::endl;//(red == green)发生了整形提升
    return 0;
}
```

* enum（非enum class\enum struct）会发生自动数据类型转换。但是enum class\enum struct是不允许这么做的。

```c++
#include <iostream>

enum color { red, green, yellow };

int main() {
    //color col = 2;//ERROR , “初始化”: 无法从“int”转换为“color”
    int i = green;//发生隐式转换
    std::cout << i << std::endl;
    std::cin.get();
    return 0;
}
```

**运行结果：**

> 1

* 可以对enum和enum class\enum struct进行强制转换。

```c++
#include <iostream>

enum class color { red, green, yellow,a,b,v };

int main() {
    int res(0);
    //res = color::red + color::green;//ERROR , “color”不定义该运算符或到预定义运算符可接收的类型的转换
    res = static_cast<int>(color::red) + static_cast<int>(color::green);
    std::cout << res << std::endl;
    return 0;
}
```

运行结果：

> 1
