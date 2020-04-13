## 目的

对于数据库来说，文件读写操作异常频繁，需要借助cache提高IO读写的效率。在MySQL源码中，数据结构IO_CACHE及其相关处理函数，正是提供了对于文件IO的CACHE读写策略。其中IO_CACHE_SHARE数据结构，提供了共享CACHE的读写策略，用于多线程共享读取CACHE。

## 数据结构

IO_CACHE数据结构的定义在MySQL源码的/include/my_sys.h文件中，该数据结构的定义较复杂，具体定义如下所示：

### cache_type

cache_type枚举类型定义了6种cache的操作类型，包括读、写、顺序读、队列读、网络读、网络写。cache根据操作类型的不同，处理的方式和策略也不同。
```c++
enum cache_type
{
  TYPE_NOT_SET= 0, READ_CACHE, WRITE_CACHE,
  SEQ_READ_APPEND		/* sequential read or append */,
  READ_FIFO, READ_NET,WRITE_NET};
```

### IO_CACHE_CALLBACK

IO_CACHE_CALLBACK是一个回调函数，输入参数是IO_CACHE数据类型。在IO_CACHE数据结构中，定义了三个回调函数，详细内容在以下介绍。

### IO_CACHE_SHARE

IO_CACHE_SHARE数据结构是用于多线程共享读cache的情况下，cache共享的处理策略。具体各个参数的含义，注释中都详细介绍。
```c++
typedef struct st_io_cache_share
{
  mysql_mutex_t       mutex;           /* To sync on reads into buffer. */
  mysql_cond_t        cond;            /* To wait for signals. */
  mysql_cond_t        cond_writer;     /* For a synchronized writer. */
  /* Offset in file corresponding to the first byte of buffer. */
  my_off_t              pos_in_file;
  /* If a synchronized write cache is the source of the data. */
  struct st_io_cache    *source_cache;
  uchar                 *buffer;         /* The read buffer. */
  uchar                 *read_end;       /* Behind last valid byte of buffer. */
  int                   running_threads; /* threads not in lock. */
  int                   total_threads;   /* threads sharing the cache. */
  int                   error;           /* Last error. */
#ifdef NOT_YET_IMPLEMENTED
  /* whether the structure should be free'd */
  my_bool alloced;
#endif
} IO_CACHE_SHARE;
```

### IO_CACHE

