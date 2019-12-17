# CREATE TABLE ... SELECT statement

```sql
CREATE TABLE new_tb1 [AS] SELECT * FROM orig_tb1;
```

MySQL creates new columns for all elements in the SELECT. For example:

```sql
CREATE TABLE test (a INT NOT NULL AUTO_INCREMENT,
PRIMARY KEY (a), KEY(b))
ENGINE=MyISAM SELECT b,c FROM test2;
```

This creates a MyISAM table with three columns, a, b, and c. The ENGINE option is part of the CREATE TABLE statement, and should not be used following the SELECT; this would result in a syntax error. The same is true for other CREATE TABLE options such as CHARSET.

Notice that the columns from the SELECT statement are appended to the right side of the table, not overlapped onto it. Take the following example:

```sql
mysql> SELECT * FROM foo;
+---+
| n |
+---+
| 1 |
+---+

mysql> CREATE TABLE bar (m INT) SELECT n FROM foo;
Query OK, 1 row affected (0.02 sec)
Records: 1  Duplicates: 0  Warnings: 0

mysql> SELECT * FROM bar;
+------+---+
| m    | n |
+------+---+
| NULL | 1 |
+------+---+
1 row in set (0.00 sec)
```

For each row in table foo, a row is inserted in bar with the values from foo and default values for the new columns.

In a table resulting from CREATE TABLE ... SELECT, columns named only in the CREATE TABLE part come first. Columns named in both parts or only in the SELECT part come after that. The data type of SELECT columns can be overridden by also specifying the column in the CREATE TABLE part.

If any errors occur while copying the data to the table, it is automatically dropped and not created.

You can precede(先于) the SELECT by IGNORE or REPLACE to indicate how to handle rows that duplicate unique key values. With IGNORE, rows that duplicate an existing row on a unique key value are discarded(丢弃). With REPLACE, new rows replace rows that have the same unique key value. If neither IGNORE nor REPLACE is specified, duplicate unique key values result in an error. For more information, see [Comparison of the IGNORE Keyword and Strict SQL Mode](https://dev.mysql.com/doc/refman/8.0/en/sql-mode.html#ignore-strict-comparison).

Because the ordering of the rows in the underlying SELECT statements cannot always be determined, CREATE TABLE ... IGNORE SELECT and CREATE TABLE ... REPLACE SELECT statements are flagged as unsafe for statement-based replication. Such statements produce a warning in the error log when using statement-based mode and are written to the binary log using the row-based format when using MIXED mode. See also [Section 17.2.1.1, "Advantages and Disadvantages of Statement-Based and Row-Based Replication"](https://dev.mysql.com/doc/refman/8.0/en/replication-sbr-rbr.html).

CREATE TABLE ... SELECT does not automatically create any indexes for you. This is done intentionally to make the statement as flexible as possible.(这样做是为了使语句尽可能灵活.) If you want to have indexes in the created table, you should specify these before the SELECT statement:

```sql
CREATE TABLE bar (UNIQUE (n)) SELECT n FROM foo;
```

For CREATE TABLE ... SELECT, the destination table does not preserve(保存) information about whether columns in the selected-from table are generated columns. The SELECT part of the statement cannot assign values to generated columns in the destination table.

For CREATE TABLE ... SELECT, the destination table does preserve expression default values from the original table.

Some conversion of data types might occur. For example, the AUTO_INCREMENT attribute is not preserved, and VARCHAR columns can become CHAR columns. Retrained attributes are NULL (or NOT NULL) and, for those columns that have them, CHARACTER SET, COLLATION, COMMENT, and the DEFAULT clause.

When creating a table with CREATE TABLE ... SELECT, make sure to alias any function calls or expressions in the query. If you do not, the CREATE statement might fail or result in undesirable(不受欢迎的) column names.

```sql
CREATE TABLE artists_and_works
    SELECT artist.name, COUNT(work.artist_id) AS number_of_works
    FROM artist LEFT JOIN work ON artist.id = work.artist_id
    GROUP BY artist.id;
```

You can also explicitly specify the data type for a column in the created table:

```sql
CREATE TABLE foo (a TINYINT NOT NULL) SELECT b+1 AS a FROM bar;
```

For CREATE TABLE ... SELECT, if IF NOT EXISTS is given and the target table exists, nothing is inserted into the destination table, and the statement is not logged.

To ensure that the binary log can be used to re-create the original tables, MySQL does not permit concurrent inserts during `CREATE TABLE ... SELECT`.

You cannot use FOR UPDATE as part of the SELECT in a statement such as `CREATE TABLE new_table SELECT ... FROM old_table ....` If you attempt to do so, the statement fails.