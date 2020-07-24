# 函数对象(仿函数)

## 什么是仿函数

**仿函数的主要功能是为了搭配STL算法使用，单独使用仿函数的情况比较少。**

仿函数(functors)在C++标准中采用的名称是函数对象(function objects)。仿函数主要用于STL中的算法中，虽然函数指针虽然也可以作为算法的参数，但是函数指针不能满足STL对抽象性的要求，也无法和STL其他组件搭配，产生更灵活变化。仿函数本质就是类重载了一个operator()，创建一个行为类似函数的对象。

对于重载了()操作符的类，可以实现类似函数调用的过程，所以叫做仿函数，实际上仿函数对象仅仅占用1字节，因为内部没有数据成员，仅仅是一个重载的方法而已。实际上可以通过传递函数指针实现类似的功能，但是为了和STL内部配合使用，他提供了仿函数的特性。

```c++
#include <iostream>
using std::cout;
using std::endl;
struct MyPlus {
    int operator()(const int &a, const int &b) const { return a + b; }
};

int main() {
    MyPlus a;
    cout << MyPlus()(1, 2) << endl;  // 1、通过产生临时对象调用重载运算符
    cout << a.operator()(1, 2) << endl;  // 2、通过对象显示调用重载运算符
    cout << a(1, 2) << endl;  // 3、通过对象类似函数调用 隐示地调用重载运算符
    return 0;
}
```

在STL中，当需要传递仿函数对象的时候，通过采用上述第1种方法，因为传递进去仅仅为了给容器内部成员赋值，所以没有必要生成对象，产生临时对象即可。

如果一个类将`()`运算符重载为成员函数，这个类就称为函数对象类，这个类的对象就是函数对象。函数对象是一个对象，但是使用的形式看起来像函数调用，实际上也执行了函数调用，因而得名。

下面是一个函数对象的例子。
```c++
#include <iostream>
using namespace std;
class CAverage
{
public:
    double operator()(int a1, int a2, int a3)
    {  //重载()运算符
        return (double)(a1 + a2 + a3) / 3;
    }
};
int main()
{
    CAverage average;  //能够求三个整数平均数的函数对象
    cout << average(3,2,3);  //等价于 cout << average.operator()(3,2,3);
    return 0;
}
```
程序的输出结果是：
2.66667

`()`是目数不限的运算符，因此重载为成员函数时，有多少个参数都可以。

## 函数对象应用实例1：在 accumulate 算法中的应用

STL中有以下实现“累加”功能的算法（函数模板）：
```c++
template <class InIt, class T, class Pred>
T accumulate(InIt first, InIt last, T val, Pred op);
```
该模板的功能是对 [first, last) 中的每个迭代器I执行 val = op(val, *I)，返回最终的 val。在 Dev C++中，numeric 头文件中 accumulate 的源代码如下：
```c++
template <class InIt, class T, class Pred>
T accumulate(InIt first, Init last, T init, Pred op)
{
    for (; first != last; ++first)
        init = op(init, *first);
    return init;
};
```
此模板被实例化后，`op(init, *first)`必须要有定义，则 op 只能是函数指针或者函数对象。因此调用该accmulate模板时，形参 op 对应的实参只能是函数名、函数指针或者函数对象。

下面的程序通过 accumulate 模板求一个 vector 中元素的平方和，其中用到了函数对象。
```c++
/* 01 */ #include <iostream>
/* 02 */ #include <numeric>  //accumulate 在此头文件定义
/* 03 */ #include <vector>
/* 04 */ using namespace std;
/* 05 */ 
/* 06 */ int SumSquares(int total, int value) { return total + value * value; }
/* 07 */ 
/* 08 */ template <class T>
/* 09 */ class SumPowers {
/* 10 */ private:
/* 11 */     int power;
/* 12 */ 
/* 13 */ public:
/* 14 */     // 构造函数
/* 15 */     SumPowers(int p) : power(p) {}
/* 16 */     const T operator()(const T& total, const T& value) {
/* 17 */         //计算 value的power次方，加到total上
/* 18 */         T v = value;
/* 19 */         for (int i = 0; i < power - 1; ++i) {
/* 20 */             v = v * value;
/* 21 */         }
/* 22 */         return total + v;
/* 23 */     }
/* 24 */ };
/* 25 */ 
/* 26 */ int main() {
/* 27 */     vector<int> v({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
/* 28 */     int result = accumulate(v.begin(), v.end(), 0, SumSquares);
/* 29 */     cout << "平方和：" << result << endl;
/* 30 */     result = accumulate(v.begin(), v.end(), 0, SumPowers<int>(3));
/* 31 */     cout << "立方和：" << result << endl;
/* 32 */     result = accumulate(v.begin(), v.end(), 0, SumPowers<int>(4));
/* 33 */     cout << "4次方和：" << result;
/* 34 */     return 0;
/* 35 */ }
```
程序的输出结果如下：

    平方和:385
    立方和3025
    4次方和:25333

