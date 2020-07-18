# MySQL5.7 的 JSON 实现

## 介绍

本文主要介绍在MySQL 5.7.7开始引入的非结构化数据类型JSON的特性以及具体的实现方式(包括存储方式)。  
在介绍 MySQL 5.7 的非结构化数据存储之前，首先介绍在之前的版本中，用户如何通过 BLOB 实现 JSON 对象的存储，以及这样处理的缺点是什么，这些缺点也就是 MySQL 5.7 支持 JSON 的理由;  
接着介绍MySQL给用户提供的JSON操作函数，以及JSON路径表达式语法，结合两者，用户可以在数据库级别操作JSON的任意键值和数据;  
最后，重点介绍JSON在服务器侧的存储结构，这也是深入理解很多其他JSON特性的根基；在最后介绍JSON作为新数据类型的比较与排序规则之前，介绍了对JSON类型数据建立索引的原理。

## 5.7 之前 BLOB 方式实现 JSON 对象的存储

MySQL 是一个关系型数据库，在 MySQL 5.7 之前，没有提供对非结构化数据的支持，但是如果用户有这样的需求，也可以通过 MySQL 的 BLOB 来存储非结构化的数据。如下所示：

```sql
mysql> create table t(json_data blob);
Query OK, 0 rows affected (0.13 sec)

mysql> insert into t values('{"key1":"data1", "key2":2, "key3":{"sub_key1":"sub_val1"}}');
Query OK, 1 row affected (0.01 sec)

mysql> select * from t;
+------------------------------------------------------------+
| json_data                                                  |
+------------------------------------------------------------+
| {"key1":"data1", "key2":2, "key3":{"sub_key1":"sub_val1"}} |
+------------------------------------------------------------+
1 row in set (0.00 sec)
```

在本例中，我们使用 BLOB 来存储 JSON 数据，使用这种方法，需要用户保证插入的数据是一个能够转换成 JSON 格式的字符串，MySQL 并不保证任何正确性。在MySQL看来，这就是一个普通的字符串，并不会进行任何有效性检查，此外提取 JSON 中的字段，也需要用户的代码中完成，如下所示：

```py
#!/usr/bin/python

import pymysql
import json

try:
    conn = pymysql.connect(host="127.0.0.1", db="test", user="root", passwd="root", port=7799)
    sql = "select * from t"
    cur = conn.cursor()
    cur.execute(sql)
    rows =  cur.fetchall()
    print json.dumps(json.loads(rows[0][0]), indent=4)
except:
    conn.close()
```

执行python脚本的结果如下所示：

    root@dev1:~# python test.py
    {
        "key3": {
            "sub_key1": "sub_val1"
        },
        "key2": 2,
        "key1": "data1"
    }

这种方式虽然也能够实现 JSON 的存储，但是有诸多缺点，最为显著的缺点有：

1. 需要用户保证 JSON 的正确性，如果用户插入的数据并不是一个有效的 JSON 字符串，MySQL 并不会报错；
2. 所有对 JSON 的操作，都需要在用户的代码里进行处理，不够友好；
3. 即使只是提取 JSON 中某一个字段，也需要读出整个 BLOB，效率不高；
4. 无法在 JSON 字段上建索引。

## 为什么需要JSON的原生支持

1. 文档合法性

如上文所述, 在MySQL5.7.7对JSON提供原生类型的支持之前，用户可以用TEXT或者BLOB类型来存储JSON文档。但对于MySQL来说，用户插入的数据只是序列化后的一个普通的字符串，不会对JSON文档本身的语法合法性做检查，文档的合法性需要用户自己保证。在引入新的JSON类型之后，插入语法错误的JSON文档，MySQL会提示错误，并在插入之后做归一化处理，保证每一个键对应一个值。

2. 更有效的访问

MySQL 5.7.7+本身提供了很多原生的函数以及路径表达式来方便用户访问JSON数据。例如对于下面的JSON文档：

```json
{ "a": [ [ 3, 2 ], [ { "c" : "d" }, 1 ] ], "b": { "c" : 6 }, "one potato": 7, "b.c" : 8 }
```

