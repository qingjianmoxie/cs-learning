#ifndef _MY_LIST_H_
#define _MY_LIST_H_

typedef int ElementType;

typedef struct _node
{
    ElementType Data;
    struct _node *Next;
} Node;

typedef struct _list
{
    Node *head;
} List;

// 在链表尾部添加数据
void add(List *pList, ElementType number);
// 打印链表所有数据
void print(List *pList);
// 求表长
int Length(List *pList);
// 按序号查找
Node *FindKth(List *pList, int K);
// 按值查找
void Find(List *pList, ElementType number);
// 插入
void Insert(List *pList, ElementType number, int i);
// 按序号删除
void DeleteKth(List *pList, int K);
// 按值删除
void Delete(List *pList, ElementType number);

#endif