IO_CACHE数据结构是包含了cache文件处理的各种参数和读写处理函数。具体参数的含义，参考注释。进一步的了解，查看以下源码中处理函数的逻辑和流程图。
```c++
typedef struct st_io_cache		/* Used when cacheing files */
{
  /* Offset in file corresponding to the first byte of uchar* buffer. */
  my_off_t pos_in_file;
  /*
    The offset of end of file for READ_CACHE and WRITE_CACHE.
    For SEQ_READ_APPEND it the maximum of the actual end of file and
    the position represented by read_end.
  */
  my_off_t end_of_file;
  /* Points to current read position in the buffer */
  uchar	*read_pos;
  /* the non-inclusive boundary in the buffer for the currently valid read */
  uchar  *read_end;
  uchar  *buffer;				/* The read buffer */
  /* Used in ASYNC_IO */
  uchar  *request_pos;

  /* Only used in WRITE caches and in SEQ_READ_APPEND to buffer writes */
  uchar  *write_buffer;
  /*
    Only used in SEQ_READ_APPEND, and points to the current read position
    in the write buffer. Note that reads in SEQ_READ_APPEND caches can
    happen from both read buffer (uchar* buffer) and write buffer
    (uchar* write_buffer).
  */
  uchar *append_read_pos;
  /* Points to current write position in the write buffer */
  uchar *write_pos;
  /* The non-inclusive boundary of the valid write area */
  uchar *write_end;

  /*
    Current_pos and current_end are convenience variables used by
    my_b_tell() and other routines that need to know the current offset
    current_pos points to &write_pos, and current_end to &write_end in a
    WRITE_CACHE, and &read_pos and &read_end respectively otherwise
  */
  uchar  **current_pos, **current_end;

  /*
    The lock is for append buffer used in SEQ_READ_APPEND cache
    need mutex copying from append buffer to read buffer.
  */
  mysql_mutex_t append_buffer_lock;
  /*
    The following is used when several threads are reading the
    same file in parallel. They are synchronized on disk
    accesses reading the cached part of the file asynchronously.
    It should be set to NULL to disable the feature.  Only
    READ_CACHE mode is supported.
  */
  IO_CACHE_SHARE *share;

  /*
    A caller will use my_b_read() macro to read from the cache
    if the data is already in cache, it will be simply copied with
    memcpy() and internal variables will be accordinging updated with
    no functions invoked. However, if the data is not fully in the cache,
    my_b_read() will call read_function to fetch the data. read_function
    must never be invoked directly.
  */
  int (*read_function)(struct st_io_cache *,uchar *,size_t);
  /*
    Same idea as in the case of read_function, except my_b_write() needs to
    be replaced with my_b_append() for a SEQ_READ_APPEND cache
  */
  int (*write_function)(struct st_io_cache *,const uchar *,size_t);
  /*
    Specifies the type of the cache. Depending on the type of the cache
    certain operations might not be available and yield unpredicatable
    results. Details to be documented later
  */
  enum cache_type type;
  /*
    Callbacks when the actual read I/O happens. These were added and
    are currently used for binary logging of LOAD DATA INFILE - when a
    block is read from the file, we create a block create/append event, and
    when IO_CACHE is closed, we create an end event. These functions could,
    of course be used for other things
  */
  IO_CACHE_CALLBACK pre_read;
  IO_CACHE_CALLBACK post_read;
  IO_CACHE_CALLBACK pre_close;
  /*
    Counts the number of times, when we were forced to use disk. We use it to
    increase the binlog_cache_disk_use and binlog_stmt_cache_disk_use status
    variables.
  */
  ulong disk_writes;
  void* arg;				/* for use by pre/post_read */
  char *file_name;			/* if used with 'open_cached_file' */
  char *dir,*prefix;
  File file; /* file descriptor */
  /*
    seek_not_done is set by my_b_seek() to inform the upcoming read/write
    operation that a seek needs to be preformed prior to the actual I/O
    error is 0 if the cache operation was successful, -1 if there was a
    "hard" error, and the actual number of I/O-ed bytes if the read/write was
    partial.
  */
  int	seek_not_done,error;
  /* buffer_length is memory size allocated for buffer or write_buffer */
  size_t	buffer_length;
  /* read_length is the same as buffer_length except when we use async io */
  size_t  read_length;
  myf	myflags;			/* Flags used to my_read/my_write */
  /*
    alloced_buffer is 1 if the buffer was allocated by init_io_cache() and
    0 if it was supplied by the user.
    Currently READ_NET is the only one that will use a buffer allocated
    somewhere else
  */
  my_bool alloced_buffer;
#ifdef HAVE_AIOWAIT
  /*
    As inidicated by ifdef, this is for async I/O, which is not currently
    used (because it's not reliable on all systems)
  */
  uint inited;
  my_off_t aio_read_pos;
  my_aio_result aio_result;
#endif
} IO_CACHE;
```

## 源码实现

通过以上数据结构的定义可知，根据不同的操作类型，对cache的主要处理函数也不同，以下对cache的主要处理函数进行分析。源码文件参考mysys/mf_iochache.c。

### init_functions()函数

init_functions()函数用于根据不同的CACHE类型，初始化IO_CACHE的read_function和write_function函数。
如果CACHE类型为READ_NET，会根据实际的调用者进行初始化；
如果CACHE类型为SEQ_READ_APPEND，read_function函数为_my_b_seq_read()处理过程；
默认情况下，如果可以多线程共享读，那么read_function函数为多线程共享读函数_my_b_read_r()，否则为_my_b_read()处理函数。write_function函数为_my_b_write()处理函数。这些处理函数是IO_CACHE的主要处理逻辑，将在以下的内容中详细说明。

该函数在init_io_cache函数初始化IO_CACHE对象时调用，初始化read_function和write_function函数。