第 28 行，第四个参数是 SumSquares 函数的名字。函数名字的类型是函数指针，因此本行将 accumulate 模板实例化后得到的模板函数定义如下：
```c++
int accumulate(vector <int>::iterator first, vector <int>::iterator last, int init, int(*op)(int, int))
{
    for (; first != last; ++first)
        init = op(init, *first);
    return init;
}
```

形参 op 是一个函数指针，而op(init, *first)就调用了指针 op 指向的函数，在第 28 行的情况下就是函数 SumSquares。

第 30 行，第四个参数是 SumPowers<int>(3)。SumPowers 是类模板的名字，SumPowers<int> 就是类的名字。类的名字后面跟着构造函数的参数列表，就代表一个临时对象。因此 SumPowers<int>(3) 就是一个 SumPowers<int> 类的临时对象。

编译器在编译此行时，会将 accumulate 模板实例化成以下函数：
```c++
int accumulate(vector<int>::iterator first, vector<int>::iterator last, int init, SumPowers<int> op)
{
    for (; first != last; ++first)
        init = op(init, *first);
    return init;
}
```

形参 op 是一个函数对象，而op(init, *first)等价于：
`op.operator()(init, *first);`

即调用了 SumPowers<int> 类的 operator() 成员函数。

对比 SumPowers 和 SumSquares 可以发现，函数对象的 operator() 成员函数可以根据对象内部的不同状态执行不同操作，而普通函数就无法做到这一点。因此函数对象的功能比普通函数更强大。

## 函数对象应用实例2：在sort算法中的应用

STL 中的排序模板 sort 能将区间从小到大排序。sort 算法有两个版本。第一个版本的原型如下：
```c++
template <class_Randlt>
void sort(_Randlt first, _RandIt last);
```
该模板可以用来将区间 [first, last) 中的元素从小到大排序，要求 first、last 是随机访问迭代器。元素比较大小是用`<`进行的。如果表达式`a<b`的值为true，则a排在b前面；如果`a<b`的值为false，则b未必排在a前面，还要看`b<a`是否成立，成立的话b才排在a前面。要使用这个版本的sort算法，待排序的对象必须能用`<`运算符进行比较。

sort 算法第二个版本的原型如下：
```c++
template <class_Randlt, class Pred>
void sort(_Randlt first, _RandIt last, Pred op);
```
这个版本和第一个版本的差别在于，元素 a、b 比较大小是通过表达式op(a, b)进行的。如果该表达式的值为 true，则a比b小；如果该表达式的值为false，也不能认为b比a小，还要看op(b, a)的值。总之，op 定义了元素比较大小的规则。下面是一个使用 sort 算法的例子。
```c++
/* 01 */ #include <algorithm>  //sort算法在此头文件中定义
/* 02 */ #include <iostream>
/* 03 */ using namespace std;
/* 04 */ 
/* 05 */ template <class T>
/* 06 */ void Printlnterva1(T first, T last) {  //用以输出 [first, last) 区间中的元素
/* 07 */     for (; first != last; ++first) cout << *first << " ";
/* 08 */     cout << endl;
/* 09 */ }
/* 10 */ 
/* 11 */ class A {
/* 12 */    public:
/* 13 */     int v;
/* 14 */     A(int n) : v(n) {}
/* 15 */ };
/* 16 */ bool operator<(const A& a1, const A& a2) {  
/* 17 */     //重载为 A 的 const 成员函数也可以，重载为非 const 成员函数在某些编译器上会出错
/* 18 */     return a1.v < a2.v;
/* 19 */ }
/* 20 */ bool GreaterA(const A& a1, const A& a2) {  
/* 21 */     // v值大的元素作为较小的数
/* 22 */     return a1.v > a2.v;
/* 23 */ }
/* 24 */ struct LessA {
/* 25 */     bool operator()(const A& a1, const A& a2) {  
/* 26 */         // v的个位数小的元素就作为较小的数
/* 27 */         return (a1.v % 10) < (a2.v % 10);
/* 28 */     }
/* 29 */ };
/* 30 */ ostream& operator<<(ostream& o, const A& a) {
/* 31 */     o << a.v;
/* 32 */     return o;
/* 33 */ }
/* 34 */ int main() {
/* 35 */     int a1[4] = {5, 2, 4, 1};
/* 36 */     A a2[5] = {13, 12, 9, 8, 16};
/* 37 */     sort(a1, a1 + 4);
/* 38 */     cout << "1)";
/* 39 */     Printlnterva1(a1, a1 + 4);  //输出 1)1 2 4 5
/* 40 */ 
/* 41 */     sort(a2, a2 + 5);           //按v的值从小到大排序
/* 42 */     cout << "2)";
/* 43 */     Printlnterva1(a2, a2 + 5);   //输出 2)8 9 12 13 16
/* 44 */ 
/* 45 */     sort(a2, a2 + 5, GreaterA);  //按v的值从大到小排序
/* 46 */     cout << "3)";
/* 47 */     Printlnterva1(a2, a2 + 5);  //输出 3)16 13 12 9 8
/* 48 */ 
/* 49 */     sort(a2, a2 + 5, LessA());  //按v的个位数从小到大排序
/* 50 */     cout << "4)";
/* 51 */     Printlnterva1(a2, a2 + 5);  //输出 4)12 13 16 8 9
/* 52 */     return 0;
/* 53 */ }
```

