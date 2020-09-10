## partition_info在语法树上的位置
在类LEX中
```c++
  /* Partition info structure filled in by PARTITION BY parse part */
  partition_info *part_info;
```
所以partition相关的信息都是存在`lex->part_info->...`上的.

接下来我们看几个常用的partition数据结构.

## partition_info

partition_info类中已知的信息有:

```c++
class partition_info : public Sql_alloc
{
public:
  // 链表中存了所有的partition元素
  List<partition_element> partitions;
  // 分区键表达式存在这个Item里
  Item *part_expr;
  // 分区键的字符串, 注意不是以'\0'结尾, 长度为part_func_len
  char *part_func_string;
  // 分区键的列的数目
  uint num_columns;
  // 分区的类型
  partition_type part_type;
  // 分区键字符串的长度
  uint part_func_len;
  // partition元素的个数
  uint num_parts;
  // 是否含有maxvalue
  bool defined_max_value;
};

enum partition_type {
  NOT_A_PARTITION= 0,
  RANGE_PARTITION,
  HASH_PARTITION,
  LIST_PARTITION
};
```

## partition_element

我们再看一下一个partition元素里面有什么:

```c++
class partition_element :public Sql_alloc {
public:
  // partition元素中的值, range分区中是1个, list分区中是多个
  List<part_elem_value> list_val_list;
  // 当是range partition时, 存储元素的值
  longlong range_value;
  // partition元素的名字
  char *partition_name;
  // partition元素的备注
  char* part_comment;
  bool has_null_value;
  bool signed_flag;                          // Range value signed
  // 是否含有maxvalue
  bool max_value;                            // MAXVALUE range
};
```

## part_elem_value

part_elem_value是存储值的数据结构, 有几个标记, 标记是否是unsigned, 是否为NULL.

```c++
/*
  This struct is used to contain the value of an element
  in the VALUES IN struct. It needs to keep knowledge of
  whether it is a signed/unsigned value and whether it is
  NULL or not.
*/

typedef struct p_elem_val
{
  longlong value;
  uint added_items;
  bool null_value;
  bool unsigned_flag;
  part_column_list_val *col_val_array;
} part_elem_value;

/*
  This struct is used to keep track of column expressions as part
  of the COLUMNS concept in conjunction with RANGE and LIST partitioning.
  The value can be either of MINVALUE, MAXVALUE and an expression that
  must be constant and evaluate to the same type as the column it
  represents.

  The data in this fixed in two steps. The parser will only fill in whether
  it is a max_value or provide an expression. Filling in
  column_value, part_info, partition_id, null_value is done by the
  function fix_column_value_function. However the item tree needs
  fixed also before writing it into the frm file (in add_column_list_values).
  To distinguish between those two variants, fixed= 1 after the
  fixing in add_column_list_values and fixed= 2 otherwise. This is
  since the fixing in add_column_list_values isn't a complete fixing.
*/

typedef struct p_column_list_val
{
  void* column_value;
  Item* item_expression;
  partition_info *part_info;
  uint partition_id;
  bool max_value;
  bool null_value;
  char fixed;
} part_column_list_val;
```

part_column_list_val 此结构在range和list分区中, 用于跟踪列表达式。该值可以是MINVALUE、MAXVALUE或者一个表达式，该表达式必须是常量，并且计算结果与它所表示的列的类型相同。
part_column_list_val 中的数据分两步fix。解析器将只填充它是max_value还是提供一个表达式。在`fix_column_value_function`函数中填充column_value, part_info, partition_id, null_value。但是，在将item tree写入frm文件(in add_column_list_values)之前，item tree也需要fix。为了区分这两个变量，在add_column_list_values中修复后fixed=1，否则fixed=2。这是因为add_column_list_values中的修复不是一个完整的修复。

## fix_parser_data

上一节中说到`fix_column_value_function`函数, 但是却找不到, 可能是`fix_parser_data`函数.

该函数的调用堆栈如下:

    partition_info::fix_parser_data
    partition_info::check_partition_info
    mysql_create_frm_image
    create_table_impl
    mysql_create_table_no_lock
    mysql_create_table
    ddl_create_frm

```c++
/**
  Fix partition data from parser.
  解析器修复partition数据.

  @details The parser generates generic data structures, we need to set them
  up as the rest of the code expects to find them. This is in reality part
  of the syntax check of the parser code.
  解析器生成通用的数据结构，我们需要按照代码的其余部分所期望的那样设置它们。
  这实际上是解析器语法检查的一部分。

  It is necessary to call this function in the case of a CREATE TABLE
  statement, in this case we do it early in the check_partition_info
  function.

  It is necessary to call this function for ALTER TABLE where we
  assign a completely new partition structure, in this case we do it
  in prep_alter_part_table after discovering that the partition
  structure is entirely redefined.

  It's necessary to call this method also for ALTER TABLE ADD/REORGANIZE
  of partitions, in this we call it in prep_alter_part_table after
  making some initial checks but before going deep to check the partition
  info, we also assign the column_list variable before calling this function
  here.

  Finally we also call it immediately after returning from parsing the
  partitioning text found in the frm file.

  This function mainly fixes the VALUES parts, these are handled differently
  whether or not we use column list partitioning. Since the parser doesn't
  know which we are using we need to set-up the old data structures after
  the parser is complete when we know if what type of partitioning the
  base table is using.

  For column lists we will handle this in the fix_column_value_function.
  For column lists it is sufficient to verify that the number of columns
  and number of elements are in synch with each other. So only partitioning
  using functions need to be set-up to their data structures.

  @param thd  Thread object

  @return Operation status
    @retval TRUE   Failure
    @retval FALSE  Success
*/
```
