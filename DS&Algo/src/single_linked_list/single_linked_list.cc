/*
 *
 *测试代码
 */

#include "single_linked_list.h"

#include <iostream>
using namespace std;
int main() {
    SingleLink<int> link;
    for (int i = 0; i < 10; i++) {
        link.insert(i, i);
    }
    cout << link.size() << endl;

    link.insert_head(1111);
    link.insert_last(2222);

    SingleLink<int>::pointer ptr = link.getHead();
    while (ptr != nullptr) {
        cout << ptr->value_ << endl;
        ptr = ptr->next_;
    }

    return 0;
}
