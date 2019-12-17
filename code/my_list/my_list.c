#include <stdio.h>
#include <stdlib.h>
#include "my_list.h"

// 在链表尾部添加数据
void add(List *pList, ElementType number)
{
    Node *p = (Node *)malloc(sizeof(Node));
    p->Data = number;
    p->Next = NULL;
    // find the last
    Node *last = pList->head;
    if (last)
    {
        while (last->Next)
        {
            last = last->Next;
        }
        // attach
        last->Next = p;
    }
    else
    {
        pList->head = p;
    }
}

// 打印链表所有数据
void print(List *pList)
{
    for (Node *p = pList->head; p; p = p->Next)
    {
        printf("%d\t", p->Data);
    }
    printf("\n");
}

// 求表长
int Length(List *pList)
{
    Node *p = pList->head;
    int j = 0;
    while (p)
    {
        p = p->Next;
        j++;
    }
    return j;
}

// 按序号查找
Node *FindKth(List *pList, int K)
{
    Node *p = pList->head;
    int i = 1;
    while (p && i < K)
    {
        p = p->Next;
        i++;
    }
    if (K == i)
        printf("第%d个结点值为%d\n", K, p->Data);
    else
        printf("第%d个结点不存在\n", K);
    return p;
}

// 按值查找
void Find(List *pList, ElementType number)
{
    Node *p = pList->head;
    int i = 1;
    while (p && p->Data != number)
    {
        p = p->Next;
        i++;
    }
    if (p)
        printf("找到了，是第%d个结点\n", i);
    else
        printf("该值在链表中不存在\n");
}

// 插入
void Insert(List *pList, ElementType number, int i)
{
    Node *p = pList->head;
    Node *tmp;
    Node *pre;
    if (1 == i) // 新结点插入在表头
    {
        tmp = (Node *)malloc(sizeof(Node)); /* 申请、填装结点 */
        tmp->Data = number;
        tmp->Next = pList->head;
    }
    pre = FindKth(pList, i - 1); // 查找第i-1个结点
    if (NULL == pre)             /* 第i-1个结点不存在 */
        printf("插入位置参数错误\n");
    else
    {
        tmp = (Node *)malloc(sizeof(Node)); /* 申请、填装结点 */
        tmp->Data = number;
        tmp->Next = pre->Next;
        pre->Next = tmp;
    }
}

// 按序号删除
void DeleteKth(List *pList, int K)
{
    Node *p = pList->head;
    if (NULL == p)
        printf("链表不存在\n");
    Node *tmp;
    Node *pre;
    if (1 == K)
    {
        tmp = p;
        p = p->Next;
        free(tmp);
    }
    pre = FindKth(pList, K - 1);
    if (NULL == pre)
        printf("第%d个结点不存在\n", K - 1);
    else if (NULL == pre->Next)
        printf("第%d个结点不存在", K);
    else
    {
        tmp = pre->Next;
        pre->Next = tmp->Next;
        free(tmp);
    }
}

// 按值删除
void Delete(List *pList, ElementType number)
{
    Node *q = NULL;
    Node *p = pList->head;
    for (; p; q = p, p = p->Next)
    {
        if (number == p->Data)
        {
            if (q)
                q->Next = p->Next;
            else
                pList->head = p->Next;
            free(p);
            printf("删除成功\n");
            break;
        }
    }
}