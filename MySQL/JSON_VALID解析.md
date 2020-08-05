此篇是对mariadb10.2.32版本json_valid函数的解析. 

在mysql中
```
MySQL [(none)]> select json_valid('1');
+-----------------+
| json_valid('1') |
+-----------------+
|               1 |
+-----------------+
1 row in set (0.00 sec)

MySQL [(none)]> select json_valid(1);
+---------------+
| json_valid(1) |
+---------------+
|             0 |
+---------------+
1 row in set (0.00 sec)
```

而在mariadb中
```
MySQL [(none)]> select json_valid('1');
+-----------------+
| json_valid('1') |
+-----------------+
|               1 |
+-----------------+
1 row in set (0.00 sec)

MySQL [(none)]> select json_valid(1);
+---------------+
| json_valid(1) |
+---------------+
|             1 |
+---------------+
1 row in set (0.00 sec)
```

因为mariadb对不加引号的数字判断为合法, mysql对不加引号的数字判断为不合法, 现在需要将mariadb的判断标准改为与mysql一致.

json_valid类如下所示:
```c++
class Item_func_json_valid: public Item_int_func
{
protected:
  String tmp_value;

public:
  Item_func_json_valid(THD *thd, Item *json) : Item_int_func(thd, json) {}
  longlong val_int();
  const char *func_name() const { return "json_valid"; }
  ...
  bool is_bool_type() { return true; }
  ...
};
```

判断是否合法时, 使用的是`val_int`成员函数
```c++
longlong Item_func_json_valid::val_int()
{
  String *js= args[0]->val_json(&tmp_value);
  json_engine_t je;

  if ((null_value= args[0]->null_value))
    return 0;

  json_scan_start(&je, js->charset(), (const uchar *) js->ptr(),
                  (const uchar *) js->ptr()+js->length());

  while (json_scan_next(&je) == 0) {}

  return je.s.error == 0;
}
```

首先函数将json_valid()括号中的内容拷贝到字符串js中, `val_json`调用的ITEM类中的虚函数
```c++
  virtual String *val_json(String *str) { return val_str(str); }
```
然后调用`Item_int_func`的`val_str`
```c++
String *Item_field::val_str(String *str)
{
  DBUG_ASSERT(fixed == 1);
  if ((null_value=field->is_null()))
    return 0;
  str->set_charset(str_value.charset());
  return field->val_str(str,&str_value);
}
```

之后执行`json_scan_start`, 初始化结构体`je`中的变量, 其中state的值为`JST_VALUE`(也就是0)
```c++
int json_scan_start(json_engine_t *je,
                    CHARSET_INFO *i_cs, const uchar *str, const uchar *end)
{
  json_string_setup(&je->s, i_cs, str, end);
  je->stack[0]= JST_DONE;
  je->stack_p= 0;
  je->state= JST_VALUE;
  return 0;
}

static void json_string_setup(json_string_t *s,
                              CHARSET_INFO *i_cs, const uchar *str,
                              const uchar *end)
{
  json_string_set_cs(s, i_cs);
  json_string_set_str(s, str, end);
}

void json_string_set_cs(json_string_t *s, CHARSET_INFO *i_cs)
{
  s->cs= i_cs;
  s->error= 0;
  s->wc= i_cs->cset->mb_wc;
}

void json_string_set_str(json_string_t *s,
                         const uchar *str, const uchar *end)
{
  s->c_str= str;
  s->str_end= end;
}
```

结构体`json_engine_t`的定义为
```c++
typedef struct st_json_engine_t
{
  json_string_t s;  /* String to parse. */
  int sav_c_len;    /* Length of the current character.
                       Can be more than 1 for multibyte charsets */

  int state; /* The state of the parser. One of 'enum json_states'.
                It tells us what construction of JSON we've just read. */

  /* These values are only set after the json_read_value() call. */
  enum json_value_types value_type; /* type of the value.*/
  const uchar *value;      /* Points to the value. */
  const uchar *value_begin;/* Points to where the value starts in the JSON. */
  int value_escaped;       /* Flag telling if the string value has escaping.*/
  uint num_flags;  /* the details of the JSON_VALUE_NUMBER, is it negative,
                      or if it has the fractional part.
                      See the enum json_num_flags. */

  /*
    In most cases the 'value' and 'value_begin' are equal.
    They only differ if the value is a string constants. Then 'value_begin'
    points to the starting quotation mark, while the 'value' - to
    the first character of the string.
  */

  const uchar *value_end; /* Points to the next character after the value. */
  int value_len; /* The length of the value. Does not count quotations for */
                 /* string constants. */

  int stack[JSON_DEPTH_LIMIT]; /* Keeps the stack of nested JSON structures. */
  int stack_p;                 /* The 'stack' pointer. */
} json_engine_t;
```

