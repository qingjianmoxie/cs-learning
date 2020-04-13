## 目的

在MySQL源码中，MEM_ROOT数据结构广泛应用在各个子系统和处理过程中，几乎无处不在。MEM_ROOT数据结构及相关处理方法，主要用于维护一些分配的内存空间，提高分配相同大小和类型的内存空间的效率。

## 数据结构

MEM_ROOT的数据结构定义在mysql源码的/include/my_alloc.h文件中。其中，MEM_ROOT中使用了一个重要数据结构：USED_MEM，该结构是已分配的内存块对象。

```c++
typedef struct st_used_mem
{				   /* struct for once_alloc (block) */
  struct st_used_mem *next;	   /* Next block in use */
  size_t left;                     /* memory left in block  */
  size_t size;                     /* size of block */
} USED_MEM;


typedef struct st_mem_root
{
  USED_MEM *free;                  /* blocks with free memory in it (free block link list的链表头指针) */
  USED_MEM *used;                  /* blocks almost without free memory (used block link list的链表头指针) */
  USED_MEM *pre_alloc;             /* preallocated block (预先分配的block) */
  /* if block have less memory it will be put in 'used' list */
  size_t min_malloc;               /* 如果block剩下的可用空间小于该值，将会从free list移动到used list */
  size_t block_size;               /* initial block size (每次初始化的空间大小) */
  unsigned int block_num;          /* allocated blocks counter (记录实际的block数量,初始化为4) */
  /* 
     first free block in queue test counter (if it exceed 
     MAX_BLOCK_USAGE_BEFORE_DROP block will be dropped in 'used' list)
  */
  unsigned int first_block_usage;  /* free list中的第一个block 测试不满足分配空间大小的次数 */

  void (*error_handler)(void);     /* 分配失败的错误处理函数 */
} MEM_ROOT;
```

首先，分析一下USED_MEM数据结构，该结构包含三个参数：next表示下一个USED_MEM对象，是一个链式结构；left表示当前内存块中剩余的内存空间；size表示内存块分配的大小。通过USED_MEM数据结构，就可以将分配的内存块连接成一个链表，供内存分配使用。

MEM_ROOT数据结构定义了三个USED_MEM指针，其中free表示有空闲内存的内存块；
used表示几乎没有空闲内存块，之所以是几乎没有，是因为当first_block_usage大于10，并且内存剩余的内存大小left小于4096时，free列表中的内存块就会被插入used列表；
pre_alloc表示预分配的内存块列表。
此外，MEM_ROOT定义了最小分配的内存大小min_malloc；初始化内存块的大小block_size；内存块数block_num；之前提到的first_block_usage参数，用于判断free内存块测试次数；以及错误处理函数指针，用于处理内存分配出错时的处理过程。

## 源码实现

源码实现核心处理函数包括MEM_ROOT的初始化函数`init_alloc_root()`，内存分配函数`alloc_root()`，内存释放函数`free_root()`。具体的实现代码在mysql源码的/mysys/my_alloc.c文件中，以下内容，主要对源码的逻辑进行分析。

### init_alloc_root()函数

`init_alloc_root()`初始化函数，主要对MEM_ROOT的参数进行初始化，默认设置参数min_malloc的值为32，参数block_num的值为4，参数block_size的值为输入参数block_size的大小减去常量值ALLOC_ROOT_MIN_BLOCK_SIZE（该常量包含USED_MEM的大小、常量MALLOC_OVERHEAD值为8，以及最小block的大小8）。以下是pre_alloc_size为0，init_alloc_root()函数的初始化状态。

| | |
| :- | :-
| USED_MEM *free | 0
| USED_MEM *used | 0
| USED_MEM *pre_alloc | 0
| size_t min_malloc | 32
| size_t block_size | block_size-8-sizeof(USED_MEM)-8
| unsigned int block_num | 4
| unsigned int first_block_usage | 0
| void (*error_handler)(void) | 0

图1 pre_alloc_size = 0

特别注意的，如果输入参数pre_alloc_size的值不为0，那么预分配内存空间，并将pre_malloc和free指针指向该内存空间。pre_alloc_size是需要的内存大小，而实际分配的内存空间大小为pre_alloc_size + ALIGN_SIZE(sizeof(USED_MEM))，因此free指针中的参数size的值为内存实际分配的大小，参数left的值为pre_alloc_size的值。pre_alloc_size不为0情况下，init_alloc_root()函数的初始化状态。

