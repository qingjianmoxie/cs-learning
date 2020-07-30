# item和field解析

mariadb10.0.12中没有json数据类型, 在参考mariadb10.2.32后, 进行了代码移植. 然而mariadb中json是使用的longblob数据类型进行存储, 直到现在也没有新增MYSQL_TYPE_JSON, 于是又参考了mysql5.7.28. 使得代码可以区分longblob和json类型, show create table显示为json. 借此机会把涉及到的item和field知识总结一下.

## create table

在yacc解析中, 识别出json, 会调用add_field_to_list函数

```c++
create:
create_body:
create_field_list:
field_list:
field_list_item:
column_def:
field_spec:
          field_ident
          {
            LEX *lex=Lex;
            lex->length=lex->dec=0;
            lex->type=0;
            lex->default_value= lex->on_update_value= 0;
            lex->comment=null_lex_str;
            lex->charset=NULL;
            lex->vcol_info= 0;
            lex->option_list= NULL;
            Lex->reset_parse_column_cs_ptr();
          }
          field_def
          {
            LEX *lex=Lex;
            if (add_field_to_list(lex->thd, &$1, (enum enum_field_types) $3,
                                  lex->length,lex->dec,lex->type,
                                  lex->default_value, lex->on_update_value, 
                                  &lex->comment,
                                  lex->change,&lex->interval_list,lex->charset,
                                  lex->uint_geom_type,
                                  lex->vcol_info, lex->option_list))
              MYSQL_YYABORT;

            if(Lex->parse_column_cs_ptr_pos)
              record_parse_replace_column_cs_info(Lex);
          }
        ;
field_def:
          type opt_attribute {}
type:
        | JSON_SYM
          {
            Lex->charset=&my_charset_bin;
            $$=MYSQL_TYPE_JSON;
          }
```

add_field_to_list函数会新生成一个Create_field对象, 并将Create_field对象存到create_list中.

```c++
/**
  Store field definition for create.

  @return
    Return 0 if ok
*/

bool add_field_to_list(THD *thd, LEX_STRING *field_name, enum_field_types type,
           char *length, char *decimals,
           uint type_modifier,
           Item *default_value, Item *on_update_value,
                       LEX_STRING *comment,
           char *change,
                       List<String> *interval_list, CHARSET_INFO *cs,
           uint uint_geom_type,
           Virtual_column_info *vcol_info,
                       engine_option_value *create_options)
{
  register Create_field *new_field;
  LEX  *lex= thd->lex;
  ...
  if (!(new_field= new Create_field()) ||
      new_field->init(thd, field_name->str, type, length, decimals, type_modifier,
                      default_value, on_update_value, comment, change,
                      interval_list, cs, uint_geom_type, vcol_info,
                      create_options, lex->check_exists))
    DBUG_RETURN(1);

  lex->alter_info.create_list.push_back(new_field);
  lex->last_field=new_field;
  DBUG_RETURN(0);
}
```

在Create_field::init函数中发现了一个兼容问题, mariadb10.0.12将MYSQL_TYPE_VIRTUAL设为245, 而mysql5.7.28将MYSQL_TYPE_JSON设为245, 导致init函数中`((uchar)fld_type == (uchar)MYSQL_TYPE_VIRTUAL)`为真, 从而出错了. 解决办法是将MYSQL_TYPE_VIRTUAL改设为244.
```c++
/* sql type stored in .frm files for virtual fields */
#define MYSQL_TYPE_VIRTUAL 244
```

```c++
bool Create_field::init(THD *thd, char *fld_name, enum_field_types fld_type,
                        char *fld_length, char *fld_decimals,
                        uint fld_type_modifier, Item *fld_default_value,
                        Item *fld_on_update_value, LEX_STRING *fld_comment,
                        char *fld_change, List<String> *fld_interval_list,
                        CHARSET_INFO *fld_charset, uint fld_geom_type,
      Virtual_column_info *fld_vcol_info,
                        engine_option_value *create_opt, bool check_exists)
{
  ...
  sql_type= fld_type;
  ...
  /* Initialize data for a computed field */
  if ((uchar)fld_type == (uchar)MYSQL_TYPE_VIRTUAL)
  {
    ...
  }
  ...
}
```

DDL生成frm元数据的函数, 调用堆栈如下所示:

    prepare_create_field
    mysql_prepare_create_table
    mysql_create_frm_image
    create_table_impl
    mysql_create_table_no_lock
    mysql_create_table
    ddl_create_frm

    pack_fields
    build_frm_image
    mysql_create_frm_image
    create_table_impl
    mysql_create_table_no_lock
    mysql_create_table
    ddl_create_frm

