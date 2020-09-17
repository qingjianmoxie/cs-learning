# AOF持久化

- [AOF持久化](#aof持久化)
  - [1 AOF持久化的实现](#1-aof持久化的实现)
    - [1.1 命令追加](#11-命令追加)
    - [1.2 AOF文件的写入与同步](#12-aof文件的写入与同步)
  - [2 AOF文件的载入与数据还原](#2-aof文件的载入与数据还原)
  - [3 AOF重写](#3-aof重写)
    - [3.1 AOF文件重写的实现](#31-aof文件重写的实现)

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

如果用户没有主动为appendfsync选项设置值，那么appendfsync选项的默认值为everysec，关于appendfsync选项的更多信息，请参考Redis项目附带的示例配置文件redis.conf。

**文件的写入和同步**
为了提高文件的写入效率，在现代操作系统中，当用户调用write函数，将一些数据写入到文件的时候，操作系统通常会将写入数据暂时保存在一个内存缓冲区里面，等到缓冲区的空间被填满、或者超过了指定的时限之后，才真正地将缓冲区中的数据写入到磁盘里面。
这种做法虽然提高了效率，但也为写入数据带来了安全问题，因为如果计算机发生停机，那么保存在内存缓冲区里面的写入数据将会丢失。
为此，系统提供了fsync和fdatasync两个同步函数，它们可以强制让操作系统立即将缓冲区中的数据写入到硬盘里面，从而确保写入数据的安全性。

举个例子，假设服务器在处理文件事件期间，执行了以下三个写入命令：

    redis> SET msg "hello"
    OK
    redis> SADD fruits "apple" "banana" "cherry"
    (integer) 3
    redis> RPUSH numbers 128 256 512
    (integer) 3

那么aof_buf缓冲区将包含这三个命令的协议内容：

```
* 3 \r \n $ 3 \r \n S E T \r \n $ 3 \r \n m s g \r \n $ 5 \r \n h e l l o \r \n 
* 5 \r \n $ 4 \r \n S A D D \r \n $ 6 \r \n f r u i t s \r \n $ 5 \r \n a p p l e \r \n $ 6 \r \n b a n a n a \r \n $ 6 \r \n c h e r r y \r \n 
* 5 \r \n $ 5 \r \n R P U S H \r \n $ 7 \r \n n u m b e r s \r \n $ 3 \r \n 1 2 8 \r \n $ 3 \r \n 2 5 6 \r \n $ 3 \r \n 5 1 2 \r \n 
```

如果这时flushAppendOnlyFile函数被调用，假设服务器当前appendfsync选项的值为everysec，并且距离上次同步AOF文件已经超过一秒钟，那么服务器会先将aof_buf中的内容写入到AOF文件中，然后再对AOF文件进行同步。

以上就是对AOF持久化功能的文件写入和文件同步这两个步骤的介绍。

**AOF持久化的效率和安全性**
服务器配置appendfsync选项的值直接决定AOF持久化功能的效率和安全性。
- 当appendfsync的值为always时，服务器在每个事件循环都要将aof_buf缓冲区中的所有内容写入到AOF文件，并且同步AOF文件，所以always的效率是appendfsync选项三个值当中最慢的一个，但从安全性来说，always也是最安全的，因为即使出现故障停机，AOF持久化也只会丢失一个事件循环中所产生的命令数据。
- 当appendfsync的值为everysec时，服务器在每个事件循环都要将aof_buf缓冲区中的所有内容写入到AOF文件，并且每隔一秒就要在子线程中对AOF文件进行一次同步。从效率上来讲，everysec模式足够快，并且就算出现故障停机，数据库也只丢失一秒钟的命令数据。
- 当appendfsync的值为no时，服务器在每个事件循环都要将aof_buf缓冲区中的所有内容写入到AOF文件，至于何时对AOF文件进行同步，则由操作系统控制。因为处于no模式下的flushAppendOnlyFile调用无须执行同步操作，所以该模式下的AOF文件写入速度总是最快的，不过因为这种模式会在系统缓存中积累一段时间的写入数据，所以该模式的单次同步时长通常是三种模式中时间最长的。从平摊操作的角度来看，no模式和everysec模式的效率类似，当出现故障停机时，使用no模式的服务器将丢失上次同步AOF文件之后的所有写命令数据。

## 2 AOF文件的载入与数据还原

因为AOF文件里面包含了重建数据库状态所需的所有写命令，所以服务器只要读入并重新执行一遍AOF文件里面保存的写命令，就可以还原服务器关闭之前的数据库状态。

Redis读取AOF文件并还原数据库状态的详细步骤如下：
1）创建一个不带网络连接的伪客户端（fake client）：因为Redis的命令只能在客户端上下文中执行，而载入AOF文件时所使用的命令直接来源于AOF文件而不是网络连接，所以服务器使用了一个没有网络连接的伪客户端来执行AOF文件保存的写命令，伪客户端执行命令的效果和带网络连接的客户端执行命令的效果完全一样。
2）从AOF文件中分析并读取出一条写命令。
3）使用伪客户端执行被读出的写命令。
4）一直执行步骤2和步骤3，直到AOF文件中的所有写命令都被处理完毕为止。

当完成以上步骤之后，AOF文件所保存的数据库状态就会被完整地还原出来。

## 3 AOF重写

因为AOF持久化是通过保存被执行的写命令来记录数据库状态的，所以随着服务器运行时间的流逝，AOF文件中的内容会越来越多，文件的体积也会越来越大，如果不加以控制的话，体积过大的AOF文件很可能对Redis服务器、甚至整个宿主计算机造成影响，并且AOF文件的体积越大，使用AOF文件来进行数据还原所需的时间就越多。

举个例子，如果客户端执行了以下命令：

    redis> RPUSH list "A" "B"      // ["A", "B"]
    (integer) 2
    redis> RPUSH list "C"          // ["A", "B", "C"]
    (integer) 3
    redis> RPUSH list "D" "E"      // ["A", "B", "C", "D", "E"]
    (integer) 5
    redis> LPOP list               // ["B", "C", "D", "E"]
    "A"
    redis> LPOP list               // ["C", "D", "E"]
    "B"
    redis> RPUSH list "F" "G"      // ["C", "D", "E", "F", "G"]
    (integer) 5

那么光是为了记录这个list键的状态，AOF文件就需要保存六条命令。

对于实际的应用程度来说，写命令执行的次数和频率会比上面的简单示例要高得多，所以造成的问题也会严重得多。

为了解决AOF文件体积膨胀的问题，Redis提供了AOF文件重写（rewrite）功能。通过该功能，Redis服务器可以创建一个新的AOF文件来替代现有的AOF文件，新旧两个AOF文件所保存的数据库状态相同，但新AOF文件不会包含任何浪费空间的冗余命令，所以新AOF文件的体积通常会比旧AOF文件的体积要小得多。

在接下来的内容中，我们将介绍AOF文件重写的实现原理，以及BGREWEITEAOF命令的实现原理。

### 3.1 AOF文件重写的实现

虽然Redis将生成新AOF文件替换旧AOF文件的功能命名为“AOF文件重写”，但实际上，AOF文件重写并不需要对现有的AOF文件进行任何读取、分析或者写入操作，这个功能是通过读取服务器当前的数据库状态来实现的。

考虑上面所述的情况，服务器为了保存当前list键的状态，必须在AOF文件中写入六条命令。如果服务器想要用尽量少的命令来记录list键的状态，那么最简单高效的办法不是去读取和分析现有AOF文件的内容，而是直接从数据库中读取键list的值，然后用一条RPUSH list"C""D""E""F""G"命令来代替保存在AOF文件中的六条命令，这样就可以将保存list键所需的命令从六条减少为一条了。

除了上面列举的列表键之外，其他所有类型的键都可以用同样的方法去减少AOF文件中的命令数量。首先从数据库中读取键现在的值，然后用一条命令去记录键值对，代替之前记录这个键值对的多条命令，这就是AOF重写功能的实现原理。整个重写过程可以用以下伪代码表示：