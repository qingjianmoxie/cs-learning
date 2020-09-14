# AOF持久化

- [AOF持久化](#aof持久化)
  - [1 AOF持久化的实现](#1-aof持久化的实现)
    - [1.1 命令追加](#11-命令追加)
    - [1.2 AOF文件的写入与同步](#12-aof文件的写入与同步)

除了RDB持久化功能之外，Redis还提供了AOF（Append Only File）持久化功能。与RDB持久化通过保存数据库中的键值对来记录数据库状态不同，AOF持久化是通过保存Redis服务器所执行的写命令来记录数据库状态的，如下图所示。

            发送                保存被执行
            写命令               的写命令
    客户端 -----------> 服务器 ----------> AOF文件

举个例子，如果我们对空白的数据库执行以下写命令，那么数据库中将包含三个键值对：

    redis> FLUSHALL
    OK
    redis> SET msg "hello"
    OK
    redis> SADD fruits "apple" "banana" "cherry"
    (integer) 3
    redis> RPUSH numbers 128 256 512
    (integer) 3

RDB持久化保存数据库状态的方法是将msg、fruits、numbers三个键的键值对保存到RDB文件中，而AOF持久化保存数据库状态的方法则是将服务器执行的SET、SADD、RPUSH三个命令保存到AOF文件中。

被写入AOF文件的所有命令都是以Redis的命令请求协议格式保存的，因为Redis的命令请求协议是纯文本格式，所以我们可以直接打开一个AOF文件，观察里面的内容。例如，对于之前执行的三个写命令来说，服务器将产生包含以下内容的AOF文件：

```
* 2 \r \n $ 6 \r \n S E L E C T \r \n $ 1 \r \n 0 \r \n 
* 1 \r \n $ 8 \r \n F L U S H A L L \r \n 
* 3 \r \n $ 3 \r \n S E T \r \n $ 3 \r \n m s g \r \n $ 5 \r \n h e l l o \r \n 
* 5 \r \n $ 4 \r \n S A D D \r \n $ 6 \r \n f r u i t s \r \n $ 5 \r \n a p p l e \r \n $ 6 \r \n b a n a n a \r \n $ 6 \r \n c h e r r y \r \n 
* 5 \r \n $ 5 \r \n R P U S H \r \n $ 7 \r \n n u m b e r s \r \n $ 3 \r \n 1 2 8 \r \n $ 3 \r \n 2 5 6 \r \n $ 3 \r \n 5 1 2 \r \n 
```

在这个AOF文件里面，除了用于指定数据库的SELECT命令是服务器自动添加的之外，其他都是我们之前通过客户端发送的命令。

服务器在启动时，可以通过载入和执行AOF文件中保存的命令来还原服务器关闭之前的数据库状态.

在本章接下来的内容中，我们将对AOF持久化功能的实现进行介绍，说明AOF文件的写入、保存、载入等操作的实现原理。

之后我们还会介绍用于减少AOF文件体积的AOF重写功能，以及该功能的实现原理。

## 1 AOF持久化的实现

AOF持久化功能的实现可以分为命令追加（append）、文件写入、文件同步（sync）三个步骤。

### 1.1 命令追加

当AOF持久化功能处于打开状态时，服务器在执行完一个写命令之后，会以协议格式将被执行的写命令追加到服务器状态的aof_buf缓冲区的末尾：

```c
struct redisServer {
    ...
    // AOF缓冲区
    sds aof_buf;      /* AOF buffer, written before entering the event loop */
    ...
};
```

举个例子，如果客户端向服务器发送以下命令：

    redis> SET KEY VALUE
    OK

那么服务器在执行这个SET命令之后，会将以下协议内容追加到aof_buf缓冲区的末尾：

    * 3 \r \n $ 3 \r \n SET \r \n $ 3 \r \n KEY \r \n $ 5 \r \n VALUE \r \n 

又例如，如果客户端向服务器发送以下命令：

    redis> RPUSH NUMBERS ONE TWO THREE
    (integer) 3

那么服务器在执行这个RPUSH命令之后，会将以下协议内容追加到aof_buf缓冲区的末尾：

    * 5 \r \n $ 5 \r \n RPUSH \r \n $ 7 \r \n NUMBERS \r \n $ 3 \r \n ONE \r \n $ 3 \r \n TWO \r \n $ 3 \r \n THREE \r \n 

以上就是AOF持久化的命令追加步骤的实现原理。

### 1.2 AOF文件的写入与同步

Redis的服务器进程就是一个事件循环（loop），这个循环中的文件事件负责接收客户端的命令请求，以及向客户端发送命令回复，而时间事件则负责执行像serverCron函数这样需要定时运行的函数。

因为服务器在处理文件事件时可能会执行写命令，使得一些内容被追加到aof_buf缓冲区里面，所以在服务器每次结束一个事件循环之前，它都会调用flushAppendOnlyFile函数，考虑是否需要将aof_buf缓冲区中的内容写入和保存到AOF文件里面，这个过程可以用以下伪代码表示：

```py
def evenLoop():
    while True:
        # 处理文件事件, 接收命令请求以及发送命令回复
        # 处理命令请求时可能会有新内容被追加到aof_buff缓冲区中
        processFileEvents()
        # 处理时间事件
        processTimeEvents()
        # 考虑是否要将aof_buf中的内容写入和保存到AOF文件里面
        flushAppendOnlyFile()
```

flushAppendOnlyFile函数的行为由服务器配置的appendfsync选项的值来决定，各个不同值产生的行为如表1所示。

表1 不同appendfsync值产生不同的持久化行为

appendfsync选项的值 | flushAppendOnlyFile函数的行为
:- | :- 
always | 将aof_buf缓冲区的所有内容写入并同步到AOF文件
everysec | 将aof_buf缓冲区的所有内容写入到AOF文件, 如果上次同步AOF文件的时间距离现在超过一秒钟, 那么再次对AOF文件进行同步, 并且这个同步操作是由一个线程专门负责执行的
no | 将aof_buf缓冲区中的所有内容写入到AOF文件, 但并不对AOF文件进行同步, 何时同步由操作系统来决定

