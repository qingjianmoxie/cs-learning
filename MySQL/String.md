# String

MySQL自己实现了一个String类, 在5.7版本中, 头文件目录为/include/sql_string.h, 源文件目录为/sql-common/sql_string.cc.

String类有5个私有变量, 如下所示:
```c++
class String
{
  // 指向字符串的指针
  char *m_ptr;

  // 字符串的长度
  size_t m_length;

  // 字符串的字符集信息
  const CHARSET_INFO *m_charset;

  // 已申请内存的长度
  uint32 m_alloced_length; // should be size_t, but kept uint32 for size reasons

  // 是否申请了内存
  bool m_is_alloced;
};
```

构造函数有6种, 如下所示:
```c++
class String
{
public:
  String()
    :m_ptr(NULL), m_length(0), m_charset(&my_charset_bin),
     m_alloced_length(0), m_is_alloced(false)
  { }
  String(size_t length_arg)
    :m_ptr(NULL), m_length(0), m_charset(&my_charset_bin),
     m_alloced_length(0), m_is_alloced(false)
  {
    (void) real_alloc(length_arg);
  }
  String(const char *str, const CHARSET_INFO *cs)
    :m_ptr(const_cast<char*>(str)), m_length(strlen(str)),
     m_charset(cs), m_alloced_length(0), m_is_alloced(false)
  { }
  String(const char *str, size_t len, const CHARSET_INFO *cs)
    :m_ptr(const_cast<char*>(str)), m_length(len),
     m_charset(cs), m_alloced_length(0), m_is_alloced(false)
  { }
  String(char *str, size_t len, const CHARSET_INFO *cs)
    :m_ptr(str), m_length(len), m_charset(cs),
     m_alloced_length(static_cast<uint32>(len)), m_is_alloced(false)
  { }
  String(const String &str)
    :m_ptr(str.m_ptr), m_length(str.m_length), m_charset(str.m_charset),
     m_alloced_length(static_cast<uint32>(str.m_alloced_length)),
     m_is_alloced(false)
  { }
};
```

析构函数如下所示:
```c++
  ~String() { mem_free(); }
```

析构函数调用的是String的成员函数mem_free():
```c++
  void mem_free()
  {
    if (m_is_alloced)
    {
      m_is_alloced= false;
      m_alloced_length= 0;
      my_free(m_ptr);
      m_ptr= NULL;
      m_length= 0;				/* Safety */
    }
  }
```


```c++
  size_t length() const { return m_length;}
```
获取String的长度.  
当const在函数名前面的时候修饰的是函数返回值，在函数名后面表示是常成员函数，该函数不能修改对象内的任何成员，只能发生读操作，不能发生写操作。  
我们都知道在调用成员函数的时候编译器会将对象自身的地址作为隐藏参数传递给函数，在const成员函数中，既不能改变this所指向的对象，也不能改变this所保存的地址，this的类型是一个指向const类型对象的const指针。


```c++
  size_t alloced_length() const { return m_alloced_length;}
```
返回已申请内存的长度.


```c++
  char& operator [] (size_t i) const { return m_ptr[i]; }
```
重载`[]`, 使得可以对String直接使用`[]`.


```c++
  void length(size_t len) { m_length= len; }
```
设置String的长度, 常用于清空String, 比如:
```c++
String str;
/* 对str进行一些操作 */
/* 代码段 */
str.length(0); // 清空str
```


```c++
  bool is_empty() const { return (m_length == 0); }
```
判断String是否为空.


```c++
  void mark_as_const() { m_alloced_length= 0;}
```
将String标记为const.


