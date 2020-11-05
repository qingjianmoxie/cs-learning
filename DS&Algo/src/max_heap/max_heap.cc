#include "max_heap.h"

int main() {
    MaxHeap<int> heap(11);
    //逐个元素构建大顶堆
    for (int i = 0; i < 10; i++) {
        heap.insert(i);
    }
    heap.print();
    heap.remove(8);
    heap.print();

    //根据指定的数组创建最大堆
    MaxHeap<int> heap2(11);
    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    heap2.createMaxHeap(a, 10);
    heap2.print();
    return 0;
}
