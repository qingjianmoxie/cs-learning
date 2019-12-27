# redis中的几种hash函数

这段时间一直在研究redis的源码，在研究其字典结构体的实现部分时，遇到了几个陌生的哈希函数。研究了一下才发现这些函数在如今的开源项目中早已得到了广泛的应用，如标准C++库、nginx、Hadoop等等。

于是乎，我对redis中的这几个函数做了点研究，自己琢磨出点东西来，在这里与大家分享一下。我写这篇文章的目的主要是想阐述一下我对哈希函数的看法，同时描述一些原理性的东西，因为我发现网上这方面的文章还比较少。

首先我们来看一下什么样的函数才能作为哈希函数。

## 一、哈希函数的构建准则

大神Thomas Wang认为，一个好的哈希函数必须具备以下两点特性：

1. 一个好的哈希函数应该是可逆的。即，对于哈希函数输入值x和输出值y，如果存在f(x) = y，就一定存在g(y) = x。说白了，就是哈希函数可以将某一个值x转换成一个key，也可以把这个key还原回成x。

2. 一个好的哈希函数应该容易造成雪崩效应。这里的雪崩效应是从比特位的角度出发的，它指的是，输入值1bit位的变化会造成输出值一半以上的bit位发生变化。

首先，我们看第一条特性，这与我们通常认为的哈希函数的特性不符，一个哈希函数为什么要做到可逆呢？一般只有加解密算法才涉及到可逆性的讨论啊。经过思考，我的结论是，具有可逆性的哈希函数可以从根本上消除哈希过程中的冲突(collisions)。原因显而易见，因为函数具有可逆性，所以输入值与输出值一定一一对应，一个输入只能产生一个唯一输出，不可能存在f(x1) = y, f(x2) =y 且 x1 != x2的情况。

同时，这样的函数存在的问题也显而易见，因为输入值与输出值的数量相同，所以面对大量输入时，会面临存储空间不足的问题。我认为Thomas Wang的想法应该是这样，哈希函数应只负责将输入值尽量均匀的分布在某一空间，而不管实际的物理内存是否可以容纳该空间。将这一问题留给具体的使用者。对于内存不足的情况，一般的处理方法是对哈希结果进行二次映射，将这些值存入到一个固定大小的物理内存块中。具体映射的方法有很多，最简单的是取余运算，但是取余的方法方法过于耗时，可以通过一个小技巧避免。我们可以将放置哈希结果的物理内存块的大小设置成2的n次方的形式，此时, tablesize = 2^n，key_addr = hash_value % tablesize = hash_value & (tablesize - 1)。这里用位运算来代替取余运算，在tablesize = 2^n 的情况下，两者的效果相同。

其次，我们再来看一下雪崩效应特性，这条特性的主要目的是使得哈希结果更为离散均匀。

那么，如何才能实现一个可逆的具有雪崩效应的哈希函数呢？

在分析了各种哈希函数的具体算法后，我发现，这些算法都是通过一系列可逆的、具有雪崩效应的运算组合实现。

那哪些运算才是可逆的、具有雪崩效应的呢？这就是我们第二章要将的内容，运算组合的可逆性与雪崩效应。

## 二、运算组合的可逆性与雪崩效应研究

因为之后的各类哈希算法中出现的基本上都是一些加、乘、位移、异或操作的组合，初看这些算法估计没人知道他到底想干啥，算法本身也没什么注释，如果直接上算法各位同学可能不好理解。所以这里我想先说一下这些运算组合的内在机理。

下面，我们分别来看一下各种运算组合与可逆性、雪崩效应这两条特性之间的关系。

1. 可逆性研究

可逆性这东西比较难理解，毕竟不是学算法的，网上有没这方面的资料，只能自己想了，这里的分析如果有错误，还请各位大神指正。

好了，我们先从简单的讲起吧。先从单个的操作说起，通过简单的思考我们可以知道以下这些运算是可逆的：

+ x + 常数 = y；
+ x - 常数 = y；
+ x ^ 常数 = y；
+ ~x = y；
+ x * 常数 =y；

在上面的式子中，我们可以通过x得到y，也可以通过y得到x，这没什么问题。那我们再看一下复杂的例子：