编译至第 45 行时，编译器将 sort 实例化得到的函数原型如下：
`void sort(A* first, A* last, bool (*op)(const A &, const A &) );`

该函数在执行过程中，当要比较两个元素 a、b 的大小时，就是看 op(a, b) 和 op(b, a) 的返回值。本程序中 op 指向 GreaterA,因此就用 GreaterA 定义的规则来比较大小。

编译至第 49 行时，编译器将 sort 实例化得到的函数原型如下：
`void sort( A* first, A* last, LessA op);`

该函数在执行过程中，当要比较两个元素 a、b 的大小时，就是看 op(a, b) 和 op(b, a) 的返回值。本程序中，op(a, b) 等价于 op.opeartor(a, b)，因此就用 LessA 定义的规则来比较大小。

STL 中定义了一些函数对象类模板，都位于头文件 functional 中。例如，greater 模板的源代码如下：
```c++
template <class T>
struct greater
{
    bool operator()(const T& x, const T& y) const{
        return x > y;
    }
};
```

假设有以下数组：
`int a[4] = {3, 5, 34, 8};`

要将该数组从大到小排序，则只需写：
`sort( a, a+4, greater<int>() );`

要使用greater模板，须确保>运算符本来就有定义，或经过了适当的重载。

list 容器的 sort 成员能将元素从小到大排序。它也有两个版本：一个是没有参数的函数，比较大小用<运算符；另一个是函数模板，原型如下：
```c++
template <class Pred>
void sort(Pred op);
```
sort 函数允许自定义比较大小的规则，即 op(x, y) 为真就认为 x 比 y 小。例如，假设有：
`list<int> lst;`

如果希望将 lst 中的元素按其整数数值从大到小排序，只需写：
`lst.sort( greater<int>() );`

在使用关联容器和许多算法时，都可以用函数对象来定义比较大小的规则，以及其他一些规则和操作。

## STL中的函数对象类模板

STL 中有一些函数对象类模板，如表 1 所示。

表1：STL 中的函数对象类模板
| 函数对象类模板 | 成员函数 T operator ( const T & x, const T & y) 的功能|
|:-|:-|
| plus <T> | return x + y; |
| minus < >	| return x - y; |
| multiplies <T> | return x * y; |
| divides <T> | return x / y; |
| modulus <T> |	return x % y; |
| | 成员函数 bool operator( const T & x, const T & y) 的功能 |
| equal_to <T> | return x == y; |
| not_equal_to <T> | return x! = y; |
| greater <T> |	return x > y; |
| less <T> | return x < y; |
| greater_equal <T> | return x > = y; |
| less_equal <T> | return x <= y; |
| logical_and <T> |	return x && y; |
| logical_or <T> | return x || y; |
| | 成员函数 T operator( const T & x) 的功能 |
| negate <T> | return - x; |
| | 成员函数 bool operator( const T & x) 的功能 |
| logical_not <T> |	return ! x; |

例如，如果要求两个 double 型变量 x、y 的乘积，可以写：
`multiplies<double> () (x, y)`


less 是 STL 中最常用的函数对象类模板，其定义如下：
```c++
template <class_Tp>
struct less
{
    bool operator() (const_Tp & __x, const_Tp & __y) const
    { return __x < __y; }
};
```

要判断两个 int 变量 x、y 中 x 是否比 y 小，可以写：
```c++
if (less<int>()(x, y)) { ... }
```

## 引入函数对象后STL中的“大”、“小”和“相等”概念

前面提到过，默认情况下，STL 中的容器和算法比较元素的大小是通过`<`运算符进行的。通过 10.3.4 节可知，sort 和 list::sort 都可以通过一个函数对象或函数自定义比较元素大小的规则。例如以下的 sort 版本：
```c++
template <class_RandIt, class Pred>
void sort(_RandIt first, _RandIt last, Pred op);
```
实际调用 sort 时，和 op 对应的实参可以是一个函数对象或者函数的名字。sort在执行过程中用 op(x, y) 比较 x 和 y 的大小，因此可以将 op 称为自定义的“比较器”。

关联容器中的元素是从小到大排序的。使用关联容器时，也可以用自定义的比较器取代`<`运算符，以规定元素之间的大小关系。STL 中还有许多算法都可以自定义比较器。在自定义比较器 op 的情况下，以下三种说法是等价的：
* x 小于 y。
* op(x,y) 的返回值为 true。
* y 大于 x。

同样地，对关联容器的find和count成员函数以及其他一些在有序区间上的STL算法而言，在自定义比较器op的情况下，x和y相等与op(x,y)和op(y,x)都为假是等价的。

## 链接

- [目录](README.md)
- 上一节：[deque](./deque.md)
- 下一节：[容器适配器](./容器适配器.md)