结构体`json_string_t`的定义为
```c++
#define my_wc_t ulong

typedef struct st_json_string_t
{
  const uchar *c_str;    /* Current position in JSON string */
  const uchar *str_end;  /* The end on the string. */
  my_wc_t c_next;        /* UNICODE of the last read character */
  int error;             /* error code. */

  CHARSET_INFO *cs;      /* Character set of the JSON string. */

  my_charset_conv_mb_wc wc; /* UNICODE conversion function. */
                            /* It's taken out of the cs just to speed calls. */
} json_string_t;
```

设置好字符串后, 开始扫描
```c++
int json_scan_next(json_engine_t *j)
{
  int t_next;

  get_first_nonspace(&j->s, &t_next, &j->sav_c_len);
  return json_actions[j->state][t_next](j);
}

#define json_next_char(j) \
  (j)->wc((j)->cs, &(j)->c_next, (j)->c_str, (j)->str_end)
#define json_eos(j) ((j)->c_str >= (j)->str_end)

static void get_first_nonspace(json_string_t *js, int *t_next, int *c_len)
{
  do
  {
    if ((*c_len= json_next_char(js)) <= 0)
      *t_next= json_eos(js) ? C_EOS : C_BAD;
    else
    {
      *t_next= (js->c_next < 128) ? json_chr_map[js->c_next] : C_ETC;
      js->c_str+= *c_len;
    }
  } while (*t_next == C_SPACE);
}
```

`js->c_next`是unicode编码, unicode码前128位与ascii码一致, `json_chr_map`是size为128的数组, 将这128种字符分为14类, 如下所示, 没有`C_BAD`. 比较巧妙的地方是设置了`NR_C_CLASSES`, 它的值就是前面枚举的数量.

```c++
enum json_char_classes {
  C_EOS,    /* end of string */
  C_LCURB,  /* {  */
  C_RCURB,  /* } */
  C_LSQRB,  /* [ */
  C_RSQRB,  /* ] */
  C_COLON,  /* : */
  C_COMMA,  /* , */
  C_QUOTE,  /* " */
  C_DIGIT,  /* -0123456789 */
  C_LOW_F,  /* 'f' (for "false") */
  C_LOW_N,  /* 'n' (for "null") */
  C_LOW_T,  /* 't' (for "true") */
  C_ETC,    /* everything else */
  C_ERR,    /* character disallowed in JSON */
  C_BAD,    /* invalid character, charset handler cannot read it */
  NR_C_CLASSES, /* Counter for classes that handled with functions. */
  C_SPACE   /* space. Doesn't need specific handlers, so after the counter.*/
};
```


举个具体的例子, 比如说我们输入的是`json_valid(1)`, `*t_next= (js->c_next < 128) ? json_chr_map[js->c_next] : C_ETC;`会从json_chr_map中取出对应的`C_DIGIT`, 然后执行`return json_actions[j->state][t_next](j);`.
此时`j->state`为`JST_VALUE`, `t_next`为`C_DIGIT`

`json_actions`二维数组的大小设置就使用到了`NR_C_CLASSES`, `NR_JSON_STATES`同理.
```c++
static json_state_handler json_actions[NR_JSON_STATES][NR_C_CLASSES]=
/*
   EOS              {            }             [             ]
   :                ,            "             -0..9         f
   n                t              ETC          ERR           BAD
*/
{
  {/*VALUE*/
    unexpected_eos, mark_object, syntax_error, mark_array,   syntax_error,
    syntax_error,   syntax_error,v_string,     v_number,     v_false,
    v_null,         v_true,       syntax_error, not_json_chr, bad_chr},
  {/*KEY*/
    unexpected_eos, skip_key,    skip_key,     skip_key,     skip_key,
    skip_key,       skip_key,    skip_colon,   skip_key,     skip_key,
    skip_key,       skip_key,     skip_key,     not_json_chr, bad_chr},
  {/*OBJ_START*/
    unexpected_eos, syntax_error, end_object,  syntax_error, syntax_error,
    syntax_error,   syntax_error, read_keyname, syntax_error, syntax_error,
    syntax_error,   syntax_error,   syntax_error,    not_json_chr, bad_chr},
  {/*OBJ_END*/
    struct_end_eos, syntax_error, struct_end_cb, syntax_error, struct_end_qb,
    syntax_error,   struct_end_cm,syntax_error,  syntax_error, syntax_error,
    syntax_error,   syntax_error,  syntax_error,    not_json_chr, bad_chr},
  {/*ARRAY_START*/
    unexpected_eos, array_item,   syntax_error, array_item,   end_array,
    syntax_error,   syntax_error, array_item,  array_item,  array_item,
    array_item,    array_item,    syntax_error,    not_json_chr, bad_chr},
  {/*ARRAY_END*/
    struct_end_eos, syntax_error, struct_end_cb, syntax_error,  struct_end_qb,
    syntax_error,   struct_end_cm, syntax_error, syntax_error,  syntax_error,
    syntax_error,   syntax_error,  syntax_error,    not_json_chr, bad_chr},
  {/*DONE*/
    done,           syntax_error, syntax_error, syntax_error, syntax_error,
    syntax_error,   syntax_error, syntax_error, syntax_error, syntax_error,
    syntax_error,   syntax_error, syntax_error, not_json_chr, bad_chr},
  {/*OBJ_CONT*/
    unexpected_eos, syntax_error, end_object,    syntax_error,   syntax_error,
    syntax_error,   next_key,     syntax_error,  syntax_error,   syntax_error,
    syntax_error,    syntax_error,    syntax_error,    not_json_chr, bad_chr},
  {/*ARRAY_CONT*/
    unexpected_eos, syntax_error, syntax_error,  syntax_error, end_array,
    syntax_error,   next_item,    syntax_error,  syntax_error, syntax_error,
    syntax_error,    syntax_error,    syntax_error,    not_json_chr, bad_chr},
  {/*READ_VALUE*/
    unexpected_eos, read_obj,     syntax_error,  read_array,    syntax_error,
    syntax_error,   syntax_error, read_strn,     read_num,      read_false,
    read_null,      read_true,    syntax_error,    not_json_chr, bad_chr},
};
```

