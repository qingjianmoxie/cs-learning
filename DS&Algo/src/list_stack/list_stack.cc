/*
 *测试代码
 */
#include "list_stack.h"

#include <iostream>
#include <string>
using namespace std;

int main() {
    LinkStack<string> lstack;
    lstack.push("hello");
    lstack.push("to");
    lstack.push("you！");

    cout << "栈的大小:" << lstack.size() << endl;
    cout << "栈顶元素:" << lstack.top() << endl;

    while (!lstack.isEmpty()) {
        lstack.pop();
    }

    cout << "栈的大小:" << lstack.size() << endl;

    return 0;
}
