#ifndef _MAX_HEAP_HCC_
#define _MAX_HEAP_HCC_

#include <iostream>

/*最大堆类定义*/
template <typename T>
class MaxHeap {
public:
    MaxHeap(int cap = 10);  //默认的数组大小为10
    ~MaxHeap();

public:
    bool insert(T val);                   // 往二叉堆中插入元素
    bool remove(T data);                  // 移除元素
    void print();                         // 打印堆
    T getTop();                           // 获取堆顶元素
    bool createMaxHeap(T a[], int size);  // 根据指定的数组来创建一个最大堆

private:
    int capacity;  // 容量，也即是数组的大小
    int size;      // 堆大小，也即是数组中有效元素的个数
    T* heap;       // 底层的数组
private:
    void filterUp(int index);             // 从index所在节点，往根节点调整堆
    void filterDown(int begin, int end);  // 从begin所在节点开始，向end方向调整堆
};

/*默认构造函数*/
template <typename T>
MaxHeap<T>::MaxHeap(int cap)
    : capacity(cap), size(0), heap(nullptr) {
    heap = new T[capacity];
};

/*析构函数*/
template <typename T>
MaxHeap<T>::~MaxHeap() {
    delete[] heap;
};

/*打印最大堆*/
template <typename T>
void MaxHeap<T>::print() {
    for (int i = 0; i < size; i++) {
        std::cout << heap[i] << " ";
    }
    std::cout << std::endl;
};

/*获取堆顶元素*/
template <typename T>
T MaxHeap<T>::getTop() {
    if (size != 0)
        return heap[0];
};

/********************
 * 插入元素
 * 在数组的最末尾插入新节点，然后自下而上地调整子节点与父节点的位置：
 * 比较当前结点与父节点的大小，若不满足最大堆的性质，则交换两节点。
 * 时间复杂度为O(logn)。
 ********************/
template <typename T>
bool MaxHeap<T>::insert(T val) {
    if (size == capacity)  //如果数组已满，则返回false
        return false;
    heap[size] = val;
    filterUp(size);
    size++;
    return true;
};
/*从下到上调整堆*/
/*插入元素时候使用*/
template <typename T>
void MaxHeap<T>::filterUp(int index) {
    T value = heap[index];  //插入节点的值

    while (index > 0)  //如果还未到达根节点，继续调整
    {
        int indexParent = (index - 1) / 2;  //求其双亲节点
        if (value < heap[indexParent])
            break;
        else {
            heap[index] = heap[indexParent];
            index = indexParent;
        }
    }
    heap[index] = value;  //12插入最后的位置
};

/*根据指定的数组来创建一个最大堆*/
template <typename T>
bool MaxHeap<T>::createMaxHeap(T a[], int size) {
    if (size > capacity)  //    堆的容量不足以创建
        return false;
    for (int i = 0; i < size; i++) {
        insert(a[i]);
    }
    return true;
};

/*****************
 * 删除元素
 * 用数组最末尾节点覆盖被删节点，再从该节点从上到下调整二叉堆
 *****************/
template <typename T>
bool MaxHeap<T>::remove(T data) {
    if (size == 0)  //如果堆是空的
        return false;
    int index;
    for (index = 0; index < size; index++)  //获取值在数组中的索引
    {
        if (heap[index] == data)
            break;
    }
    if (index == size)  //数组中没有该值
        return false;

    heap[index] = heap[size - 1];  //使用最后一个节点来代替当前结点，然后再向下调整当前结点。

    filterDown(index, size--);

    return true;
};
/*从上到下调整堆*/
/*删除元素时候使用*/
template <typename T>
void MaxHeap<T>::filterDown(int current, int end) {
    int child = current * 2 + 1;  //当前结点的左孩子

    T value = heap[current];  //保存当前结点的值

    while (child <= end) {
        if (child < end && heap[child] < heap[child + 1])  //选出两个孩子中较大的孩子
            child++;
        if (value > heap[child]) {
            //无须调整, 调整结束
            break;
        } else {
            heap[current] = heap[child];  //孩子节点覆盖当前结点
            current = child;              //向下移动
            child = child * 2 + 1;
        }
    }
    heap[current] = value;
};

#endif
