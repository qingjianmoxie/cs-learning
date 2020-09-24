#include <unordered_map>
#include <list>

using namespace std;

class LRUCache {
    // key -> iterator to pair(key, val) in the list
    unordered_map<int, list<pair<int, int>>::iterator> map;
    
    // pair(k1, v1) <-> pair(k2, v2)...
    list<pair<int, int>> cache;
    
    // 最大容量
    int cap;
public:
    LRUCache(int capacity) : cap(capacity) {}
    
    int get(int key) {
        // key不存在
        if(map.find(key) == map.end()) {
            return -1;
        }
        
        int val = map[key]->second;
        
        // 利用 put 方法把该数据提前
        put(key, val);
        
        return val;
    }
    
    void put(int key, int value) {
        pair<int, int> x = {key, value};

        // key已存在
        if (map.find(key) != map.end()) {
            // 删除旧的节点
            cache.erase(map[key]);
            // 新的插到头部
            cache.emplace_front(x);
            // 更新 map 中对应的数据
            map[key] = cache.begin();
        } else {
            // cache已满
            if(cap == cache.size()) {
                // 删除链表最后一个数据
                pair<int, int> last = cache.back();
                cache.pop_back();

                map.erase(last.first);
            }
            
            // 直接添加到头部
            cache.emplace_front(x);
            map[key] = cache.begin();
        }
    }
};