先有一个准备工作, 比如设置pack_flag的值
```c++
/*
  Prepare a create_table instance for packing

  SYNOPSIS
    prepare_create_field()
    sql_field     field to prepare for packing
    blob_columns  count for BLOBs
    table_flags   table flags

  DESCRIPTION
    This function prepares a Create_field instance.
    Fields such as pack_flag are valid after this call.

  RETURN VALUES
   0	ok
   1	Error
*/

int prepare_create_field(Create_field *sql_field, 
			 uint *blob_columns, 
			 longlong table_flags)
{
  ...
  switch (sql_field->sql_type) {
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
    sql_field->pack_flag=FIELDFLAG_BLOB |
      pack_length_to_packflag(sql_field->pack_length -
                              portable_sizeof_char_ptr);
    if (sql_field->charset->state & MY_CS_BINSORT)
      sql_field->pack_flag|=FIELDFLAG_BINARY;
    sql_field->length=8;			// Unireg field length
    sql_field->unireg_check=Field::BLOB_FIELD;
    (*blob_columns)++;
    break;
  case MYSQL_TYPE_JSON:
    // JSON fields are stored as BLOBs.
    sql_field->pack_flag=FIELDFLAG_JSON |
      pack_length_to_packflag(sql_field->pack_length -
                              portable_sizeof_char_ptr);
    IF (sql_field->charset->state & MY_CS_BINSORT)
      sql_field->pack_flag|=FIELDFLAG_BINARY;
    sql_field->length=8;                        // Unireg field length
    sql_field->unireg_check=Field::BLOB_FIELD;
    (*blob_columns)++;
    break;
  ...
  DBUG_RETURN(0);
}
```

## show create table

show create table时, 会读取元数据, 读取元数据的函数为`TABLE_SHARE::init_from_binary_frm_image`:
```c++
int TABLE_SHARE::init_from_binary_frm_image(THD *thd, bool write,
                                            const uchar *frm_image,
                                            size_t frm_length)
{
  for (i=0 ; i < share->fields; i++, strpos+=field_pack_length, field_ptr++)
  {
    uint pack_flag, interval_nr, unireg_type, recpos, field_length;
    uint vcol_info_length=0;
    uint vcol_expr_length=0;
    enum_field_types field_type;
    CHARSET_INFO *charset=NULL;
    Field::geometry_type geom_type= Field::GEOM_GEOMETRY;
    LEX_STRING comment;
    Virtual_column_info *vcol_info= 0;
    bool fld_stored_in_db= TRUE;

    if (new_frm_ver >= 3)
    {
      /* new frm file in 4.1 */
      field_length= uint2korr(strpos+3);
      recpos=      uint3korr(strpos+5);
      pack_flag=    uint2korr(strpos+8);
      unireg_type=  (uint) strpos[10];
      interval_nr=  (uint) strpos[12];
      uint comment_length=uint2korr(strpos+15);
      field_type=(enum_field_types) (uint) strpos[13];
      ...
    }
    ...
    *field_ptr= reg_field=
      make_field(share, record+recpos,
     (uint32) field_length,
     null_pos, null_bit_pos,
     pack_flag,
     field_type,
     charset,
     geom_type,
     (Field::utype) MTYP_TYPENR(unireg_type),
     (interval_nr ?
      share->intervals+interval_nr-1 :
      (TYPELIB*) 0),
     share->fieldnames.type_names[i]);
    if (!reg_field)        // Not supported field type
      goto err;
}
```

此函数调用了make_field函数:
```c++
Field *make_field(TABLE_SHARE *share, uchar *ptr, uint32 field_length,
      uchar *null_pos, uchar null_bit,
      uint pack_flag,
      enum_field_types field_type,
      CHARSET_INFO *field_charset,
      Field::geometry_type geom_type,
      Field::utype unireg_check,
      TYPELIB *interval,
      const char *field_name)
{
  ...
  if (f_is_alpha(pack_flag))
  {
    ...
    if (f_is_json(pack_flag))
      return new Field_json(ptr, null_pos, null_bit,
                            unireg_check, field_name, share,
                            pack_length);
    if (f_is_blob(pack_flag))
      return new Field_blob(ptr,null_pos,null_bit,
          unireg_check, field_name, share,
          pack_length, field_charset);
  }
  switch (field_type) {
    ...
  }
  return 0;
}

根据不同的pack_flag以及field_type, 生成不同的field, Field_json如下所示:

```c++
/// A field that stores a JSON value.
class Field_json :public Field_blob
{
public:
  Field_json(uchar *ptr_arg, uchar *null_ptr_arg, uint null_bit_arg,
             enum utype unireg_check_arg, const char *field_name_arg,
             TABLE_SHARE *share, uint blob_pack_length)
    : Field_blob(ptr_arg, null_ptr_arg, null_bit_arg, unireg_check_arg,
                 field_name_arg, share, blob_pack_length, &my_charset_bin)
  {}

