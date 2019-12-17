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