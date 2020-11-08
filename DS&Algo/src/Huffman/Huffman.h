#ifndef _HUFFMAN_TREE_HPP_
#define _HUFFMAN_TREE_HPP_

#include <algorithm>
#include <deque>
#include <iostream>
using namespace std;
/*哈夫曼树的节点定义*/
template <typename T>
struct HuffmanNode {
    HuffmanNode(T k, HuffmanNode<T>* l = nullptr, HuffmanNode<T>* r = nullptr)
        : key(k), lchild(l), rchild(r) {}
    ~HuffmanNode(){};

    T key;                   // 节点的值
    HuffmanNode<T>* lchild;  // 节点左孩
    HuffmanNode<T>* rchild;  // 节点右孩
};
template <typename T>
class Huffman {
public:
    Huffman();
    ~Huffman();

    void preOrder();   // 前序遍历哈夫曼树
    void inOrder();    // 中序遍历哈夫曼树
    void postOrder();  // 后序遍历哈夫曼树

    void creat(T a[], int size);  // 创建哈夫曼树
    void destory();               // 销毁哈夫曼树
    void print();                 // 打印哈夫曼树

private:
    void preOrder(HuffmanNode<T>* pnode);
    void inOrder(HuffmanNode<T>* pnode);
    void postOrder(HuffmanNode<T>* pnode);
    void print(HuffmanNode<T>* pnode);
    void destroy(HuffmanNode<T>* pnode);

private:
    HuffmanNode<T>* root;
    deque<HuffmanNode<T>*> forest;  // 森林, 这里使用deque来存储森林中树的根节点
};

/*默认构造函数*/
template <typename T>
Huffman<T>::Huffman() : root(nullptr){};

/*析构函数*/
template <typename T>
Huffman<T>::~Huffman() {
    destroy(root);
};

/*********************
 * 创建哈夫曼树
 * 这里仅仅示范构建哈夫曼树的过程，没有经过性能上的优化和完善的异常处理。
 * 这里使用deque双端队列来存储森林中树根节点，使用库函数sort来对根节点依权值排序。
 * 这里也可以使用我们之前介绍的小顶堆来完成工作。
 ********************/
template <typename T>
void Huffman<T>::creat(T a[], int size) {
    for (int i = 0; i < size; i++)  //每个节点都作为一个森林
    {
        // 为初始序列的元素构建节点。每个节点作为一棵树加入森林中
        HuffmanNode<T>* ptr = new HuffmanNode<T>(a[i], nullptr, nullptr);
        forest.push_back(ptr);
    }
    for (int i = 0; i < size - 1; i++) {
        // 排序，以选出根节点权值最小两棵树
        sort(forest.begin(), forest.end(), [](HuffmanNode<T>* a, HuffmanNode<T>* b) { return a->key < b->key; });
        // 构建新节点
        HuffmanNode<T>* node = new HuffmanNode<T>(forest[0]->key + forest[1]->key, forest[0], forest[1]);
        // 新节点加入森林中
        forest.push_back(node);
        // 删除两棵权值最小的树
        forest.pop_front();
        forest.pop_front();
    }
    root = forest.front();
    forest.clear();
};

/*前序遍历哈夫曼树*/
template <typename T>
void Huffman<T>::preOrder(HuffmanNode<T>* pnode) {
    if (pnode != nullptr) {
        cout << pnode->key;
        preOrder(pnode->lchild);
        preOrder(pnode->rchild);
    }
};
template <typename T>
void Huffman<T>::preOrder() {
    preOrder(root);
};
/*中序遍历哈夫曼树*/
template <typename T>
void Huffman<T>::inOrder(HuffmanNode<T>* pnode) {
    if (pnode != nullptr) {
        inOrder(pnode->lchild);
        cout << pnode->key;
        inOrder(pnode->rchild);
    }
};
template <typename T>
void Huffman<T>::inOrder() {
    inOrder(root);
};
/*后序遍历哈夫曼树*/
template <typename T>
void Huffman<T>::postOrder(HuffmanNode<T>* pnode) {
    if (pnode != nullptr) {
        postOrder(pnode->lchild);
        postOrder(pnode->rchild);
        cout << pnode->key;
    }
};
template <typename T>
void Huffman<T>::postOrder() {
    postOrder(root);
};

/*打印哈夫曼树*/
template <typename T>
void Huffman<T>::print(HuffmanNode<T>* pnode) {
    if (pnode != nullptr) {
        cout << "当前结点：" << pnode->key << ".";
        if (pnode->lchild != nullptr)
            cout << "它的左孩子节点为：" << pnode->lchild->key << ".";
        else
            cout << "它没有左孩子.";
        if (pnode->rchild != nullptr)
            cout << "它的右孩子节点为：" << pnode->rchild->key << ".";
        else
            cout << "它没有右孩子.";
        cout << endl;
        print(pnode->lchild);
        print(pnode->rchild);
    }
};
template <typename T>
void Huffman<T>::print() {
    print(root);
};
/*销毁哈夫曼树*/
template <typename T>
void Huffman<T>::destory() {
    destroy(root);
};
template <typename T>
void Huffman<T>::destroy(HuffmanNode<T>* pnode) {
    if (pnode != nullptr) {
        destroy(pnode->lchild);
        destroy(pnode->rchild);
        delete pnode;
        pnode = nullptr;
    }
};

#endif