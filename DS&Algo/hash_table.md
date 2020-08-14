# hash table

哈希表 是一种使用哈希函数组织数据的数据结构，它支持快速插入和搜索。

散列表的速度很快！还记得关于数组和链表的讨论吗？你可以立即获取数组中的元素，而散列表也使用数组来存储数据，因此其获取元素的速度与数组一样快。

## 哈希表的原理

哈希表（又称散列表）的原理为：借助 哈希函数，将键映射到存储桶地址。更确切地说，

+ 首先开辟一定长度的，具有连续物理地址的桶数组；

+ 当我们插入一个新的键时，哈希函数将决定该键应该分配到哪个桶中，并将该键存储在相应的桶中；

+ 当我们想要搜索一个键时，哈希表将使用哈希函数来找到对应的桶，并在该桶中进行搜索。

**示例**

![](https://pic.leetcode-cn.com/210cf5c195c2e0f624bb38accb0a543e6f5c184130c8267d20aad00500e65da2-1.png)

在示例中，我们使用 `y = x ％ 5` 作为哈希函数，来完成插入和搜索策略。

1. 插入：我们通过哈希函数解析键，将它们映射到相应的桶中。

    + 例如，根据哈希函数，1987 将分配给桶 2，而 24 分配给桶 4。

2. 搜索：我们通过哈希函数解析键，得到桶地址，然后在该存储桶中搜索。

    + 如果我们搜索 1987，我们将使用哈希函数将 1987 映射到 2。因此我们在桶 2 中搜索，我们在那个桶中成功找到了 1987。

    + 例如，如果我们搜索 23，将映射 23 到 3，并在桶 3 中搜索。我们发现 23 不在桶 3 中，这意味着 23 不在哈希表中。

注意到键 1987 和 2 被映射到了同一个桶中，我们称之为 哈希冲突，哈希冲突与哈希函数有关，但又难以避免。有关处理哈希冲突的办法，我们将在下一节详细介绍。

## 负载因子

负载因子 又叫装填因子，是哈希表的一个重要参数，它反映了哈希表的装满程度。

在上面的例子中，我们发现有的桶处于空闲状态，而有的桶产生了哈希冲突，如何解决这个问题呢？

不难想到，哈希函数是导致冲突的原因之一，更进一步，如果我们增加桶的数量，再采用合适的哈希函数，可使发生冲突的可能性大大减小。

然而，桶的个数太多则会造成大量的空间浪费。比如一个公司的电话号码共有 10 个，每个电话号码是 00000000 到 99999999 内的任意八位数字，如果我们想到一种方案，将每个可能的电话号码作为一个桶，使得电话号码与桶一一对应，则一共需要创建 10^8 个桶。

![](https://pic.leetcode-cn.com/863a2c3059cb9353d884420fb23c8e6e4ec65602d145782970bae39c9ba3957c-2.png)

**实际利用桶的个数** 与 **桶的总数** 的比值，称为负载因子。在这个实例中，负载因子太小甚至接近于 0，这样的方案显然是不现实的。

比较合理的负载因子是 `0.7`，如果数据量是 7，则会创建 10 个桶，以此类推。随着插入的数据量的增加，计算机会逐渐增加桶的个数，并选择合适的哈希函数，使得数据经过映射之后能均匀地分布在桶中。

## 设计哈希表时的问题

理解了什么是哈希表之后，我们再来讨论一下设计哈希表时应当注意的几个问题。

### 1. 哈希函数

哈希函数是哈希表中最重要的组件，用于将键映射到特定的桶。在上一节的示例中，我们使用 y = x % 5 作为散列函数，其中 x 是键值，y 是映射之后对应的桶的索引。

一个好的哈希函数，应当具备以下几个特点：

1. 哈希函数的键与桶的对应关系具有确定性。也就是说，对于 key 所映射的桶地址，只由 key 键本身决定，而不由其他因素决定；

2. 哈希函数不应太过复杂。太过于复杂的哈希函数将导致计算桶地址不能快速完成，从而无法快速定位桶；

3. 映射结果的分布应具有均匀性。对于特定的桶空间，我们应尽量保证数据经过哈希函数映射之后，能够均匀地分布在桶的整个地址空间中。

下面是一些哈希函数的示例：

键的种类 | 键的范围 | 桶的数量 | 相关hash函数
:- | :- | :- | :-
int | 0~100,000 | 1000 | y = x % 1000
char | 'a'~'z' | 26 | y = x - 'a'
arr(int) | arr.length < 10, arr[i]∈[0,1] | 1024 | y = x0 * 2^0 + x1 * 2^1 + ... + x9 * 2^9
arr(int) | arr.length < 10, arr[i]∈[0,3] | 1024 | y = x0 * 4^0 + x1 * 4^1 + ... + x4 * 4^4

*在上面的表格中, 我们使用 x 表示键, y 表示映射后的结果*

理想情况下，完美的哈希函数将是键和桶之间的一对一映射。然而，在大多数情况下，完美的哈希函数并不多见。一般来讲，结果分布越随机，越均匀的哈希函数，它的性能越好。一方面，如果分布过于集中在某些桶中，会加剧这些桶发生冲突的概率；另一方面，剩余的桶由于没有得到有效利用导致空间利用率较低。

### 2. 冲突解决

一般情况下，哈希函数会起到压缩键的地址空间的作用，设键的地址空间为 S，桶的地址空间为 T，则有 S ≫ T。

![](https://pic.leetcode-cn.com/89e7240c6c7aa9f1a5c3e514aeae560a9e17878ed67be8b3b439b2b486ff6096-2.png)


因此，经过映射之后，不同的数据会不可避免地分配到同一个桶中，这时便产生了冲突。例如，在我们之前的哈希函数 y = x ％ 5 中，1987 和 2 都分配给了桶 2，这是一个冲突。

为了避免哈希冲突，我们简要介绍几种解决冲突的办法。

**线性试探法**

线性试探法属于开放定址法的一种，除此之外，开放定址法还包括二次探测法、双重哈希法等。

所谓线性试探法，就是当插入键 key 时，如果发现桶单元 `bucket[hash(key)]` 已经被占用，则向下线性寻找，直到找到可以使用的空桶。具体说来，经过第 i 次试探之后，桶单元应为：

    bucket[(hash(key)+i) mod M],  i=1,2,3…

例如对于键的集合为 {8, 9, 21, 17, 34, 21, 4}，哈希函数为 y = x % 8，集合中的前三个数 8, 9, 21，它们分别映射到 0, 1, 5 号桶中。

而对于第四个数 17，它本应映射到 1 号桶中，由于 1 号桶中已经保存了 9，因此将 17 保存在下一个空桶—— 2 号桶中。同理，34 余数为 2，会保存在 3 号桶中。

    +----+----+----+----+----+----+----+----+
    | 8  | 9  | 17 | 34 | 4  | 21 |    |    |
    +----+----+----+----+----+----+----+----+
      0    1    2    3    4    5    6    7   

当 **查找** 某个键时，首先会通过哈希函数计算出桶的地址，然后比较该桶中保存的值是否为该键，如果不是，则继续向下寻找。如果查找到末尾，则会从头开始查找。

而 **删除** 某个键时，为了避免查找过程中出现信息丢失，会将删除位置标记为 deleted，这样当进行线性查找时，遇到 deleted 会继续向下查找而不会中断。

![](https://pic.leetcode-cn.com/5348d0bdaca8ced199813335706677c023160d43e0dd0f4923766e20969a44c8-4.png)

**链地址法**

解决冲突的另一种办法是将桶内产生冲突的键串联成一个链表。仍然以前一个方法中的冲突为例，实现方式如下图所示。

![](https://pic.leetcode-cn.com/9a91efc10a493b2e8c2aaac80da9b920ad89baba79278554bfe09c02bf9bdfc0-5.png)

与线性探测法类似，发生冲突时，由于都需要进行线性查找，因此会导致查找的成本增加。

**再哈希法**

再哈希法比较典型的应用是双重哈希法，即发生冲突时，通过使用另一个哈希函数来避免冲突。不难想到，另一个哈希函数在构造时，需要具备一些约束条件才能避免再次冲突。

然而，双重哈希法同样存在一些问题：

+ 与线性试探法相比，双重哈希法会消耗较多的时间。

+ 在双重哈希法中，删除会使问题变复杂，如果逻辑删除数量太多，则应重新构造哈希表。

**公共溢出区法**

顾名思义，公共溢出区法就是建立另一个哈希表 dict_overflow 作为公共溢出区，当发成冲突时则将该键保存在该哈希表中。

![](https://pic.leetcode-cn.com/8af7fe012602ba3492b01dc563c08c81d5f9a3f54797d05b65d7a2af2965100c-6.png)

如图所示，若查找的键发生冲突，则在公共溢出区进行线性查找。

到目前为止，你应该对哈希表的概念有了基本的认识。接下来，我们将提供一些相关练习，来帮助你进一步认识哈希表。快来试试吧！

## 设计哈希集合

不使用任何内建的哈希表库设计一个哈希集合

具体地说，你的设计应该包含以下的功能

+ add(value)：向哈希集合中插入一个值。
+ contains(value) ：返回哈希集合中是否存在这个值。
+ remove(value)：将给定值从哈希集合中删除。如果哈希集合中没有这个值，什么也不做。

示例:

    MyHashSet hashSet = new MyHashSet();
    hashSet.add(1);         
    hashSet.add(2);         
    hashSet.contains(1);    // 返回 true
    hashSet.contains(3);    // 返回 false (未找到)
    hashSet.add(2);          
    hashSet.contains(2);    // 返回 true
    hashSet.remove(2);          
    hashSet.contains(2);    // 返回  false (已经被删除)

注意：

+ 所有的值都在 [0, 1000000]的范围内。
+ 操作的总数目在[1, 10000]范围内。
+ 不要使用内建的哈希集合库。

```c++
class MyHashSet {
public:
    /** Initialize your data structure here. */
    MyHashSet() {

    }
    
    void add(int key) {

    }
    
    void remove(int key) {

    }
    
    /** Returns true if this set contains the specified element */
    bool contains(int key) {

    }
};

/**
 * Your MyHashSet object will be instantiated and called as such:
 * MyHashSet* obj = new MyHashSet();
 * obj->add(key);
 * obj->remove(key);
 * bool param_3 = obj->contains(key);
 */
```

### 题解

使用 线性试探法 写了一个, 性能不佳.

```c++
class MyHashSet {
public:
    /** Initialize your data structure here. */
    MyHashSet() { memset(arr, -1, sizeof(arr)); }

    void add(int key) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (-1 == arr[i]) {
                arr[i] = key;
                return;
            } else if (key == arr[i]) {
                return;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (-1 == arr[i]) {
                arr[i] = key;
                return;
            }
        }
    }

    void remove(int key) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (key == arr[i]) {
                arr[i] = -1;
                return;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (key == arr[i]) {
                arr[i] = -1;
                return;
            }
        }
    }

    /** Returns true if this set contains the specified element */
    bool contains(int key) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (key == arr[i]) {
                return true;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (key == arr[i]) {
                return true;
            }
        }
        return false;
    }

private:
    const int N = 10007;
    int arr[10007];
};
```

使用 链地址法 写了一个, 性能较好.

```c++
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};
class MyHashSet {
public:
    /** Initialize your data structure here. */
    MyHashSet() {
        // 注释中的初始化, 指针都指向同一块内存
        // arr = vector<ListNode *>(N, new ListNode(-1));
        arr.reserve(N);
        for (int i = 0; i < N; ++i) {
            arr.push_back(new ListNode(-1));
        }
    }

    void add(int key) {
        int id = key % N;

        ListNode *tmp = arr[id];
        while (NULL != tmp->next) {
            tmp = tmp->next;
            if (key == tmp->val) {
                return;
            }
        }
        tmp->next = new ListNode(key);
    }

    void remove(int key) {
        int id = key % N;
        ListNode *tmp = arr[id];
        while (NULL != tmp->next) {
            ListNode *tmp_next = tmp->next;
            if (key == tmp_next->val) {
                tmp->next = tmp_next->next;
                delete tmp_next;
                return;
            }
            tmp = tmp_next;
        }
    }

    /** Returns true if this set contains the specified element */
    bool contains(int key) {
        int id = key % N;
        ListNode *tmp = arr[id];
        while (NULL != tmp->next) {
            tmp = tmp->next;
            if (key == tmp->val) {
                return true;
            }
        }
        return false;
    }

private:
    const int N = 769;
    vector<ListNode *> arr;
};
```

> tips
[为什么哈希函数要模质数](https://www.cnblogs.com/cryingrain/p/11144225.html)  
[Hash时取模一定要模质数吗？](https://www.zhihu.com/question/20806796/answer/21359160)

## 设计哈希映射

不使用任何内建的哈希表库设计一个哈希映射

具体地说，你的设计应该包含以下的功能

+ put(key, value)：向哈希映射中插入(键,值)的数值对。如果键对应的值已经存在，更新这个值。
+ get(key)：返回给定的键所对应的值，如果映射中不包含这个键，返回-1。
+ remove(key)：如果映射中存在这个键，删除这个数值对。

示例：

    MyHashMap hashMap = new MyHashMap();
    hashMap.put(1, 1);          
    hashMap.put(2, 2);         
    hashMap.get(1);            // 返回 1
    hashMap.get(3);            // 返回 -1 (未找到)
    hashMap.put(2, 1);         // 更新已有的值
    hashMap.get(2);            // 返回 1 
    hashMap.remove(2);         // 删除键为2的数据
    hashMap.get(2);            // 返回 -1 (未找到) 

注意：

+ 所有的值都在 [0, 1000000]的范围内。
+ 操作的总数目在[1, 10000]范围内。
+ 不要使用内建的哈希库。

```c++
class MyHashMap {
public:
    /** Initialize your data structure here. */
    MyHashMap() {

    }
    
    /** value will always be non-negative. */
    void put(int key, int value) {

    }
    
    /** Returns the value to which the specified key is mapped, or -1 if this map contains no mapping for the key */
    int get(int key) {

    }
    
    /** Removes the mapping of the specified value key if this map contains a mapping for the key */
    void remove(int key) {

    }
};

/**
 * Your MyHashMap object will be instantiated and called as such:
 * MyHashMap* obj = new MyHashMap();
 * obj->put(key,value);
 * int param_2 = obj->get(key);
 * obj->remove(key);
 */
```

## 题解

写了一个 线性试探法 和 链地址法 的结合, 却不知道为什么报错. **TODO: 分析错误原因**

> AddressSanitizer: SEGV on unknown address 0x6320002e8340 (pc 0x0000003a4bb2 bp 0x7fff11b2def0 sp 0x7fff11b2deb0 T0)

```c++
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};
class MyHashMap {
public:
    /** Initialize your data structure here. */
    MyHashMap() {
        // 注释中的初始化, 指针都指向同一块内存
        // arr = vector<ListNode *>(N, new ListNode(-1));
        arr.reserve(N);
        for (int i = 0; i < N; ++i) {
            arr.push_back(new ListNode(-1));
        }
    }

    /** value will always be non-negative. */
    void put(int key, int value) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (-1 == arr[key]->val) {
                arr[i]->val = key;
                arr[i]->next = new ListNode(value);
                return;
            } else if (key == arr[i]->val) {
                arr[i]->next->val = value;
                return;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (-1 == arr[key]->val) {
                arr[i]->val = key;
                arr[i]->next = new ListNode(value);
                return;
            } else if (key == arr[i]->val) {
                arr[i]->next->val = value;
                return;
            }
        }
    }

    /** Returns the value to which the specified key is mapped, or -1 if this
     * map contains no mapping for the key */
    int get(int key) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (key == arr[i]->val) {
                return arr[i]->next->val;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (key == arr[i]->val) {
                return arr[i]->next->val;
            }
        }
        return -1;
    }

    /** Removes the mapping of the specified value key if this map contains a
     * mapping for the key */
    void remove(int key) {
        int id = key % N;
        for (int i = id; i < N; ++i) {
            if (key == arr[i]->val) {
                arr[i]->val = -1;
                delete arr[i]->next;
                arr[i]->next = NULL;
                return;
            }
        }
        for (int i = 0; i < id; ++i) {
            if (key == arr[i]->val) {
                arr[i]->val = -1;
                delete arr[i]->next;
                arr[i]->next = NULL;
                return;
            }
        }
    }

private:
    const int N = 10007;
    vector<ListNode *> arr;
};
```

```c++
struct Node {
    int key;
    int val;
    Node *next;
    Node(int x, int y) : key(x), val(y), next(NULL) {}
};
class MyHashMap {
public:
    /** Initialize your data structure here. */
    MyHashMap() {
        arr.reserve(N);
        for (int i = 0; i < N; ++i) {
            arr.push_back(new Node(-1, -1));
        }
    }

    /** value will always be non-negative. */
    void put(int key, int value) {
        int id = key % N;
        Node *tmp = arr[id];
        while (NULL != tmp->next) {
            tmp = tmp->next;
            if (key == tmp->key) {
                tmp->val = value;
                return;
            }
        }
        tmp->next = new Node(key, value);
    }

    /** Returns the value to which the specified key is mapped, or -1 if this
     * map contains no mapping for the key */
    int get(int key) {
        int id = key % N;
        Node *tmp = arr[id];
        while (NULL != tmp->next) {
            tmp = tmp->next;
            if (key == tmp->key) {
                return tmp->val;
            }
        }
        return -1;
    }

    /** Removes the mapping of the specified value key if this map contains a
     * mapping for the key */
    void remove(int key) {
        int id = key % N;
        Node *tmp = arr[id];
        while (NULL != tmp->next) {
            Node *tmp_next = tmp->next;
            if (key == tmp_next->key) {
                tmp->next = tmp_next->next;
                delete tmp_next;
                return;
            }
            tmp = tmp_next;
        }
    }

private:
    const int N = 769;
    vector<Node *> arr;
};
```

## 设计哈希表 - 解决方案

这里分别给出设计哈希集合和哈希映射的解决方案。为了解决哈希冲突，我们使用 二维数组 来实现哈希集合，数组中的每个元素都可以看作一个桶，而在每个桶中又包含一个一维数组，以便保存产生冲突的键。

**哈希集合**
```c++
#define MAX_LEN 100000          // 初始化桶的数量

class MyHashSet {
private:
    vector<int> set[MAX_LEN];   // 使用数组实现哈希集合
    
    /** 返回对应的桶的索引 */
    int getIndex(int key) {
        return key % MAX_LEN;
    }
    
    /** 在特定的桶中搜索键，如果该键不存在则返回 -1 */
    int getPos(int key, int index) {
        // 每个桶中包含一个列表，遍历所有桶中的元素来寻找特定的键
        for (int i = 0; i < set[index].size(); ++i) {
            if (set[index][i] == key) {
                return i;
            }
        }
        return -1;
    }
public:
    
    MyHashSet() {
        
    }
    
    void add(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos < 0) {
            // 如果键不存在，则添加
            set[index].push_back(key);
        }
    }
    
    void remove(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos >= 0) {
            // 如果键存在，则删除
            set[index].erase(set[index].begin() + pos);
        }
    }
    
    bool contains(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        return pos >= 0;
    }
};
```

**哈希映射**
```c++
#define MAX_LEN 100000            // 初始化桶的数量

class MyHashMap {
private:
    vector<pair<int, int>> map[MAX_LEN];       // 使用数组实现哈希集合
    
    /** 返回指定桶的索引 */
    int getIndex(int key) {
        return key % MAX_LEN;
    }
    
    /** 在桶中搜索键，如果不存在则返回 -1 */
    int getPos(int key, int index) {
        // 每个桶包含一个数组，遍历桶中的所有元素来查找指定的 key
        for (int i = 0; i < map[index].size(); ++i) {
            if (map[index][i].first == key) {
                return i;
            }
        }
        return -1;
    }
    
public:
    MyHashMap() {
        
    }
    
    /** value 始终为正 */
    void put(int key, int value) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos < 0) {
            map[index].push_back(make_pair(key, value));
        } else {
            map[index][pos].second = value;
        }
    }
    
    /** 如果存在映射关系，则返回 value，否则返回 -1 */
    int get(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos < 0) {
            return -1;
        } else {
            return map[index][pos].second;
        }
    }
    
    /** 如果存在 key 的映射，则删除该映射关系 */
    void remove(int key) {
        int index = getIndex(key);
        int pos = getPos(key, index);
        if (pos >= 0) {
            map[index].erase(map[index].begin() + pos);
        }
    }
};
```

**进一步探索**
我们来看看 “删除” 操作，**找到元素的位置之后**，我们需要从数组中 “删除” 元素。

这是由数组本身的特点决定的。在探索卡片 [数组的操作](https://leetcode-cn.com/explore/featured/card/array-and-string/198/introduction-to-array/1428/) 中，我们对数组的 “删除” 操作进行了解释，即删除第 i 个元素，需要将后面的 n - i 个元素向前移动一个位置。其中，n 为数组的长度。

考虑 i 取不同值的情况：

+ 删除末尾元素，移动 0 次；

+ 删除开头元素，移动 n - 1 次。

平均而言，我们将要移动的次数为：

    (0+1+...+(n-2)+(n-1))/n = (n-1)/2

因此，找到待删除元素后，从数组中删除元素的时间复杂度将为 O(n)。

以下两种方案可以将时间复杂度从 O(n) 降低到 O(1)。

1. 交换

我们可以使用一种巧妙的策略。首先，用存储桶中的最后一个元素交换要移除的元素。然后删除最后一个元素。通过这种方法，我们成功地在 O(1) 的时间复杂度中去除了元素。

1. 链表

实现此目标的另一种方法是使用链表而不是数组列表。通过这种方式，我们可以在不修改数组中元素顺序的情况下删除元素。该策略时间复杂度为 O(1)。

然而，以上两种策略只改进了 找到元素之后 进行删除的时间复杂度，而对于整个删除操作，由于先要进行线性查找，所以时间复杂度仍为 O(n)，n 为桶的大小。

要进一步改进此问题，我们可以使用 平衡二叉搜索树 作为桶，当进行删除操作时，则可以使用二分查找来快速定位被删除元素，使得删除操作的时间复杂度由 O(n) 减小为 O(log n)，n 为桶的大小。

## 复杂度分析

对于哈希表，插入和搜索的平均时间复杂度为 O(1)，空间复杂度为 O(n)。下面将进一步分析。

我们知道哈希表在设计的过程中总是要考虑冲突问题，这是因为不发生冲突几乎是不可能的。

例如我们经常谈到的——“在 50 人中，至少有 2 人同一天生日的概率是 97%”，这就意味着，如果我们将 365 天看作 365 个桶，每个桶保存一个人的生日，那么发生冲突概率则高达 97%，而此时的装填因子仅为

    50 / 365 = 13.7%

也就是说，为了使插入和查询的时间复杂度达到 O(1)，我们可以选择牺牲空间复杂度，增加桶的个数，使其不发生冲突。然而经过分析，我们发现即使装填因子很低的情况下，冲突仍然难以避免。因此就有了前面文章中关于冲突解决的各种办法，本质上是为了达到时间与空间的平衡。

既然如此，插入和搜索的时间复杂度就不会总是 O(1)，它将取决于桶内部存储发生冲突的键的方式。

+ 如果桶内使用 链表 或者 数组，则最坏情况下，插入和搜索的时间复杂度为 O(n)；

+ 如果桶内使用 高度平衡的二叉树 来保存键，则最坏情况下，插入和搜索的时间复杂度为 O(log n)。

其中，n 为桶的大小。且无论使用以上那种方式，平均时间复杂度 均为 O(1)。

![](https://pic.leetcode-cn.com/64a5bde5a559c6557928419ee0c8cc81b2eea06dbde819706c88e44170d6f052-3.jpg)

对于空间复杂度，如果总共有 M 个键，那么至少需要分配 M 个桶，在实际的编程语言中，有各自的动态扩容机制，但总的来说，桶的个数与键呈线性关系，空间复杂度为 O(M)。



## 散列表的应用

+ 查找
+ 防止重复
+ 用作缓存
