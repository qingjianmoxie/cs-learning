/*
 * 基于C++template实现的单链表类
 */

#ifndef SINGLE_LINKED_LIST_H_
#define SINGLE_LINKED_LIST_H_

//节点结构
template <typename T>
class Node {
public:
    T value_;
    Node* next_;

public:
    Node() = default;
    Node(T value, Node* next) : value_(value), next_(next) {}
};

//单链表
template <typename T>
class SingleLink {
public:
    typedef Node<T>* pointer;
    SingleLink();
    ~SingleLink();

    int size();      //获取长度
    bool isEmpty();  //判空

    pointer insert(int index, T t);  //在指定位置进行插入
    pointer insert_head(T t);        //在链表头进行插入
    pointer insert_last(T t);        //在链表尾进行插入

    pointer del(int index);  //在指定位置进行删除
    pointer delete_head();   //删除链表头
    pointer delete_last();   //删除链表尾

    T get(int index);  //获取指定位置的元素
    T get_head();      //获取链表头元素
    T get_last();      //获取链表尾元素

    pointer getHead();  //获取链表头节点

private:
    int count_;
    pointer phead_;

private:
    pointer getNode(int index);  //获取指定位置的节点
};

//默认构造函数
template <typename T>
SingleLink<T>::SingleLink() : count_(0), phead_(nullptr) {
    //创建头节点
    phead_ = new Node<T>();
    phead_->next_ = nullptr;
};

/*
返回指定索引的前一个位置节点，若链表为空，则返回头节点
*/
template <typename T>
Node<T>* SingleLink<T>::getNode(int index) {
    if (index > count_ || index < 0) return nullptr;
    int temp = 0;
    Node<T>* preNode = phead_;
    while (temp < index) {
        temp++;
        preNode = preNode->next_;
    }
    return preNode;
}

/*
析构函数
*/
template <typename T>
SingleLink<T>::~SingleLink() {
    Node<T>* pNode = phead_->next_;
    while (nullptr != pNode) {
        Node<T>* temp = pNode;
        pNode = pNode->next_;
        delete temp;
    }
    //进行销毁
    delete phead_;
    phead_ = nullptr;
};

//返回链表的大小
template <typename T>
int SingleLink<T>::size() {
    return count_;
};

//链表判空
template <typename T>
bool SingleLink<T>::isEmpty() {
    return count_ == 0;
};

template <typename T>
Node<T>* SingleLink<T>::getHead() {
    return phead_->next_;
}

/*
在指定位置插入新节点
*/
template <typename T>
Node<T>* SingleLink<T>::insert(int index, T t) {
    Node<T>* preNode = getNode(index);
    if (preNode) {
        //构建新节点，构建时指明节点的next节点
        Node<T>* newNode = new Node<T>(t, preNode->next_);
        preNode->next_ = newNode;
        count_++;
        return newNode;
    }
    return nullptr;
};

/*
从头部插入
*/
template <typename T>
Node<T>* SingleLink<T>::insert_head(T t) {
    return insert(0, t);
};

/*
从尾部进行插入
*/
template <typename T>
Node<T>* SingleLink<T>::insert_last(T t) {
    return insert(count_, t);
};

/*
删除链表指定位置元素
*/
template <typename T>
Node<T>* SingleLink<T>::del(int index) {
    if (isEmpty()) return nullptr;
    Node<T>* ptrNode = getNode(index);
    Node<T>* delNode = ptrNode->next_;
    ptrNode->next_ = delNode->next_;
    count_--;
    delete delNode;
    return ptrNode->next_;
};

/*
删除头节点
*/
template <typename T>
Node<T>* SingleLink<T>::delete_head() {
    return del(0);
};

/*
删除尾节点
*/
template <typename T>
Node<T>* SingleLink<T>::delete_last() {
    return del(count_);
};
#endif