```c++
  const char *ptr() const { return m_ptr; }

  char *c_ptr()
  {
    DBUG_ASSERT(!m_is_alloced || !m_ptr || !m_alloced_length ||
                (m_alloced_length >= (m_length + 1)));

    if (!m_ptr || m_ptr[m_length])		/* Should be safe */
      (void) mem_realloc(m_length);
    return m_ptr;
  }

  char *c_ptr_quick()
  {
    if (m_ptr && m_length < m_alloced_length)
      m_ptr[m_length]= 0;
    return m_ptr;
  }

  char *c_ptr_safe()
  {
    if (m_ptr && m_length < m_alloced_length)
      m_ptr[m_length]= 0;
    else
      (void) mem_realloc(m_length);
    return m_ptr;
  }
```
返回字符串首地址的成员函数有四个.  
`ptr`直接返回字符串的首地址;  
`c_ptr`判断如果指针为NULL或者字符串后一位不为'\0', 则重新分配内存;  
`c_ptr_quick`强制将字符串的后一位置'\0';  
`c_ptr_safe`判断如果指针为NULL或者字符串长度大于等于申请的内存长度, 则重新分配内存.

注意:`c_ptr`和`c_ptr_safe`都有可能重新分配内存, 使用过后需要进行内存释放, 否则有内存泄漏~


```c++
struct st_mysql_lex_string
{
  char *str;
  size_t length;
};
typedef struct st_mysql_lex_string MYSQL_LEX_STRING;

struct st_mysql_const_lex_string
{
  const char *str;
  size_t length;
};

typedef struct st_mysql_lex_string LEX_STRING;
typedef struct st_mysql_const_lex_string LEX_CSTRING;
```
```c++
  LEX_STRING lex_string() const
  {
    LEX_STRING lex_string = { (char*) ptr(), length() };
    return lex_string;
  }

  LEX_CSTRING lex_cstring() const
  {
    LEX_CSTRING lex_cstring = { ptr(), length() };
    return lex_cstring;
  }
```
这里可以看出LEX_CSTRING与LEX_STRING的区别, LEX_CSTRING表示const_string.

```c++
  /*
    This is a method that works the same as perl's "chop". It simply
    drops the last character of a string. This is useful in the case
    of the federated storage handler where I'm building a unknown
    number, list of values and fields to be used in a sql insert
    statement to be run on the remote(远程) server, and have a comma(逗号) after each.
    When the list is complete, I "chop" off the trailing comma

    ex.
      String stringobj;
      stringobj.append("VALUES ('foo', 'fi', 'fo',");
      stringobj.chop();
      stringobj.append(")");

    In this case, the value of string was:

    VALUES ('foo', 'fi', 'fo',
    VALUES ('foo', 'fi', 'fo'
    VALUES ('foo', 'fi', 'fo')

  */
  void chop()
  {
    m_length--;
    m_ptr[m_length]= '\0';
    DBUG_ASSERT(strlen(m_ptr) == m_length);
  }
```
删除字符串的最后一个字符.

```c++
  bool append(const String &s);
  bool append(const char *s);
  bool append(const char *s, size_t arg_length);
```
```c++
bool String::append(const String &s)
{
  if (s.length())
  {
    DBUG_ASSERT(!this->uses_buffer_owned_by(&s));
    DBUG_ASSERT(!s.uses_buffer_owned_by(this));

    if (mem_realloc_exp((m_length + s.length())))
      return true;
    memcpy(m_ptr + m_length,s.ptr(), s.length());
    m_length+=s.length();
  }
  return false;
}

/*
  Append a 0-terminated ASCII string
*/
bool String::append(const char *s)
{
  return append(s, (uint) strlen(s));
}

/*
  Append an ASCII string to the a string of the current character set
*/
bool String::append(const char *s, size_t arg_length)
{
  if (!arg_length)
    return false;

  /*
    For an ASCII incompatible string, e.g. UCS-2, we need to convert
  */
  if (m_charset->mbminlen > 1)
  {
    size_t add_length= arg_length * m_charset->mbmaxlen;
    uint dummy_errors;
    if (mem_realloc(m_length + add_length))
      return true;
    m_length+= copy_and_convert(m_ptr + m_length, add_length, m_charset,
                                s, arg_length, &my_charset_latin1,
                                &dummy_errors);
    return false;
  }

  /*
    For an ASCII compatinble string we can just append.
  */
  if (mem_realloc_exp(m_length + arg_length))
    return true;
  memcpy(m_ptr + m_length, s, arg_length);
  m_length+= arg_length;
  return false;
}
```
在字符串后面添加给定的字符串.
