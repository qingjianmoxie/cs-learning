 Thread safety
 -------------

Writes require external synchronization, most likely a mutex. Reads require a guarantee that the SkipList will not be destroyed while the read is in progress.  Apart from that, reads progress without any internal locking or synchronization.

写操作需要外部同步，很可能是互斥锁。读操作需要保证在读取过程中skiplist不会被破坏。除此之外，读进程没有任何内部锁定或同步。

Invariants(不变量):

(1) Allocated nodes are never deleted until the SkipList is destroyed.  This is trivially guaranteed by the code since we never delete any skip list nodes.

分配的节点在SkipList被销毁之前不会被删除。由于我们从不删除任何跳表节点，所以代码很容易保证这一点。

(2) The contents of a Node except for the next/prev pointers are immutable after the Node has been linked into the SkipList. Only Insert() modifies the list, and it is careful to initialize a node and use release-stores to publish the nodes in one or more lists.

在节点链接到SkipList之后，节点的内容（next/prev指针除外）是不可变的。只有Insert()可以修改链表，初始化一个节点, 以及使用release-stores来发布节点时要小心。

... prev vs. next pointer ordering ...

首先看看其节点的定义，Node包含了节点值key，取出和设置Next指针的函数，Next表示下一个节点的首地址，只有一个元素。

```c++
// Implementation details follow
template <typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
  explicit Node(const Key& k) : key(k) {}

  Key const key;

  // Accessors/mutators for links.  Wrapped in methods so we can
  // add the appropriate barriers as necessary.
  Node* Next(int n) {
    assert(n >= 0);
    // Use an 'acquire load' so that we observe a fully initialized
    // version of the returned Node.
    return next_[n].load(std::memory_order_acquire);
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    // Use a 'release store' so that anybody who reads through this
    // pointer observes a fully initialized version of the inserted node.
    next_[n].store(x, std::memory_order_release);
  }

  // No-barrier variants that can be safely used in a few locations.
  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_relaxed);
  }
  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_relaxed);
  }

 private:
  // Array of length equal to the node height.  next_[0] is lowest level link.
  std::atomic<Node*> next_[1];
};
```

下面看SkipList的定义。首先看看成员变量，
`compare_`用于表示节点之间的比较关系，
`arena_`用于给跳表和节点申请内存，
`head_`是头结点，
`max_height_`整个跳表的最大高度, 只会被Insert()修改;
`rnd_`用于随机生成节点高度, 只会被Insert()读写。
除此之外，还有在SkipList类内有一个Iterator内置类。成员函数的作用可以由其名可知。

```c++
template <typename Key, class Comparator>
class SkipList {
 private:
  struct Node;

 public:
  // Create a new SkipList object that will use "cmp" for comparing keys,
  // and will allocate memory using "*arena".  Objects allocated in the arena
  // must remain allocated for the lifetime of the skiplist object.
  explicit SkipList(Comparator cmp, Arena* arena);

  SkipList(const SkipList&) = delete;
  SkipList& operator=(const SkipList&) = delete;

  // Insert key into the list.
  // REQUIRES: nothing that compares equal to key is currently in the list.
  void Insert(const Key& key);

  // Returns true iff an entry that compares equal to key is in the list.
  bool Contains(const Key& key) const;

  // Iteration over the contents of a skip list
  class Iterator {
   public:
    // Initialize an iterator over the specified list.
    // The returned iterator is not valid.
    explicit Iterator(const SkipList* list);

    // Returns true iff the iterator is positioned at a valid node.
    bool Valid() const;

    // Returns the key at the current position.
    // REQUIRES: Valid()
    const Key& key() const;

    // Advances to the next position.
    // REQUIRES: Valid()
    void Next();

    // Advances to the previous position.
    // REQUIRES: Valid()
    void Prev();

    // Advance to the first entry with a key >= target
    void Seek(const Key& target);

    // Position at the first entry in list.
    // Final state of iterator is Valid() iff list is not empty.
    void SeekToFirst();

    // Position at the last entry in list.
    // Final state of iterator is Valid() iff list is not empty.
    void SeekToLast();

   private:
    const SkipList* list_;
    Node* node_;
    // Intentionally copyable
  };

 private:
  enum { kMaxHeight = 12 };

  inline int GetMaxHeight() const {
    return max_height_.load(std::memory_order_relaxed);
  }

  Node* NewNode(const Key& key, int height);
  int RandomHeight();
  bool Equal(const Key& a, const Key& b) const { return (compare_(a, b) == 0); }

  // Return true if key is greater than the data stored in "n"
  bool KeyIsAfterNode(const Key& key, Node* n) const;

  // Return the earliest node that comes at or after key.
  // Return nullptr if there is no such node.
  //
  // If prev is non-null, fills prev[level] with pointer to previous
  // node at "level" for every level in [0..max_height_-1].
  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  // Return the latest node with a key < key.
  // Return head_ if there is no such node.
  Node* FindLessThan(const Key& key) const;

  // Return the last node in the list.
  // Return head_ if list is empty.
  Node* FindLast() const;

  // Immutable after construction
  Comparator const compare_;
  Arena* const arena_;  // Arena used for allocations of nodes

  Node* const head_;

  // Modified only by Insert().  Read racily by readers, but stale
  // values are ok.
  std::atomic<int> max_height_;  // Height of the entire list

  // Read/written only by Insert().
  Random rnd_;
};
```
