mysql5.7定义了一套自己的json大小比较规则. 下面是我的学习笔记.

```c++
void Field_json::make_sort_key(uchar *to, size_t length)
{
  Json_wrapper wr;
  if (val_json(&wr))
  {
    /* purecov: begin inspected */
    memset(to, 0, length);
    return;
    /* purecov: end */
  }
  wr.make_sort_key(to, length);
}
```

`Json_wrapper`类是用于访问JSON值的抽象，而不管其是binary JSON values还是JSON DOM values。这样做的目的是允许调用者进行统一访问。它允许我们访问binary JSON values，而不必构建DOM（因此，除非有必要，否则必须读取整个值，例如只访问数组中单个列或者对象的字段）。
该类的实例通常在栈上创建。在某些情况下，实例被缓存在一个Item中并被重用，在这种情况下，它们是从query-duration memory中分配的（这就是Json_wrapper从Sql_alloc继承的原因）
```c++
class Json_wrapper : Sql_alloc
{
private:
  bool m_is_dom;      //!< Wraps a DOM iff true
  bool m_dom_alias;   //!< If true, don't deallocate in destructor
  json_binary::Value m_value;
  Json_dom *m_dom_value;
public:
  ...
  /**
    Make a sort key that can be used by filesort to order JSON values.

    @param[out] to      a buffer to which the sort key is written
    @param[in]  length  the length of the sort key
  */
  void make_sort_key(uchar *to, size_t length) const;
```

```c++
/// Wrapper around a sort key buffer.
class Wrapper_sort_key
{
private:
  uchar *m_buffer;  ///< the buffer into which to write
  size_t m_length;  ///< the length of the buffer
  size_t m_pos;     ///< the current position in the buffer

public:
  Wrapper_sort_key(uchar *buf, size_t len)
    : m_buffer(buf), m_length(len), m_pos(0)
  {}

  /// Get the remaining space in the buffer.
  size_t remaining() const
  {
    return m_length - m_pos;
  }
  ...
  /**
    Copy an integer to the buffer and format it in a way that makes it
    possible to sort the integers with memcpy().

    @param target_length  the number of bytes to write to the buffer
    @param from           the buffer to copy the integer from (in little-endian
                          format)
    @param from_length    the size of the from buffer
    @param is_unsigned    true if the from buffer contains an unsigned integer,
                          false otherwise
  */
  void copy_int(size_t target_length,
                const uchar* from, size_t from_length,
                bool is_unsigned)
  {
    size_t to_length= std::min(remaining(), target_length);
    copy_integer<false>(m_buffer + m_pos, to_length,
                        from, from_length, is_unsigned);
    m_pos+= to_length;
  }
```

```c++
/**
   Copies an integer value to a format comparable with memcmp(). The
   format is characterized by the following:

   - The sign bit goes first and is unset for negative values.
   - The representation is big endian.

   The function template can be instantiated to copy from little or
   big endian values.

   @tparam Is_big_endian True if the source integer is big endian.

   @param to          Where to write the integer.
   @param to_length   Size in bytes of the destination buffer.
   @param from        Where to read the integer.
   @param from_length Size in bytes of the source integer
   @param is_unsigned True if the source integer is an unsigned value.
*/
template<bool Is_big_endian>
void copy_integer(uchar *to, size_t to_length,
                  const uchar* from, size_t from_length,
                  bool is_unsigned)
{
  if (Is_big_endian)
  {
    if (is_unsigned)
      to[0]= from[0];
    else
      to[0]= (char)(from[0] ^ 128); // Reverse the sign bit.
    memcpy(to + 1, from + 1, to_length - 1);
  }
  else
  {
    const int sign_byte= from[from_length - 1];
    if (is_unsigned)
      to[0]= sign_byte;
    else
      to[0]= static_cast<char>(sign_byte ^ 128); // Reverse the sign bit.
    for (size_t i= 1, j= from_length - 2; i < to_length; ++i, --j)
      to[i]= from[j];
  }
}
```

