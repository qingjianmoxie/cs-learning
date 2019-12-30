# 10.1 The Item Class

To us, the word Item means more than just "thingamabob"; it is a technical term with a precise definition in the context of our source code. Item is a class. Each instance of the Item class has:
对于我们来说, Item这个词仅仅意味着某个没有说明的东西; 但Item是一个技术名词, 它在源码中有准确的定义. Item是一个类. 每个Item类的实例都有

+ an analogue in the SQL language

+ a value

+ a data type descriptor(数据类型描述符)

All of the following SQL "thingamabobs" are modeled in the Item class:

+ literals

+ column references(列引用)

+ session or global variables

+ procedure variables

+ parameters

+ SQL functions (not a surprise since SQL functions have data types and return values).

In the function category we include operators such as + and ||, because operators are merely functions that return values. We also include operators such as = and LIKE, which are operators that return boolean values. Consider the following statement:

```sql
SELECT UPPER(column1) FROM t WHERE column2 = @x;
```

For this statement, MySQL will need to store a list of items for the select list ('column1' column reference and UPPER function), and a list of items for the WHERE clause ('column2' column reference and '@x' variable and '=' operator).

Terminology: an Item instance in a MySQL program roughly corresponds to a "site", which according to the standard_SQL definition is "a place that holds an instance of a value of a specified data type", Another word that you'll see often in MySQL code is "field", which means column reference, and the Item_field subclass is generally for column values that occur for the intersection of a row and column in a table.
术语：MySQL程序中的一个Item实例大致对应一个"site"，根据标准SQL的定义，这个"site"是"存放指定数据类型值的实例的地方"，在MySQL代码中经常看到的另一个词是"field"，也就是列引用，Item_field子类通常用于表中行和列相交时出现的列值。

MySQL's Item class is defined in .../sql/item.h, and its subclasses are defined in .../sql/item*.h (that is, in item.h, item_cmpfunc.h, item_func.h, item_geofunc.h, item_row.h, item_strfunc.h, item_subselect.h, item_sum.h, item_timefunc.h). Page-width limitations prevent us from displaying the whole tree, but these are the main Item subclasses, and the subclasses of the subclasses:

    Item_ident (Item_field, Item_ref)
    Item_null
    Item_num (Item_int, Item_real)
    Item_param
    Item_string (Item_static_string_func, Item_datetime, Item_empty_string)
    Item_hex_string (Item_bin_string)
    Item_result_field (all "item_func.h" "item_subselect.h" "item_sub.h" classes)
    Item_copy_string
    Item_cache (Item_cache_int, Item_cache_real, Item_cache_str, Item_cache_row)
    Item_type_holder
    Item_row

There's no formal classification of subclasses, but the main distinctions are by use (field, parameter, function) and by data type (num, string).

So, how does MySQL use items? You'll find that nearly every .cc program in the /sql directory makes some use of the Item class and its subclasses, so this list of programs is only partial and very general:

    sql_parse.cc:      Makes new items in add_field_to_list()
    item_sum.cc:       Uses item_func subclasses for COUNT, AVG, SUM
    item_buff.cc:      Where buffers for item values can be stored
    item_cmpfunc.cc:   Comparison functions with item_func subclasses
    item_create.cch:   For creating items that the lex might use
    item_subselect.cc: Subqueries are another type of function
    mysqld.cc:         When main() ends, it uses clean_up() for items
    opt_range.cc:      Uses field, compare-condition, and value subclasses
    procedure.cc:      Notice Procedure * has a pointer to an item list
    protocol.cc:       Uses send_fields() to pass item values back to users
    sys_var.cc:        System variables have Item associations too
    sql_base.cc:       Thread-specific Item searchers like find_field_in_table()
    sql_class.cc:      Look at cleanup_after_query()
    sql_delete.cc:     This (like sql_insert.cc etc.) has field references
    sql_error.cc:      Has one of many examples of SHOW's use of items
    sql_lex.cc:        Notice "add...to_list" functions
    sql_select.cc:     The largest program that uses items, apparently
    udf_example.cc:    The comments in this program are extensive

Whenever there's a need for an SQL operation that assigns, compares, aggregates, accepts, sends, or validates a site, you'll find a MySQL use of Item and its subclasses.
每当需要一个分配、比较、聚合、接受、发送或验证site的SQL操作时，你就会发现Item及其子类在MySQL中的使用。