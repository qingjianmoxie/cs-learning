/*
 *基于数组的栈实现
 */
#ifndef ARRAY_STACK_HPP
#define ARRAY_STACK_HPP

template <typename T>
class ArrayStack {
public:
    ArrayStack(int s = 10);  //默认的栈容量为10
    ~ArrayStack();

public:
    T top();         //获取栈顶元素
    void push(T t);  //压栈操作
    T pop();         //弹栈操作
    bool isEmpty();  //判空操作
    int size();      //求栈的大小

private:
    int count_;     //栈的元素数量
    int capacity_;  //栈的容量
    T *array_;      //底层为数组
};

/*构造函数*/
template <typename T>
ArrayStack<T>::ArrayStack(int s) : count_(0), capacity_(s), array_(nullptr) {
    array_ = new T[capacity_];
};

/*析构函数*/
template <typename T>
ArrayStack<T>::~ArrayStack() {
    if (array_) {
        delete[] array_;
        array_ = nullptr;
    }
};

/*栈的判空操作*/
template <typename T>
bool ArrayStack<T>::isEmpty() {
    return count_ == 0;  //栈元素为0时为栈空
};

/*返回栈的大小*/
template <typename T>
int ArrayStack<T>::size() {
    return count_;
};

/*插入元素*/
template <typename T>
void ArrayStack<T>::push(T t) {
    if (count_ != capacity_)  //先判断是否栈满
    {
        array_[count_++] = t;
    }
};

/*弹栈*/
template <typename T>
T ArrayStack<T>::pop() {
    if (count_ != 0)  //先判断是否是空栈
    {
        return array_[--count_];
    }
};

/*获取栈顶元素*/
template <typename T>
T ArrayStack<T>::top() {
    if (count_ != 0) {
        return array_[count_ - 1];
    }
};

#endif