```c++
void Json_wrapper::make_sort_key(uchar *to, size_t to_length) const
{
  Wrapper_sort_key key(to, to_length);
  const Json_dom::enum_json_type jtype= type();
  switch (jtype)
  {
  ...
  case Json_dom::J_INT:
    {
      longlong i= get_int();
      char buff[MAX_BIGINT_WIDTH + 1];
      size_t len= longlong10_to_str(i, buff, -10) - buff;
      make_json_numeric_sort_key(buff, len, i < 0, &key);
      break;
    }
  ...
  }

  key.pad_fill(0, key.remaining());
}
```
[longlong10_to_str](./int2str.md)返回的是指向结尾'\0'的指针, 所以与buff相减得到的是字符串的长度.

从JSON数值的字符串表示形式为其创建sort key。输入的字符串可以是科学格式（如1.234e2）或直接格式（如12.34）。
The sort key will have the following parts:
1. 正负号标记, 占一个字节. 正, 0, 负 分别对应为JSON_KEY_NUMBER_POS, JSON_KEY_NUMBER_ZERO, JSON_KEY_NUMBER_NEG
2. 表示数字的十进制指数（数字的log10，截断为整数), 占两个字节
3. 数字的所有有效位，不包含前导零
4. buffer剩余部分填充尾随零, 以确保相等的数字排序相等

如果数字为零, 2,3,4部分将忽略.
对于负数，2,3,4部分的值需要颠倒，以便较大的负数在较小的负数之前排序

```c++
/**
  @param[in]     from     the string representation of the number
  @param[in]     len      the length of the input string
  @param[in]     negative true if the number is negative, false otherwise
  @param[in,out] to       the target sort key
*/
static void make_json_numeric_sort_key(const char *from, size_t len,
                                       bool negative, Wrapper_sort_key *to)
{
  const char *end= from + len;

  // Find the start of the exponent part, if there is one.
  const char *end_of_digits= std::find(from, end, 'e');

  /*
    Find the first significant digit. Skip past sign, leading zeros
    and the decimal point, until the first non-zero digit is found.
  */
  const char *first_significant_digit=
    std::find_if(from, end_of_digits, is_non_zero_digit);

  if (first_significant_digit == end_of_digits)
  {
    // We didn't find any significant digits, so the number is zero.
    to->append(JSON_KEY_NUMBER_ZERO);
    return;
  }

  longlong exp;
  if (end_of_digits != end)
  {
    // Scientific format. Fetch the exponent part after the 'e'.
    char *endp= const_cast<char *>(end);
    exp= my_strtoll(end_of_digits + 1, &endp, 10);
  }
  else
  {
    /*
      Otherwise, find the exponent by calculating the distance between the
      first significant digit and the decimal point.
    */
    const char *dec_point= std::find(from, end_of_digits, '.');
    if (!dec_point)
    {
      // There is no decimal point. Just count the digits.
      exp= end_of_digits - first_significant_digit - 1;
    }
    else if (first_significant_digit < dec_point)
    {
      // Non-negative exponent.
      exp= dec_point - first_significant_digit - 1;
    }
    else
    {
      // Negative exponent.
      exp= dec_point - first_significant_digit;
    }
  }

  if (negative)
  {
    to->append(JSON_KEY_NUMBER_NEG);
    /*
      For negative numbers, we have to invert the exponents so that numbers
      with high exponents sort before numbers with low exponents.
    */
    exp= -exp;
  }
  else
  {
    to->append(JSON_KEY_NUMBER_POS);
  }

  /*
    Store the exponent part before the digits. Since the decimal exponent of a
    double can be in the range [-323, +308], we use two bytes for the
    exponent. (Decimals and bigints also fit in that range.)
  */
  uchar exp_buff[2];
  int2store(exp_buff, static_cast<int16>(exp));
  to->copy_int(sizeof(exp_buff), exp_buff, sizeof(exp_buff), false);

  /*
    Append all the significant digits of the number. Stop before the exponent
    part if there is one, otherwise go to the end of the string.
  */
  for (const char *ch= first_significant_digit; ch < end_of_digits; ++ch)
  {
    if (my_isdigit(&my_charset_numeric, *ch))
    {
      /*
        If the number is negative, the digits must be inverted so that big
        negative numbers sort before small negative numbers.
      */
      if (negative)
        to->append('9' - *ch + '0');
      else
        to->append(*ch);
    }
  }

  /*
    Pad the rest of the buffer with zeros, so that the number of trailing
    zeros doesn't affect how the number is sorted. As above, we need to invert
    the digits for negative numbers.
  */
  to->pad_fill(negative ? '9' : '0', to->remaining());
}
```