用户可以使用

`$.a[1][0]`获取`{ "c" : "d" }`

`$.a[1]`获取`[ { "c" : "d" }, 1 ]`

还可以使用通配符 `*` 和 `**` 来进行模糊匹配，详见下一段。

3. 性能优化

在MySQL提供JSON原生支持之前，如果用户需要获取或者修改某个JSON文档的键值，需要把TEXT或者BLOB整个字符串读出来反序列化成JSON对象，然后通过各种库函数访问JSON数据。显然这样是非常没有效率的，特别是对较大的文档。而原生JSON的性能，特别是读性能非常好。根据Oracle公司针对200K+数据文档做的性能测试表明，同样的数据用TEXT和JSON类型的查询性能差异达到两个数量级以上，而且用户还可以对经常访问的JSON键值做索引，进一步提升性能。JSON数据操作性能的提升是基于JSON数据本身的存储结构的，下文会进一步介绍。

## 5.7中的JSON实现

MySQL本身已经是一个比较完备的数据库系统，对于底层存储并不适合有太大的改动，那么 MySQL 是如何支持 JSON 格式的呢？说来也巧，和我们前面的做法几乎一样----通过 BLOB 来存储。也就是说，MySQL 5.7支持 JSON 的做法是，在server层提供了一堆便于操作 JSON 的函数，至于存储，就是简单地将 JSON 编码成 BLOB，然后交由存储引擎层进行处理，也就是说，MySQL 5.7的JSON 支持与存储引擎没有关系，MyISAM 存储引擎也支持 JSON 格式，如下所示：

```sql
mysql> create table t_innodb(data json)engine=innodb;
Query OK, 0 rows affected (0.18 sec)

mysql> insert into t_innodb values('{"key":"val"}');
Query OK, 1 row affected (0.03 sec)

mysql> create table t_myisam(data json)engine=myisam;
Query OK, 0 rows affected (0.02 sec)

mysql> insert into t_myisam values('{"key":"val"}');
Query OK, 1 row affected (0.00 sec)

```

### JSON的操作接口及路径表达式

