# std::move

在C++11中，标准库在`<utility>`中提供了一个有用的函数std::move，std::move并不能移动任何东西，它唯一的功能是将一个左值强制转化为右值引用，继而可以通过右值引用使用该值，以用于移动语义。从实现上讲，std::move基本等同于一个类型转换：`static_cast<T&&>(lvalue)`;

std::move函数可以以非常简单的方式将左值引用转换为右值引用。[左值,右值,左值引用,右值引用概念](./左值,右值,左值引用,右值引用.md)

1. C++ 标准库使用比如vector::push_back 等这类函数时,会对参数的对象进行复制,连数据也会复制.这就会造成对象内存的额外创建, 本来原意是想把参数push_back进去就行了,通过std::move，可以避免不必要的拷贝操作。
2. std::move是将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存的搬迁或者内存拷贝所以可以提高利用效率,改善性能.。
3. 对指针类型的标准库对象并不需要这么做.

### 用法:

原lvalue值被moved from之后值被转移,所以为空字符串. 

```c++
//摘自https://zh.cppreference.com/w/cpp/utility/move
#include <iostream>
#include <utility>
#include <vector>
#include <string>
int main()
{
    std::string str = "Hello";
    std::vector<std::string> v;
    //调用常规的拷贝构造函数，新建字符数组，拷贝数据
    v.push_back(str);
    std::cout << "After copy, str is \"" << str << "\"\n";
    //调用移动构造函数，掏空str，掏空后，最好不要使用str
    v.push_back(std::move(str));
    std::cout << "After move, str is \"" << str << "\"\n";
    std::cout << "The contents of the vector are \"" << v[0]
                                         << "\", \"" << v[1] << "\"\n";
}
```

输出:

    After copy, str is "Hello"
    After move, str is ""
    The contents of the vector are "Hello", "Hello"

### std::move 的函数原型定义

```c++
template <typename T>
typename remove_reference<T>::type&& move(T&& t)
{
	return static_cast<typename remove_reference<T>::type&&>(t);
}
```

### 原型定义中的原理实现:

1. 首先，函数参数T&&是一个指向模板类型参数的右值引用，通过引用折叠，此参数可以与任何类型的实参匹配（可以传递左值或右值，这是std::move主要使用的两种场景)。关于引用折叠如下：

+ 公式一: X& &、X&& &、X& &&都折叠成X&，用于处理左值

```
string s("hello");
std::move(s) => std::move(string& &&) => 折叠后 std::move(string& )
此时：T的类型为string&
typename remove_reference<T>::type为string 
整个std::move被实例化如下
string&& move(string& t) //t为左值，移动后不能在使用t
{
    //通过static_cast将string&强制转换为string&&
    return static_cast<string&&>(t); 
}
```

+ 公式二: X&& &&折叠成X&&，用于处理右值

```
std::move(string("hello")) => std::move(string&&)
//此时：T的类型为string 
//     remove_reference<T>::type为string 
//整个std::move被实例如下
string&& move(string&& t) //t为右值
{
    return static_cast<string&&>(t);  //返回一个右值引用
}
```

简单来说，右值经过T&&传递类型保持不变还是右值，而左值经过T&&变为普通的左值引用.

2. 对于static_cast<>的使用注意：任何具有明确定义的类型转换，只要不包含底层const,都可以使用static_cast。

```c++
double d = 1;
void* p = &d;
double *dp = static_cast<double*> p; //正确
 
const char *cp = "hello";
char *q = static_cast<char*>(cp); //错误：static不能去掉const性质
static_cast<string>(cp); //正确 
```

3. 对于remove_reference是通过类模板的部分特例化进行实现的，其实现代码如下

```c++
//原始的，最通用的版本
template <typename T> struct remove_reference{
    typedef T type;  //定义T的类型别名为type
};
 
//部分版本特例化，将用于左值引用和右值引用
template <class T> struct remove_reference<T&> //左值引用
{ typedef T type; }
 
template <class T> struct remove_reference<T&&> //右值引用
{ typedef T type; }   
  
//举例如下,下列定义的a、b、c三个变量都是int类型
int i;
remove_refrence<decltype(42)>::type a;             //使用原版本，
remove_refrence<decltype(i)>::type  b;             //左值引用特例版本
remove_refrence<decltype(std::move(i))>::type  b;  //右值引用特例版本 
```

### 总结：

std::move实现，首先，通过右值引用传递模板实现，利用引用折叠原理将右值经过T&&传递类型保持不变还是右值，而左值经过T&&变为普通的左值引用，以保证模板可以传递任意实参，且保持类型不变。然后我们通过static_cast<>进行强制类型转换返回T&&右值引用，而static_cast<T>之所以能使用类型转换，是通过remove_refrence<T>::type模板移除T&&，T&的引用，获取具体类型T。

## 参考

[c++ 之 std::move 原理实现与用法总结](https://blog.csdn.net/p942005405/article/details/84644069/)
