# JSON with MariaDB Platform: What Is JSON and Why Use It – With Examples

Posted on test June 27, 2019 by [Anders Karlsson](https://mariadb.com/resources/blog/author/anderskarlsson/)

*This is an update of the popular post on JSON with MariaDB 10.2\. We've brought the details up-to-date with the latest MariaDB releases. *

**What is JSON and why use it?**

JSON is fast becoming the standard format for data interchange and for unstructured data, and [MariaDB Platform](https://mariadb.com/products/mariadb-platform/) (in fact, all MariaDB versions 10.2 and later) include a range of [JSON supporting functions](https://mariadb.com/topic/json/ "JSON supporting functions"). There are a few good reasons why a JSON datatype hasn't been implemented(注:**MariaDB没有json数据类型**), but one is that there are just not many advantages to that, as JSON is a text-based format. Here I'll describe JSON and the use cases for it, as well as the MariaDB JSON functions and their uses. We'll also look at some other MariaDB features that are useful for JSON processing.

So to begin, *Why do we need JSON?* Or to put it differently(或者换句话说), why do we not store all data in JSON? Well, the reason as I see it is that some data we work with really is best treated as schemaless, whereas other data really should be handled more strictly in a schema. In my mind that means that mixing relational data with unstructured data is what we really want.(在我看来，这意味着将关系数据与非结构化数据混合是我们真正想要的。) (For more detail on JSON functions, including sample data and queries, check out this white paper: [Flexible Data Modeling with MariaDB Server: JSON Functions](https://go.mariadb.com/flexible-data-modeling-with-JSON-white-paper.html).)

Using JSON for unstructured data is rather neat(整齐), and JSON is even standardized (see [json.org](http://www.json.org/ "json.org")). But that hasn't always been so. When the sad old git who is writing this stuff started working in this industry, computers were rare, expensive, handled only by experts (so how I got to work with them is a mystery).(当写这文章的可悲的老混蛋开始在这个行业工作时，计算机稀有，昂贵，仅由专家来处理) Also, they were as powerful as a slide ruler, except they were fed with punch cards (and probably powered by steam).(而且，它们像滑尺一样强大，除了它们被打孔卡喂食, 并可能由蒸汽驱动) Anyway, no one in their right mind would have considered string pictures of cute cats as something to be on a computer, or actually stuff to be stored in a database. The little that would fit was the really important stuff, like price, amount in stock, customer names, billing addresses and such – and nothing else. And not only that, but stuff that was stored had some kind of value, somehow, which meant it had to follow certain rules. Like, a price had to be a number of some kind, with a value 0 or higher and some other restrictions. As you see, these were hard and relentless(苛刻的) times.

And then time moved on and people started buying things on the Internet and stuff such as Facebook and Google came around. The issue with computer storage was now not how to fit all that nicely structured data in it, but rather once we have filled that hard drive on our desktop with all the product, customers and transactions from Amazon and a full 17.85% of that drive is now occupied by that old-style structured data, what more do we put in there? Maybe we could add some more information on the items for sale in that database, and some general information on who is buying the stuff? That should fill up that disk nicely, right? Well, yes, but that new data, although related to the structured data I already have, is largely unstructured.

Say, for example, you write a review of a product on Amazon late in the morning after a good deal of heavy “partying”, the contents of that would hardly be considered “structured.” If you didn't like the product (which you probably didn't), then the appropriate terms for large parts of that review would probably be “profanity” or “foul language”.

The way to deal with this situation is a mix of structured and unstructured data, with some kind of relation between the two. Like a column of unstructured data in each relational database table. 

**JSON examples: Structured and unstructured data**

With that out of the way, let's consider an example using structured as well as unstructured JSON data. Assume we have a store selling different types of clothing, pants, jackets and shoes and we are to create a table to hold the inventory(库存). This table would have some columns that are always there and that have the same meaning for all rows in the table, like name, amount in stock and price. These are items that are well suited to a relational format.

On top of this we want to add attributes that have different meaning for each type of or even each instance of items. Here we have things like color, width, length and size. These we consider non-relational as the interpretation(解释) of these attributes(属性) is different depending of the type of garment(服装) (like size M or shoe sizes or a “zebra striped” color) and some garments might have a unique attribute, like designer or recommended by staff. Our table might then look something like this:

```sql
MariaDB> CREATE TABLE products(id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL,
  price DECIMAL(9,2) NOT NULL,
  stock INTEGER NOT NULL,
  attr VARCHAR(1024));
```

In this table we have a few columns that look like columns in any relational database table, and then we have a column, called attr, that can hold any relevant attribute for the garment in question and we will store that as a JSON string. You probably notice that we aren't using a JSON datatype here as that is not present in MariaDB, despite the fact that there are JSON functions, but those JSON functions act on a text string with JSON content.

But there is one issue with the above that I don't particularly care for: as the attr column is plain text(纯文本), any kind of data can be put in the attr column, even non-valid JSON. The good thing is that there is a fix for this in MariaDB 10.2 and later, which is CHECK constraints that actually work. This kind of constraint kicks in whenever a row is INSERTed or UPDATEed, at which point any CHECK constraint runs and validates the data – and if the validation fails the operation also fails. Before I show an example I just want to mention one JSON function we are to use here: JSON_VALID, which takes a string and checks if it is valid JSON. Note that although CHECK constraints are particularly valid here, they can be used for any kind of data validation.

Armed with this, let's rewrite the statement that creates the table like this:

```sql
MariaDB> CREATE TABLE products(id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL,
  price DECIMAL(9,2) NOT NULL,
  stock INTEGER NOT NULL,
  attr VARCHAR(1024),
  CHECK (JSON_VALID(attr)));
```

Let's give this a try now:

```sql
MariaDB> INSERT INTO products VALUES(NULL, 'Jeans', 10.5, 165, NULL);
ERROR 4025 (23000): CONSTRAINT `CONSTRAINT_1` failed for `inventory`.`products`
```

OK, that didn't work out. What happens here is that a NULL string isn't a valid JSON value, so we need to rewrite our table definition:

```sql
MariaDB> CREATE TABLE products(id INTEGER NOT NULL PRIMARY KEY AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL,
  price DECIMAL(9,2) NOT NULL,
  stock INTEGER NOT NULL,
  attr VARCHAR(1024),
  CHECK (attr IS NULL OR JSON_VALID(attr)));
```

Let's try it again:

```sql
MariaDB> INSERT INTO products VALUES(NULL, 'Jeans', 10.5, 165, NULL);
Query OK, 1 row affected (0.01 sec)
MariaDB> INSERT INTO products VALUES(NULL, 'Shirt', 10.5, 78, '{"size": 42, "colour": "white"}');
Query OK, 1 row affected (0.01 sec)
MariaDB> INSERT INTO products VALUES(NULL, 'Blouse', 17, 15, '{"colour": "white}');
ERROR 4025 (23000): CONSTRAINT `CONSTRAINT_1` failed for `inventory`.`products`
```

That last statement failed because of malformed JSON (a double quote was forgotten about), so let's correct that:

```sql
MariaDB> INSERT INTO products VALUES(NULL, 'Blouse', 17, 15, '{"colour": "white"}');
Query OK, 1 row affected (0.01 sec)
```

**Indexes on JSON values**

One thing that has yet to be discussed is indexes on JSON values. As the attr column in our example is plain text, we can of course index it as usual, but that is probably not what you want to do. Rather it would be neat to create an index on individual attributes in that JSON string. MariaDB doesn't yet support functional indexes (i.e., functions not on values but on computed values). What MariaDB does have, though, is Virtual Columns, which can be indexed (and as of MariaDB 10.2 these virtual columns don't have to be persistent(持久的)). [Learn more about Virtual Columns.](https://mariadb.com/resources/blog/putting-virtual-columns-good-use)

The easiest way to explain this is with an example. Let's say we want an index on the colour attribute, if such a thing exists. For this we need two things: A virtual column that contains the colour attribute as extracted(提取) from the attr column, and then an index on that. In this case we will be using the JSON_VALUE function that takes a JSON value and a path, the latter describing the JSON operation to be performed, somewhat like a query language for JSON.

We end up with something like this:

```sql
MariaDB> ALTER TABLE products ADD attr_colour VARCHAR(32) AS (JSON_VALUE(attr, '$.colour'));
MariaDB> CREATE INDEX products_attr_colour_ix ON products(attr_colour);
```

With that in place, let's see how that works:

```sql
MariaDB> SELECT * FROM products WHERE attr_colour = 'white';
+----+--------+-------+-------+---------------------------------+-------------+
| id | name   | price | stock | attr                            | attr_colour |
+----+--------+-------+-------+---------------------------------+-------------+
|  2 | Shirt  | 10.50 |    78 | {"size": 42, "colour": "white"} | white       |
|  3 | Blouse | 17.00 |    15 | {"colour": "white"}             | white       |
+----+--------+-------+-------+---------------------------------+-------------+
2 rows in set (0.00 sec)
```

And let's see if that index is working as it should:

```sql
MariaDB> EXPLAIN SELECT * FROM products WHERE attr_colour = 'white';
+------+-------------+----------+------+-------------------------+-------------------------+---------+-------+------+------------+
| id   | select_type | table    | type | possible_keys           | key                     | key_len | ref   | rows | Etra       |
+------+-------------+----------+------+-------------------------+-------------------------+---------+-------+------+------------+
|    1 | SIMPLE      | products | ref  | products_attr_colour_ix | products_attr_colour_ix | 99      | const |    2 | Uing where |
+------+-------------+----------+------+-------------------------+-------------------------+---------+-------+------+------------+
1 row in set (0.00 sec)
```

And just to show that the column attr_colour is a computed column that depends on the attr column, let's try updating the colour for the blouse and make that red instead of white and then search that. To replace a value in a JSON object MariaDB provides the JSON_REPLACE functions. (For all JSON functions in MariaDB see [http://mariadb.com/kb/en/mariadb/json-functions/](http://mariadb.com/kb/en/mariadb/json-functions/).)

```sql
MariaDB> UPDATE products SET attr = JSON_REPLACE(attr, '$.colour', 'red') WHERE name = 'Blouse';
Query OK, 1 row affected (0.01 sec)
Rows matched: 1  Changed: 1  Warnings: 0
MariaDB> SELECT attr_colour FROM products WHERE name = 'blouse';
+-------------+
| attr_colour |
+-------------+
| red         |
+-------------+
1 row in set (0.00 sec)
```
