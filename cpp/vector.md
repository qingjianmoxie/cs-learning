# std::vector

定义于头文件 [`<vector>`](https://zh.cppreference.com/w/cpp/header/vector)

```c++
template<
    class T,
    class Allocator = std::allocator<T>
> class vector;
```
[std::allocator](http://zh.cppreference.com/w/cpp/memory/allocator)

`std::vector` 是封装动态数组的顺序容器。

```c++
namespace pmr {
    template <class T>
    using vector = std::vector<T, std::pmr::polymorphic_allocator<T>>;
}

// (C++17 起) 
```
[std::pmr::polymorphic_allocator](http://zh.cppreference.com/w/cpp/memory/polymorphic_allocator)

`std::pmr::vector` 是使用[多态分配器](https://zh.cppreference.com/w/cpp/memory/polymorphic_allocator)的模板别名。

元素相继存储，这意味着不仅可通过迭代器，还能用指向元素的常规指针访问元素。这意味着指向 vector 元素的指针能传递给任何期待指向数组元素的指针的函数。(C++03 起)

vector 的存储是自动管理的，按需扩张收缩。 vector 通常占用多于静态数组的空间，因为要分配更多内存以管理将来的增长。 vector 所用的方式不在每次插入元素时，而只在额外内存耗尽时重分配。分配的内存总量可用 [capacity()](https://zh.cppreference.com/w/cpp/container/vector/capacity) 函数查询。额外内存可通过对 [shrink_to_fit()](https://zh.cppreference.com/w/cpp/container/vector/shrink_to_fit) 的调用返回给系统。 (C++11 起)

重分配通常是性能上有开销的操作。若元素数量已知，则 [reserve()](https://zh.cppreference.com/w/cpp/container/vector/reserve) 函数可用于消除重分配。

vector 上的常见操作复杂度（效率）如下：

* 随机访问——常数 O(1)
* 在末尾插入或移除元素——均摊常数 O(1)
* 插入或移除元素——与到 vector 结尾的距离成线性 O(n)

`std::vector` （对于 `bool` 以外的 `T` ）满足[*容器* (Container)](https://zh.cppreference.com/w/cpp/named_req/Container)、[*具分配器容器* (AllocatorAwareContainer)](https://zh.cppreference.com/w/cpp/named_req/AllocatorAwareContainer)、[*序列容器* (SequenceContainer)](https://zh.cppreference.com/w/cpp/named_req/SequenceContainer)、[*连续容器* (ContiguousContainer)](https://zh.cppreference.com/w/cpp/named_req/ContiguousContainer)(C++17 起)及[*可逆容器* (ReversibleContainer) ](https://zh.cppreference.com/w/cpp/named_req/ReversibleContainer)的要求。

### 模板参数

**T** - 元素的类型。

`T` 必须满足[*可复制赋值* (CopyAssignable) ](https://zh.cppreference.com/w/cpp/named_req/CopyAssignable)和[*可复制构造* (CopyConstructible) ](https://zh.cppreference.com/w/cpp/named_req/CopyConstructible)的要求。 (C++11 前)  
加诸元素的要求依赖于容器上进行的实际操作。泛言之，要求元素类型是完整类型并满足[*可擦除* (Erasable) ](https://zh.cppreference.com/w/cpp/named_req/Erasable)的要求，但许多成员函数附带了更严格的要求。 (C++11 起) (C++17 前)  
加诸元素的要求依赖于容器上进行的实际操作。泛言之，要求元素类型满足[*可擦除* (Erasable) ](https://zh.cppreference.com/w/cpp/named_req/Erasable)的要求，但许多成员函数附带了更严格的要求。若分配器满足[分配器完整性要求](https://zh.cppreference.com/w/cpp/named_req/Allocator#.E5.88.86.E9.85.8D.E5.99.A8.E5.AE.8C.E6.95.B4.E6.80.A7.E8.A6.81.E6.B1.82)，则容器（但非其成员）能以不完整元素类型实例化。 (C++17 起)  

**Allocator** - 用于获取/释放内存及构造/析构内存中元素的分配器。类型必须满足[*分配器* (Allocator) ](https://zh.cppreference.com/w/cpp/named_req/Allocator)的要求。若 Allocator::value_type 与 T 不同则行为未定义。 |

### 特化

标准库提供 `std::vector` 对类型 bool 的特化，它可能为空间效率优化。

[vector<bool>](https://zh.cppreference.com/w/cpp/container/vector_bool "cpp/container/vector bool")    节省空间的动态 bitset (类模板)

### 成员类型

| 成员类型 | 定义 |
| :- | :- |
| `value_type` | `T` |
| `allocator_type` | `Allocator` |
| `size_type` | 无符号整数类型（通常是 [std::size_t](https://zh.cppreference.com/w/cpp/types/size_t) ） |
| `difference_type` | 有符号整数类型（通常是 [std::ptrdiff_t](https://zh.cppreference.com/w/cpp/types/ptrdiff_t) ） |
| `reference` | `Allocator::reference` (C++11 前) |
| `reference` | `value_type&` (C++11 起) |
| `const_reference` | `Allocator::const_reference` (C++11 前) |
| `const_reference` | `const value_type&` (C++11 起) |
| `pointer` | `Allocator::pointer` (C++11 前) |
| `pointer` | `std::allocator_traits<Allocator>::pointer` (C++11 起) |
| `const_pointer` | `Allocator::const_pointer` (C++11 前) |
| `const_pointer` | `std::allocator_traits<Allocator>::const_pointer` (C++11 起) |
| `iterator` | [*遗留随机访问迭代器* (LegacyRandomAccessIterator)](https://zh.cppreference.com/w/cpp/named_req/RandomAccessIterator) |
| `const_iterator` | 常随机访问迭代器 |
| `reverse_iterator` | `std::reverse_iterator<iterator>` |
| `const_reverse_iterator` | `std::reverse_iterator<const_iterator>` |

### 成员函数

| 成员函数 | 作用 |
| :- | :- |
| [(构造函数)](https://zh.cppreference.com/w/cpp/container/vector/vector) | 构造 `vector`
(公开成员函数) |
| [(析构函数)](https://zh.cppreference.com/w/cpp/container/vector/~vector) | 析构 `vector`
(公开成员函数) |
| [operator=](https://zh.cppreference.com/w/cpp/container/vector/operator%3D) | 赋值给容器
(公开成员函数) |
| [assign](https://zh.cppreference.com/w/cpp/container/vector/assign) | 将值赋给容器
(公开成员函数) |
| [get_allocator](https://zh.cppreference.com/w/cpp/container/vector/get_allocator) | 返回相关的分配器 (公开成员函数) |

##### 元素访问

| 元素访问 | 作用 |
| :- | :- |
| [at](https://zh.cppreference.com/w/cpp/container/vector/at) | 访问指定的元素，同时进行越界检查
(公开成员函数) |
| [operator[]](https://zh.cppreference.com/w/cpp/container/vector/operator_at) | 访问指定的元素
(公开成员函数) |
| [front](https://zh.cppreference.com/w/cpp/container/vector/front) | 访问第一个元素
(公开成员函数) |
| [back](https://zh.cppreference.com/w/cpp/container/vector/back) | 访问最后一个元素
(公开成员函数) |
| [data](https://zh.cppreference.com/w/cpp/container/vector/data) | 返回指向内存中数组第一个元素的指针(公开成员函数) |

##### 迭代器

| 迭代器 | 作用 |
| :- | :- |
| [begin / cbegin](https://zh.cppreference.com/w/cpp/container/vector/begin) | 返回指向容器第一个元素的迭代器 (公开成员函数) |
| [end / cend](https://zh.cppreference.com/w/cpp/container/vector/end) | 返回指向容器尾端的迭代器 (公开成员函数) |
| [rbegin / crbegin](https://zh.cppreference.com/w/cpp/container/vector/rbegin) | 返回指向容器最后元素的逆向迭代器 (公开成员函数) |
| [rend / crend](https://zh.cppreference.com/w/cpp/container/vector/rend) | 返回指向前端的逆向迭代器 (公开成员函数) |

##### 容量

| 容量 | 作用 |
| :- | :- |
| [empty](https://zh.cppreference.com/w/cpp/container/vector/empty) | 检查容器是否为空 (公开成员函数) |
| [size](https://zh.cppreference.com/w/cpp/container/vector/size) | 返回容纳的元素数 (公开成员函数) |
| [max_size](https://zh.cppreference.com/w/cpp/container/vector/max_size) | 返回可容纳的最大元素数 (公开成员函数) |
| [reserve](https://zh.cppreference.com/w/cpp/container/vector/reserve) | 预留存储空间 (公开成员函数) |
| [capacity](https://zh.cppreference.com/w/cpp/container/vector/capacity) | 返回当前存储空间能够容纳的元素数 (公开成员函数) |
| [shrink_to_fit](https://zh.cppreference.com/w/cpp/container/vector/shrink_to_fit)(C++11) | 通过释放未使用的内存减少内存的使用 (公开成员函数) |

##### 修改器

| 修改器 | 作用 |
| :- | :- |
| [clear](https://zh.cppreference.com/w/cpp/container/vector/clear) | 清除内容 (公开成员函数) |
| [insert](https://zh.cppreference.com/w/cpp/container/vector/insert) | 插入元素 (公开成员函数) |
| [emplace](https://zh.cppreference.com/w/cpp/container/vector/emplace)(C++11) | 原位构造元素 (公开成员函数) |
| [erase](https://zh.cppreference.com/w/cpp/container/vector/erase) | 擦除元素 (公开成员函数) |
| [push_back](https://zh.cppreference.com/w/cpp/container/vector/push_back) | 将元素添加到容器末尾 (公开成员函数) |
| [emplace_back](https://zh.cppreference.com/w/cpp/container/vector/emplace_back)(C++11) | 在容器末尾就地构造元素 (公开成员函数) |
| [pop_back](https://zh.cppreference.com/w/cpp/container/vector/pop_back) | 移除末元素 (公开成员函数) |
| [resize](https://zh.cppreference.com/w/cpp/container/vector/resize) | 改变容器中可存储元素的个数 (公开成员函数) |
| [swap](https://zh.cppreference.com/w/cpp/container/vector/swap) | 交换内容 (公开成员函数) |

# vector

vector 是顺序容器的一种。vector 是可变长的动态数组，支持随机访问迭代器，所有STL算法都能对 vector 进行操作。要使用 vector，需要包含头文件`#include <vector>`。

在 vector 容器中，根据下标随机访问某个元素的时间是常数，在尾部添加一个元素的时间大多数情况下也是常数，总体来说速度很快。

在中间插入或删除元素时，因为要移动多个元素，因此速度较慢，平均花费的时间和容器中的元素个数成正比。

在 vector 容器中，用一个动态分配的数组来存放元素，因此根据下标访问某个元素的时间是固定的，与元素个数无关。

vector 容器在实现时，动态分配的存储空间一般都大于存放元素所需的空间。例如，哪怕容器中只有一个元素，也会分配 32 个元素的存储空间。这样做的好处是，在尾部添加一个新元素时不必重新分配空间，直接将新元素写入适当位置即可。在这种情况下，添加新元素的时间也是常数。

但是，如果不断添加新元素，多出来的空间就会用完，此时再添加新元素，就不得不重新分配内存空间，把原有内容复制过去后再添加新的元素。碰到这种情况，添加新元素所花的时间就不是常数，而是和数组中的元素个数成正比。

至于在中间插入或删除元素，必然涉及元素的移动，因此时间不是固定的，而是和元素个数有关。

vector 有很多成员函数，常用的如表 1 所示。

<caption>表1：vector中常用的成员函数</caption>

| 成员函数 | 作 用 |
|:-|:-|
| vector() | 无参构造函数，将容器初始化为空 |
| vector(int n) | 将容器初始化为有n个元素 |
| vector(int n, const T & val) | 假定元素的类型是T，此构造函数将容器初始化为有n个元素，每个元素的值都是val |
| vector(iterator first, iterator last) | first和last可以是其他容器的迭代器。本构造函数初始化的结果就是将vector容器的内容变成与其他容器上的区间[first, last)一致的内容 |
| void clear() | 删除所有元素 |
| bool empty() | 判断容器是否为空 |
| void pop_back() | 删除容器末尾的元素 |
| void push_back( const T & val) | 将val添加到容器末尾 |
| int size() | 返回容器中元素的个数 |
| T & front() | 返回容器中第一个元素的引用 |
| T & back() | 返回容器中最后一个元素的引用 |
| iterator insert(iterator i, const T & val) | 将val插入迭代器i指向的位置，返回i |
| iterator insert( iterator i, iterator first, iterator last) | 将其他容器上的区间 [first, last) 中的元素插入迭代器 i 指向的位置 |
| iterator erase(iterator i) | 删除迭代器i指向的元素，返回值是被删元素后面的元素的迭代器 |
| iterator erase(iterator first, iterator last) | 删除容器中的区间 [first, last) |
| void swap( vector <T> & v) | 将容器自身的内容和另一个同类型的容器v互换 |

下面的程序演示了 vector 的基本用法。
```c++
#include <iostream>
#include <vector> //使用vector需要包含此头文件
using namespace std;
template <class T>
void PrintVector(const vector<T> &v)
{ //用于输出vector容器的全部元素的函数模板
    typename vector<T>::const_iterator i;
    //typename 用来说明 vector <T>::const_iterator 是一个类型，在 Visual Studio 中不写也可以
    for (i = v.begin(); i != v.end(); ++i)
        cout << *i << " ";
    cout << endl;
}
int main()
{
    int a[5] = {1, 2, 3, 4, 5};
    vector<int> v(a, a + 5);                      //将数组a的内容放入v
    cout << "1) " << v.end() - v.begin() << endl; //两个随机迭代器可以相减，输出：1)5
    cout << "2)";
    PrintVector(v);              //输出：2)1 2 3 4 5
    v.insert(v.begin() + 2, 13); //在 begin()+2 位置插人 13
    cout << "3)";
    PrintVector(v);         //输出：3)1 2 13 3 4 5
    v.erase(v.begin() + 2); //删除位于 begin()+2 位置的元素
    cout << "4)";
    PrintVector(v);                                      //输出：4)1 2 3 4 5
    vector<int> v2(4, 100);                              //v2 有 4 个元素，都是 100
    v2.insert(v2.begin(), v.begin() + 1, v.begin() + 3); //将v的一段插入v2开头
    cout << "5)v2:";
    PrintVector(v2);                       //输出：5)v2:2 3 100 100 100 100
    v.erase(v.begin() + 1, v.begin() + 3); //删除 v 上的一个区间，即 [2,3)
    cout << "6)";
    PrintVector(v); //输出：6)1 4 5
    return 0;
}
```
思考题：程序中的 PrintVector 模板演示了将容器的引用作为函数参数的用法。就完成输出整个容器内容这个功能来说，写成 PrintVector 模板这样是比较笨拙的，该模板的适用范围太窄。有没有更好的写法？

vector 还可以嵌套以形成可变长的[二维数组](http://c.biancheng.net/c/array/)。例如：

```c++
#include <iostream>
#include <vector>
using namespace std;
int main()
{
    vector<vector<int>> v(3); //v有3个元素，每个元素都是vector<int>容器
    for (int i = 0; i < v.size(); ++i)
        for (int j = 0; j < 4; ++j)
            v[i].push_back(j);
    for (int i = 0; i < v.size(); ++i)
    {
        for (int j = 0; j < v[i].size(); ++j)
            cout << v[i][j] << " ";
        cout << endl;
    }
    return 0;
}
```
程序的输出结果是：  
0 1 2 3  
0 1 2 3  
0 1 2 3

vector< vector<int> > v(3);定义了一个 vector 容器，该容器中的每个元素都是一个vector <int>容器。即可以认为，v 是一个二维数组，一共 3 行，每行都是一个可变长的一维数组。

## 链接

- [目录](README.md)
- 上一节：[STL中“大”、“小”和“相等”的概念](./STL中“大”、“小”和“相等”的概念.md)
- 下一节：[list](./list.md)
