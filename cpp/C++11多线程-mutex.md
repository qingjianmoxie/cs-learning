# C++11多线程-mutex

mutex又称互斥量，用于提供对共享变量的互斥访问。C++11中mutex相关的类都在`<mutex>`头文件中。共四种互斥类:

| 序号 | 名称 | 用途 |
| :- | :- | :- |
| 1 | std::mutex | 最基本也是最常用的互斥类 |
| 2 | std::recursive_mutex | 同一线程内可递归(重入)的互斥类 |
| 3 | std::timed_mutex | 除具备mutex功能外，还提供了带时限请求锁定的能力 |
| 4 | std::recursive_timed_mutex | 同一线程内可递归(重入)的timed_mutex |

与std::thread一样，mutex相关类不支持拷贝构造、不支持赋值。同时mutex类也不支持move语义(move构造、move赋值)。不用担心会误用这些操作，真要这么做了的话，编译器会阻止你的。

## lock, try_lock, unlock

mutex的标准操作，四个mutex类都支持这些操作，但是不同类在行为上有些微的差异。

### 1.1 lock

锁住互斥量。调用lock时有三种情况:

1. 如果互斥量没有被锁住，则调用线程将该mutex锁住，直到调用线程调用unlock释放。
2. 如果mutex已被其它线程lock，则调用线程将被阻塞，直到其它线程unlock该mutex。
3. 如果当前mutex已经被调用者线程锁住，则std::mutex死锁，而recursive系列则成功返回。

### 1.2 try_lock

尝试锁住mutex，调用该函数同样也有三种情况：

1. 如果互斥量没有被锁住，则调用线程将该mutex锁住(返回true)，直到调用线程调用unlock释放。
2. 如果mutex已被其它线程lock，则调用线程将失败，并返回false。
3. 如果当前mutex已经被调用者线程锁住，则std::mutex死锁，而recursive系列则成功返回true。

### 1.3 unlock

解锁mutex，释放对mutex的所有权。值得一提的时，对于recursive系列mutex，unlock次数需要与lock次数相同才可以完全解锁。

下面给出一个mutex小例子

```c++
#include <iostream>
#include <thread>
#include <mutex>

void inc(std::mutex &mutex, int loop, int &counter) {
    for (int i = 0; i < loop; i++) {
        mutex.lock();
        ++counter;
        mutex.unlock();
    }
}
int main() {
    std::thread threads[5];
    std::mutex mutex;
    int counter = 0;

    for (std::thread &thr: threads) {
        thr = std::thread(inc, std::ref(mutex), 1000, std::ref(counter));
    }
    for (std::thread &thr: threads) {
        thr.join();
    }

    // 输出：5000，如果inc中调用的是try_lock，则此处可能会<5000
    std::cout << counter << std::endl;

    return 0;
}
//: g++ -std=c++11 main.cpp
```

## try_lock_for, try_lock_until

这两个函数仅用于timed系列的mutex(std::timed_mutex, std::recursive_timed_mutex)，函数最多会等待指定的时间，如果仍未获得锁，则返回false。除超时设定外，这两个函数与try_lock行为一致。

```c++
// 等待指定时长
template <class Rep, class Period>
    try_lock_for(const chrono::duration<Rep, Period>& rel_time);
// 等待到指定时间
template <class Clock, class Duration>
    try_lock_until(const chrono::time_point<Clock, Duration>& abs_time);
```

try_lock_for相关代码

```c++
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

void run500ms(std::timed_mutex &mutex) {
    auto _500ms = std::chrono::milliseconds(500);
    if (mutex.try_lock_for(_500ms)) {
        std::cout << "获得了锁" << std::endl;
    } else {
        std::cout << "未获得锁" << std::endl;
    }
}
int main() {
    std::timed_mutex mutex;

    mutex.lock();
    std::thread thread(run500ms, std::ref(mutex));
    thread.join();
    mutex.unlock();

    return 0;
}
//输出：未获得锁
```

## lock_guard

C++11在提供了常规mutex的基础上，还提供了一些易用性的类，本节后续内容将介绍这些类。

lock_guard利用了C++ RAII的特性，在构造函数中上锁，析构函数中解锁。lock_guard是一个模板类，其原型为

```c++
template <class Mutex> class lock_guard
```

模板参数Mutex代表互斥量，可以是上面介绍的std::mutex, std::timed_mutex, std::recursive_mutex, std::recursive_timed_mutex中的任何一个，也可以是std::unique_lock(下面即将介绍)，这些都提供了lock和unlock的能力。

lock_guard仅用于上锁、解锁，不对mutex承担供任何生命周期的管理，因此在使用的时候，请确保lock_guard管理的mutex一直有效。

同其它mutex类一样，locak_guard不允许拷贝，即拷贝构造和赋值函数被声明为delete。

```c++
lock_guard(lock_guard const&) = delete;
lock_guard& operator=(lock_guard const&) = delete;
```

lock_guard的设计保证了即使程序在锁定期间发生了异常，也会安全的释放锁，不会发生死锁。代码如下所示:

```c++
#include <iostream>
#include <mutex>

std::mutex mutex;

void safe_thread() {
    try {
        std::lock_guard<std::mutex> _guard(mutex);
        throw std::logic_error("logic error");
    }
    catch (std::exception &ex) {
        std::cerr << "[caught] " << ex.what() << std::endl;
    }
}

int main() {
    safe_thread();
    // 此处仍能上锁
    mutex.lock();
    std::cout << "OK, still locked" << std::endl;
    mutex.unlock();

    return 0;
}
```

