我们提供了一个类：

    public class Foo {
      public void one() { print("one"); }
      public void two() { print("two"); }
      public void three() { print("three"); }
    }

三个不同的线程将会共用一个 Foo 实例。

+ 线程 A 将会调用 one() 方法
+ 线程 B 将会调用 two() 方法
+ 线程 C 将会调用 three() 方法

请设计修改程序，以确保 two() 方法在 one() 方法之后被执行，three() 方法在 two() 方法之后被执行。

示例 1:

    输入: [1,2,3]
    输出: "onetwothree"
    解释: 
    有三个线程会被异步启动。
    输入 [1,2,3] 表示线程 A 将会调用 one() 方法，线程 B 将会调用 two() 方法，线程 C 将会调用 three() 方法。
    正确的输出是 "onetwothree"。

示例 2:

    输入: [1,3,2]
    输出: "onetwothree"
    解释: 
    输入 [1,3,2] 表示线程 A 将会调用 one() 方法，线程 B 将会调用 three() 方法，线程 C 将会调用 two() 方法。
    正确的输出是 "onetwothree"。

注意:

尽管输入中的数字似乎暗示了顺序，但是我们并不保证线程在操作系统中的调度顺序。

你看到的输入格式主要是为了确保测试的全面性。

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/print-in-order
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

```c++
class Foo {
public:
    Foo() {}

    void first(function<void()> printFirst) {
        // 等待直至 main() 发送数据
        std::unique_lock<std::mutex> lk(mutex_);
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        // 通知前完成手动解锁，以避免等待线程才被唤醒就阻塞（细节见 notify_one）
        counter_++;
        cv1_.notify_one();
    }

    void second(function<void()> printSecond) {
        std::unique_lock<std::mutex> lk(mutex_);
        cv1_.wait(lk, [this]() { return counter_ == 2; });  // 阻塞当前线程，直到条件变量被唤醒
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        counter_++;
        cv2_.notify_one();
    }

    void third(function<void()> printThird) {
        std::unique_lock<std::mutex> lk(mutex_);
        cv2_.wait(lk, [this]() { return counter_ == 3; });
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }

private:
    int counter_ = 1;
    std::condition_variable cv1_;
    std::condition_variable cv2_;
    // 使用lock和unlock手动加锁
    std::mutex mutex_;
};
```
