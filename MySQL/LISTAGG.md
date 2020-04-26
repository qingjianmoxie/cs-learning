# LISTAGG

```sql
create table jie(DEPTNO char(10),ENAME char(10));
insert into jie(DEPTNO, ENAME) values('20', '张三');
insert into jie(DEPTNO, ENAME) values('20', '李四');
insert into jie(DEPTNO, ENAME) values('20', '王二');
insert into jie(DEPTNO, ENAME) values('20', '麻子');

select listagg(ENAME,',') within group(order by ENAME) from jie;
```

    LISTAGG(ENAME,',')WITHINGROUP(ORDERBYENAME)
    --------------------------------------------------------------------------------
    张三 ,麻子 ,李四 ,王二

MySQL中相同效果的实现:

```sql
select DEPTNO, group_concat(ENAME order by ENAME separator ',') from jie where DEPTNO = '20';
```

    +--------+--------------------------------------------------+
    | DEPTNO | group_concat(ENAME order by ENAME separator ',') |
    +--------+--------------------------------------------------+
    | 20     | 张三,李四,王二,麻子                      |
    +--------+--------------------------------------------------+

[Oracle官方文档](https://docs.oracle.com/en/database/oracle/oracle-database/19/sqlrf/LISTAGG.html)