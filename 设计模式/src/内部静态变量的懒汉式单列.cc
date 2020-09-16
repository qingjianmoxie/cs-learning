#include <iostream>  // std::cout
#include <list>      // std::list
#include <mutex>     // std::mutex
#include <thread>    // std::thread

///////////////////  内部静态变量的懒汉实现  //////////////////
class SingleInstance {
public:
    // 获取单实例对象
    static SingleInstance &GetInstance();

    // 打印实例地址
    void Print() { printf("我的实例内存地址是: %p\n", this); }

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    SingleInstance();
    ~SingleInstance();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    SingleInstance(const SingleInstance &signal);
    const SingleInstance &operator=(const SingleInstance &signal);
};


SingleInstance &SingleInstance::GetInstance() {
    // 局部静态特性的方式实现单实例
    static SingleInstance signal;
    return signal;
}

SingleInstance::SingleInstance() { printf("构造函数\n"); }

SingleInstance::~SingleInstance() { printf("析构函数\n"); }
///////////////////  内部静态变量的懒汉实现  //////////////////

// 线程函数
void PrintHello(int threadid) {
    printf("Hi, I'm thread ID: [%d]\n", threadid);

    // 打印实例地址
    SingleInstance::GetInstance().Print();
}

const int NUM_THREADS = 5;  // 线程个数

int main(void) {
    std::cout << "main() : 开始 ... " << std::endl;

    std::list<std::thread> lstThread;
    for (int i = 0; i < NUM_THREADS; i++) {
        lstThread.emplace_back(PrintHello, i);
    }

    for (std::thread &thr : lstThread) {
        thr.join();
    }

    std::cout << "main() : 结束! " << std::endl;

    return 0;
}
