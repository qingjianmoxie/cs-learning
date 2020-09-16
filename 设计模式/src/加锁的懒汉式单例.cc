#include <iostream>  // std::cout
#include <list>      // std::list
#include <mutex>     // std::mutex
#include <thread>    // std::thread

///////////////////  加锁的懒汉式实现  //////////////////
class SingleInstance {
public:
    // 获取单实例对象
    static SingleInstance *&GetInstance();

    //释放单实例，进程退出时调用
    static void deleteInstance();

    // 打印实例地址
    void Print() { printf("我的实例内存地址是: %p\n", this); }

private:
    // 将其构造和析构成为私有的, 禁止外部构造和析构
    SingleInstance();
    ~SingleInstance();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    SingleInstance(const SingleInstance &signal);
    const SingleInstance &operator=(const SingleInstance &signal);

private:
    // 唯一单实例对象指针
    static SingleInstance *m_SingleInstance;
    static std::mutex m_Mutex;
};

//初始化静态成员变量
SingleInstance *SingleInstance::m_SingleInstance = NULL;
std::mutex SingleInstance::m_Mutex;

SingleInstance *&SingleInstance::GetInstance() {
    //  这里使用了两个if判断语句的技术称为双检锁；好处是，只有判断指针为空的时候才加锁，
    //  避免每次调用 GetInstance的方法都加锁，锁的开销毕竟还是有点大的。
    if (m_SingleInstance == NULL) {
        std::unique_lock<std::mutex> lock(m_Mutex);  // 加锁
        if (m_SingleInstance == NULL) {
            m_SingleInstance = new (std::nothrow) SingleInstance;
        }
    }

    return m_SingleInstance;
}

void SingleInstance::deleteInstance() {
    std::unique_lock<std::mutex> lock(m_Mutex);  // 加锁
    if (m_SingleInstance) {
        delete m_SingleInstance;
        m_SingleInstance = NULL;
    }
}

SingleInstance::SingleInstance() { printf("构造函数\n"); }

SingleInstance::~SingleInstance() { printf("析构函数\n"); }
///////////////////  加锁的懒汉式实现  //////////////////

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
