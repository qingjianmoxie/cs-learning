lex.h文件中增加关键字
```c++
static SYMBOL symbols[] = {
  ...
  { "JSON",		SYM(JSON_SYM)},
  ...
};
```

yacc解析中, field_type将json解析为`MYSQL_TYPE_LONG_BLOB`类型

**sql_yacc.yy**
```c++
field_type:
        | JSON_SYM
          {
            Lex->charset= &my_charset_utf8mb4_bin;
            $$.set(MYSQL_TYPE_LONG_BLOB);
          }
```

json函数都写在`item_jsonfunc.h`和`item_jsonfunc.cc`文件中.

json函数是怎么执行的呢?

**item_create.cc**
```c++
struct Native_func_registry
{
  LEX_STRING name;
  Create_func *builder;
};

#define BUILDER(F) & F::s_singleton

/*
  MySQL native functions.
  MAINTAINER:
  - Keep sorted for human lookup. At runtime, a hash table is used.
  - do **NOT** conditionally (#ifdef, #ifndef) define a function *NAME*:
    doing so will cause user code that works against a --without-XYZ binary
    to fail with name collisions against a --with-XYZ binary.
    Use something similar to GEOM_BUILDER instead.
  - keep 1 line per entry, it makes grep | sort easier
*/

static Native_func_registry func_array[] =
{
  ...
  { { C_STRING_WITH_LEN("JSON_ARRAY") }, BUILDER(Create_func_json_array)},
  { { C_STRING_WITH_LEN("JSON_ARRAY_APPEND") }, BUILDER(Create_func_json_array_append)},
  { { C_STRING_WITH_LEN("JSON_ARRAY_INSERT") }, BUILDER(Create_func_json_array_insert)},
  { { C_STRING_WITH_LEN("JSON_COMPACT") }, BUILDER(Create_func_json_compact)},
  { { C_STRING_WITH_LEN("JSON_CONTAINS") }, BUILDER(Create_func_json_contains)},
  { { C_STRING_WITH_LEN("JSON_CONTAINS_PATH") }, BUILDER(Create_func_json_contains_path)},
  { { C_STRING_WITH_LEN("JSON_DEPTH") }, BUILDER(Create_func_json_depth)},
  { { C_STRING_WITH_LEN("JSON_DETAILED") }, BUILDER(Create_func_json_detailed)},
  { { C_STRING_WITH_LEN("JSON_EXISTS") }, BUILDER(Create_func_json_exists)},
  { { C_STRING_WITH_LEN("JSON_EXTRACT") }, BUILDER(Create_func_json_extract)},
  { { C_STRING_WITH_LEN("JSON_INSERT") }, BUILDER(Create_func_json_insert)},
  { { C_STRING_WITH_LEN("JSON_KEYS") }, BUILDER(Create_func_json_keys)},
  { { C_STRING_WITH_LEN("JSON_LENGTH") }, BUILDER(Create_func_json_length)},
  { { C_STRING_WITH_LEN("JSON_LOOSE") }, BUILDER(Create_func_json_loose)},
  { { C_STRING_WITH_LEN("JSON_MERGE") }, BUILDER(Create_func_json_merge)},
  { { C_STRING_WITH_LEN("JSON_MERGE_PATCH") }, BUILDER(Create_func_json_merge_patch)},
  { { C_STRING_WITH_LEN("JSON_MERGE_PRESERVE") }, BUILDER(Create_func_json_merge)},
  { { C_STRING_WITH_LEN("JSON_QUERY") }, BUILDER(Create_func_json_query)},
  { { C_STRING_WITH_LEN("JSON_QUOTE") }, BUILDER(Create_func_json_quote)},
  { { C_STRING_WITH_LEN("JSON_OBJECT") }, BUILDER(Create_func_json_object)},
  { { C_STRING_WITH_LEN("JSON_REMOVE") }, BUILDER(Create_func_json_remove)},
  { { C_STRING_WITH_LEN("JSON_REPLACE") }, BUILDER(Create_func_json_replace)},
  { { C_STRING_WITH_LEN("JSON_SET") }, BUILDER(Create_func_json_set)},
  { { C_STRING_WITH_LEN("JSON_SEARCH") }, BUILDER(Create_func_json_search)},
  { { C_STRING_WITH_LEN("JSON_TYPE") }, BUILDER(Create_func_json_type)},
  { { C_STRING_WITH_LEN("JSON_UNQUOTE") }, BUILDER(Create_func_json_unquote)},
  { { C_STRING_WITH_LEN("JSON_VALID") }, BUILDER(Create_func_json_valid)},
  { { C_STRING_WITH_LEN("JSON_VALUE") }, BUILDER(Create_func_json_value)},
  ...
  { {0, 0}, NULL}
};
```

