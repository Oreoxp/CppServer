[toc]

# LRUCache



## 缓存使用

​		在分析 LRUCache 的实现之前，首先了解下 LRUCache 的使用方法，以明确 LRUCache 要解决的问题。以此为基础，我们才能了解为什么要这么实现，甚至更进一步，探讨有没有更好的实现。

​		首先来看下 LevelDB 的导出接口 [Cache]( [leveldb/cache.h at main · google/leveldb (github.com)](https://github.com/google/leveldb/blob/main/include/leveldb/cache.h) )：

```cpp
//   插入一个键值对（key，value）到缓存（cache）中，
// 并从缓存总容量中减去该键值对所占额度（charge） 。
//   返回指向该键值对的句柄（handle），调用者在用完句柄后，
// 需要调用 this->Release(handle) 进行释放。
//   在键值对不再被使用时，键值对会被传入的 deleter 参数
// 释放。
virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                       void (*deleter)(const Slice& key, void* value)) = 0;

//   如果缓存中没有相应键（key），则返回 nullptr。
//   否则返回指向对应键值对的句柄（Handle）。调用者用完句柄后，
// 要记得调用 this->Release(handle) 进行释放。
virtual Handle* Lookup(const Slice& key) = 0;

// 释放 Insert/Lookup 函数返回的句柄
// 要求：该句柄没有被释放过，即不能多次释放
// 要求：该句柄必须是同一个实例返回的
virtual void Release(Handle* handle) = 0;

// 获取句柄中的值，类型为 void*（表示任意用户自定义类型）
// 要求：该句柄没有被释放
// 要求：该句柄必须由同一实例所返回
virtual void* Value(Handle* handle) = 0;

// 如果缓存中包含给定键所指向的条目，则删除之。
// 需要注意的是，只有在所有持有该条目句柄都释放时，该条目所占空间才会真正被释放
virtual void Erase(const Slice& key) = 0;

// 返回一个自增的数值 id。当一个缓存实例由多个客户端共享时，
// 为了避免多个客户端的键冲突，每个客户端可能想获取一个独有
// 的 id，并将其作为键的前缀。类似于给每个客户端一个单独的命名空间。
virtual uint64_t NewId() = 0;

// 驱逐全部没有被使用的数据条目
// 内存吃紧型的应用可能想利用此接口定期释放内存。
// 基类中的 Prune 默认实现为空，但强烈建议所有子类自行实现。
// 将来的版本可能会增加一个默认实现。
virtual void Prune() {}

// 返回当前缓存中所有数据条目所占容量总和的一个预估
virtual size_t TotalCharge() const = 0;
```

依据上述接口，可捋出 LevelDB 缓存相关需求：

1. 多线程支持
2. 性能需求
3. 数据条目的生命周期管理



 用状态机来表示 Cache 中的 Entry 的生命周期如下： 

![03lrustatus](.\markdownimage\03lrustatus.png)

​		综上所述，leveldb 的需求在普通 LRU 的基础上增加了引用计数，并且得支持多线程调用，多了许多状态。



## 思路总览

​		总体上来说，LevelDB 的 LRUCache 也使用了哈希表和双向链表的实现思路，但又有些不同：

1. 使用数组+链表处理冲突定制了一个极简哈希表，便于控制分配以及伸缩。
2. 多线程支持。了提高并发，引入了分片；为了区分是否被客户端引用，引入两个双向链表。

整个代码相当简洁，思想也比较直观。



### 定制简单哈希表

​		LevelDB 中哈希表保持桶的个数为 2 的次幂，从而使用位运算来通过键的哈希值快速计算出桶位置。通过 key 的哈希值来获取桶的句柄方法如下：

```cpp
LRUHandle** ptr = &list_[hash & (length_ - 1)];
```

​		每次调整时，在扩张时将桶数量增加一倍，在缩减时将桶数量减少一倍，并需要对所有数据条目进行重新分桶。

操作：

```c++
  LRUHandle* Insert(LRUHandle* h) {
    LRUHandle** ptr = FindPointer(h->key(), h->hash);
    LRUHandle* old = *ptr;
    h->next_hash = (old == nullptr ? nullptr : old->next_hash);
    *ptr = h;
    if (old == nullptr) {
      ++elems_;
      if (elems_ > length_) {
        // 由于每个缓存条目都相当大，我们的目标是较小的平均链表长度（<= 1）。
        Resize();
      }
    }
    return old;
  }

  LRUHandle* Remove(const Slice& key, uint32_t hash) {
    LRUHandle** ptr = FindPointer(key, hash);
    LRUHandle* result = *ptr;
    if (result != nullptr) {
      *ptr = result->next_hash;
      --elems_;
    }
    return result;
  }
```

内部函数：

```c++
  // Return a pointer to slot that points to a cache entry that
  // matches key/hash.  If there is no such cache entry, return a
  // pointer to the trailing slot in the corresponding linked list.
  LRUHandle** FindPointer(const Slice& key, uint32_t hash) {
    LRUHandle** ptr = &list_[hash & (length_ - 1)];
    while (*ptr != nullptr && ((*ptr)->hash != hash || key != (*ptr)->key())) {
      ptr = &(*ptr)->next_hash;
    }
    return ptr;
  }

  void Resize() {
    uint32_t new_length = 4;
    while (new_length < elems_) {
      new_length *= 2;
    }
    LRUHandle** new_list = new LRUHandle*[new_length];
    memset(new_list, 0, sizeof(new_list[0]) * new_length);
    uint32_t count = 0;
    for (uint32_t i = 0; i < length_; i++) {
      LRUHandle* h = list_[i];
      while (h != nullptr) {
        LRUHandle* next = h->next_hash;
        uint32_t hash = h->hash;
        LRUHandle** ptr = &new_list[hash & (new_length - 1)];
        h->next_hash = *ptr;
        *ptr = h;
        h = next;
        count++;
      }
    }
    assert(elems_ == count);
    delete[] list_;
    list_ = new_list;
    length_ = new_length;
  }
```









































