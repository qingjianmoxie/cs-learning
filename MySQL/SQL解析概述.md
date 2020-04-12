# SQL解析概述

由于这几年MySQL数据库中间件的兴起，需要支持读写分离、分库分表等功能，就必须从SQL中抽出表名、库名以及相关字段的值。因此像Java语言编写的Druid，C语言编写的MaxScale，Go语言编写的Kingshard等，都会对SQL进行部分解析。而真正把SQL解析技术用于数据库维护的产品较少，主要有如下几个：

美团点评开源的SQLAdvisor。它基于MySQL原生态词法解析，结合分析SQL中的where条件、聚合条件、多表Join关系给出索引优化建议。

1. 词法解析

通常情况下，词法分析可以使用Flex来生成。

但是mariadb并未使用该工具，而是手写了词法分析部分。具体代码在sql/lex.h和sql/sql_lex.cc文件中。

词法分析的核心代码在sql/sql_lex.cc文件中的MYSQLlex->lex_one_token

2. 词法分析

在大家阅读sql_yacc.yy代码的过程中，会发现Bison中嵌入了c++的代码。通过c++代码，把解析到的信息存储到相关对象中。例如表信息会存储到TABLE_LIST中，order_list存储order by子句里的信息，where字句存储在Item中。有了这些信息，再辅助以相应的算法就可以对SQL进行更进一步的处理了。

在SQL解析中，最核心的结构是SELECT_LEX，其定义在sql/sql_lex.h中。下面仅列出部分内容。

SELECT_LEX:
```
        List<Item> item_list              Item *where
            |                                 |
            V                                 V
select username, ismale from userinfo where age > 20 and level > 5 and 1 =1
                               ^
                               |
                            SQL_I_List<TABLE_LIST> table_list
```
上面图示中，列名username、ismale存储在item_list中，表名存储在table_list中，条件存储在where中。其中以where条件中的Item层次结构最深，表达也较为复杂，如下图所示：

