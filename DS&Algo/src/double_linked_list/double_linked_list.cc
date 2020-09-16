/*
双链表测试代码
*/

#include "double_linked_list.h"

#include <iostream>
using namespace std;

int main() {
    DoubleLink<int> dlink;
    //插入测试
    for (int i = 0; i < 10; i++) {
        dlink.insert(0, i + 10);
    }
    dlink.insert(0, 100);
    dlink.insert_last(1000);
    cout << "链表长度:" << dlink.size() << endl;

    //删除测试
    dlink.delete_front();
    dlink.delete_last();
    dlink.del(3);

    DoubleLink<int>::pointer ptr = dlink.getHead();
    ptr = ptr->next_ptr;
    while (ptr != dlink.getHead()) {
        cout << ptr->value << endl;
        ptr = ptr->next_ptr;
    }

    return 0;
}
