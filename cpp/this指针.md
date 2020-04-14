# this指针

在 C++ 中, 成员函数通过名为**this**的额外的隐式参数来访问调用它的那个对象. 当我们调用一个成员函数时, 用请求该函数的对象地址初始化this.

举个栗子
```c++
#include <iostream>
using namespace std;

class Box
{
    double length;  // Length of a box
    double breadth; // Breadth of a box
    double height;  // Height of a box

public:
    // 构造函数定义
    Box(double l = 2.0, double b = 2.0, double h = 2.0)
    {
        cout << "Constructor called." << endl;
        length = l;
        breadth = b;
        height = h;
    }
    double getLength() const { return length; }
    double getVolume() const { return length * breadth * height; }
    bool compare(const Box &box) const { return this->getVolume() > box.getVolume(); }
};

int main(void)
{
    Box Box1(3.3, 1.2, 1.5); // Declare box1
    Box Box2(8.5, 6.0, 2.0); // Declare box2

    cout << "The length of Box1 is " << Box1.getLength() << endl;
    if (Box1.compare(Box2))
    {
        cout << "Box2 is smaller than Box1" << endl;
    }
    else
    {
        cout << "Box2 is equal to or larger than Box1" << endl;
    }
    return 0;
}
```

当上面的代码被编译和执行时，它会产生下列结果：

    Constructor called.
    Constructor called.
    The length of Box1 is 3.3
    Box2 is equal to or larger than Box1

当调用`Box1.getLength()`的时候, 编译器负责把Box1的地址传递给getLength的隐式形参this.

因为this的目的总是指向"这个"对象, 所以this是一个常量指针, 我们不允许改变this中保存的地址.

另外, 友元函数没有 this 指针，因为友元不是类的成员。只有成员函数才有 this 指针。