[](http://file.elecfans.com/web1/M00/55/52/o4YBAFsrHBWABOAuAABruwtC-0w894.jpg)

MySQL官方文档专门介绍了Item类。

参考链接：https://dev.mysql.com/doc/internals/en/item-class.html

阿里的MySQL小组也有类似的文章。如需更详细的了解，就需要去查看源码中sql/item_*等文件。

[Bison的官方文档](http://www.gnu.org/software/bison/manual/bison.html)

MySQL源码分析（1）：源码结构及主要数据结构

经过多年的发展，mysql的主要模块已经稳定，基本不会有大的修改。本文将对MySQL的整体架构及重要目录进行讲述。

1. 源码结构（MySQL-5.5.0-m2）

BUILD: 内含在各个平台、各种编译器下进行编译的脚本。如compile-pentium-debug表示在pentium架构上进行编译的脚本。

Client: 客户端工具，如mysql, mysqladmin之类。

Cmd-line-utils: readline, libedit工具。

Config: 给aclocal使用的配置文件。

Dbug: 提供一些调试用的宏定义。

Extra: 提供innochecksum，resolveip等额外的小工具。

Include: 包含的头文件

Libmysql: 库文件，生产libmysqlclient.so。

Libmysql_r: 线程安全的库文件，生成libmysqlclient_r.so。

Libservices: 5.5.0中新加的目录，实现了打印功能。

Man: 手册页。

Mysql-test: mysqld的测试工具一套。

Mysys:包含了对于系统调用的封装，用以方便实现跨平台。MySQL自己实现了一套常用的数据结构和算法，如string, hash等。

Netware: 在netware平台上进行编译时需要的工具和库。

Plugin:插件的目录，目前有一个全文搜索插件（只能用在myisam存储引擎）。

Pstack: 异步栈追踪工具。

Regex: 正则表达式工具。

Scripts: 提供脚本工具，如mysql_install_db等

Sql:
mysql主要代码，数据库主程序mysqld所在的地方。大部分的系统流程都发生在这里。你还能看到sql_insert.cc, sql_update.cc, sql_select.cc，等等，分别实现了对应的SQL命令。后面我们还要经常提到这个目录下的文件。
大概有如下及部分:
SQL解析器代码: sql_lex.cc, sql_yacc.yy, sql_yacc.cc, sql_parse.cc等，实现了对SQL语句的解析操作。
"handler"代码: handle.cc, handler.h，定义了存储引擎的接口。
"item"代码：item_func.cc, item_create.cc，定义了SQL解析后的各个部分。
SQL语句执行代码: sql_update.cc, sql_insert.cc sql_select.cc, sql_show.cc, sql_load.cc，执行SQL对应的语句。当你要看"SELECT ..."的执行的时候，直接到sql_select.cc去看就OK了。
辅助代码: net_serv.cc实现网络操作
还有其他很多代码。


Sql-bench: 一些评测代码。

Sql-common: 存放部分服务器端和客户端都会用到的代码。

Storage：

这个目录包含了所谓的Mysql存储引擎 (storage engine)。存储引擎是数据库系统的核心，封装了数据库文件的操作，是数据库系统是否强大最重要的因素。Mysql实现了一个抽象接口层，叫做handler(sql/handler.h)，其中定义了接口函数，比如：ha_open, ha_index_end, ha_create等等，存储引擎需要实现这些接口才能被系统使用。这个接口定义超级复杂，有900多行 :-(，不过我们暂时知道它是干什么的就好了，没必要深究每行代码。对于具体每种引擎的特点，我推荐大家去看mysql的在线文档: http://dev.mysql.com/doc/refman/5.1/en/storage-engines.html

应该能看到如下的目录:

* innobase, innodb的目录，当前最流行的存储引擎

* myisam, 最早的Mysql存储引擎,一直到innodb出现以前，使用最广的引擎。

* heap, 基于内存的存储引擎

* federated, 一个比较新的存储引擎

* example, csv，这几个大家可以作为自己写存储引擎时的参考实现，比较容易读懂


Strings: string库。

Support-files: my.cnf示例配置文件。

Tests: 测试文件所在目录。

Unittest: 单元测试。

Vio:封装了virtual IO接口，主要是封装了各种协议的网络操作。

Win: 给windows平台提供的编译环境。

Zip: zip库工具


2. 主要数据结构

THD 线程描述符(sql/sql_class.h)
包含处理用户请求时需要的相关数据，每个连接会有一个线程来处理，在一些高层函数中，此数据结构常被当作第一个参数传递。

NET   net; // 客户连接描述符

Protocol *protocol; // 当前的协议

Protocol_text   protocol_text; // 普通协议

Protocol_binary protocol_binary; // 二进制协议

HASH    user_vars; //用户变量的hash值

String  packet; // 网络IO时所用的缓存

String  convert_buffer;               // 字符集转换所用的缓存

struct  sockaddr_in remote; //客户端socket地址

THR_LOCK_INFO lock_info; // 当前线程的锁信息

THR_LOCK_OWNER main_lock_id; // 在旧版的查询中使用

THR_LOCK_OWNER *lock_id; //若非main_lock_id, 指向游标的lock_id

pthread_mutex_t LOCK_thd_data; //thd的mutex锁，保护THD数据（thd->query, thd->query_length）不会被其余线程访问到。

Statement_map stmt_map; //prepared statements和stored routines 会被重复利用

int insert(THD *thd, Statement *statement); // statement的hash容器

class Statement::

LEX_STRING name; /* prepared statements的名字 */

LEX *lex; //语法树描述符

bool set_db(const char *new_db, size_t new_db_len)

void set_query(char *query_arg, uint32 query_length_arg);

{

pthread_mutex_lock(&LOCK_thd_data);

set_query_inner(query_arg, query_length_arg);

pthread_mutex_unlock(&LOCK_thd_data);

}


NET 网络连接描述符（sql/mysql_com.h）
网络连接描述符，对内部数据包进行了封装，是client和server之间的通信协议。

Vio *vio; //底层的网络I/O socket描述符

unsigned char *buff,*buff_end,*write_pos,*read_pos; //缓存相关

unsigned long remain_in_buf,length, buf_length, where_b;

unsigned long max_packet,max_packet_size; //当前值;最大值

unsigned int pkt_nr,compress_pkt_nr; //当前（未）压缩包的顺序值

my_bool compress; //是否压缩

unsigned int write_timeout, read_timeout, retry_count; //最大等待时间

unsigned int *return_status; //thd中的服务器状态

unsigned char reading_or_writing;

/*1 代表读, 2 代表写, 0代表无状态 */

unsigned int last_errno; //返回给客户端的错误号

unsigned char error;

/*0：执行成功

1：在协议层有逻辑错误

2：系统调用或标准库出错

3：特例，表示缓存不能装下当前这么大的包

*/


TABLE 数据库表描述符（sql/table.h）
数据库表描述符，分成TABLE和TABLE_SHARE两部分。
```c++
handler *file; //指向这张表在storage engine中的handler的指针

THD *in_use;  /* 使用这张表的thread号 */

Field **field; /* 指向数据域的指针*/

uchar *record[2]; /* 指向记录的指针*/

uchar *write_row_record; /* 在THD::write_row中用来做优化 */

uchar *insert_values;                  /* INSERT ... UPDATE语句用 */

/*

可用来直接获取表中数据而不用读取行的当前key的映射值

*/

key_map covering_keys;

key_map quick_keys, merge_keys;

key_map keys_in_use_for_query;

/* 可用以生成GROUP BY结果的key映射值 */

key_map keys_in_use_for_group_by;

/* 可用以生成ORDER BY 结果的key映射值 */

key_map keys_in_use_for_order_by;

KEY  *key_info; /* 数据库中key的信息*/

HASH name_hash; //数据域名字的hash值

MEM_ROOT mem_root; //内存块

LEX_STRING db;

LEX_STRING table_name;

LEX_STRING table_cache_key;

enum db_type db_type //当前表的storage engine类型

enum row_type row_type //当前记录是定长还是变长

uint primary_key;

uint next_number_index; //自动增长key的值

bool is_view ;

bool crashed;
```

FIELD 字段描述符（sql/field.h）
域描述符，是各种字段的抽象基类。
```c++
uchar *ptr; // 记录中数据域的位置

uchar *null_ptr; // 记录 null_bit 位置的byte

TABLE *table;           // 指向表的指针

TABLE *orig_table;       // 指向原表的指针

const char **table_name, *field_name;

LEX_STRING comment;

/* 数据域是下列key的一部分 */

key_map key_start, part_of_key, part_of_key_not_clustered;

key_map       part_of_sortkey;

/*各种数据域的类型*/

enum utype  { NONE,DATE,SHIELD,NOEMPTY,CASEUP,PNR,BGNR,PGNR,YES,NO,REL,

CHECK,EMPTY,UNKNOWN_FIELD,CASEDN,NEXT_NUMBER,INTERVAL_FIELD,

BIT_FIELD, TIMESTAMP_OLD_FIELD, CAPITALIZE, BLOB_FIELD,

TIMESTAMP_DN_FIELD, TIMESTAMP_UN_FIELD, TIMESTAMP_DNUN_FIELD};

…..

virtual int  store(const char *to, uint length,CHARSET_INFO *cs)=0;

inline String *val_str(String *str) { return val_str(str, str); }
```

Utility API Calls 各种API
各种核心的工具，例如内存分配，字符串操作或文件管理。标准C库中的函数只使用了很少一部分，C++中的函数基本没用。

void *my_malloc(size_t size, myf my_flags) //对malloc的封装

size_t my_write(File Filedes, const uchar *Buffer, size_t Count, myf MyFlags) //对write的封装


Preprocessor Macros 处理器宏
Mysql中使用了大量的C预编译，随编译参数的不同最终代码也不同。
```c++
#define max(a, b) ((a) > (b) ? (a) : (b)) //得出两数中的大者

do \

{ \

char compile_time_assert[(X) ? 1 : -1] \

__attribute__ ((unused)); \

} while(0)
```

使用gcc的attribute属性指导编译器行为

+ Global variables 全局变量

+ configuration settings

+ server status information

+ various data structures shared among threads

主要包括一些全局的设置，服务器信息和部分线程间共享的数据结构。

struct system_status_var global_status_var; //全局的状态信息

struct system_variables global_system_variables; //全局系统变量

3. 主要调用流程
MySQL启动
主要代码在sql/mysqld.cc中，精简后的代码如下：
int main(int argc, char **argv) //标准入口函数
MY_INIT(argv[0]); //调用mysys/My_init.c->my_init()，初始化mysql内部的系统库
logger.init_base(); //初始化日志功能
init_common_variables(MYSQL_CONFIG_NAME,argc, argv, load_default_groups) //调用load_defaults(conf_file_name, groups, &argc, &argv)，读取配置信息
user_info= check_user(mysqld_user);//检测启动时的用户选项
set_user(mysqld_user, user_info);//设置以该用户运行
init_server_components();//初始化内部的一些组件，如table_cache, query_cache等。
network_init();//初始化网络模块，创建socket监听
start_signal_handler();// 创建pid文件
mysql_rm_tmp_tables() || acl_init(opt_noacl)//删除tmp_table并初始化数据库级别的权限。
init_status_vars(); // 初始化mysql中的status变量
start_handle_manager();//创建manager线程
handle_connections_sockets();//主要处理函数，处理新的连接并创建新的线程处理之

监听接收链接
主要代码在sql/mysqld.cc中，精简后的代码如下：
THD *thd;
FD_SET(ip_sock,&clientFDs); //客户端socket
while (!abort_loop)
readFDs=clientFDs;
if (select((int) max_used_connection,&readFDs,0,0,0) error && net->vio != 0 &&
!(thd->killed == THD::KILL_CONNECTION))
{
if(do_command(thd)) //处理客户端发出的命令
break;
}
end_connection(thd);
}

预处理连接
thread_count++;//增加当前连接的线程
thread_scheduler.add_connection(thd);
for (;;) {
lex_start(thd);
login_connection(thd); // 认证
prepare_new_connection_state(thd); //初始化thd描述符
while(!net->error && net->vio != 0 &&
!(thd->killed == THD::KILL_CONNECTION))
{
if(do_command(thd)) //处理客户端发出的命令
break;
}
end_connection(thd);
}

处理
do_command在sql/sql_parse.cc中：读取客户端传递的命令并分发。
NET *net= &thd->net;
packet_length= my_net_read(net);
packet= (char*) net->read_pos;
command= (enum enum_server_command) (uchar) packet[0]; //从net结构中获取命令
dispatch_command(command, thd, packet+1, (uint) (packet_length-1));//分发命令
在dispatch_command函数中，根据命令的类型进行分发。
thd->command=command;
switch( command ) {
case COM_INIT_DB: ...;
case COM_TABLE_DUMP: ...;
case COM_CHANGE_USER: ...;
….
case COM_QUERY: //如果是查询语句
{
alloc_query(thd, packet, packet_length)//thd->set_query(query, packet_length);
mysql_parse(thd, thd->query(), thd->query_length(), &end_of_stmt);
// 解析查询语句
….
}
在mysql_parse函数中，
lex_start(thd);
if (query_cache_send_result_to_client(thd, (char*) inBuf, length) sql_command
在mysql_execute_command中，根据命令类型，转到相应的执行函数。
switch (lex->sql_command) {
LEX *lex= thd->lex;
TABLE_LIST *all_tables;
case SQLCOM_SELECT:
check_table_access(thd, lex->exchange ? SELECT_ACL | FILE_ACL : SELECT_ACL, all_tables, UINT_MAX, FALSE); //检查用户权限
execute_sqlcom_select(thd, all_tables); //执行select命令
break;

case SQLCOM_INSERT:
{ res= insert_precheck(thd, all_tables) //rights
mysql_insert(thd, all_tables, lex->field_list, lex->many_values,
lex->update_list, lex->value_list,
lex->duplicates, lex->ignore);
break;
在execute_sqlcom_select函数中，
res= open_and_lock_tables(thd, all_tables)//directly and indirectly
res= handle_select(thd, lex, result, 0);
handle_select在sql_select.cc中，调用mysql_select ，在mysql_select中，
join->prepare();//Prepare of whole select (including sub queries in future).
join->optimize();//global select optimisation.
join->exec();//
在mysql_insert函数中，
open_and_lock_tables(thd, table_list)
mysql_prepare_insert(); //prepare item in INSERT statment
while ((values= its++))
write_record(thd, table ,&info);//写入新的数据
在write_record函数中，
table->file->ha_write_row(table->record[0])
ha_write_row在Handler.cc中，只是一个接口
write_row(buf); //调用表存储所用的引擎

当客户端链接上mysql服务端时，系统为其分配一个链接描述符thd，用以描述客户端的所有信息，将作为参数在各个模块之间传递。一个典型的客户端查询在MySQL的主要模块之间的调用关系如下图所示：


当mysql启动完毕后，调用handle_connection_sockets等待客户端连接。当客户端连接上服务器时，服务处理函数将接受连 接，会为其创建链接线程，并进行认证。如认证通过，每个连接线程将会被分配到一个线程描述符thd，可能是新创建的，也可能是从 cached_thread线程池中复用的。该描述符包含了客户端输入的所有信息，如查询语句等。服务器端会层层解析命令，根据命令类型的不同，转到相应 的sql执行函数，进而给传递给下层的存储引擎模块，处理磁盘上的数据库文件，最后将结果返回。执行完毕后thd将被加入cached_thread中。