![图2 pre_alloc_size不为0](http://blog.chinaunix.net/attachment/201211/17/26896862_1353151884d1AZ.jpg)
图2 pre_alloc_size不为0

init_alloc_root()代码如下
```c++
/*
  Initialize memory root

  SYNOPSIS
    init_alloc_root()
      mem_root       - memory root to initialize
      block_size     - size of chunks (blocks) used for memory allocation
                       (It is external size of chunk i.e. it should include
                        memory required for internal structures, thus it
                        should be no less than ALLOC_ROOT_MIN_BLOCK_SIZE)
      pre_alloc_size - if non-0, then size of block that should be
                       pre-allocated during memory root initialization.
      my_flags	       MY_THREAD_SPECIFIC flag for my_malloc

  DESCRIPTION
    This function prepares memory root for further use, sets initial size of
    chunk for memory allocation and pre-allocates first block if specified.
    Altough error can happen during execution of this function if
    pre_alloc_size is non-0 it won't be reported. Instead it will be
    reported as error in first alloc_root() on this memory root.

    We don't want to change the structure size for MEM_ROOT.
    Because of this, we store in MY_THREAD_SPECIFIC as bit 1 in block_size
*/

void init_alloc_root(MEM_ROOT *mem_root, size_t block_size,
		     size_t pre_alloc_size __attribute__((unused)),
                     myf my_flags)
{
  DBUG_ENTER("init_alloc_root");
  DBUG_PRINT("enter",("root: 0x%lx", (long) mem_root));

  mem_root->free= mem_root->used= mem_root->pre_alloc= 0;
  mem_root->min_malloc= 32;
  mem_root->block_size= (block_size - ALLOC_ROOT_MIN_BLOCK_SIZE) & ~1;  // & ~1的作用是使二进制数的最低位为0
  if (MY_TEST(my_flags & MY_THREAD_SPECIFIC))
    mem_root->block_size|= 1;

  mem_root->error_handler= 0;
  mem_root->block_num= 4;			/* We shift this with >>2 */
  mem_root->first_block_usage= 0;

#if !(defined(HAVE_valgrind) && defined(EXTRA_DEBUG))
  if (pre_alloc_size)
  {
    if ((mem_root->free= mem_root->pre_alloc=
	 (USED_MEM*) my_malloc(pre_alloc_size+ ALIGN_SIZE(sizeof(USED_MEM)),
			       MYF(my_flags))))
    {
      mem_root->free->size= pre_alloc_size+ALIGN_SIZE(sizeof(USED_MEM));
      mem_root->free->left= pre_alloc_size;
      mem_root->free->next= 0;
    }
  }
#endif
  DBUG_VOID_RETURN;
}
```

### alloc_root()函数

`alloc_root()`函数式MEM_ROOT最核心的函数，用于内存的分配。该函数的处理逻辑为：首先查看free内存列表中，是否有符合分配长度的block块。如果不存在，那么重新申请内存空间，生成一个新的block块。否则，从找到的block块上分配内存空间，并返回当前block剩余的首地址，并修改left值。如果分配后，该block块的值小于参数min_malloc的值，那么将该block添加到used内存列表中。特别注意的是，在查看free内存列表时，如果free中第一个block的first_block_usage值大于10次（即第一个block块查找分配失败次数大于10次），并且left的空间小于4096时，将该block添加到used列表中。

具体处理逻辑的简化流程图如下所示：

![图3 alloc_root()流程图](http://blog.chinaunix.net/attachment/201211/17/26896862_135315191658RM.jpg)
图3 alloc_root()流程图

alloc_root()代码如下:
```c++
void *alloc_root(MEM_ROOT *mem_root, size_t length)
{
  size_t get_size, block_size;
  uchar* point;
  reg1 USED_MEM *next= 0;
  reg2 USED_MEM **prev;
  DBUG_ENTER("alloc_root");
  DBUG_PRINT("enter",("root: 0x%lx", (long) mem_root));
  DBUG_ASSERT(alloc_root_inited(mem_root));

  DBUG_EXECUTE_IF("simulate_out_of_memory",
                  {
                    /* Avoid reusing an already allocated block */
                    if (mem_root->error_handler)
                      (*mem_root->error_handler)();
                    DBUG_SET("-d,simulate_out_of_memory");
                    DBUG_RETURN((void*) 0); /* purecov: inspected */
                  });
  length= ALIGN_SIZE(length);
  if ((*(prev= &mem_root->free)) != NULL)
  {
    if ((*prev)->left < length &&
	mem_root->first_block_usage++ >= ALLOC_MAX_BLOCK_USAGE_BEFORE_DROP &&
	(*prev)->left < ALLOC_MAX_BLOCK_TO_DROP)
    {
      next= *prev;
      *prev= next->next;			/* Remove block from list */
      next->next= mem_root->used;
      mem_root->used= next;
      mem_root->first_block_usage= 0;
    }
    for (next= *prev ; next && next->left < length ; next= next->next)
      prev= &next->next;
  }
  if (! next)
  {						/* Time to alloc new block */
    block_size= (mem_root->block_size & ~1) * (mem_root->block_num >> 2);
    get_size= length+ALIGN_SIZE(sizeof(USED_MEM));
    get_size= MY_MAX(get_size, block_size);

    if (!(next = (USED_MEM*) my_malloc(get_size,
                                       MYF(MY_WME | ME_FATALERROR |
                                           MALLOC_FLAG(mem_root->
                                                       block_size)))))
    {
      if (mem_root->error_handler)
	(*mem_root->error_handler)();
      DBUG_RETURN((void*) 0);                      /* purecov: inspected */
    }
    mem_root->block_num++;
    next->next= *prev;
    next->size= get_size;
    /* bug:如果该block是通过mem_root->block_size * (mem_root->block_num >> 2)计算出来的，则已经去掉了ALIGN_SIZE(sizeof(USED_MEM)，这里重复了。 */
    next->left= get_size-ALIGN_SIZE(sizeof(USED_MEM));
    *prev=next;
  }

  point= (uchar*) ((char*) next+ (next->size-next->left));
  /*TODO: next part may be unneded due to mem_root->first_block_usage counter*/
  if ((next->left-= length) < mem_root->min_malloc)
  {						/* Full block */
    *prev= next->next;				/* Remove block from list */
    next->next= mem_root->used;
    mem_root->used= next;
    mem_root->first_block_usage= 0;
  }
  TRASH_ALLOC(point, length);
  DBUG_PRINT("exit",("ptr: 0x%lx", (ulong) point));
  DBUG_RETURN((void*) point);
}
```

### free_root()函数

`free_root()`函数主要用于释放分配的内存空间。但是该函数根据输入不同的标志，用于释放内存的方式有些不同。该函数的处理逻辑为：如果输入的标志仅仅用于标示该内存空间为释放，而不是真正的释放内存，那么调用mark_blocks_free()函数标记所有的空间已经释放，以便于重用。否则，如果输入的标志不需要保留预分配的内存空间，那么预分配空间置为空，并分别释放used和free列表中的block内存列表。

具体处理逻辑的简化流程图如图4所示：

![图4 free_root()流程图](http://blog.chinaunix.net/attachment/201211/17/26896862_135315193799k9.jpg) 
图4 free_root()流程图

其中mark_blocks_free()函数的处理逻辑较为简单，仅仅是将free列表中的每个block的left值设置为block块的实际可用的内存大小，并将used列表中的block添加到free列表中，执行相同的操作。而free过程在流程图中用函数free代替，该处理代码是对链表的处理，需要特别注意的是，在释放之前需要判断是否是pre_alloc的内存空间。如果是pre_alloc指向的内存空间，不释放该block的内存空间。

free_root()代码如下:
```c++
/*
  Deallocate everything used by alloc_root or just move
  used blocks to free list if called with MY_USED_TO_FREE

  SYNOPSIS
    free_root()
      root		Memory root
      MyFlags		Flags for what should be freed:

        MY_MARK_BLOCKS_FREED	Don't free blocks, just mark them free
        MY_KEEP_PREALLOC	If this is not set, then free also the
        		        preallocated block

  NOTES
    One can call this function either with root block initialised with
    init_alloc_root() or with a bzero()-ed block.
    It's also safe to call this multiple times with the same mem_root.
*/

void free_root(MEM_ROOT *root, myf MyFlags)
{
  reg1 USED_MEM *next,*old;
  DBUG_ENTER("free_root");
  DBUG_PRINT("enter",("root: 0x%lx  flags: %u", (long) root, (uint) MyFlags));

  if (MyFlags & MY_MARK_BLOCKS_FREE)
  {
    mark_blocks_free(root);
    DBUG_VOID_RETURN;
  }
  if (!(MyFlags & MY_KEEP_PREALLOC))
    root->pre_alloc=0;

  for (next=root->used; next ;)
  {
    old=next; next= next->next ;
    if (old != root->pre_alloc)
      my_free(old);
  }
  for (next=root->free ; next ;)
  {
    old=next; next= next->next;
    if (old != root->pre_alloc)
      my_free(old);
  }
  root->used=root->free=0;
  if (root->pre_alloc)
  {
    root->free=root->pre_alloc;
    root->free->left=root->pre_alloc->size-ALIGN_SIZE(sizeof(USED_MEM));
    TRASH_MEM(root->pre_alloc);
    root->free->next=0;
  }
  root->block_num= 4;
  root->first_block_usage= 0;
  DBUG_VOID_RETURN;
}
```

## 结论

通过以上分析MEM_ROOT数据结构及相关处理操作。可以发现，这种内存分配管理方式，在free_root()时，将内存空间标记为释放，可以使得内存空间重用，提高重复使用相同大小或类型的内存空间的效率，而降低真正内存分配的时间代价。
MEM_ROOT的内存分配采用的是启发式分配算法，随着后续block的数量越多，单个block的内存也会越大: block_size= mem_root->block_size * (mem_root->block_num >> 2).

## 文章来源

[MySQL数据结构分析--MEM_ROOT](http://blog.chinaunix.net/uid-26896862-id-3412033.html)