json函数存储在类型为`Native_func_registry`的数组`func_array`中, 在服务器启动时, 会为native functions生成一张hash表.

**item_create.cc**
```c++
/*
  Load the hash table for native functions.
  Note: this code is not thread safe, and is intended to be used at server
  startup only (before going multi-threaded)
*/

int item_create_init()
{
  Native_func_registry *func;

  DBUG_ENTER("item_create_init");

  if (my_hash_init(& native_functions_hash,
                   system_charset_info,
                   array_elements(func_array),
                   0,
                   0,
                   (my_hash_get_key) get_native_fct_hash_key,
                   NULL,                          /* Nothing to free */
                   MYF(0)))
    DBUG_RETURN(1);

  for (func= func_array; func->builder != NULL; func++)
  {
    if (my_hash_insert(& native_functions_hash, (uchar*) func))
      DBUG_RETURN(1);
  }

  DBUG_RETURN(0);
}
```

以`select json_type('1');`为例, 在yacc解析中识别json函数的顺序如下:

```c++
select:
          opt_with_clause select_init

select_init:
          SELECT_SYM select_options_and_item_list select_init3

select_options_and_item_list:
          select_options select_item_list

select_item_list:
          select_item_list ',' select_item
        | select_item
        | '*'

select_item:
        | remember_name expr remember_end select_alias

expr:
        | bool_pri

bool_pri:
        | predicate

predicate:
        | bit_expr

bit_expr:
        | simple_expr

simple_expr:
          column_default_non_parenthesized_expr

/*
  Expressions that the parser allows in a column DEFAULT clause
  without parentheses. These expressions cannot end with a COLLATE clause.

  If we allowed any "expr" in DEFAULT clause, there would be a confusion
  in queries like this:
    CREATE TABLE t1 (a TEXT DEFAULT 'a' COLLATE latin1_bin);
  It would be not clear what COLLATE stands for:
  - the collation of the column `a`, or
  - the collation of the string literal 'a'

  This restriction allows to parse the above query unambiguiusly:
  COLLATE belongs to the column rather than the literal.
  If one needs COLLATE to belong to the literal, parentheses must be used:
    CREATE TABLE t1 (a TEXT DEFAULT ('a' COLLATE latin1_bin));
  Note: the COLLATE clause is rather meaningless here, but the query
  is syntactically correct.

  Note, some of the expressions are not actually allowed in DEFAULT,
  e.g. sum_expr, window_func_expr, ROW(...), VALUES().
  We could move them to simple_expr, but that would make
  these two queries return a different error messages:
    CREATE TABLE t1 (a INT DEFAULT AVG(1));
    CREATE TABLE t1 (a INT DEFAULT (AVG(1)));
  The first query would return "syntax error".
  Currenly both return:
   Function or expression 'avg(' is not allowed for 'DEFAULT' ...
*/
column_default_non_parenthesized_expr:
        | function_call_generic

/*
  Regular function calls.
  The function name is *not* a token, and therefore is guaranteed to not
  introduce side effects to the language in general.
  MAINTAINER:
  All the new functions implemented for new features should fit into
  this category. The place to implement the function itself is
  in sql/item_create.cc
*/
function_call_generic:
          IDENT_sys '('
          {...}
          opt_udf_expr_list ')'
          {
            ...
            builder= find_native_function_builder(thd, $1);
            if (builder)
            {
              item= builder->create_func(thd, $1, $4);
            }
            ...
          }
```

寻找hash表中是否有JSON_TYPE函数, 有的话调用这个类的`create_func`成员函数
