# STL算法

STL 提供能在各种容器中通用的算法（大约有70种），如插入、删除、查找、排序等。算法就是函数模板。算法通过迭代器来操纵容器中的元素。

许多算法操作的是容器上的一个区间（也可以是整个容器），因此需要两个参数，一个是区间起点元素的迭代器，另一个是区间终点元素的后面一个元素的迭代器。例如，排序和查找算法都需要这两个参数来指明待排序或待查找的区间。

有的算法返回一个迭代器。例如，find() 算法在容器中查找一个元素，并返回一个指向该元素的迭代器。

算法可以处理容器，也可以处理普通的数组。

有的算法会改变其所作用的容器。例如：
+ copy：将一个容器的内容复制到另一个容器。
+ remove：在容器中删除一个元素。
+ random_shuffle：随机打乱容器中的元素。
+ fill：用某个值填充容器。

有的算法不会改变其所作用的容器。例如：
+ find：在容器中查找元素。
+ count_if：统计容器中符合某种条件的元素的个数。

STL 中的大部分常用算法都在头文件 <algorithm> 中定义。此外，头文件 <numeric> 中也有一些算法。

下面介绍一个常用算法 find，以便对算法是什么、怎么用有一个基本的概念。find 算法和其他算法一样都是函数模板。find 模板的原型如下：
```c++
template <class InIt, class T>
InIt find(InIt first, InIt last, const T& val);
```

其功能可以是在迭代器 first、last 指定的容器的一个区间 [first, last) 中，按顺序查找和 val 相等的元素。函数返回值是一个迭代器。如果找到，则该迭代器指向被找到的元素；如果找不到，则该迭代器等于last。

>[first, last) 这个区间是一个左闭右开的区间，即 last 指向的元素其实不在此区间内。实际上，在STL中，区间都是左闭右开的。

find 模板使用==运算符判断元素是否相等。因此，如果 [first, last) 区间中存放的是对象，则==运算符应该被适当重载，使得两个对象可以用==运算符比较。

注意：上一段话说的是“其功能可以是”，而不是“其功能就是”。这是因为模板只是一种代码形式，这种代码形式具体能完成什么功能，取决于程序员对该模板写法的了解及其想象力。按照语法，调用 find 模板时，first 和 last 只要类型相同就可以，不一定必须是迭代器。

演示 find 用法的程序如下：

```c++
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
int main()  {
    int a[10] = {10,20,30,40};
    vector<int> v;
    v.push_back(1);    v.push_back(2);
    v.push_back(3);    v.push_back(4); //此后v里放着4个元素：1,2,3,4
    vector<int>::iterator p;
    p = find(v.begin(),v.end(),3); //在v中查找3
    if(p != v.end()) //若找不到,find返回 v.end()
        cout << "1) " <<  * p << endl; //找到了
    p = find(v.begin(),v.end(),9);
    if(p == v.end())
        cout << "not found " << endl; //没找到
    p = find(v.begin()+1,v.end()-1,4); //在2,3 这两个元素中查找4
    cout << "2) " << * p << endl;
    int * pp = find(a,a+4,20);
    if(pp == a + 4)
        cout << "not found" << endl;
    else
        cout << "3) " <<* pp << endl;
}
```
程序的输出结果是：
1) 3
not found
2) 4
3) 20

第 11 行，要查找的区间是 [v.begin(), v.end( ))，v.end() 不在查找范围内，因此没有问题。本行的查找会成功，因此 p 指向找到的元素 3。

第 17 行，因为要查找的区间是 [v.begin()+l，v.end()-1)，这个区间中只有 2、3 这两个元素，因此查找会失败，p 的值变为 v.end() - 1，因此 p 正好指向 4 这个元素。

第 19 行，数组 a 是一个容器。数组名 a 的类型是 `int *`，可以做迭代器使用，表达式a+4的类型也是 `int*`，因此也能做迭代器。本次调用 find,查找区间是 [a, a+4)，即数组 a 的前 4 个元素。如果查找失败，find 就会返回 a+4。

STL 中还有一个常用的算法 sort，用于对容器排序，其原型为：
```c++
template<class_RandIt>
void sort(_RandIt first, _RandIt last);
```

该算法可以用来对区间 [first, last) 从小到大进行排序。下面两行程序就能对数组 a 排序：
```c++
int a[4] = {3, 4, 2, 1};
sort(a, a+4);
```

## 链接

- [目录](README.md)
- 上一节：[STL迭代器 iterator](./STL迭代器iterator.md)
- 下一节：[STL中“大”、“小”和“相等”的概念](./STL中“大”、“小”和“相等”的概念.md)