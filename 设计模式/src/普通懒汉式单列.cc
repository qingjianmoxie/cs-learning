///////////////////  普通懒汉式实现 -- 线程不安全 //////////////////
#include <iostream>  // std::cout
#include <list>      // std::list
#include <mutex>     // std::mutex
#include <thread>    // std::thread

class SingleInstance {
public:
    // 获取单例对象
    static SingleInstance *GetInstance();

    // 释放单例，进程退出时调用
    static void deleteInstance();

    // 打印单例地址
    void Print() { printf("我的实例内存地址是: %p\n", this); }

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    SingleInstance();
    ~SingleInstance();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    SingleInstance(const SingleInstance &signal);
    const SingleInstance &operator=(const SingleInstance &signal);

private:
    // 唯一单例对象指针
    static SingleInstance *m_SingleInstance;
};

//初始化静态成员变量
SingleInstance *SingleInstance::m_SingleInstance = NULL;

SingleInstance *SingleInstance::GetInstance() {
    if (m_SingleInstance == NULL) {
        // 没有加锁是线程不安全的，当线程并发时会创建多个实例
        m_SingleInstance = new (std::nothrow) SingleInstance;
    }

    return m_SingleInstance;
}

void SingleInstance::deleteInstance() {
    if (m_SingleInstance) {
        delete m_SingleInstance;
        m_SingleInstance = NULL;
    }
}

SingleInstance::SingleInstance() { printf("构造函数\n"); }

SingleInstance::~SingleInstance() { printf("析构函数\n"); }
///////////////////  普通懒汉式实现 -- 线程不安全  //////////////////

// 线程函数
void PrintHello(int threadid) {
    printf("Hi, I'm thread ID: [%d]\n", threadid);

    // 打印实例地址
    SingleInstance::GetInstance()->Print();
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
    SingleInstance::deleteInstance();
    std::cout << "main() : 结束! " << std::endl;

    return 0;
}