`json_actions`调用的函数为`v_number`
```c++
/* Scalar numeric. */
static int v_number(json_engine_t *j)
{
  return skip_num_constant(j) || json_scan_next(j);
}

static int skip_num_constant(json_engine_t *j)
{
  int state= json_num_states[NS_GO][json_num_chr_map[j->s.c_next]];
  int c_len;

  j->num_flags= 0;
  for (;;)
  {
    j->num_flags|= json_num_state_flags[state];
    if ((c_len= json_next_char(&j->s)) > 0 && j->s.c_next < 128)
    {
      if ((state= json_num_states[state][json_num_chr_map[j->s.c_next]]) > 0)
      {
        j->s.c_str+= c_len;
        continue;
      }
      break;
    }

    if ((j->s.error=
          json_eos(&j->s) ? json_num_states[state][N_END] : JE_BAD_CHR) < 0)
      return 1;
    else
      break;
  }

  j->state= j->stack[j->stack_p];
  return 0;
}

enum json_num_states {
  NS_OK,  /* Number ended. */
  NS_GO,  /* Initial state. */
  NS_GO1, /* If the number starts with '-'. */
  NS_Z,   /* If the number starts with '0'. */
  NS_Z1,  /* If the numbers starts with '-0'. */
  NS_INT, /* Integer part. */
  NS_FRAC,/* Fractional part. */
  NS_EX,  /* Exponential part begins. */
  NS_EX1, /* Exponential part continues. */
  NS_NUM_STATES
};
```
`json_num_chr_map`同样是将128个字符分为9个种类, 此处例子取出的是`N_DIGIT`, json_num_states得到的值为`NS_INT`, `json_num_state_flags[state]`为0, `json_num_states[state][N_END]`为`NS_OK`(也就是0)
所以`j->s.error`为0, 返回值为0.

## 魔改json_valid

在结构体`json_engine_t`中新增两个标记
```c++
  /**
   * 目前主要作用是区分是不是json_valid
   * 如果是json_valid, 纯数字要报错
   */
  enum json_func_types func_type;
  /* 区分是 XXX 还是 'XXX' , 1 表示 'XXX', 0 表示 XXX */
  int flag_for_json_valid;
```
枚举类型是用来区分json函数的类型
```c++
enum json_func_types
{
  UNKNOWN_JSON_FUNC,
  JSON_VALID_FUNC,
  JSON_EXISTS_FUNC,
  JSON_VALUE_FUNC,
  JSON_UNQUOTE_FUNC
};
```
经过测试发现, 当输入为`json_valid('1')`时, 字符串中alloced为false; 当输入为`json_valid(1)`时, 字符串中alloced为true. 所以以此来作区分.
`Item_func_json_valid::val_int`函数中添加如下内容
```c++
  je.func_type = JSON_VALID_FUNC;
  je.flag_for_json_valid = js->is_alloced() ? 0 : 1;
```

`v_number`函数增加判断.
```c++
/* Scalar numeric. */
static int v_number(json_engine_t *j)
{
  if (JSON_VALID_FUNC == j->func_type && 0 == j->flag_for_json_valid) {
    j->s.error = JE_BAD_CHR;
    return 1;
  }
  return skip_num_constant(j) || json_scan_next(j);
}
```