  Field_json(uint32 len_arg, bool maybe_null_arg, const char *field_name_arg)
    :Field_blob(len_arg, maybe_null_arg, field_name_arg, &my_charset_bin, false)
  {}

  enum_field_types type() const { return MYSQL_TYPE_JSON; }
  void sql_type(String &str) const;

  uint is_equal(Create_field *new_field);
};

/**
  Get the type of this field (json).
  @param str  the string that receives the type
*/
void Field_json::sql_type(String &str) const
{
  str.set_ascii(STRING_WITH_LEN("json"));
}

/**
  Check if a new field is compatible with this one.
  @param new_field  the new field
  @return true if new_field is compatible with this field, false otherwise
*/
uint Field_json::is_equal(Create_field *new_field)
{
  // All JSON fields are compatible with each other.
  return (new_field->sql_type == real_type());
}
```

生成create table语句的函数是`ddl_store_create_info`
```c++
/*
  Build a CREATE TABLE statement for a table.

  SYNOPSIS
    ddl_store_create_info()
    thd               The thread
    tb                TABLE object
    packet            Pointer to a string where statement will be
                      written.
  
  NOTE
    Currently always return 0, but might return error code in the
    future.
    
  RETURN
    0       OK
*/

int ddl_store_create_info(THD *thd, TABLE *tb, String *packet)
{
  Field **ptr,*field;
  TABLE *table= tb;
  ...
  // fields information
  for (ptr=table->field ; (field= *ptr); ptr++)
  {
    uint flags = field->flags;

    if (ptr != table->field)
      packet->append(STRING_WITH_LEN(",\n"));

    packet->append(STRING_WITH_LEN("  "));
    append_identifier(thd,packet,field->field_name, strlen(field->field_name));
    packet->append(' ');
    // check for surprises from the previous call to Field::sql_type()
    if (type.ptr() != tmp)
      type.set(tmp, sizeof(tmp), system_charset_info);
    else
      type.set_charset(system_charset_info);

    field->sql_type(type);
    packet->append(type.ptr(), type.length(), system_charset_info);
    ...
  }
  ...
}
```

## select子查询

先看一下select where子查询的语法解析, 由于不熟, 找的地方可能不对.

```c++
/*
  Select : retrieve data from table
*/
select:
select_init:
          SELECT_SYM select_init2
select_init2:
          select_part2
select_part2:
          select_options select_item_list
          select_into select_lock_type
        ;

select_item_list:
        | select_item
select_item:
        | remember_name expr remember_end select_alias
expr:
bool_pri:
predicate:
bit_expr:
simple_expr:
simple_ident:
          ident
          {
            ...
            {
              SELECT_LEX *sel=Select;
              if ((sel->parsing_place != IN_HAVING) ||
                  (sel->get_in_sum_expr() > 0))
              {
                $$= new (thd->mem_root) Item_field(Lex->current_context(),
                                                   NullS, NullS, $1.str);
              }
              else
              {
                $$= new (thd->mem_root) Item_ref(Lex->current_context(),
                                                 NullS, NullS, $1.str);
              }
              if ($$ == NULL)
                MYSQL_YYABORT;
            }
          }

select_into:
select_from:
          FROM join_table_list where_clause group_clause having_clause start_with_connect_by
          opt_order_clause opt_limit_clause procedure_clause
where_clause:
        | WHERE
          {
            Select->parsing_place= IN_WHERE;
          }
          expr
expr:
bool_pri:
        | bool_pri comp_op predicate %prec EQ
          {
            $$= (*$2)(0)->create($1,$3);
            if ($$ == NULL)
              MYSQL_YYABORT;
          }
```

FIELD和ITEM的关系为: Item类中的tmp_table_field_from_field_type函数将一个Item类转化为一个Filed类.
比较困惑的是没有新增Item_json, 但是field_type()函数返回值仍然是MYSQL_TYPE_JSON.

```c++
/**
  Create a field based on field_type of argument.

  For now, this is only used to create a field for
  IFNULL(x,something) and time functions

  @retval
    NULL  error
  @retval
    \#    Created field
*/

Field *Item::tmp_table_field_from_field_type(TABLE *table, bool fixed_length)
{
  Field *field;
  switch (field_type()) {
  ...
  case MYSQL_TYPE_JSON:
    field= new Field_json(max_length, maybe_null, name);
  }
  if (field)
    field->init(table);
  return field;
}

研究后了解了代码的大致流程, item类是Item_field, Item_field调用fix_fields函数, 获取Field_json的信息. 调用堆栈如下:

    find_field_in_table （从table->field中获取field）
    find_field_in_table_ref
    find_field_in_tables
    Item_field::fix_fields

```c++
class Item_field :public Item_ident
{
public:
  Field *field,*result_field;
  ...
  enum_field_types field_type() const
  {
    return field->type();
  }
  ...
}
```