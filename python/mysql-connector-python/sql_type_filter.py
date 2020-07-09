# _*_ coding:utf-8 _*_
import re

insert_pattern  = re.compile(r"^\s*insert.*", re.I | re.S)
update_pattern  = re.compile(r"^\s*update.*", re.I | re.S)
delete_pattern  = re.compile(r"^\s*delete.*", re.I | re.S)
select_pattern  = re.compile(r"^\s*select.*", re.I | re.S)
show_pattern    = re.compile(r"^\s*show.*", re.I | re.S)
explain_pattern = re.compile(r"^\s*explain.*", re.I | re.S)

def is_need_prepare(sql):
    if insert_pattern.match(sql):
        return True
    elif update_pattern.match(sql):
        return True
    elif delete_pattern.match(sql):
        return True
    elif select_pattern.match(sql):
        return True
    else:
        return False

def need_commit(sql):
    if select_pattern.match(sql):
        return False
    else:
        return True

def check_sql_has_record(sql):
    if select_pattern.match(sql):
        return True
    elif show_pattern.match(sql):
        return True
    elif explain_pattern.match(sql):
        return True
    else:
        return False


if __name__ == "__main__":

    insert = "insert into xxxxxxxxxxxxxxxxx"
    update = "update xx.xx set"
    delete = "delete from xxxx"
    select = "select xxx,xxx,,xxx from"
    create = "create database"

    print is_need_prepare(insert)
    print is_need_prepare(update)
    print is_need_prepare(delete)
    print is_need_prepare(select)
    print is_need_prepare(create)
