## 目的

对于数据库来说，文件读写操作异常频繁，需要借助cache提高IO读写的效率。在MySQL源码中，数据结构IO_CACHE及其相关处理函数，正是提供了对于**文件**IO的CACHE读写策略。
其中IO_CACHE_SHARE数据结构，提供了共享CACHE的读写策略，用于多线程共享读取CACHE。

## 数据结构

IO_CACHE数据结构的定义在MySQL源码的/include/my_sys.h文件中，该数据结构的定义较复杂，具体定义如下所示：

### cache_type

cache根据操作类型的不同，处理的方式和策略也不同。
```c++
enum cache_type
{
  TYPE_NOT_SET= 0,
  READ_CACHE,       // 读
  WRITE_CACHE,      // 写
  SEQ_READ_APPEND		/* sequential read or append */, // 顺序读
  READ_FIFO,        // 队列读
  READ_NET,         // 网络读
  WRITE_NET         // 网络写
};
```

**READ_CACHE**

read_cache模式为只读模式，每次读取文件中的一块内容，存放到缓存中。当发生读文件请求时，首先从缓存中读取文件内容。这种读文件的方式可以间接的将读文件合并，达到减少IO操作的目的，提升IO性能。

在读取当前不会被修改的文件时，使用这种模式来读取文件，dump线程、SQL线程在读取非正在使用的文件时，均使用这种模式的IO_CACHE。

**WRITE_CACHE**

write_cache为写入模式，IO_CACHE申请一块buffer作为写入缓存，将需要写入文件的内容先写入到缓存buffer中，然后统一写入到文件中，间接达到合并IO操作的目的，提高IO的性能。

在需要写入文件时，使用这种模式来操作文件，binlog的默认打开方式为WRITE_CACHE。

### IO_CACHE_CALLBACK

