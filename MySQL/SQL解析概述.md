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