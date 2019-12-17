#include <stdio.h>
#include <stdlib.h>
#include "my_list.h"

int main()
{
    List list;
    list.head = NULL;
    add(&list, 11);
    add(&list, 22);
    add(&list, 33);
    print(&list);
    printf("表长为%d\n", Length(&list));
    FindKth(&list, 1);
    Find(&list, 11);
    Insert(&list, 44, 3);
    print(&list);
    DeleteKth(&list,3);
    print(&list);
    Delete(&list,33);
    return 0;
}