程序输出

    [caught] logic error
    OK, still locked

异常处理的相关知识可以点[这里](./try_catch_throw.md)

## unique_lock

lock_guard提供了简单上锁、解锁操作，但当我们需要更灵活的操作时便无能为力了。这些就需要unique_lock上场了。unique_lock拥有对Mutex的**所有权**，一但初始化了unique_lock，其就接管了该mutex, 在unique_lock结束生命周期前(析构前)，其它地方就不要再直接使用该mutex了。unique_lock提供的功能较多，此处不一一列举，下面列出unique_lock的类声明，及部分注释，供大家参考

```c++
template <class Mutex>
class unique_lock
{
public:
    typedef Mutex mutex_type;
    // 空unique_lock对象
    unique_lock() noexcept;
    // 管理m, 并调用m.lock进行上锁，如果m已被其它线程锁定，该构造函数会阻塞。
    explicit unique_lock(mutex_type& m);
    // 仅管理m，构造函数中不对m上锁。可以在初始化后调用lock, try_lock, try_lock_xxx系列进行上锁。
    unique_lock(mutex_type& m, defer_lock_t) noexcept;
    // 管理m, 并调用m.try_lock，上锁不成功不会阻塞当前线程
    unique_lock(mutex_type& m, try_to_lock_t);
    // 管理m, 该函数假设m已经被当前线程锁定，不再尝试上锁。
    unique_lock(mutex_type& m, adopt_lock_t);
    // 管理m, 并调用m.try_lock_unitil函数进行加锁
    template <class Clock, class Duration>
        unique_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time);
    // 管理m，并调用m.try_lock_for函数进行加锁
    template <class Rep, class Period>
        unique_lock(mutex_type& m, const chrono::duration<Rep, Period>& rel_time);
    // 析构，如果此前成功加锁(或通过adopt_lock_t进行构造)，并且对mutex拥有所有权，则解锁mutex
    ~unique_lock();

    // 禁止拷贝操作
    unique_lock(unique_lock const&) = delete;
    unique_lock& operator=(unique_lock const&) = delete;

    // 支持move语义
    unique_lock(unique_lock&& u) noexcept;
    unique_lock& operator=(unique_lock&& u) noexcept;

    void lock();
    bool try_lock();

    template <class Rep, class Period>
        bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);
    template <class Clock, class Duration>
        bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time);

    // 显示式解锁，该函数调用后，除非再次调用lock系列函数进行上锁，否则析构中不再进行解锁
    void unlock();

    // 与另一个unique_lock交换所有权
    void swap(unique_lock& u) noexcept;
    // 返回当前管理的mutex对象的指针，并释放所有权
    mutex_type* release() noexcept;

    // 当前实例是否获得了锁
    bool owns_lock() const noexcept;
    // 同owns_lock
    explicit operator bool () const noexcept;
    // 返回mutex指针，便于开发人员进行更灵活的操作
    // 注意：此时mutex的所有权仍归unique_lock所有，因此不要对mutex进行加锁、解锁操作
    mutex_type* mutex() const noexcept;
};
```

## std::call_once

该函数的作用顾名思义：保证call_once调用的函数只被执行一次。该函数需要与std::once_flag配合使用。std::once_flag被设计为对外封闭的，即外部没有任何渠道可以改变once_flag的值，仅可以通过std::call_once函数修改。一般情况下我们在自己实现call_once效果时，往往使用一个全局变量，以及双重检查锁(DCL)来实现，即便这样该实现仍然会有很多坑(多核环境下)。有兴趣的读者可以搜索一下DCL来看，此处不再赘述。
C++11为我们提供了简便的解决方案，所需做的仅仅像下面这样使用即可。

```c++
#include <iostream>
#include <thread>
#include <mutex>

void initialize() {
    std::cout << __FUNCTION__ << std::endl;
}

std::once_flag of;
void my_thread() {
    std::call_once(of, initialize);
}

int main() {
    std::thread threads[10];
    for (std::thread &thr: threads) {
        thr = std::thread(my_thread);
    }
    for (std::thread &thr: threads) {
        thr.join();
    }
    return 0;
}
// 仅输出一次：initialize
```

## std::try_lock

当有多个mutex需要执行try_lock时，该函数提供了简便的操作。try_lock会按参数从左到右的顺序，对mutex**顺次执行**try_lock操作。当其中某个mutex.try_lock失败(返回false或抛出异常)时，已成功锁定的mutex都将被解锁。
需要注意的是，该函数成功时返回-1， 否则返回失败mutex的索引，索引从0开始计数。

```c++
template <class L1, class L2, class... L3>
  int try_lock(L1&, L2&, L3&...);
```

## std::lock

std::lock是较智能的批量上锁方式，采用死锁算法来锁定给定的mutex列表，避免死锁。该函数对mutex列表的上锁顺序是不确定的。该函数保证: 如果成功，则所有mutex全部上锁，如果失败，则全部解锁。

```c++
template <class L1, class L2, class... L3>
  void lock(L1&, L2&, L3&...);
```

## 链接

- [目录](README.md)
- 上一节：[C++11多线程](./C++11多线程.md)
- 下一节：[C++11多线程-条件变量](./C++11多线程-condition_variable.md)