```c++
static void
init_functions(IO_CACHE* info)
{
  enum cache_type type= info->type;
  switch (type) {
  case READ_NET:
    /*
      Must be initialized by the caller. The problem is that
      _my_b_net_read has to be defined in sql directory because of
      the dependency on THD, and therefore cannot be visible to
      programs that link against mysys but know nothing about THD, such
      as myisamchk
    */
    break;
  case SEQ_READ_APPEND:
    info->read_function = _my_b_seq_read;
    info->write_function = 0;			/* Force a core if used */
    break;
  default:
    info->read_function = info->share ? _my_b_read_r : _my_b_read;
    info->write_function = _my_b_write;
  }

  setup_io_cache(info);
}
```

### init_io_cache()函数

init_io_cache()函数是IO_CACHE对象的初始化函数。具体初始化IO_CACHE的值如下图1所示。

![图1 init_io_cache()初始化](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849242233L.jpg)
图1 init_io_cache()初始化

根据输入的文件描述符file，确定seek_not_done的值，该值用于通知读写操作需要首先调用seek函数，查看当前文件位置。根据输入参数cachesize，用于初始化cache的大小，并进一步初始化buffer。然而，实际初始化过程中，该值并不一定是输入的值，init_io_cache()函数会根据不同cache类型和参数进行调整，具体确定cachesize值的处理逻辑如图2所示。输入参数seek_offset用于初始化读写CACHE的起始地址，该值也会影响cachesize的值。输入参数type用于初始化IO_CACHE的cache类型，cache的类型直接关系到IO_CACHE的初始化过程。输入参数use_async_io用于判断是否使用异步IO操作，该值将影响最小CACHE大小，如果使用异步IO，最小CACHE大小为`IO_SIZE*4(IO_SIZE=4096)`，否则为`IO_SIZE*2`。输入参数cache_myflags用于初始化cache的一些标志。

init_io_cache()函数的逻辑处理流程图如下图2所示，其中主要的思想是根据CACHE的类型和cachesize的值，初始化IO_CACHE的参数。

![图2 init_io_cache()流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849259964A.jpg)
图2 init_io_cache()流程图

特别注意的是，IO_CACHE初始化的大小不一定是输入参数cachesize的值，在内存分配过程中，会根据文件的大小和最小CACHE大小，以及内存分配成功与否，对cachesize进行调整。如果分配失败，将cachesize大小调整为之前的3/4大小，再次进行分配，直到等于最小的CACHE值，如果仍然分配失败，那么初始化失败。

### _my_b_read()函数

_my_b_read()函数是默认的非共享读函数read_function。该函数的基本思想是：首先读取当前CACHE中的内容到输入参数Buffer中；然后，如果当前读的数据长度大于IO_SIZE（4096）的值，那么直接从文件中读取（(Count & ~(IO_SIZE-1))-diff_length）长度是内容到Buffer中；最后，从文件中读取IO_CACHE能够存放的最大长度的内容到IO_CACHE的buffer中，然后从IO_CACHE的buffer中读取剩余长度的内容到Buffer中。

_my_b_read()函数的详细处理逻辑如下图3所示：

![图3 _my_b_read()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849285A3PT.jpg)
图3 _my_b_read()函数流程图

从以上处理逻辑来看，_my_b_read()函数对数据量相对较小的连续读取性能较高。因为读取少量数据时，IO_CACHE也会读取IO_CACHE所能容纳的最大的数据内容。因此，多次连续读取的数据会始终从IO_CACHE中获得。而对于数据较大的读取来说，IO_CACHE的读取策略就没有了优势。

### init_io_cache_share()函数

init_io_cache_share()函数是多线程操作IO_CACHE的初始化函数。该函数初始化IO_CACHE的成员变量share，share是IO_CACHE_SHARE数据类型。通过share变量，使得每个线程共享读取IO_CACHE的buffer内容，并且每个线程维护各自的buffer、错误以及buffer读取的偏移地址。此外，将IO_CACHE的read_function初始化为_my_b_read_r()多线程共享读函数。

初始化IO_CACHE_SHARE的初始化状态如下图4所示，而对IO_CACHE的初始化主要对读cache（输入参数read_cache）的share、current_pos、current_end以及read_function进行初始化。在调用init_io_cache_share之前，已经对read_cache和write_cache调用init_io_cache()函数进行初始化。

