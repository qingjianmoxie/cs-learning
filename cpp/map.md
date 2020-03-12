## map是什么

顺序容器是管理数据的宝贵工具，但对大多数应用程序而言，顺序容器不提供方便的数据访问机制。举个简单的示例，当我们用它处理姓名和地址时，在这种场景下，顺序容器可能并不能如我们所愿。一种典型的方法是通过名称来寻找地址。如果记录保存在顺序容器中，就只能通过搜索得到这些数据。相比而言，map 容器提供了一种更有效的存储和访问数据的方法。

map 容器是关联容器的一种。在关联容器中，对象的位置取决于和它关联的键的值。键可以是基本类型，也可以是类类型。字符串经常被用来作为键，如果想要保存姓名和地址的记录，就可以这么使用。名称通常可能是一个或多个字符串。关联容器中的对象位置的确定取决于容器中的键的类型，而且对于特定容器类型的内部组织方式，不同的 STL 有不同的实现。

`map<K,T>`类模板定义在 map 文件头中，它定义了一个保存 T 类型对象的 map，每个 T 类型的对象都有一个关联的 K 类型的键。容器内对象的位置是通过比较键决定的。可以用适当的键值从 map 容器中检索对象。图 1 展示了一个用名称作为键的 `map<K,T>` 容器，对象是整数值，用来表示年龄。

