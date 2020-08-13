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

```c++
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(NULL) {}
};
class MyHashSet {
public:
    /** Initialize your data structure here. */
    MyHashSet() { 
        arr = vector<ListNode *>(N, new ListNode(-1));
    }

    void add(int key) {
        int id = key % N;
        if (-1 == arr[id]->val) {
            arr[id]->val = key;
            return;
        }
        ListNode *tmp = arr[id]->next;
        while (NULL != tmp) {
            if (key == tmp->val) {
                return;
            }
        }
    }

    void remove(int key) {
        int id = key % N;

    }

    /** Returns true if this set contains the specified element */
    bool contains(int key) {
        int id = key % N;

    }

private:
    const int N = 769;
    vector<ListNode *> arr;
};
```


[为什么哈希函数要模质数](https://www.cnblogs.com/cryingrain/p/11144225.html)

[Hash时取模一定要模质数吗？](https://www.zhihu.com/question/20806796/answer/21359160)



## 散列表的应用

+ 查找
+ 防止重复
+ 用作缓存