MySQL 5.7 提供了很多操作 JSON 的函数，都是为了提高易用性，可以参考[官方文档](https://dev.mysql.com/doc/refman/5.7/en/json-function-reference.html)。

1. JSON的操作接口

根据MySQL官方文档的介绍，服务器端JSON函数的实现需要满足以下条件：

Requirements:

   1. Lets users construct JSON data values from other relational data.
   2. Lets users extract relational data from JSON data values.
   3. Lets users minimally introspect the structure of JSON values and text (validity, length, depth, keys).
   4. Works on strings which are utf8mb4 encoded.
   5. Performance should be suitable for read-intensive applications.

Non-requirements:

   1. May produce surprising results on strings which are not utf8mb4 encoded.
   2. There is limited support for decimal values nested inside JSON documents.
   3. Performance may not be suitable for write-intensive applications.

提供的函数列表具体为：

    JSON_APPEND() JSON_ARRAY_INSERT() JSON_UNQUOTE() JSON_ARRAY()
    JSON_REPLACE() JSON_CONTAINS() JSON_DEPTH() JSON_EXTRACT()
    JSON_INSERT() JSON_KEYS() JSON_LENGTH() JSON_VALID()
    JSON_MERGE() JSON_OBJECT() JSON_QUOTE() JSON_REMOVE()
    JSON_CONTAINS_PATH() JSON_SEARCH() JSON_SET() JSON_TYPE()

以上函数的调用规则大多形如：

    JSON_APPEND(json_doc, path, val[, path, val] ...)

第一个参数`json_doc`为JSON文档，或者是表里面的某一列，也可以是JSON文档里面的嵌套子文档变量；

第二个参数`path`为路径表达式，用来定位要访问的键，`path`(即路径表达式)下面紧接着会介绍；

第三个参数`val`有的函数可能没有，若有表示键对应的操作数值。

2. JSON路径表达式

为了更方便快速的访问JSON的键值，MySQL 5.7.7+提供了新的路径表达式语法支持。前文提到的`$.a[1][0]`就是路径表达式的一个具体的示例。完整的路径表达式语法为：

    pathExpression> ::= scope  [ ( pathLeg )* ]
    scope ::= [ columnReference ] dollarSign
    columnReference ::= [ [ databaseIdentifier period  ] tableIdentifier period ] columnIdentifier
    databaseIdentifier ::= sqlIdentifier
    tableIdentifier ::= sqlIdentifier
    columnIdentifier ::= sqlIdentifier
    pathLeg ::= member | arrayLocation | doubleAsterisk
    member ::= period ( keyName | asterisk )
    arrayLocation ::= leftBracket ( non-negative-integer | asterisk ) rightBracket
    keyName ::= ECMAScript-identifier | double-quoted-string-literal
    doubleAsterisk ::= **

还是以

```json
{ "a": [ [ 3, 2 ], [ { "c" : "d" }, 1 ] ], "b": { "c" : 6 }, "one potato": 7, "b.c" : 8 }
```

为例，再举几个例子说明：

`$.a[1]` 获取的值为 `[ { "c" : "d" }, 1 ]`

`$.b.c` 获取的值为 6

`$."b.c"` 获取的值为 8

对比上面最后两个例子，可以看到用引号包围的表达式会被当作一个字符串键值。

关于通配符`*`和`**`来进行模糊匹配需要做进一步的说明。

两个连着星号`**`不能作为表达式的结尾，不能出现连续的三个星号`***`

单个星号`*`表示匹配某个JSON对象中所有的成员

`[*]`表示匹配某个JSON数组中的所有元素

`prefix**suffix`表示所有以prefix开始，以suffix结尾的路径

举个具体的例子，直接在MySQL命令行里面输入：

```sql
select json_extract('{ "a": [ [ 3, 2 ], [ { "c" : "d" }, 1 ] ], "b": { "c" : 6 }, "one potato": 7, "b.c" : 8 }','$**.c');
```

得到显示结果：["d", 6]。

### JSON的存储结构及具体实现

在处理JSON时，MySQL使用的utf8mb4字符集，utf8mb4是utf8和ascii的超集。由于历史原因，这里utf8并非是我们常说的UTF-8 Unicode变长编码方案，而是MySQL自身定义的utf8编码方案，最长为三个字节。具体区别非本文重点，请大家自行Google了解。

MySQL在内存中是以DOM的形式表示JSON文档，而且在MySQL解析某个具体的路径表达式时，只需要反序列化和解析路径上的对象，而且速度极快。要弄清楚MySQL是如何做到这些的，我们就需要了解JSON在硬盘上的存储结构。有个有趣的点是，JSON对象是BLOB的子类，在其基础上做了特化。

根据MySQL官方文档的表述：

On a high level, we will store the contents of the JSON document in three sections(部分):

A table of pointers to all the keys and values, in the order in which the keys and values are stored. Each pointer contains information about where the data associated with the key or the value is located, as well as type information about the key or value pointed to.

*All the keys. The keys are sorted, so that lookup can use binary search to locate the key quickly.

All the values, in the same order as their corresponding keys.
If the document is an array, it has two sections only: the dictionary and the values.
If the document is a scalar(标量), it has a single section which contains the scalar value

关于MySQL 5.7的JSON存储，MySQL的源码里写得比较清楚，在sql/json_binary.h中有下面这段注释：

```
  If the value is a JSON object, its binary representation will have a
  header that contains:

  - the member count
  - the size of the binary value in bytes
  - a list of pointers to each key
  - a list of pointers to each value

  The actual keys and values will come after the header, in the same
  order as in the header.

  Similarly, if the value is a JSON array, the binary representation
  will have a header with

  - the element count
  - the size of the binary value in bytes
  - a list of pointers to each value

  followed by the actual values, in the same order as in the header.
```

从注释里面我们可以知道，对于JSON数组和JSON对象，MySQL如何编码成BLOB对象，数组比较简单，下面给出JSON对象的示意图（见json_binary.cc中的`serialize_json_object`函数），如下所示：

![JSON 对象](http://mysql.taobao.org/monthly/pic/2016-01-03/json-objects.png)
![JSON 对象](https://mc.qcloudimg.com/static/img/f6e178a22a7c6727a4778e8d3cb7a815/image.png)

<center>JSON 对象</center>

JSON文档本身是层次化的结构，因而MySQL对JSON存储也是层次化的。对于每一级对象，存储的最前面为存放当前对象的元素个数，然后存放的是转换成 BLOB 以后的字节数，接下来存放的是key pointers和value pointers。

此外，对于key pointers，有如下注释：

```c++
/*
  The size of key entries for objects when using the small storage
  format or the large storage format. In the small format it is 4
  bytes (2 bytes for key length and 2 bytes for key offset). In the
  large format it is 6 (2 bytes for length, 4 bytes for offset).
*/
#define KEY_ENTRY_SIZE_SMALL      (2 + SMALL_OFFSET_SIZE)
#define KEY_ENTRY_SIZE_LARGE      (2 + LARGE_OFFSET_SIZE)
```

也就是说，在MySQL 5.7中，key的长度只用2个字节保存（65535），如果超过这个长度，MySQL将报错，如下所示：

```sql
mysql> insert into t1 values(JSON_OBJECT(repeat('a', 65535), 'val'));
Query OK, 1 row affected (0.37 sec)

mysql> insert into t1 values(JSON_OBJECT(repeat('a', 65536), 'val'));
ERROR 3151 (22032): The JSON object contains a key name that is too long.
```

如果查看MySQL的源码，可以看到，与JSON相关的文件有：

    json_binary.cc
    json_binary.h
    json_dom.cc
    json_dom.h
    json_path.cc
    json_path.h

其中，json_binary 处理JSON 的编码、解码，json_dom 是 JSON 的内存表示，json_path 用以将字符串解析成 JSON，具体说明见[WL#7909](http://dev.mysql.com/worklog/task/?id=7909)。

对于 JSON 的编码，入口是json_binary.cc 文件中的`serialize`函数，对于 JSON 的解码，即将 BLOB 解析成 JSON 对象，入口是json_binary.cc文件中的`parse_binary`函数，只要搞清楚了 JSON 的存储格式，这两个函数是很好理解的。

需要注意的是：

+ JSON对象的Key索引（图中橙色部分）都是排序好的，先按长度排序，长度相同的按照code point排序；Value索引（图中黄色部分）根据对应的Key的位置依次排列，最后面真实的数据存储（图中白色部分）也是如此
+ Key和Value的索引对存储了对象内的偏移和大小，单个索引的大小固定，可以通过简单的算术跳转到距离为N的索引
+ 通过MySQL5.7.16源代码可以看到，在序列化JSON文档时，MySQL会动态检测单个对象的大小，如果小于64KB使用两个字节的偏移量，否则使用四个字节的偏移量，以节省空间。同时，动态检查单个对象是否是大对象，会造成对大对象进行两次解析，源代码中也指出这是以后需要优化的点
+ 现在受索引中偏移量和存储大小四个字节大小的限制，单个JSON文档的大小不能超过4G；单个KEY的大小不能超过两个字节，即64K
+ 索引存储对象内的偏移是为了方便移动，如果某个键值被改动，只用修改受影响对象整体的偏移量
+ 索引的大小现在是冗余信息，因为通过相邻偏移可以简单的得到存储大小，主要是为了应对变长JSON对象值更新，如果长度变小，JSON文档整体都不用移动，只需要当前对象修改大小
+ 现在MySQL对于变长大小的值没有预留额外的空间，也就是说如果该值的长度变大，后面的存储都要受到影响
+ 结合JSON的路径表达式可以知道，JSON的搜索操作只用反序列化路径上涉及到的元素，速度非常快，实现了读操作的高性能
+ 不过，MySQL对于大型文档的变长键值的更新操作可能会变慢，可能并不适合写密集的需求

## JSON的索引

现在MySQL不支持对JSON列进行索引，官网文档的说明是：

JSON columns cannot be indexed. You can work around this restriction by creating an index on a generated column that extracts(提取) a scalar value from the JSON column.

虽然不支持直接在JSON列上建索引，但MySQL规定，可以首先使用路径表达式对JSON文档中的标量值建立虚拟列，然后在虚拟列上建立索引。这样用户可以使用表达式对自己感兴趣的键值建立索引。举个具体的例子来说明：

```sql
CREATE TABLE features (
 id INT NOT NULL AUTO_INCREMENT,
 feature JSON NOT NULL,
 PRIMARY KEY (id)
);
```

插入它的JSON数据的格式为：

```json
{
   "type":"Feature",
   "properties":{
      "TO_ST":"0",
      "BLKLOT":"0001001",
      "STREET":"UNKNOWN",
      "FROM_ST":"0",
      "LOT_NUM":"001",
      "ST_TYPE":null,
      "ODD_EVEN":"E",
      "BLOCK_NUM":"0001",
      "MAPBLKLOT":"0001001"
   }
}
```

使用：

```sql
ALTER TABLE features ADD feature_street VARCHAR(30) AS (JSON_UNQUOTE(feature->"$.properties.STREET"));
ALTER TABLE features ADD INDEX (feature_street);
```

两个步骤，可以对feature列中properties键值下的STREET键(`feature->"$.properties.STREET"`)创建索引。

其中，`feature_street`列就是新添加的虚拟列。之所以取名虚拟列，是因为与它对应的还有一个存储列(stored column)。它们最大的区别为虚拟列只修改数据库的metadata，并不会存储真实的数据在硬盘上，读取过程也是实时计算的方式；而存储列会把表达式的列存储在硬盘上。两者使用的场景不一样，默认情况下通过表达式生成的列为虚拟列。

这样虚拟列的添加和删除都会非常快，而在虚拟列上建立索引跟传统的建立索引的方式并没有区别，会提高虚拟列读取的性能，减慢整体插入的性能。虚拟列的特性结合JSON的路径表达式，可以方便的为用户提供高效的键值索引功能。

## JSON比较与排序

JSON值可以使用=, <, <=, >, >=, <>, !=, <=>等操作符，BETWEEN, IN, GREATEST, LEAST等操作符现在还不支持。JSON值使用的两级排序规则，第一级基于JSON的类型，类型不同的使用每个类型特有的排序规则。

JSON类型按照优先级从高到低为

    BLOB
    BIT
    OPAQUE
    DATETIME
    TIME
    DATE
    BOOLEAN
    ARRAY
    OBJECT
    STRING
    INTEGER, DOUBLE
    NULL

优先级高的类型大，不用再进行其他的比较操作；如果类型相同，每个类型按自己的规则排序。具体的规则如下：

1. BLOB/BIT/OPAQUE: 比较两个值前N个字节，如果前N个字节相同，短的值小
2. DATETIME/TIME/DATE: 按照所表示的时间点排序
3. BOOLEAN: false小于true
4. ARRAY: 两个数组如果长度和在每个位置的值相同时相等，如果不相等，取第一个不相同元素的排序结果，空元素最小
5. OBJECT: 如果两个对象有相同的KEY，并且KEY对应的VALUE也都相同，两者相等。否则，两者大小不等，但相对大小未规定。
6. STRING: 取两个STRING较短的那个长度为N，比较两个值utf8mb4编码的前N个字节，较短的小，空值最小
7. INTEGER/DOUBLE: 包括精确值和近似值的比较，稍微有点复杂，可能出现与直觉相悖的结果，具体参见[官方文档](https://dev.mysql.com/doc/refman/5.7/en/json.html#json-comparison)相关说明。

任何JSON值与SQL的NULL常量比较，得到的结果是UNKNOWN。对于JSON值和非JSON值的比较，按照一定的[规则](https://dev.mysql.com/doc/refman/5.7/en/json.html#json-comparison)将非JSON值转化为JSON值，然后按照以上的规则进行比较。

## 参考链接

[MySQL5.7 的 JSON 实现](http://mysql.taobao.org/monthly/2016/01/03/)
[MySQL 5.7 JSON 实现简介](https://cloud.tencent.com/developer/article/1004449)