![图1 map<K,T>容器的概念展示图](http://c.biancheng.net/uploads/allimg/180914/2-1P914095K52Z.jpg)

图1 map<K,T>容器的概念展示图

图1 表示的是`map<Name,size_t>`类型的容器，其中的 Name 类可以这样定义：

```c++
class Name
{
private:
    std::string firstname{};
    std::string secondname{};

public:
    Name(std::string first, std::string second) : firstname{first}, secondname{second} {};
    Name() = default;
    bool operator<(const Name &name)
    {
        return secondname < name.secondname || ((secondname == name.secondname) && (firstname < name.firstname));
    }
};
```

为了可以在必要时生成默认的元素，容器保存的对象通常需要定义一个默认的构造函数。当两个 Name 对象的 secondname 不同时，成员函数 operator<() 通过比较 secondname 来确定对象的顺序。如果 secondname 相等，比较结果就由 firstname 决定。string 类定义了 operator<()，因而可以用默认的`less<string>`来比较。

不要因为 map 使用`less<K>`对元素排序就被误导，这些元素并没有被组织成一个简单的有序序列，STL map 容器对元素的组织方式并没有具体要求，但元素一般都会保存在一个**平衡二叉树**中。容器中的元素被组织成一个平衡二叉树，因而树的高度——根节点和叶节点之间的高度是最低的。如果每个节点的左子树和右子树的高度差不超过 1，那么可以说这棵二叉树就是平衡的。图 2 展示了图 1 所表示的 map 容器可能的平衡二叉树。

![图2 map容器的内部组织图](http://c.biancheng.net/uploads/allimg/180914/2-1P91409592C35.jpg)

图2 map容器的内部组织图

图 2 所示的树有 3 层，所以从根节点开始，找到任意的元素最多需要 3 步。这里选择的根节点可以使树的高度最小，而且对于每个父节点来说，它的键值大于它的左子节点，但小于它的右子节点。为了保持二叉树的平衡，当添加一个新的元素时，可能会导致根节点发生改变。所以显然，在添加新元素时，为了保持树的平衡，会产生一些额外的开销。作为回报，容器中的元素越多，相对于线性排列和非平衡树，平衡树组织元素的效率也越高。从包含 n 个元素的平衡二叉树中检索一个随机元素所需的时间为 O(log<sub>2</sub>n)，从序列中检索元素所需的时间为 O(n)。

注意，O(n) 计算时间随着参数的增加而增加。O 被认为是有序的，O(n) 表明线性执行时间在以 n 增加。O(log<sub>2</sub>n) 计算时间远没有 n 增加得快，因为它是以 log<sub>2</sub>n 计算的。

## map的创建

map 类模板有 4 个类型参数，但一般只需要指定前两个模板参数的值。第 1 个是键的类型，第 2 个是所保存对象的类型，第 3 个和第 4 个模板参数分别定义了用来比较键的函数对象的类型以及为 map 分配内存的对象的类型。最后两个参数有默认值。在本节稍后部分会展示如何定义不同类型的比较键的函数对象，但不会定义可替代的分配器类型。

map<> 容器类的默认构造函数会创建一个空的 map 容器。例如，可以创建一个这样的容器，size_t 类型的值表示年龄，作为它保存的值，string 类型的值表示名称，作为它的键：

```c++
std::map<std::string, size_t>  people;
```

第 1 个模板类型参数指定键的类型是字符串，第 2 个模板类型参数指定值的类型为 size_t。当然，这里的模板类型参数可以是任何类型，唯一的要求是键必须可以用 `less<K>` 比或用自己指定的另一个函数对象来替代。

`map<K,T>`中的每个元素都是同时封装了对象及其键的`pair<const K, T>`类型对象，这里不能修改 const K。`pair<T1,T2>`类的模板定义在 utility 头文件中，它被包含在 map 头文件中。因此 people 容器中的元素是`pair<const string, size_t>`类型的。`pair<T1，T2>`这种模板类型并不是专门在这种情况下使用的。必要时可以用它将两个不同类型的对象组装成一个对象。本章稍后将讲解更多这方面的内容。

我们可以用初始化列表来指定 map 的初始值，但因为 map 中包含的是 `pair<const K, T>` 类型的元素，所以初始化列表中的值也必须是这种类型。下面展示了如何为 people 容器设置初始值：
```c++
std::map<std::string, size_t>  people{{"Ann", 25}, {"Bill", 46},{"Jack", 32},{"Jill", 32}};
```

初始化列表中的值是通过将每个嵌套花括号中的两个值传递给构造函数产生的，因此列表会包含4个`pair<const string,size_t>`对象。

utility头文件中定义了`make_pair <T1, T2>()`函数模板，它提供了一种组合 T1 和 T2 类型对象的简单方法。因此，可以按如下方式创建 pair 对象来初始化 map:
```c++
std::map<std::string,size_t>  people{std::make_pair("Ann",25),std::make_pair("Bill",46),std::make_pair("Jack", 32),std::make_pair("Jill",32)};
```

`make_pair<T1,T2>()`函数模板从函数参数中推断出类型参数值，因而由参数列表中调用`make_pair<>()`返回的是`<char const*,int>`类型的对象。因为这些对象都是 map 容器 people 的初始值，所以这些 pair 对象会被转换成 map 中元素的类型，即`pair<const string,size_t>`。

`pair<T1, T2>`的公共成员变量 first 和 second 分别保存了存储 T1 和 T2 类型的对象。只要原始 pair 对象的成员变量 first 和 second 可以隐式转换为与目标 pair 对象成员变量相同类型的变量，`pair<T1;T2>`模板的构造函数就可以提供这种类型的隐式转换。

`map<K,T>`模板定义了移动和复制构造函数，所以可以复制现有的容器。例如：
```c++
std::map<std::string, size_t>  personnel {people}; // Duplicate people map
```

map 容器 personnel 包含 people 元素的副本。

可以用另一个容器的一段元素来创建一个 map，用开始和结束迭代器以通常的方式指定元素。显然，迭代器指向的 pair 元素的类型必须和容器兼容。这里有一个示例：
```c++
std::map<std::string, size_t>  personnel {std::begin(people),std::end(people)};
```

这样就生成了 personnel，并且用 people 容器的迭代器指定的元素对它进行了初始化。map 容器提供了双向迭代器，这样就可以通过自增或自减访问元素。map 容器还提供了反向迭代器，所以可以从最后一个元素遍历到第一个元素。personnel 容器包含的元素和 people 完全相同。当然，也可以用另一个容器的元素子集来创建容器：
```c++
std::map<std::string,size_t>  personnel {++std::begin(people),std::end(people)};
```

## map插入数据

`map<K,T>`容器的成员函数 insert() 有多个版本，它们可以在 map 中插入一个或多个`pair<const K,T>`对象。只能插入 map 中不存在的元素。下面这个代码片段展示了如何插入单个元素：

```c++
std::map<std::string,size_t>  people {std::make_pair("Ann",25),std::make_pair("Bill",46) , std::make_pair ("Jack",32), std::make_pair("Jill",32)};
auto pr = std::make_pair("Fred",22); //Create a pair element and insert it
auto ret_pr = people.insert(pr);
std::cout << ret_pr.first->first << " "<< ret_pr.first->second<< "" << std::boolalpha <<ret_pr.second << "\n"; // Fred 22 true
```

第一条语句生成了一个 map 容器，并用初始化列表中的 4 个值对它进行了初始化；在这种情况下，这些值会被隐式转换为要求的类型。第二条语句生成了另一个被插入的 pair 对象 pr。pr 对象的类型是 pair<const char*,int>，因为 make_pair<>() 函数模板的类型参数是从参数类型推断出来的；但是在 insert() 操作中，这个对象会被隐式转换为容器元素类型。当然，如果不想依靠隐式转换，可以生成所要求类型的 pair 对象：
```c++
auto pr = std::make_pair<std:: string, size_t> (std:: string { "Fred"},22);
```

make_pair<>() 模板的显式类型参数决定了返回的 pair 对象的类型。可以把文字字符串作为第一个参数，然后通过隐式转换创建键需要的字符串对象。可以省略 make_pair<>() 的模板类型参数，让编译器去推断它们。假设像下面这样声明：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  auto pr = std::make_pair ("Fred",22) ; // pair<const char*, int>

</pre>

这里会返回和所要求类型不同的 pair 对象。当允许编译器推断模板参数类型时，make_pair() 的参数可以准确地确定模板参数的类型。第一个参数是一个 const char* 类型的字符串，第二个参数是 int 类型。尽管已经说明了元素的类型，但在这种情况下，并没有多大的用处，因为在插入一个新元素时，pair 对象可以被隐式转换为容器所需类型。当 make_pair() 的参数的类型不能隐式转换成容器的键和对象的类型时，就需要注意了。

成员函数 insert() 会返回一个 pair<iterator,bool> 对象。对象的成员 first 是一个迭代器，它要么指向插入元素，要么指向阻止插入的元素。如果 map 中已经保存了一个和这个键相同的对象，就会出现后面这种情况。这个对象的成员变量 second (布尔型)是返回对象，如果插入成功，返回值为 true，否则为 false。

输出语句像我们看到的那样，访问插入 pair 的成员变量 first 的表达式是 ret_pr.first->first。ret_pr 的成员变量 first 是一个指向 pair 对象的迭代器，所以可以使用->操作符来访问它的成员变量 first。输出展示了插入的元素。可以通过下面这个循环进行验证：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  for (const auto& p : people)
2.  std::cout << std::setw(10) << std::left << p.first << " "<< p.second <<"\n";

</pre>

循环变量 p 通过引用依次访问 map 容器 people 中的每个元素。输出如下：

Ann 25
Bill 46
Fred 22
Jack 32
Jill 32

元素是以键的升序排列的，因为 map 中默认使用 less<string> 函数对象对它们进行排序。

通过执行下面这两条语句，可以看出元素插入后的效果：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  ret_pr = people.insert(std::make_pair("Bill", 48));
2.  std:: cout << ret_pr.first->first <<" "<<ret_pr.first->second<< " "<<std::boolalpha<<ret_pr.second << "\n"; // Bill 46 false

</pre>

程序会输出如注释所示的内容。insert() 返回了一个 pair 对象 ret_pr，它的成员变量 first 指向 map 中已有的和键匹配的元素，成员变量 second 为 false，表明元素没有插入成功。

当元素已经存在时，如果想将键值“Bill”对应的年龄值改为 48，可以像下面这样使用 insert() 返回的 pair 对象来做到这一点：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  if(!ret_pr.second) // If the element is there change the age
2.  ret_pr.first—>second = 48;

</pre>

当键已经存在于 map 容器中时，ret_pr 的成员变量 second为false，所以这段代码会将 map 中这个元素的成员变量 second 的值设为 48。

可以用 pair 构造函数生成的对象作为 insert() 的参数：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  ret_pr = people.insert(std::pair<const std::string, size_t> {"Bill", 48});

</pre>

这里会调用一个具有右值引用参数的 insert() 版本，所以假如元素不在容器中，那么它会被移到容器中。

也可以提供一个提示符来指出元素插入的位置。提示符是迭代器的形式，它指向容器中的一个现有元素，通常从提示符指示的位置开始查找新元素的插入位置。好的提示符可以提高插入操作的速度，反之亦然。例如：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  auto ret_pr = people.insert(std::make_pair("Jim", 48));
2.  people.insert (ret_pr.first, std::make_pair ("Ian", 38)）；

</pre>

第一条语句插入了一个元素，并像前面那样返回了一个对象。pair 对象的成员变量 first 是一个指向被插入的元素或容器中与插入元素有相同键的元素的迭代器。

下一个 insert() 函数的第一个参数和上面的提示符有关，所以这里就是插入元素的地方。insert() 的第二个参数指定的新元素会被插入到提示符的前面，并尽可能地靠近它。如果提示符不能以这种方式使用，那么将忽略它。同样地，如果被插入的元素已经在 map 中，会导致元素插入失败。带提示符的 insert() 调用会返回一个指向被插入元素或容器中阻止此插入操作的元素的迭代器，因此可以使用返回值来确定插入是否成功。当确定元素不存在时，可以只提供一个插入符，这是一个好的想法。如果不那么确定，而且仍然想使用插入符，map 中的 count() 函数对我们有一些帮助。它会返回 map 中指定键对应元素的数目，这个数目可能是 0 或 1。

因此代码可以这样写：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  if (!people.count("Ian"))
2.  people.insert (ret_pr.first, std::make_pair ("Ian", 38));

</pre>

只有当 count() 函数返回 0 时，insert() 才会被调用，这说明“Ian”键不在 map 中。当然，在不用提示插入元素时，需要做一次这样的检查，但 insert() 的返回值不管怎样都能告诉我们插入结果。

也可以将外部源中的一段元素插入 map 中，这些元素不必来自另一个 map 容器，但必须和被插入容器中的元素是同类型。这里有一些示例：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  std::map<std::string, size_t>  crowd {{"May", 55}, {"Pat",66}, {"Al", 22}, {"Ben", 44}};
2.  auto iter = std::begin(people);
3.  std::advance(iter, 4);  // begin iterator+ 4
4.  crowd.insert(++std::begin(people),iter); // Insert 2nd, 3rd, and 4th elements from people

</pre>

这里生成了一个新的 map 容器 crowd，它有 4 个初始元素。iter 被初始化为 people 的开始迭代器。map 容器的迭代器是双向的，这样就可以对它们进行自增或自减，但是不能加上或减去一个值。这里使用了一个在前面章节中用过的 advance() 函数模板的实例来对 iter 加 4，所以它将会指向第 5 个元素，它在下一行被当作 crowd 的成员函数 insert() 的参数，用来作为指定元素段的结束迭代器。map 容器 people 的开始迭代器加 1，然后用它作为插入元素段的开始迭代器，所以会从 crowd 的第 2 个元素开始插入 3 个元素。

下面是一个接受初始化列表作为参数的 insert() 版本：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  crowd.insert({{"Bert", 44}, {"Ellen”, 99}});

</pre>

这里会将初始化列表中的两个元素插入到 map 容器 crowd 中。参数表达式生成的 initializer_list<> 对象是 initializer_list<const string，size_t> 类型，因为编译器知道这时 insert() 函数的参数是这种类型。当然，也可以单独创建一个初始化列表，然后将它作为参数传递给 insert() 函数：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  std::initializer_list<std::pair<const std:: string, size_t>>init {{"Bert", 44}, {"Ellen", 99}};
2.  crowd.insert(init);

</pre>

initializer-list 模板的第一个类型参数必须为 const 类型。initializer_list <string,size_t> 无法隐式转换为 initializer_list<const string, size_t>，所以前者类型的对象不能作为 insert() 函数的参数。

下面你会看到这些操作的一个完整示例。此处会定义一个有点不同的对象。Name 类型代表人名，这个类定义的头文件的内容如下：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  // Defines a person's name
2.  #ifndef NAME_H
3.  #define NAME_H
4.  #include <string>                                // For string class
5.  #include <ostream>                               // For output streams
6.  #include <istream>                               // For input streams

8.  class  Name
9.  {
10.  private:
11.  std::string  first {};
12.  std::string  second {};

14.  public:
15.  Name(const std::string& name1, const std::string& name2) : first (name1), second (name2) {}
16.  Name() = default;
17.  // Less-than operator
18.  bool operator<(const Name& name) const
19.  {
20.  return second < name.second || (second == name.second && first < name.first);
21.  }
22.  friend std::istream& operator>>(std::istream& in, Name& name);
23.  friend std::ostream& operator<<(std::ostream& out, const Name& name);
24.  };

26.  // Extraction operator overload
27.  inline std::istream& operator>>(std::istream& in, Name& name)
28.  {
29.  in >> name.first >> name.second;
30.  return in;
31.  }

33.  // Insertion operator overload
34.  inline std::ostream& operator<<(std::ostream& out, const Name& name)
35.  {
36.  out << name.first + " " + name.second;
37.  return out;
38.  }

40.  #endif

</pre>

这个类非常简单，只有两个 string 类型的私有成员变量 first 和 second。这个构造函数可以接受 string 类型的参数或字符串常量参数。为了可以用这种对象作为 map 容器的键，必须为这个类定义 operator<()。为了便于对象的输入输出，也需要为流定义插入和提取操作。

map 容器中的元素是 std::pair<const Name，size_t> 类型，我们可以用下面定义的别名来简化代码：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  u[sin](http://c.biancheng.net/ref/sin.html)g Entry = std::pair<const Name, size_t>;

</pre>

当容器是 map<name;site_t> 类型时，我们只能用 Entry 作为容器元素的类型。为了便于 map 元素的输出，我们可以把别名放到一个函数的定义中：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  Entry  get_entry()
2.  {
3.  std::cout << "Enter first and second names followed by the age: ";
4.  Name  name {};
5.  size_t  age {};
6.  std::cin >> name >> age;
7.  return make_pair(name, age);
8.  }

</pre>

从 cin 先后读入了一个 Name 对象和一个年龄值，并用它们生成了一个 pair 对象。从输入中读取 name 激发了定义在 Name.h 中的 istream 对象的重载函数 operator>>()，同样也支持流对象对 Name 对象的读入。

可以输出容器元素的辅助函数也是很有用的：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  void list_entries(const map<Name, size_t>& people)
2.  {
3.  for(auto& entry : people)
4.  {
5.  std::cout << std::left << std::setw(30) << entry.first<< std::right << std::setw(4) << entry.second << std::endl;
6.  }
7.  }

</pre>

这里只用了基于范围的 [for 循环](http://c.biancheng.net/view/172.html)来对元素进行遍历。循环变量 entry 依次引用 map 的每个元素。每一个 map 元素都是一个 pair 对象，它的成员变量 first 是 Name 类型的对象，成员变量 second 是 size_t 类型的值。

包含 main() 函数的源文件中的内容如下：

<pre class="cpp sh_cpp snippet-formatted sh_sourceCode" style="margin: 0px; display: block; padding: 0px; font-size: 14px; line-height: 1.6em; color: rgb(102, 102, 102); white-space: pre-wrap; overflow-wrap: break-word; background: none; border: none; border-radius: 0px;">

1.  // Storing names and ages
2.  #include <iostream>                              // For s[tan](http://c.biancheng.net/ref/tan.html)dard streams
3.  #include <iomanip>                               // For stream manipulators
4.  #include <string>                                // For string class
5.  #include <map>                                   // For map container class
6.  #include <utility>                               // For pair<> & make_pair<>()
7.  #include <cctype>                                // For [toupper](http://c.biancheng.net/ref/toupper.html)()
8.  #include "Name.h"
9.  using std::string;
10.  using Entry = std::pair<const Name, size_t>;
11.  using std::make_pair;
12.  using std::map;

14.  // Create a map entry from input
15.  Entry  get_entry()
16.  {
17.  std::cout << "Enter first and second names followed by the age: ";
18.  Name  name {};
19.  size_t  age {};
20.  std::cin >> name >> age;
21.  return make_pair(name, age);
22.  }

24.  // Output the elements in a map
25.  void list_entries(const map<Name, size_t>& people)
26.  {
27.  for(auto& entry : people)
28.  {
29.  std::cout << std::left << std::setw(30) << entry.first
30.  << std::right << std::setw(4) << entry.second << std::endl;
31.  }
32.  }

35.  int main()
36.  {
37.  map<Name, size_t>  people {{{"Ann", "Dante"}, 25}, {{"Bill", "Hook"}, 46}, {{"Jim", "Jams"}, 32}, {{"Mark", "Time"}, 32}};

39.  std::cout << "\nThe initial contents of the map is:\n";
40.  list_entries(people);

42.  char answer {'Y'};
43.  std::cout << "\nEnter a Name and age entry.\n";
44.  while(std::toupper(answer) == 'Y')
45.  {
46.  Entry  entry {get_entry()};
47.  auto pr = people.insert(entry);
48.  if(!pr.second)
49.  { // It's there already - check whether we should update
50.  std::cout << "Key \"" << pr.first->first << "\" already present. Do you want to update the age (Y or N)? ";
51.  std::cin >> answer;
52.  if(std::toupper(answer) == 'Y')
53.  pr.first->second = entry.second;
54.  }
55.  // Check whether there are more to be entered
56.  std::cout << "Do you want to enter another entry(Y or N)? ";
57.  std::cin >> answer;
58.  }

60.  std::cout << "\nThe map now contains the following entries:\n";
61.  list_entries(people);
62.  }

</pre>

定义一些额外的别名可以减少代码冗余。可以用 using::namespace 来完全消除对 std 名称限定的需要，但不赞成这么做，因为 std 中的所有命名都被有效导入到当前作用域内，这违背了定义命名空间的初衷。

map 容器的定义中有一些初始值，它们是初始化列表中的元素。这里只是为了说明如何在这种情况下使用嵌套花括号。在定义元素的每个初始化列表中，花括号中的 Name 对象都是初始化列表，每个初始化元素的花括号中都是一个 Name 对象和一个年龄值。最外面的一对花括号包括了所有元素的初始值。

辅助函数 list_entries() 用来显示容器的初始状态。从 for 循环中读入了更多的 entry 对象。循环由 answer 的值控制，如果一开始它的值就是 'Y'，那么最少执行一次循环，最少从键盘输入一个元素。entry 对象的类型是 Entry 它也是容器元素的类型。辅助函数 get_entry() 的返回对象被作为 entry 的初始值。然后将它作为 insert() 的参数插入到容器中。这个函数返回的 pair 对象有一个成员变量 first,它指向容器中和 entry 的键匹配的元素。如果在插入之前，容器中就存在这个元素，那么它指向的是原始容器中的元素。如果键已经存在于容器中，插入不会成功，并且 pr 的成员变量 second 为 false。

pr.first 是容器中元素的一个迭代器，因此 pr.first->second 可以访问与键关联的对象，如果用户确认需要更新，pr.first->second 的值会变为 entry.second。循环中的最后一个操作可以决定是否输入更多的 entry。在不需要输入更多的 entry 时，循环结束，用 list_entries() 函数输出这个容器最终的内容。

下面是这个示例的一些输出结果:

The initial contents of the map is:
Ann Dante                       25
Bill Hook                       46
Jim Jams                        32
Mark Time                       32

Enter a Name and age entry.
Enter first and second names followed by the age: Emma Nate 42
Do you want to enter another entry(Y or N)? y
Enter first and second names followed by the age: Emma Nate 43
Key "Emma Nate" already present. Do you want to update the age (Y or N)? Y
Do you want to enter another entry(Y or N)? y
Enter first and second names followed by the age: Eamonn Target 56
Do you want to enter another entry(Y or N)? n

The map now contains the following entries:
Ann Dante                       25
Bill Hook                       46
Jim Jams                        32
Emma Nate                       43
Eamonn Target                   56
Mark Time                       32

这些元素是以键的升序排列的，因为容器中的元素是使用 less<Name> 进行排序的。Name::operator<() 先比较姓，在姓相同时才比较名。这导致返回的姓名是正常的顺序。