+ (x + 常数)  + (x << n) = y；

这个式子是否可逆呢？我们可以通过x得到y，那我们怎样通过y得到x呢？为了方便理解，我们来看一个具体的例子（例子中数字为二进制表示）：(x +(101111)) + (x << 3) = (11101100)，根据交换律，这个式子能转换为x + (x << 3) = (11101100) - (101111) = (10111101)。这个式子我们可用下图表示：

[](https://img-blog.csdn.net/20140116181234828?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvamFzcGVyX3h1bGVp/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

通过转换后的式子，我们可以画出这样一个表格，x + (x << 3)的最终结果为(10111101)，而x<<3的最后三位为000，此时我们可以得到x的后三位为101，继而得到x<<3的后三位为相同101。如下图：

[](https://img-blog.csdn.net/20140116181258593?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvamFzcGVyX3h1bGVp/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

此时，我们又可以得到x的第3、4、5位分别是0、1、0。依次类推，最终我们可以得到x=(10101)，如下图：

[](https://img-blog.csdn.net/20140116181304578?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvamFzcGVyX3h1bGVp/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center)

所以，我们可以得知，式子是可逆的。特别的，即使在位移过程中出现溢出截断，也不会影响结果。

+ (x + 常数)  + (x >> n) = y；

我查到的资料显示（资料为个人博客，非权威），这个式子是不可逆的，但是我想了半天也无法找到其不可逆的例子。有待考证.

+ (x + 常数)  ^ (x << n) = y 与 (x + 常数)  ^ (x >> n) = y；

这两个式子都是可逆的。其推导过程与第一个式子相似，在此就不多说了。只提一点，因为异或不存在进位的问题，所以不论推导顺序是从高位到低位还是从低位到高位，我们都可以得到唯一的结果，所以说两个式子都是可逆的。

还有一些式子都和上述的式子比较相近，我就不介绍了，大家可以自行判断。

研究这些式子的可逆性问题有什么用呢？别急，等我们看到具体的哈希算法以后，就明白了。

2. 雪崩效应研究

雪崩效应之前也讲过，说白了，就是输入数据1bit位的变化会导致输出数据N bit位的变化，这个N是大于等于1/2输出数据长度的。我们还是从简单的运算说起。

1）加减运算

加减运算很容易引起雪崩响应，这很容易理解，例如(1111)+ (1) = (10000) 以及(1000) - (1) = (111) 这两个式子。

2）位移运算

取反运算也很容易产生雪崩效应，例如(00001111) << 2 = (00111100)。

3）乘除运算

乘除运算的本质就是位移与加减法的组合，所以也是可以一起雪崩效应的。

4）取反、异或运算

取反和异或运算也很容易产生雪崩效应，如~(1111) = (0000)以及(1101) ^ (1010) = (0111)。

## 三、几种哈希算法的罗列

讲完了原理性的东西，我想再看这些算法就很简单了，本文主要想陈述的东西基本上就是这些原理，这一章，我们将结合上面的原理在看看几个具体的例子。这些例子都来自redis项目中的Dict.c文件，看个大概就好，不必深究，主要是原理性的东西我们知道就好。

1、32 bit Mix Function

这是redis源码中的一个哈希函数，很具有代表性，下面我们来具体看一下他的函数：

```c
/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}
```

我们可以看到，这个函数都是由我们上面讨论的一些基本的式子，之所以让采用自身赋值（即f(key) = key）的原因是为了传递可逆性与雪崩效应。怎么样，通过第二章的讨论，我们是不是可以很容易的理解该函数内藏的玄机呢。

2、64 bit Mix Functions

这是redis源码中的一个指纹生成函数，它会根据dcit结构体的当前状态生成一个标示，如果之后该结构体的内容发生变化，其指纹也会发生变化。这又是一个哈希函数的很好的应用场景，我们主要看语句`for (j = 0; j <6; j++)`里面的东西，是不是很熟悉？没错，这和上面讲的"32 bit MixFunction"很像，是"64 bit MixFunctions"的一个应用。应用这样可逆的哈希函数可以在dict结构体中某一数值发生改变时，通过其他的值与其指纹逆推出发生变化的数值。

代码段中的中文注释是我自己加上的，有兴趣的同学也可以自己查看源码。这里我也是初学哈希函数，所以只能针对我碰到的具体应用进行讲解。

```c
/* A fingerprint is a 64 bit number that represents the state of the dictionary
 * at a given time, it's just a few dict properties xored together.
 * When an unsafe iterator is initialized, we get the dict fingerprint, and check
 * the fingerprint again when the iterator is released.
 * If the two fingerprints are different it means that the user of the iterator
 * performed forbidden operations against the dictionary while iterating. */
long long dictFingerprint(dict *d) {
    long long integers[6], hash = 0;
    int j;

    //将dict结构体中的几个状态放入到数组中，以便后面应用到64 bit MixFunctions中。
    //dict结构体其实就是一个hash表的实现，而这些状态其实就是第一、第二哈希表的表地址、表大小与
    //已用条目的数量
    integers[0] = (long) d->ht[0].table;
    integers[1] = d->ht[0].size;
    integers[2] = d->ht[0].used;
    integers[3] = (long) d->ht[1].table;
    integers[4] = d->ht[1].size;
    integers[5] = d->ht[1].used;

    /* We hash N integers by summing every successive integer with the integer
     * hashing of the previous sum. Basically:
     *
     * Result = hash(hash(hash(int1)+int2)+int3) ...
     *
     * This way the same set of integers in a different order will (likely) hash
     * to a different number. */
    //利用64 bit Mix Functions，将这些状态信息混合到hash中，组成最后的指纹，如果这些状态中有一个
    //出现变化，可以通过一个算法逆推出该状态变化之前的值。例如，d->ht[0].size发生变化，则我们可
    //以通过hash和其他的几个状态，逆推出d->ht[0].size的最初值。
    for (j = 0; j < 6; j++) {
        hash += integers[j];
        /* For the hashing step we use Tomas Wang's 64 bit integer hash. */
        hash = (~hash) + (hash << 21); // hash = (hash << 21) - hash - 1;
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8); // hash * 265
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4); // hash * 21
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);
    }
    return hash;
}
```

3、MurmurHash2

MurmurHash是一种很出名的非加密型哈希函数，适用于一般的哈希检索操作, 由Austin Appleby在2008年发明，并且有多个变种; 该算法的作者2011去了google, 开发出来了新的算法CityHash。目前有三个版本（MurmurHash1、MurmurHash2、MurmurHash3）。最新的是MurmurHash3，可以产生出32-bit或128-bit哈希值。redis中应用的是MurmurHash2，能产生32-bit或64-bit哈希值。与上面介绍的整数哈希不同，MurmurHash是针对一个字符串进行哈希的，对于规律性较强的key，该算法能表现出较好的离散性特征。同样，我们来看看redis的源码。

该算法的基本思想就是把key分成n组，每组4个字符，把这4个字符看成是一个uint_32，进行n次运算，得到一个h，然会在对h进行处理，得到一个相对离散的哈希结果。代码中出现了一些如dict_hash_function_seed、m和r的常数变量，别问我问什么选这些数，就像英文注释中写得那样，他们就是效果好...

```c
/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */
unsigned int dictGenHashFunction(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}
```

4、djb hash

原始代码如下:

```c
unsigned int DJBHash(char *str)
{
    unsigned int hash = 5381;

    while (*str){
        hash = ((hash << 5) + hash) + (*str++); /* times 33 */
    }
    hash &= ~(1 << 31); /* strip the highest bit */
    return hash;
}
```

djb哈希算法很简单，只有几行代码，其功能与MurmurHash类似，都是将字符串转换为hash值，Redis对其进行了部分调整，不区分大小写：

```c
/* And a case insensitive hash function (based on djb hash) */
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len) {
    unsigned int hash = (unsigned int)dict_hash_function_seed;

    while (len--)
        hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
    return hash;
}
```

该算法的思想是利用字符串中的ascii码值与一个随机seed，通过len次变换，得到最后的hash值。

## 参考文献

[redis中几种哈希函数的研究](https://blog.csdn.net/jasper_xulei/article/details/18364313)
[Thomas Wang大神本人的链接](http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm)

使用新的BKDR算法