![图4 IO_CACHE_SHARE初始化状态](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849302bHd2.jpg)
图4 IO_CACHE_SHARE初始化状态

通过以上初始化可知，IO_CACHE_SHARE结构主要用于IO_CACHE的_my_b_read_r()函数进行多线程共享读CACHE中的内容。在分析多线程共享读IO_CACHE的处理函数_my_b_read_r()之前，对其处理逻辑中的锁处理函数：lock_io_cache()函数和unlock_io_cache()函数进行简要的分析。

### lock_io_cache()函数

lock_io_cache()函数式是多线程共享读时，对CACHE进行加锁控制。该函数的主要思想是：首先对CACHE加锁，并且被锁定的线程数减1。如果当前CACHE的share中source_cache是WRITE_CACHE，那么将当前WRITE_CACHE同步到READ_CACHE上。在同步之前，等待所有的读线程操作都结束，这时写线程被唤醒，并且线程持有锁。此时，新的读线程请求锁，都需要等待写线程释放锁；如果当前CACHE只有READ_CACHE，那么读线程等待获取读锁。读锁获取后，CACHE的锁释放，所有读线程都可以进行共享读操作。

lock_io_cache()函数的详细处理流程如图5所示。

![图5 lock_io_cache()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849637NA63.jpg)
图5 lock_io_cache()函数流程图

### unlock_io_cache()函数

unlock_io_cache()函数用于释放IO_CACHE上的锁，并重置IO_CACHE_SHARE的running_threads参数的值为total_threads。由于unlock_io_cache()函数释放锁的操作比较简单，因此不做分析。

### _my_b_read_r()函数

_my_b_read_r()函数是多线程共享读IO_CACHE的处理函数，该函数的处理逻辑类似_my_b_read()函数。首先，读取IO_CACHE的buffer中剩余的数据，并拷贝到Buffer中。如果当前读取的内容的长度Count大于0，需要继续从文件中读取数据，此时该线程调用lock_io_cache()函数，等待其他线程都要读取文件中的数据。当最后一个线程到达后，获得锁，并且从文件中读取内容到IO_CACHE的buffer中，调用unlock_io_cache()函数，通知其他线程共享读IO_CACHE的buffer中的数据内容。如果Count仍然大于0，重复该过程，直到Count为0或者文件读到文件末尾。

_my_b_read_r()函数的流程图如下图6所示。其中调用lock_io_cache()函数的处理逻辑和流程图参考lock_io_cache()函数部分。

![图6 _my_b_read_r()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849340jSJ4.jpg)
图6 _my_b_read_r()函数流程图

### _my_b_seq_read()函数

_my_b_seq_read()函数是顺序读SEQ_READ_APPEND类型CACHE的处理函数。该函数读取处理逻辑分为三个部分：首先从当前读位置开始，读取buffer中剩余的数据内容；其次，文件当前位置超出文件末尾，则从write_buffer中读取内容到Buffer中，并将write_buffer中剩余的内容拷贝到buffer中；最后，如果文件当前位置没有超出文件末尾，并且读取数据的长度大于IO_CACHE的大小，则从文件中读取数据到Buffer中。如果读到文件末尾，仍然不能满足数据长度，则跳转到第二步从write_buffer中读取数据到Buffer中。然后，读取IO_CACHE可以容纳的数据到IO_CACHE的buffer中，并拷贝剩余长度的数据到Buffer。同样，如果从文件中读取数据到IO_CACHE的buffer时，数据长度不能满足时，同样跳转到第二步，从write_buffer中读取数据。

_my_b_seq_read()函数流程图如下所示。其中lock_append_buffer()和unlock_append_buffer函数分别用于对追加buffer操作加锁和释放锁。

![图8 _my_b_seq_read()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849354H5gn.jpg)
图8 _my_b_seq_read()函数流程图

### copy_to_read_buffer()函数

copy_to_read_buffer()函数将write_buffer中的内容拷贝到IO_CACHE的buffer中，供多线程共享读。该函数的处理逻辑为：首先，调用lock_io_cache()函数，等待所有读线程，直到最后一个线程到达时，获得写锁。然后，将write_buffer中的内容拷贝到buffer中。最终，调用unlock_io_cache()函数通知其他线程，开始共享读。重复以上过程，直到write_buffer中没有内容时结束。

