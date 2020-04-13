## 目的

在MySQL的SQL层源码中，链表(Linked list)数据结构是使用最频繁、应用最广泛的结构之一，是SQL各个数据结构的基础结构。本文分析MySQL在SQL层的所有链表结构，以便于分析SQL层其他复杂数据结构。

## 数据结构

MySQL的SQL层基本是C++代码，因此数据结构不是单纯的存储结构，而会涉及到class的继承。链表数据结构的源码在MySQL源码的/sql/sql_list.cc、/sql/sql_list.h，包含所有与链表相关的数据结构和操作。

在SQL层，MySQL的链表结构包括两类：单项链表结构和双向链表结构。以下分别对这两种链表进行分析：

**单向链表**：base_list是链表的头结点，first和last分别指向链表的第一个和最后一个结点，elements表示链表中的结点个数；
list_node是链表中的结点，next指向下一个结点的地址，info指针指向存储数据的具体地址。

![图1 单向链表结构](http://blog.chinaunix.net/attachment/201301/22/26896862_13588291054Kgg.jpg)
图1 单向链表结构

list_node代码如下：
```c++
/*
  Basic single linked list
  Used for item and item_buffs.
  All list ends with a pointer to the 'end_of_list' element, which
  data pointer is a null pointer and the next pointer points to itself.
  This makes it very fast to traverse lists as we don't have to
  test for a specialend condition for list that can't contain a null
  pointer.
*/

/**
  list_node - a node of a single-linked list.
  @note We never call a destructor(析构函数) for instances of this class.
*/

struct list_node :public Sql_alloc
{
  list_node *next;
  void *info;
  list_node(void *info_par,list_node *next_par)
    :next(next_par),info(info_par)
  {}
  list_node()					/* For end_of_list */
  {
    info= 0;
    next= this;
  }
};
```

**双向链表**：base_ilist是双向链表的头结点，first和last指针分别指向双向链表的第一个和最后一个结点；
ilink是双向链表中结点的数据结构，prev指针指向结点的前驱结点，next指向当前结点的下一个结点。
与单向链表不同的是：base_ilist头结点中没有表示链表中数据结点个数的变量，这与ilink结点的结构相关；ilink结点中没有指向存储数据的结点指针，这是考虑到该结构可以链接任意数据结构，具体数据结构类型和数据的内容，双向链表都不关心，这也解释了头结点中没有必要记录数据结点的个数。

![图2 双向链表结构](http://blog.chinaunix.net/attachment/201301/22/26896862_13588291176S84.jpg)
图2 双向链表结构

ilink代码如下所示：
```c++
/*
  A simple intrusive list which automaticly removes element from list
  on delete (for THD element)
*/

struct ilink
{
  struct ilink **prev,*next;
  static void *operator new(size_t size) throw ()
  {
    return (void*)my_malloc((uint)size, MYF(MY_WME | MY_FAE | ME_FATALERROR));
  }
  static void operator delete(void* ptr_arg, size_t size)
  {
     my_free(ptr_arg);
  }

  inline ilink()
  {
    prev=0; next=0;
  }
  inline void unlink()
  {
    /* Extra tests because element doesn't have to be linked */
    if (prev) *prev= next;
    if (next) next->prev=prev;
    prev=0 ; next=0;
  }
  virtual ~ilink() { unlink(); }		/*lint -e1740 */
};
```

在实际的实现中，为了适用于不同数据结构，链表结构设计并没有那么简单。以下是整个单向链表数据结构的UML设计图，其中：
Sql_alloc数据结构定义了内存分配相关的操作；
```c++
/* mysql standard class memory allocator */

class Sql_alloc
{
public:
  static void *operator new(size_t size) throw ()
  {
    return sql_alloc(size);
  }
  static void *operator new[](size_t size) throw ()
  {
    return sql_alloc(size);
  }
  static void *operator new[](size_t size, MEM_ROOT *mem_root) throw ()
  { return alloc_root(mem_root, size); }
  static void *operator new(size_t size, MEM_ROOT *mem_root) throw ()
  { return alloc_root(mem_root, size); }
  static void operator delete(void *ptr, size_t size) { TRASH(ptr, size); }
  static void operator delete(void *ptr, MEM_ROOT *mem_root)
  { /* never called */ }
  static void operator delete[](void *ptr, MEM_ROOT *mem_root)
  { /* never called */ }
  static void operator delete[](void *ptr, size_t size) { TRASH(ptr, size); }
#ifdef HAVE_valgrind
  bool dummy;
  inline Sql_alloc() :dummy(0) {}
  inline ~Sql_alloc() {}
#else
  inline Sql_alloc() {}
  inline ~Sql_alloc() {}
#endif
};
```
SQL_I_List是SQL相关的链表结构，该结构是泛型类型，应用于TABLE_LIST、ORDER等数据结构；
list_node是struct结构，是链表的结点；
base_list是链表的头结点，并且该结点与base_list_iterator和error_list_iterator数据结构是友元关系（friend）；
base_list_iterator是base_list结构的迭代器，用于遍历base_list中的所有数据结点；
List类继承base_list，对外提供服务，主要应用在Item、Item_field、field、handler以及存储引擎接口等结构中；
而List_iterator和List_iterator_fast继承base_list_iterator，实现了两种遍历的List的不同方式，具体的差异是List_iterator_fast增加了sublist方法，并且将replace、remove、after以及ref函数的可见性定义为protected，具体的实现，由子类重写实现。

![图3 单向链表UML设计图](http://blog.chinaunix.net/attachment/201301/22/26896862_1358829069e0NO.jpg)
图3 单向链表UML设计图

双向链表中，结点ilink派生了两种数据类型，i_string用于存储字符类型，i_string_pair用于存储key/value结构的字符类型。这两种结构主要用于满足字符或字符对存储的要求，其中i_string广泛应用在各个结构或处理中，而i_string_pair主要应用在复制过滤处理中。具体的UML设计图如下所示：

![图4 ilink的UML设计图](http://blog.chinaunix.net/attachment/201301/22/26896862_13588291379qKp.jpg)
图4 ilink的UML设计图

双向链表的设计如下所示，其中：
base_ilist是表头结构，与base_ilist_iterator是友元关系；
base_ilist_iterator是遍历base_ilist的迭代器；
I_List是双向链表对外提供服务的结构，广泛应用在keycache、复制过滤、查询等子系统中，与I_List_iterator是友元关系；
I_List_iterator继承自base_ilist_iterator，是I_List的迭代器。具体如下所示：

![图5 双向链表UML设计图](http://blog.chinaunix.net/attachment/201301/22/26896862_1358829153n94j.jpg)
图5 双向链表UML设计图

## 结论

通过对MySQL的SQL层的链表结构进行分析，作为SQL层的基础数据结构，提供了单项链表和双向链表两种结构，以及丰富的基本操作，为SQL层的复杂数据结构提供基础服务。

## 文章来源

[MySQL数据结构分析—sql_list](http://blog.chinaunix.net/uid-26896862-id-3479523.html)
