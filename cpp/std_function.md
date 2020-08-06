# std::function

类模版`std::function`是一种通用、多态的函数封装。`std::function`的实例可以对任何可以调用的目标实体进行存储、复制、和调用操作，这些目标实体包括普通函数、Lambda表达式、函数指针、以及其它函数对象等。`std::function`对象是对C++中现有的可调用实体的一种类型安全的包裹（我们知道像函数指针这类可调用实体，是类型不安全的）。

通常std::function是一个函数对象类，它包装其它任意的函数对象，被包装的函数对象具有类型为T1, …,TN的N个参数，并且返回一个可转换到R类型的值。`std::function`使用 模板转换构造函数接收被包装的函数对象；特别是，闭包类型可以隐式地转换为`std::function`。

最简单的理解就是：

> 通过`std::function`对C++中各种可调用实体（普通函数、Lambda表达式、函数指针、以及其它函数对象等）的封装，形成一个新的可调用的`std::function`对象；让我们不再纠结那么多的可调用实体。一切变的简单粗暴。

## 怎么使用std::function

使用`std::function`的感觉就是“万众归一”，下面就通过实际的代码例子，看看究竟怎么使用`std::function`。会使用了才是王道。

```c++
#include <functional>
#include <iostream>
using namespace std;

std::function<int(int)> Functional;

// 普通函数
int TestFunc(int a) { return a; }

// Lambda表达式
auto lambda = [](int a) -> int { return a; };

// 仿函数(functor)
class Functor {
   public:
    int operator()(int a) { return a; }
};

// 1.类成员函数
// 2.类静态函数
class TestClass {
   public:
    int ClassMember(int a) { return a; }
    static int StaticMember(int a) { return a; }
};

int main() {
    // 普通函数
    Functional = TestFunc;
    int result = Functional(10);
    cout << "普通函数：" << result << endl;

    // Lambda表达式
    Functional = lambda;
    result = Functional(20);
    cout << "Lambda表达式：" << result << endl;

    // 仿函数
    Functor testFunctor;
    Functional = testFunctor;
    result = Functional(30);
    cout << "仿函数：" << result << endl;

    // 类成员函数
    TestClass testObj;
    Functional = std::bind(&TestClass::ClassMember, testObj, std::placeholders::_1);
    result = Functional(40);
    cout << "类成员函数：" << result << endl;

    // 类静态函数
    Functional = TestClass::StaticMember;
    result = Functional(50);
    cout << "类静态函数：" << result << endl;

    return 0;
}
```

对于各个可调用实体转换成`std::function`类型的对象，上面的代码都有，运行一下代码，阅读一下上面那段简单的代码。总结了简单的用法以后，来看看一些需要注意的事项：

* 关于可调用实体转换为`std::function`对象需要遵守以下两条原则：
    * 转换后的`std::function`对象的参数能转换为可调用实体的参数；
    * 可调用实体的返回值能转换为`std::function`对象的返回值。
* `std::function`对象最大的用处就是在实现函数回调，使用者需要注意，它不能被用来检查相等或者不相等，但是可以与NULL或者nullptr进行比较。

## 为什么要用std::function

好用并实用的东西才会加入标准的。因为好用，实用，我们才在项目中使用它。`std::function`实现了一套类型消除机制，可以统一处理不同的函数对象类型。以前我们使用函数指针来完成这些；现在我们可以使用更安全的`std::function`来完成这些任务。

还有为什么？我也不知道还有为什么？等以后发现了更好的实际应用实例再回来说为什么吧。