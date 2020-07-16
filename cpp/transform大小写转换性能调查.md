今天工作中遇到一个情况, 程序频繁调用transform转换string大小写, 导致性能急剧下降. 个人感觉不是stl的锅, 不管是哪种大小写转换函数, 频繁调用都会导致性能下降. 为了验证我的想法, 我写了一个大并发下循环调用转换函数的demo, 查看一下耗时.

代码如下:
```c++
#include <time.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

using std::cout;
using std::endl;
using std::string;
using std::thread;
using std::transform;

#define LOOPCOUNT 10000

void mytolower_1(string s) {
    for (int i = 0; i < LOOPCOUNT; i++) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
    }
}

void mytolower_2(string s) {
    for (int i = 0; i < LOOPCOUNT; i++) {
        for (size_t iPos = 0; iPos < s.length(); iPos++) {
            char cVal = s.at(iPos);
            if (cVal >= 'A' && cVal <= 'Z') {
                s[iPos] = cVal - 'A' + 'a';
            }
            /*
            使用以下写法会让运行时间长一倍, 推断是s[iPos]比较耗时
            if (s[iPos] >= 'A' && s[iPos] <= 'Z') {
                s[iPos] = s[iPos] - 'A' + 'a';
            }
            */
        }
    }
}

int main() {
    clock_t start, end;
    start = clock();
    for (int i = 0; i < 100; i++) {
        thread thread(mytolower_1, "HELLO WORLD");
        thread.join();
    }
    end = clock();
    cout << "1 run time is: " << (double)(end - start) / CLOCKS_PER_SEC << "s"
         << endl;

    start = clock();
    for (int i = 0; i < 100; i++) {
        thread thread(mytolower_2, "HELLO WORLD");
        thread.join();
    }
    end = clock();
    cout << "2 run time is: " << (double)(end - start) / CLOCKS_PER_SEC << "s"
         << endl;
    return 0;
}
```

系统信息如下:
Red Hat Enterprise Linux Server release 7.4 (Maipo)
gcc 版本 4.8.5 20150623 (Red Hat 4.8.5-16) (GCC)

编译并执行程序时崩溃了

    $ g++ -std=c++11 -o test test.cpp
    $ ./test

    terminate called after throwing an instance of 'std::system_error'
      what():  Enable multithreading to use std::thread: Operation not permitted
    已放弃(吐核)

网上查到的解决方案是编译命令改为如下形式:

    g++ -std=c++11 -pthread -o test test.cpp

结果如下:

线程数| 循环次数 | 1耗时 | 2耗时
:- | :- | :- | :-
100  | 10000   | 0.2   | 0.06
100  | 100000  | 1.98  | 0.64
100  | 1000000 | 19.75 | 6.2
1000 | 100000  | 19.94 | 6.37

我被打脸了, 性能还是相差3倍左右...
