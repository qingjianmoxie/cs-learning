class ListNode:
    def __init__(self, key: int, val: int):
        self.key = key
        self.val = val
        self.prev = None
        self.next = None


class LRUCache:
    def __init__(self, capacity: int):
        # 最大容量
        self.cap = capacity
        self.cache = {}
        # 哨兵节点
        self.sentinel = ListNode(None, None)
        # 尾节点： 用于链表容量超过最大容量是快速定位、删除尾节点
        self.tail = ListNode(None, None)
        # 初始化双向链表
        self.sentinel.next = self.tail
        self.tail.prev = self.sentinel

    def get(self, key: int) -> int:
        if key in self.cache:
            node = self.cache[key]
            # 从链表中删除该节点
            self.remove_node_from_list(node)
            # 把该节点添加到链表头部
            self.push_node_to_front(node)
            return node.val
        else:
            return -1

    def put(self, key: int, value: int) -> None:
        # 如果该节点已经存在那么删除该节点
        if key in self.cache:
            self.remove_node_from_list(self.cache[key])

        # 把该节点添加到链表头部
        node = ListNode(key, value)
        self.cache[key] = node
        self.push_node_to_front(node)

        # 如果链表超过最大容量，删除链表尾部节点
        if len(self.cache) > self.cap:
            last_node = self.tail.prev
            self.remove_node_from_list(last_node)
            self.cache.pop(last_node.key)

    # 从链表中删除节点
    def remove_node_from_list(self, node: "ListNode") -> None:
        prev = node.prev
        nxt = node.next
        prev.next = nxt
        nxt.prev = prev

    # 添加节点到链表头部
    def push_node_to_front(self, node: "ListNode") -> None:
        nxt = self.sentinel.next
        self.sentinel.next = node
        node.next = nxt
        node.prev = self.sentinel
        nxt.prev = node