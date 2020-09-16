/*
 * content: 循环双向链表
 */

#ifndef DOUBLE_LINKED_LIST_H_
#define DOUBLE_LINKED_LIST_H_
/*
双向链表的节点结构
*/
template <typename T>
struct Node {
    Node() = default;
    Node(T value, Node<T>* preptr, Node<T>* nextptr)
        : value(value), pre_ptr(preptr), next_ptr(nextptr) {}

    T value;
    Node<T>* pre_ptr;
    Node<T>* next_ptr;
};

/*
 * 双向链表类
 */
template <typename T>
class DoubleLink {
public:
    typedef Node<T>* pointer;

public:
    DoubleLink();
    ~DoubleLink(){};

public:
    pointer insert(int index, T value);
    pointer insert_front(T value);
    pointer insert_last(T value);

    pointer del(int index);
    pointer delete_front();
    pointer delete_last();

    bool isEmpty();
    int size();

    T get(int index);
    T get_front();
    T get_last();
    pointer getHead();

private:
    pointer phead_;
    int count_;

private:
    pointer getNode(int index);
};

/*
 * 构造函数
 */
template <typename T>
DoubleLink<T>::DoubleLink() {
    phead_ = new Node<T>(0, nullptr, nullptr);
    phead_->next_ptr = phead_;
    phead_->pre_ptr = phead_;
    count_ = 0;
}

template <typename T>
Node<T>* DoubleLink<T>::getHead() {
    return phead_;
}

/*
 *返回链表长度
 */
template <typename T>
int DoubleLink<T>::size() {
    return count_;
}

/*
获取指定下标的元素
*/
template <typename T>
Node<T>* DoubleLink<T>::getNode(int index) {
    if (index >= count_ || index < 0) return nullptr;

    if (index <= count_ / 2)  //如果在前半部分
    {
        Node<T>* pnode = phead_->next_ptr;
        while (index) {
            pnode = pnode->next_ptr;
            index--;
        }
        return pnode;
    }
    //在后半部分
    index = count_ - index - 1;
    Node<T>* pnode = phead_->pre_ptr;
    while (index) {
        pnode = pnode->pre_ptr;
        index--;
    }
    return pnode;
}

/*
 *将新节点插到第一个位置
 */
template <typename T>
Node<T>* DoubleLink<T>::insert_front(T value) {
    Node<T>* newNode = new Node<int>(value, phead_, phead_->next_ptr);
    phead_->next_ptr->pre_ptr = newNode;
    phead_->next_ptr = newNode;
    count_++;
    return newNode;
}

/*
 *将新节点插到链表尾部
 */
template <typename T>
Node<T>* DoubleLink<T>::insert_last(T value) {
    Node<T>* newNode = new Node<int>(value, phead_->pre_ptr, phead_);
    phead_->pre_ptr->next_ptr = newNode;
    phead_->pre_ptr = newNode;
    count_++;
    return newNode;
}

/*
 *将节点位置插到index位置之前
 */
template <typename T>
Node<T>* DoubleLink<T>::insert(int index, T value) {
    if (index == 0) return insert_front(value);

    Node<T>* pNode = getNode(index);
    if (pNode == nullptr) return nullptr;
    Node<T>* newNode = new Node<T>(value, pNode->pre_ptr, pNode);
    pNode->pre_ptr->next_ptr = newNode;
    pNode->pre_ptr = newNode;
    count_++;

    return newNode;
}

/*
 *删除链表第一个节点
 *返回删除后链表第一个节点
 */
template <typename T>
Node<T>* DoubleLink<T>::delete_front() {
    if (count_ == 0)  //空树，返回nullptr
    {
        return nullptr;
    }
    Node<T>* pnode = phead_->next_ptr;
    phead_->next_ptr = pnode->next_ptr;
    pnode->next_ptr->pre_ptr = phead_;
    delete pnode;
    count_--;
    return phead_->next_ptr;
}

/*
 *删除链表的末尾节点
 *返回删除后链表尾部元素
 */
template <typename T>
Node<T>* DoubleLink<T>::delete_last() {
    if (count_ == 0) {
        return nullptr;
    }
    Node<T>* pnode = phead_->pre_ptr;
    pnode->pre_ptr->next_ptr = phead_;
    phead_->pre_ptr = pnode->pre_ptr;
    delete pnode;
    count_--;
    return phead_->pre_ptr;
}

/*
 *删除指定位置的元素
 */
template <typename T>
Node<T>* DoubleLink<T>::del(int index) {
    if (index == 0) return delete_front();
    if (index == count_ - 1) return delete_last();
    if (index >= count_) return nullptr;
    Node<T>* pnode = getNode(index);
    pnode->pre_ptr->next_ptr = pnode->next_ptr;
    pnode->next_ptr->pre_ptr = pnode->pre_ptr;

    Node<T>* ptemp = pnode->pre_ptr;
    delete pnode;
    count_--;
    return ptemp;
}

template <typename T>
bool DoubleLink<T>::isEmpty() {
    return count_ == 0;
}

/*
 *获取第一个节点的值
 */
template <typename T>
T DoubleLink<T>::get_front() {
    return phead_->next_ptr->value;
}

/*
 *获取最后一个节点的值
 */
template <typename T>
T DoubleLink<T>::get_last() {
    return phead_->pre_ptr->value;
}

/*
 *获取指定位置节点的值
 */
template <typename T>
T DoubleLink<T>::get(int index) {
    Node<T> pnode = getNode(index);
    return pnode->value;
}

#endif