copy_to_read_buffer()函数流程图如下图7所示，调用lock_io_cache()函数的处理逻辑和流程图参考lock_io_cache()函数部分。

![图7 copy_to_read_buffer()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849369PY5X.jpg)
图7 copy_to_read_buffer()函数流程图

### my_b_flush_io_cache()函数

my_b_flush_io_cache()函数是将write_buffer的内容写到磁盘中的函数。该函数的处理逻辑是：如果当前文件不存在，则调用real_open_cached_file()函数创建临时文件。如果是共享IO_CACHE，则首先调用copy_to_read_buffer()函数拷贝write_buffer的数据到buffer。最终，将write_buffer中的内容写到文件。

my_b_flush_io_cache()函数流程图如下所示:

![图8 my_b_flush_io_cache()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_135484938466Yu.jpg)
图8 my_b_flush_io_cache()函数流程图

其中，copy_to_read_buffer()函数的处理逻辑参考对应函数部分。real_open_cached_file()函数为创建临时文件函数，具体的处理过程参考源码mysys\mf_cache.c:81。

### _my_b_write()函数

_my_b_write()函数用于将Buffer中的内容拷贝到IO_CACHE，如果IO_CACHE填满，则刷新到磁盘。该函数的处理逻辑是：首先将Buffer中的数据拷贝到write_buffer中，调用my_b_flush_io_cache()函数，将IO_CACHE中的内容刷新到文件中。如果Buffer中的数据大于IO_SIZE，则直接将Buffer中大于IO_SIZE的数据直接写到文件中。如果是共享IO_CACHE，则调用copy_to_read_buffer()函数将Buffer拷贝到IO_CACHE的读buffer中。最后将剩余的Buffer内容，拷贝到write_buffer中。

_my_b_write()函数流程图如下图9所示，my_b_flush_io_cache()函数和copy_to_read_buffer()函数分别参考相应函数的分析过程。

![图9 _my_b_write()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_1354849401wM76.jpg)
图9 _my_b_write()函数流程图

### my_b_append()函数

my_b_append()函数用于追加数据内容到write_buffer。该函数的处理逻辑是：如果追加的数据内容小于write_buffer容纳的空间时，直接将数据内容拷贝到write_buffer。否则，首先将write_buffer写满，并调用my_b_flush_io_cache()函数刷新write_buffer到磁盘文件；然后，直接将Buffer中大于IO_SIZE的数据写到文件；最后，将剩余的Buffer的数据写到write_buffer中。

my_b_append()函数流程图如下图10所示，其中my_b_flush_io_cache()函数的处理流程图参照该函数的分析过程。

![图10 my_b_append()函数流程图](http://blog.chinaunix.net/attachment/201212/7/26896862_13548494235zac.jpg)
图10 my_b_append()函数流程图

### end_io_cache()函数

end_io_cache()函数是IO_CACHE的析构函数，该函数主要是将write_buffer中的内容flush到磁盘，释放分配的内存空间等操作。由于处理过程较为简单，不再赘述。

除了以上IO_CACHE的基本处理函数之外，在源码mysys/mf_iocache2.c中，还提供了IO_CACHE的一些常用的方法，包括：my_b_copy_to_file()函数、my_b_append_tell()函数、my_b_seek()函数、my_b_fill()函数、my_b_gets()函数、my_b_filelength()函数、my_b_printf()函数、my_b_vprintf()函数。

## 结论

通过以上分析发现，IO_CACHE数据结构及提供的相关处理方法，通过将读/写的数据首先从加载或者写入到CACHE中，可以有效提高IO的读写效率。尤其对于数据长度远远小于IO_SIZE大小的数据进行读写时，IO的读写性能有明显提高。从应用场景来看，IO_CACHE几乎应用在所有IO相关处理的情况下，主要包括：log子系统（error log、general log、binlog等日志子系统）、myisam存储引擎、replication子系统等等。

## 文章来源

[MySQL数据结构分析—IO_CACHE](http://blog.chinaunix.net/uid-26896862-id-3432594.html)