```c++
typedef int (*IO_CACHE_CALLBACK)(struct st_io_cache*);
```
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
} IO_CACHE_SHARE;
```

### IO_CACHE

IO_CACHE数据结构是包含了cache文件处理的各种参数和读写处理函数。具体参数的含义，参考注释。

```c++
typedef struct st_io_cache		/* Used when cacheing files */
{
  /* Offset in file corresponding to the first byte of uchar* buffer. */
  // buffer在整个文件中的偏移量
  my_off_t pos_in_file;
  /*
    The offset of end of file for READ_CACHE and WRITE_CACHE.
    For SEQ_READ_APPEND it the maximum of the actual end of file and
    the position represented by read_end.
  */
  my_off_t end_of_file;
  /* Points to current read position in the buffer */
  // 读取到的当前位置
  uchar	*read_pos;
  /* the non-inclusive boundary in the buffer for the currently valid read */
  // 最大允许读取的位置, buffer的结束位置
  uchar  *read_end;
  // 文件中读取的文件内容
  uchar  *buffer;				/* The read buffer */
  /* Used in ASYNC_IO */
  uchar  *request_pos;

  /* Only used in WRITE caches and in SEQ_READ_APPEND to buffer writes */
  // 用在 WRITE 或者 SEQ_READ_APPEND 中的写缓存
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
    根据缓存的类型，某些操作可能不可用并产生无法预测的结果
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
  PSI_file_key file_key; /* instrumented file key */

  /*
    seek_not_done is set by my_b_seek() to inform the upcoming read/write
    operation that a seek needs to be preformed prior to the actual I/O
    error is 0 if the cache operation was successful, -1 if there was a
    "hard" error, and the actual number of I/O-ed bytes if the read/write was
    partial.
    如果缓存操作成功, 在实际I/O error之前需要执行seek, 为0
    如果发生"hard"错误, 为-1
    如果是部分读/写, 则为I/O-ed字节的实际数量
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
} IO_CACHE;
```

## 初始化

通过以上数据结构的定义可知，根据不同的操作类型，对cache的主要处理函数也不同，以下对cache的主要处理函数进行分析。源码文件参考mysys/mf_iochache.c。

### init_functions()函数

init_functions()函数用于根据不同的CACHE类型，初始化IO_CACHE的read_function和write_function函数。
该函数在init_io_cache函数初始化IO_CACHE对象时调用。
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

init_io_cache()函数是IO_CACHE对象的初始化函数。其中主要的思想是根据CACHE的类型和cachesize的值，初始化IO_CACHE的参数。

特别注意的是，IO_CACHE初始化的大小不一定是输入参数cachesize的值，在内存分配过程中，会根据文件的大小和最小CACHE大小，以及内存分配成功与否，对cachesize进行调整。如果分配失败，将cachesize大小调整为之前的3/4大小，再次进行分配，直到等于最小的CACHE值，如果仍然分配失败，那么初始化失败。

```c++
/*
  Initialize an IO_CACHE object

  SYNOPSIS
    init_io_cache() - Wrapper for init_io_cache_ext()

  NOTE
    This function should be used if the IO_CACHE tempfile is not instrumented.
*/

int init_io_cache(IO_CACHE *info, File file, size_t cachesize,
                  enum cache_type type, my_off_t seek_offset,
                  pbool use_async_io, myf cache_myflags)
{
  return init_io_cache_ext(info, file, cachesize, type, seek_offset,
                           use_async_io, cache_myflags, key_file_io_cache);
}

/*
  Initialize an IO_CACHE object

  SYNOPSIS
    init_io_cache_ext()
    info               cache handler to initialize
    file               File that should be associated to to the handler
                       If == -1 then real_open_cached_file()
                       will be called when it's time to open file.
    cachesize          Size of buffer to allocate for read/write
                       If == 0 then use my_default_record_cache_size
    type               Type of cache
    seek_offset        Where cache should start reading/writing
    use_async_io       Set to 1 of we should use async_io (if avaiable)
    cache_myflags      Bitmap of different flags
                       MY_WME | MY_FAE | MY_NABP | MY_FNABP |
                       MY_DONT_CHECK_FILESIZE
    file_key           Instrumented file key for temporary cache file

  RETURN
    0  ok
    #  error
*/

int init_io_cache_ext(IO_CACHE *info, File file, size_t cachesize,
                      enum cache_type type, my_off_t seek_offset,
                      pbool use_async_io, myf cache_myflags,
                      PSI_file_key file_key)
{
  size_t min_cache;
  my_off_t pos;
  my_off_t end_of_file= ~(my_off_t) 0;
  DBUG_ENTER("init_io_cache_ext");
  DBUG_PRINT("enter",("cache: 0x%lx  type: %d  pos: %ld",
             (ulong) info, (int) type, (ulong) seek_offset));

  info->file= file;
  info->file_key= file_key;
  info->type= TYPE_NOT_SET;	    /* Don't set it until mutex are created */
  // WRITE_CACHE模式下, 写入的起始点
  info->pos_in_file= seek_offset;
  info->pre_close = info->pre_read = info->post_read = 0;
  info->arg = 0;
  info->alloced_buffer = 0;
  info->buffer=0;
  info->seek_not_done= 0;

  if (file >= 0)
  {
    pos= mysql_file_tell(file, MYF(0));
    if ((pos == (my_off_t) -1) && (my_errno() == ESPIPE))
    {
      /*
         This kind of object doesn't support seek() or tell(). Don't set a
         flag that will make us again try to seek() later and fail.
      */
      info->seek_not_done= 0;
      /*
        Additionally, if we're supposed to start somewhere other than the
        the beginning of whatever this file is, then somebody made a bad
        assumption.
      */
      DBUG_ASSERT(seek_offset == 0);
    }
    else
      info->seek_not_done= MY_TEST(seek_offset != pos);
  }

  info->disk_writes= 0;
  info->share=0;

  if (!cachesize && !(cachesize= my_default_record_cache_size))
    DBUG_RETURN(1);        /* No cache requested */
  // use_async_io 是否使用异步IO操作
  min_cache=use_async_io ? IO_SIZE*4 : IO_SIZE*2;
  if (type == READ_CACHE || type == SEQ_READ_APPEND)
  {                       /* Assume file isn't growing */
    if (!(cache_myflags & MY_DONT_CHECK_FILESIZE))
    {
      /* Calculate end of file to avoid allocating oversized buffers */
      end_of_file= mysql_file_seek(file, 0L, MY_SEEK_END, MYF(0));
      /* Need to reset seek_not_done now that we just did a seek. */
      info->seek_not_done= end_of_file == seek_offset ? 0 : 1;
      if (end_of_file < seek_offset)
        end_of_file=seek_offset;
      /* Trim cache size if the file is very small */
      if ((my_off_t) cachesize > end_of_file-seek_offset+IO_SIZE*2-1)
      {
        cachesize= (size_t) (end_of_file-seek_offset)+IO_SIZE*2-1;
        use_async_io=0;    /* No need to use async */
      }
    }
  }
  cache_myflags &= ~MY_DONT_CHECK_FILESIZE;
  if (type != READ_NET && type != WRITE_NET)
  {
    /* Retry allocating memory in smaller blocks until we get one */
    // 对cachesize以最小单位min_cache做对齐
    cachesize= ((cachesize + min_cache-1) & ~(min_cache-1));
    for (;;)
    {
      size_t buffer_block;
      /*
        Unset MY_WAIT_IF_FULL bit if it is set, to prevent conflict with
        MY_ZEROFILL.
      */
      myf flags= (myf) (cache_myflags & ~(MY_WME | MY_WAIT_IF_FULL));

      if (cachesize < min_cache)
        cachesize = min_cache;
      buffer_block= cachesize;
      if (type == SEQ_READ_APPEND)
        buffer_block *= 2;
      if (cachesize == min_cache)
        flags|= (myf) MY_WME;

      if ((info->buffer= (uchar*) my_malloc(key_memory_IO_CACHE,
                                            buffer_block, flags)) != 0)
      {
        info->write_buffer=info->buffer;
        if (type == SEQ_READ_APPEND)
          info->write_buffer = info->buffer + cachesize;
        info->alloced_buffer=1;
        break;    /* Enough memory found */
      }
      if (cachesize == min_cache)
        DBUG_RETURN(2);    /* Can't alloc cache */
      /* Try with less memory */
      cachesize= (cachesize*3/4 & ~(min_cache-1));
    }
  }

  DBUG_PRINT("info",("init_io_cache: cachesize = %lu", (ulong) cachesize));
  info->read_length=info->buffer_length=cachesize;
  info->myflags=cache_myflags & ~(MY_NABP | MY_FNABP);
  info->request_pos= info->read_pos= info->write_pos = info->buffer;
  if (type == SEQ_READ_APPEND)
  {
    info->append_read_pos = info->write_pos = info->write_buffer;
    info->write_end = info->write_buffer + info->buffer_length;
    mysql_mutex_init(key_IO_CACHE_append_buffer_lock,
                     &info->append_buffer_lock, MY_MUTEX_INIT_FAST);
  }
#if defined(SAFE_MUTEX)
  else
  {
    /* Clear mutex so that safe_mutex will notice that it's not initialized */
    memset(&info->append_buffer_lock, 0, sizeof(info->append_buffer_lock));
  }
#endif

  if (type == WRITE_CACHE)
    info->write_end=
      info->buffer+info->buffer_length- (seek_offset & (IO_SIZE-1));
  else
    // 在READ_CACHE模式下，初始化后缓存中是没有内容的
    info->read_end=info->buffer;		/* Nothing in cache */

  /* End_of_file may be changed by user later */
  info->end_of_file= end_of_file;
  info->error=0;
  info->type= type;
  init_functions(info);
  DBUG_RETURN(0);
}  /* init_io_cache_ext */
```

## 读取文件

使用宏my_b_read来读取
```c++
#define my_b_read(info,Buffer,Count) \
  ((info)->read_pos + (Count) <= (info)->read_end ?\
   // 如果info->buffer中有足够的长度，直接从buffer中读取
   (memcpy(Buffer,(info)->read_pos,(size_t) (Count)), \
    ((info)->read_pos+=(Count)),0) :\
   // 如果info->buffer中没有足够的长度(或者刚初始化)，使用读取函数读取
   (*(info)->read_function)((info),Buffer,Count))
```

### _my_b_read()函数

`_my_b_read()`函数是默认的非共享读函数read_function。
该函数的流程是：
1. 将当前CACHE中的内容全部读取到输入参数Buffer中;
2. 从文件中以IO_SIZE为单位读取到足够的内容到Buffer中;
3. 文件剩余的内容读取到IO_CACHE的buffer中之后，然后从IO_CACHE的buffer中读取到Buffer中.

需要注意的是，每次读取均是以IO_CACHE为单位，如果读取的起始位置不为文件头，则通过diff_length进行对齐。

从以上处理逻辑来看，_my_b_read()函数对数据量相对较小的连续读取性能较高。因为读取少量数据时，IO_CACHE也会读取IO_CACHE所能容纳的最大的数据内容。因此，多次连续读取的数据会始终从IO_CACHE中获得。而对于数据较大的读取来说，IO_CACHE的读取策略就没有了优势。

```c++
/*
  Read buffered.

  SYNOPSIS
    _my_b_read()
      info                      IO_CACHE pointer
      Buffer                    Buffer to retrieve count bytes from file
      Count                     Number of bytes to read into Buffer

  NOTE
    This function is only called from the my_b_read() macro when there
    isn't enough characters in the buffer to satisfy the request.

  WARNING

    When changing this function, be careful with handling file offsets
    (end-of_file, pos_in_file). Do not cast them to possibly smaller
    types than my_off_t unless you can be sure that their value fits.
    Same applies to differences of file offsets.

    When changing this function, check _my_b_read_r(). It might need the
    same change.

  RETURN
    0      we succeeded in reading all data
    1      Error: couldn't read requested characters. In this case:
             If info->error == -1, we got a read error.
             Otherwise info->error contains the number of bytes in Buffer.
*/

int _my_b_read(IO_CACHE *info, uchar *Buffer, size_t Count)
{
  size_t length,diff_length,left_length, max_length;
  my_off_t pos_in_file;
  DBUG_ENTER("_my_b_read");

  /* If the buffer is not empty yet, copy what is available. */
  if ((left_length= (size_t) (info->read_end-info->read_pos)))
  {
    DBUG_ASSERT(Count >= left_length);	/* User is not using my_b_read() */
    memcpy(Buffer,info->read_pos, left_length);
    Buffer+=left_length;
    Count-=left_length;
  }

  /* pos_in_file always point on where info->buffer was read */
  pos_in_file=info->pos_in_file+ (size_t) (info->read_end - info->buffer);

  /*
    Whenever a function which operates on IO_CACHE flushes/writes
    some part of the IO_CACHE to disk it will set the property
    "seek_not_done" to indicate this to other functions operating
    on the IO_CACHE.
  */
  if (info->seek_not_done)
  {
    if ((mysql_file_seek(info->file, pos_in_file, MY_SEEK_SET, MYF(0)) 
        != MY_FILEPOS_ERROR))
    {
      /* No error, reset seek_not_done flag. */
      info->seek_not_done= 0;
    }
    else
    {
      /*
        If the seek failed and the error number is ESPIPE, it is because
        info->file is a pipe or socket or FIFO.  We never should have tried
        to seek on that.  See Bugs#25807 and #22828 for more info.
      */
      DBUG_ASSERT(my_errno() != ESPIPE);
      info->error= -1;
      DBUG_RETURN(1);
    }
  }

  /*
    Calculate, how much we are within a IO_SIZE block. Ideally this
    should be zero.
  */
  // 计算文件的偏移量，保证读取的文件内容是以IO_SIZE为单位
  diff_length= (size_t) (pos_in_file & (IO_SIZE-1));

  /*
    If more than a block plus the rest of the current block is wanted,
    we do read directly, without filling the buffer.
  */
  if (Count >= (size_t) (IO_SIZE+(IO_SIZE-diff_length)))
  {					/* Fill first intern buffer */
    size_t read_length;
    if (info->end_of_file <= pos_in_file)
    {
      /* End of file. Return, what we did copy from the buffer. */
      info->error= (int) left_length;
      DBUG_RETURN(1);
    }
    /*
      Crop the wanted count to a multiple of IO_SIZE and subtract,
      what we did already read from a block. That way, the read will
      end aligned with a block.
    */
    length=(Count & (size_t) ~(IO_SIZE-1))-diff_length;
    if ((read_length= mysql_file_read(info->file,Buffer, length, info->myflags))
	!= length)
    {
      /*
        If we didn't get, what we wanted, we either return -1 for a read
        error, or (it's end of file), how much we got in total.
      */
      info->error= (read_length == (size_t) -1 ? -1 :
		    (int) (read_length+left_length));
      DBUG_RETURN(1);
    }
    Count-=length;
    Buffer+=length;
    pos_in_file+=length;
    left_length+=length;
    diff_length=0;
  }

  /*
    At this point, we want less than one and a partial block.
    We will read a full cache, minus the number of bytes, we are
    within a block already. So we will reach new alignment.
  */
  max_length= info->read_length-diff_length;
  /* We will not read past end of file. */
  if (info->type != READ_FIFO &&
      max_length > (info->end_of_file - pos_in_file))
    max_length= (size_t) (info->end_of_file - pos_in_file);
  /*
    If there is nothing left to read,
      we either are done, or we failed to fulfill the request.
    Otherwise, we read max_length into the cache.
  */
  if (!max_length)
  {
    if (Count)
    {
      /* We couldn't fulfil the request. Return, how much we got. */
      info->error= (int)left_length;
      DBUG_RETURN(1);
    }
    length=0;				/* Didn't read any chars */
  }
  else if ((length= mysql_file_read(info->file,info->buffer, max_length,
                            info->myflags)) < Count ||
	   length == (size_t) -1)
  {
    /*
      We got an read error, or less than requested (end of file).
      If not a read error, copy, what we got.
    */
    if (length != (size_t) -1)
      memcpy(Buffer, info->buffer, length);
    info->pos_in_file= pos_in_file;
    /* For a read error, return -1, otherwise, what we got in total. */
    info->error= length == (size_t) -1 ? -1 : (int) (length+left_length);
    info->read_pos=info->read_end=info->buffer;
    DBUG_RETURN(1);
  }
  /*
    Count is the remaining number of bytes requested.
    length is the amount of data in the cache.
    Read Count bytes from the cache.
  */
  info->read_pos=info->buffer+Count;
  info->read_end=info->buffer+length;
  info->pos_in_file=pos_in_file;
  memcpy(Buffer, info->buffer, Count);
  DBUG_RETURN(0);
}
```

## 写入文件

使用宏my_b_write来写入
```c++
#define my_b_write(info,Buffer,Count) \
 ((info)->write_pos + (Count) <=(info)->write_end ?\
  // 如果剩余空间足够写入，使用当前buffer做缓存
  (memcpy((info)->write_pos, (Buffer), (size_t)(Count)),\
   ((info)->write_pos+=(Count)),0) : \
   // 如果剩余空不足，则调用函数进行写入
   (*(info)->write_function)((info),(uchar *)(Buffer),(Count)))
```

### _my_b_write()函数

_my_b_write()函数用于将Buffer中的内容拷贝到IO_CACHE，如果IO_CACHE填满，则刷新到磁盘。
该函数的处理逻辑是：
1. 将Buffer中内容写入IO_CACHE->buffer, 直到IO_CACHE填满. 将IO_CACHE内容刷新到文件中.
2. 如果剩余的Buffer内容大于IO_SIZE，则以IO_SIZE为单位, 将Buffer中的数据直接写到文件中。
3. 剩余的Buffer内容写入IO_CACHE->buffer.

```c++
/* 
   Write a byte buffer to IO_CACHE and flush to disk
   if IO_CACHE is full.

   RETURN VALUE
    1 On error on write
    0 On success
   -1 On error; my_errno contains error code.
*/

int _my_b_write(IO_CACHE *info, const uchar *Buffer, size_t Count)
{
  size_t rest_length,length;
  my_off_t pos_in_file= info->pos_in_file;

  DBUG_EXECUTE_IF("simulate_huge_load_data_file",
                  {
                    pos_in_file=(my_off_t)(5000000000ULL);
                  });
  if (pos_in_file+info->buffer_length > info->end_of_file)
  {
    errno=EFBIG;
    set_my_errno(EFBIG);
    return info->error = -1;
  }

  // 将部分Buffer中的内容写入IO_CACHE->buffer, 使得buffer填满, 并刷新到文件中
  rest_length= (size_t) (info->write_end - info->write_pos);
  memcpy(info->write_pos,Buffer,(size_t) rest_length);
  Buffer+=rest_length;
  Count-=rest_length;
  info->write_pos+=rest_length;

  if (my_b_flush_io_cache(info,1))
    return 1;
  // 如果剩余的长度大于IO_SIZE,以IO_SIZE为单位，将所有大于IO_SIZE的内容直接写入文件
  if (Count >= IO_SIZE)
  {					/* Fill first intern buffer */
    length=Count & (size_t) ~(IO_SIZE-1);
    if (info->seek_not_done)
    {
      /*
        Whenever a function which operates on IO_CACHE flushes/writes
        some part of the IO_CACHE to disk it will set the property
        "seek_not_done" to indicate this to other functions operating
        on the IO_CACHE.
      */
      if (mysql_file_seek(info->file, info->pos_in_file, MY_SEEK_SET, MYF(0)))
      {
        info->error= -1;
        return (1);
      }
      info->seek_not_done=0;
    }
    if (mysql_file_write(info->file, Buffer, length, info->myflags | MY_NABP))
      return info->error= -1;

    /*
      In case of a shared I/O cache with a writer we normally do direct
      write cache to read cache copy. Simulate this here by direct
      caller buffer to read cache copy. Do it after the write so that
      the cache readers actions on the flushed part can go in parallel
      with the write of the extra stuff. copy_to_read_buffer()
      synchronizes writer and readers so that after this call the
      readers can act on the extra stuff while the writer can go ahead
      and prepare the next output. copy_to_read_buffer() relies on
      info->pos_in_file.
    */
    if (info->share)
      copy_to_read_buffer(info, Buffer, length);

    Count-=length;
    Buffer+=length;
    info->pos_in_file+=length;
  }
  // 将剩余Buffer内容写入IO_CACHE中
  memcpy(info->write_pos,Buffer,(size_t) Count);
  info->write_pos+=Count;
  return 0;
}
```

### my_b_flush_io_cache()函数

my_b_flush_io_cache()函数是将write_buffer的内容写到磁盘中的函数。
该函数的处理逻辑是：如果当前文件不存在，则调用real_open_cached_file()函数创建临时文件。如果是共享IO_CACHE，则首先调用copy_to_read_buffer()函数拷贝write_buffer的数据到buffer。最终，将write_buffer中的内容写到文件。

其中，copy_to_read_buffer()函数的处理逻辑参考对应函数部分。real_open_cached_file()函数为创建临时文件函数，具体的处理过程参考源码mysys\mf_cache.c:81。

```c++
int my_b_flush_io_cache(IO_CACHE *info,
                        int need_append_buffer_lock MY_ATTRIBUTE((unused)))
{
  size_t length;
  my_off_t pos_in_file;
  my_bool append_cache= (info->type == SEQ_READ_APPEND);
  DBUG_ENTER("my_b_flush_io_cache");
  DBUG_PRINT("enter", ("cache: 0x%lx", (long) info));

  DBUG_EXECUTE_IF("simulate_error_during_flush_cache_to_file",
                  {
                    DBUG_RETURN(TRUE);
                  });
  if (!append_cache)
    need_append_buffer_lock= 0;

  if (info->type == WRITE_CACHE || append_cache)
  {
    if (info->file == -1)
    {
      if (real_open_cached_file(info))
        DBUG_RETURN((info->error= -1));
    }
    LOCK_APPEND_BUFFER;

    // 计算IO_CACHE中的buffer大小
    if ((length=(size_t) (info->write_pos - info->write_buffer)))
    {
      /*
        In case of a shared I/O cache with a writer we do direct write
        cache to read cache copy. Do it before the write here so that
        the readers can work in parallel with the write.
        copy_to_read_buffer() relies on info->pos_in_file.
      */
      if (info->share)
        copy_to_read_buffer(info, info->write_buffer, length);

      pos_in_file=info->pos_in_file;
      /*
	If we have append cache, we always open the file with
	O_APPEND which moves the pos to EOF automatically on every write
      */
      if (!append_cache && info->seek_not_done)
      {					/* File touched, do seek */
        if (mysql_file_seek(info->file, pos_in_file, MY_SEEK_SET, MYF(0)) ==
            MY_FILEPOS_ERROR)
        {
          UNLOCK_APPEND_BUFFER;
          DBUG_RETURN((info->error= -1));
        }
        if (!append_cache)
          info->seek_not_done=0;
      }
      if (!append_cache)
        info->pos_in_file+=length;
      // 保证整个缓存以IO_SIZE对齐，即如果写满，一定为IO_SIZE的整数倍
      info->write_end= (info->write_buffer+info->buffer_length-
			((pos_in_file+length) & (IO_SIZE-1)));

      if (mysql_file_write(info->file,info->write_buffer,length,
		   info->myflags | MY_NABP))
        info->error= -1;
      else
        info->error= 0;
      if (!append_cache)
      {
        set_if_bigger(info->end_of_file,(pos_in_file+length));
      }
      else
      {
        info->end_of_file+=(info->write_pos-info->append_read_pos);
        DBUG_ASSERT(info->end_of_file == mysql_file_tell(info->file, MYF(0)));
      }

      // 重置info->write_pos
      info->append_read_pos=info->write_pos=info->write_buffer;
      ++info->disk_writes;
      UNLOCK_APPEND_BUFFER;
      DBUG_RETURN(info->error);
    }
  }
  UNLOCK_APPEND_BUFFER;
  DBUG_RETURN(0);
}
```

### init_io_cache_share()函数

init_io_cache_share()函数是多线程操作IO_CACHE的初始化函数。该函数初始化IO_CACHE的成员变量share，share是IO_CACHE_SHARE数据类型。通过share变量，使得每个线程共享读取IO_CACHE的buffer内容，并且每个线程维护各自的buffer、错误以及buffer读取的偏移地址。此外，将IO_CACHE的read_function初始化为_my_b_read_r()多线程共享读函数。

初始化IO_CACHE_SHARE的初始化状态如下图4所示，而对IO_CACHE的初始化主要对读cache（输入参数read_cache）的share、current_pos、current_end以及read_function进行初始化。在调用init_io_cache_share之前，已经对read_cache和write_cache调用init_io_cache()函数进行初始化。

通过以上初始化可知，IO_CACHE_SHARE结构主要用于IO_CACHE的_my_b_read_r()函数进行多线程共享读CACHE中的内容。在分析多线程共享读IO_CACHE的处理函数_my_b_read_r()之前，对其处理逻辑中的锁处理函数：lock_io_cache()函数和unlock_io_cache()函数进行简要的分析。

### lock_io_cache()函数

lock_io_cache()函数式是多线程共享读时，对CACHE进行加锁控制。该函数的主要思想是：首先对CACHE加锁，并且被锁定的线程数减1。如果当前CACHE的share中source_cache是WRITE_CACHE，那么将当前WRITE_CACHE同步到READ_CACHE上。在同步之前，等待所有的读线程操作都结束，这时写线程被唤醒，并且线程持有锁。此时，新的读线程请求锁，都需要等待写线程释放锁；如果当前CACHE只有READ_CACHE，那么读线程等待获取读锁。读锁获取后，CACHE的锁释放，所有读线程都可以进行共享读操作。

### unlock_io_cache()函数

unlock_io_cache()函数用于释放IO_CACHE上的锁，并重置IO_CACHE_SHARE的running_threads参数的值为total_threads。由于unlock_io_cache()函数释放锁的操作比较简单，因此不做分析。

### _my_b_read_r()函数

_my_b_read_r()函数是多线程共享读IO_CACHE的处理函数，该函数的处理逻辑类似_my_b_read()函数。首先，读取IO_CACHE的buffer中剩余的数据，并拷贝到Buffer中。如果当前读取的内容的长度Count大于0，需要继续从文件中读取数据，此时该线程调用lock_io_cache()函数，等待其他线程都要读取文件中的数据。当最后一个线程到达后，获得锁，并且从文件中读取内容到IO_CACHE的buffer中，调用unlock_io_cache()函数，通知其他线程共享读IO_CACHE的buffer中的数据内容。如果Count仍然大于0，重复该过程，直到Count为0或者文件读到文件末尾。

_my_b_read_r()函数的流程图如下图6所示。其中调用lock_io_cache()函数的处理逻辑和流程图参考lock_io_cache()函数部分。

### _my_b_seq_read()函数

_my_b_seq_read()函数是顺序读SEQ_READ_APPEND类型CACHE的处理函数。该函数读取处理逻辑分为三个部分：首先从当前读位置开始，读取buffer中剩余的数据内容；其次，文件当前位置超出文件末尾，则从write_buffer中读取内容到Buffer中，并将write_buffer中剩余的内容拷贝到buffer中；最后，如果文件当前位置没有超出文件末尾，并且读取数据的长度大于IO_CACHE的大小，则从文件中读取数据到Buffer中。如果读到文件末尾，仍然不能满足数据长度，则跳转到第二步从write_buffer中读取数据到Buffer中。然后，读取IO_CACHE可以容纳的数据到IO_CACHE的buffer中，并拷贝剩余长度的数据到Buffer。同样，如果从文件中读取数据到IO_CACHE的buffer时，数据长度不能满足时，同样跳转到第二步，从write_buffer中读取数据。

_my_b_seq_read()函数流程图如下所示。其中lock_append_buffer()和unlock_append_buffer函数分别用于对追加buffer操作加锁和释放锁。

### copy_to_read_buffer()函数

copy_to_read_buffer()函数将write_buffer中的内容拷贝到IO_CACHE的buffer中，供多线程共享读。该函数的处理逻辑为：首先，调用lock_io_cache()函数，等待所有读线程，直到最后一个线程到达时，获得写锁。然后，将write_buffer中的内容拷贝到buffer中。最终，调用unlock_io_cache()函数通知其他线程，开始共享读。重复以上过程，直到write_buffer中没有内容时结束。

copy_to_read_buffer()函数流程图如下图7所示，调用lock_io_cache()函数的处理逻辑和流程图参考lock_io_cache()函数部分。

### my_b_append()函数

my_b_append()函数用于追加数据内容到write_buffer。该函数的处理逻辑是：如果追加的数据内容小于write_buffer容纳的空间时，直接将数据内容拷贝到write_buffer。否则，首先将write_buffer写满，并调用my_b_flush_io_cache()函数刷新write_buffer到磁盘文件；然后，直接将Buffer中大于IO_SIZE的数据写到文件；最后，将剩余的Buffer的数据写到write_buffer中。

### end_io_cache()函数

end_io_cache()函数是IO_CACHE的析构函数，该函数主要是将write_buffer中的内容flush到磁盘，释放分配的内存空间等操作。由于处理过程较为简单，不再赘述。

除了以上IO_CACHE的基本处理函数之外，在源码mysys/mf_iocache2.c中，还提供了IO_CACHE的一些常用的方法，包括：my_b_copy_to_file()函数、my_b_append_tell()函数、my_b_seek()函数、my_b_fill()函数、my_b_gets()函数、my_b_filelength()函数、my_b_printf()函数、my_b_vprintf()函数。

## 结论

通过以上分析发现，IO_CACHE数据结构及提供的相关处理方法，通过将读/写的数据首先从加载或者写入到CACHE中，可以有效提高IO的读写效率。尤其对于数据长度远远小于IO_SIZE大小的数据进行读写时，IO的读写性能有明显提高。从应用场景来看，IO_CACHE几乎应用在所有IO相关处理的情况下，主要包括：log子系统（error log、general log、binlog等日志子系统）、myisam存储引擎、replication子系统等等。

## 参考

[文件读取模块IO_CACHE](https://zhuanlan.zhihu.com/p/26662917)
