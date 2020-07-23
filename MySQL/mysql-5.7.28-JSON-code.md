mysql-5.7.28的关键字比mariadb多.
```c++
%token  JSON_SEPARATOR_SYM            /* MYSQL */
%token  JSON_UNQUOTED_SEPARATOR_SYM   /* MYSQL */
%token  JSON_SYM                      /* MYSQL */
/*
   Tokens from MySQL 8.0
*/
%token  JSON_OBJECTAGG                /* SQL-2015-R */
%token  JSON_ARRAYAGG                 /* SQL-2015-R */
```

yacc解析中, type将json解析为`MYSQL_TYPE_JSON`类型

**sql_yacc.yy**
```c++
type:
        | JSON_SYM
          {
            Lex->charset=&my_charset_bin;
            $$=MYSQL_TYPE_JSON;
          }
```

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
  { { C_STRING_WITH_LEN("JSON_VALID") }, BUILDER(Create_func_json_valid)},
  { { C_STRING_WITH_LEN("JSON_CONTAINS") }, BUILDER(Create_func_json_contains)},
  { { C_STRING_WITH_LEN("JSON_CONTAINS_PATH") }, BUILDER(Create_func_json_contains_path)},
  { { C_STRING_WITH_LEN("JSON_LENGTH") }, BUILDER(Create_func_json_length)},
  { { C_STRING_WITH_LEN("JSON_DEPTH") }, BUILDER(Create_func_json_depth)},
  { { C_STRING_WITH_LEN("JSON_PRETTY") }, BUILDER(Create_func_json_pretty)},
  { { C_STRING_WITH_LEN("JSON_TYPE") }, BUILDER(Create_func_json_type)},
  { { C_STRING_WITH_LEN("JSON_KEYS") }, BUILDER(Create_func_json_keys)},
  { { C_STRING_WITH_LEN("JSON_EXTRACT") }, BUILDER(Create_func_json_extract)},
  { { C_STRING_WITH_LEN("JSON_ARRAY_APPEND") }, BUILDER(Create_func_json_array_append)},
  { { C_STRING_WITH_LEN("JSON_INSERT") }, BUILDER(Create_func_json_insert)},
  { { C_STRING_WITH_LEN("JSON_ARRAY_INSERT") }, BUILDER(Create_func_json_array_insert)},
  { { C_STRING_WITH_LEN("JSON_OBJECT") }, BUILDER(Create_func_json_row_object)},
  { { C_STRING_WITH_LEN("JSON_SEARCH") }, BUILDER(Create_func_json_search)},
  { { C_STRING_WITH_LEN("JSON_SET") }, BUILDER(Create_func_json_set)},
  { { C_STRING_WITH_LEN("JSON_REPLACE") }, BUILDER(Create_func_json_replace)},
  { { C_STRING_WITH_LEN("JSON_ARRAY") }, BUILDER(Create_func_json_array)},
  { { C_STRING_WITH_LEN("JSON_REMOVE") }, BUILDER(Create_func_json_remove)},
  { { C_STRING_WITH_LEN("JSON_MERGE") }, BUILDER(Create_func_json_merge)},
  { { C_STRING_WITH_LEN("JSON_MERGE_PATCH") }, BUILDER(Create_func_json_merge_patch)},
  { { C_STRING_WITH_LEN("JSON_MERGE_PRESERVE") }, BUILDER(Create_func_json_merge_preserve)},
  { { C_STRING_WITH_LEN("JSON_QUOTE") }, BUILDER(Create_func_json_quote)},
  { { C_STRING_WITH_LEN("JSON_STORAGE_SIZE") }, BUILDER(Create_func_json_storage_size)},
  { { C_STRING_WITH_LEN("JSON_UNQUOTE") }, BUILDER(Create_func_json_unquote)},
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
                   MYF(0),
                   key_memory_native_functions))
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
          select_init

/* Need first branch for subselects. */
select_init:
          SELECT_SYM select_part2 opt_union_clause

select_part2:
          select_options_and_item_list
          opt_order_clause
          opt_limit_clause
          opt_select_lock_type
          {
            $$= NEW_PTN PT_select_part2($1, NULL, NULL, NULL, NULL, NULL,
                                        $2, $3, NULL, NULL, $4);
          }

select_options_and_item_list:
          {
            /*
              TODO: remove this semantic action (currently this removal
              adds shift/reduce conflict)
            */
          }
          select_options select_item_list
          {
            $$= NEW_PTN PT_select_options_and_item_list($2, $3);
          }

select_item_list:
        | select_item

select_item:
        | expr select_alias

/* all possible expressions */
expr:
        | bool_pri

bool_pri:
        | predicate

predicate:
        | bit_expr

bit_expr:
        | simple_expr

simple_expr:
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
          IDENT_sys '(' opt_udf_expr_list ')'
          {
            $$= NEW_PTN PTI_function_call_generic_ident_sys(@1, $1, $3);
          }
```

寻找hash表中是否有JSON_TYPE函数, 有的话调用这个类的`create_func`成员函数
