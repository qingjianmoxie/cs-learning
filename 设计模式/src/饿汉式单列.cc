#include <iostream>  // std::cout
#include <list>      // std::list
#include <mutex>     // std::mutex
#include <thread>    // std::thread

////////////////////////// 饿汉实现 /////////////////////
class Singleton {
public:
    // 获取单实例
    static Singleton *GetInstance();

    // 释放单实例，进程退出时调用
    static void deleteInstance();

    // 打印实例地址
    void Print() { printf("我的实例内存地址是: %p\n", this); }

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    Singleton();
    ~Singleton();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    Singleton(const Singleton &signal);
    const Singleton &operator=(const Singleton &signal);

private:
    // 唯一单实例对象指针
    static Singleton *g_pSingleton;
};

// 代码一运行就初始化创建实例 ，本身就线程安全
Singleton *Singleton::g_pSingleton = new (std::nothrow) Singleton;

Singleton *Singleton::GetInstance() { return g_pSingleton; }

void Singleton::deleteInstance() {
    if (g_pSingleton) {
        delete g_pSingleton;
        g_pSingleton = NULL;
    }
}

Singleton::Singleton() { printf("构造函数\n"); }

Singleton::~Singleton() { printf("析构函数\n"); }
////////////////////////// 饿汉实现 /////////////////////

// 线程函数
void PrintHello(int threadid) {
    printf("Hi, I'm thread ID: [%d]\n", threadid);

    // 打印实例地址
    Singleton::GetInstance()->Print();
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

    // 手动释放单实例的资源
    Singleton::deleteInstance();
    std::cout << "main() : 结束! " << std::endl;

    return 0;
}
