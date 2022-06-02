/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_UOMAP_HPP
#define SJTU_UOMAP_HPP

// only for std::equal_to<T> and std::hash<T>
//#include <functional>
//#include <cstddef>
//#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
/**
 * In linked_hashmap, iteration ordering is differ from map,
 * which is the order in which keys were inserted into the map.
 * You should maintain a doubly-linked list running through all
 * of its entries to keep the correct iteration order.
 *
 * Note that insertion order is not affected if a key is re-inserted
 * into the map.
 */

template<
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>
>
class linked_hashmap {
 public:
  typedef std::pair<const Key, T> value_type;

 private:
  struct Node {
    value_type *value = nullptr;
    Node *pre = nullptr, *nxt = nullptr, *fa = nullptr, *son = nullptr;
    Node() = default;
    Node(const value_type &val) { value = new value_type(val); }
    ~Node() {
      if (value)delete value;
      pre = nxt = fa = son = nullptr;
    }
  };

  Node *head = nullptr, *tail = nullptr;
  Node **storageList = nullptr;
  const int default_capacity = 25678, LoadFactor = 4;
  int Capacity = 25678, NodeNum = 0;

  void reSize() {
    for (Node *i = head->nxt; i != tail; i = i->nxt) {
      if (!(i->fa->fa)) delete i->fa;
      if (!(i->son->son)) delete i->son;
    }

    delete[]storageList;
    Capacity *= 2;
    storageList = new Node *[Capacity];

    memset(storageList, 0, Capacity * sizeof(Node*));

    int hash;
    for (Node *i = head->nxt; i != tail; i = i->nxt) {
      hash = Hash()(i->value->first) % Capacity;
      if (!storageList[hash]) {
        storageList[hash] = new Node;
        storageList[hash]->son = new Node;
        storageList[hash]->son->fa = storageList[hash];
      }
      Node *start = storageList[hash];
      i->fa = start;
      i->son = start->son;
      i->son->fa = i;
      start->son = i;
    }
  }

 public:
  class const_iterator;

  class iterator {
    friend class linked_hashmap;
   private:
    Node *nodePointer;
    linked_hashmap *mapPointer;
   public:
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the following
    // typedef) to work properly.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
    // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

    iterator() {
      nodePointer = nullptr;
      mapPointer = nullptr;
    }

    iterator(Node *Node_, linked_hashmap *mapPointer_) {
      nodePointer = Node_;
      mapPointer = mapPointer_;
    }

    iterator(const iterator &other) {
      nodePointer = other.nodePointer;
      mapPointer = other.mapPointer;
    }

    iterator operator++(int) {
      if (!nodePointer || nodePointer == mapPointer->tail)throw invalid_iterator();
      iterator it(*this);
      nodePointer = nodePointer->nxt;
      return it;
    }

    iterator &operator++() {
      if (!nodePointer || nodePointer == mapPointer->tail)throw invalid_iterator();
      nodePointer = nodePointer->nxt;
      return *this;
    }

    iterator operator--(int) {
      if (!nodePointer || nodePointer->pre == mapPointer->head)throw invalid_iterator();
      iterator it(*this);
      nodePointer = nodePointer->pre;
      return it;
    }

    iterator &operator--() {
      if (!nodePointer || nodePointer->pre == mapPointer->head)throw invalid_iterator();
      nodePointer = nodePointer->pre;
      return *this;
    }

    linked_hashmap::value_type &operator*() const { return *(nodePointer->value); }
    bool operator==(const iterator &rhs) const { return nodePointer == rhs.nodePointer; }
    bool operator==(const const_iterator &rhs) const { return nodePointer == rhs.nodePointer; }
    bool operator!=(const iterator &rhs) const { return nodePointer != rhs.nodePointer; }
    bool operator!=(const const_iterator &rhs) const { return nodePointer != rhs.nodePointer; }
    linked_hashmap::value_type *operator->() const noexcept { return nodePointer->value; }
  };

  class const_iterator {
    friend class linked_hashmap;
   private:
    Node *nodePointer;
    const linked_hashmap *mapPointer;
   public:
    const_iterator() {
      nodePointer = nullptr;
      mapPointer = nullptr;
    }
    const_iterator(Node *Node_, const linked_hashmap *mapPointer_) {
      nodePointer = Node_;
      mapPointer = mapPointer_;
    }
    const_iterator(const const_iterator &other) {
      nodePointer = other.nodePointer;
      mapPointer = other.mapPointer;
    }
    const_iterator(const iterator &other) {
      nodePointer = other.nodePointer;
      mapPointer = other.mapPointer;
    }

    const_iterator operator++(int) {
      if (!nodePointer || nodePointer == mapPointer->tail)throw invalid_iterator();
      const_iterator it(*this);
      nodePointer = nodePointer->nxt;
      return it;
    }

    const_iterator &operator++() {
      if (!nodePointer || nodePointer == mapPointer->tail)throw invalid_iterator();
      nodePointer = nodePointer->nxt;
      return *this;
    }

    const_iterator operator--(int) {
      if (!nodePointer || nodePointer->pre == mapPointer->head)throw invalid_iterator();
      const_iterator it(*this);
      nodePointer = nodePointer->pre;
      return it;
    }

    const_iterator &operator--() {
      if (!nodePointer || nodePointer->pre == mapPointer->head)throw invalid_iterator();
      nodePointer = nodePointer->pre;
      return *this;
    }

    linked_hashmap::value_type &operator*() const { return *(nodePointer->value); }
    bool operator==(const iterator &rhs) const { return nodePointer == rhs.nodePointer; }
    bool operator==(const const_iterator &rhs) const { return nodePointer == rhs.nodePointer; }
    bool operator!=(const iterator &rhs) const { return nodePointer != rhs.nodePointer; }
    bool operator!=(const const_iterator &rhs) const { return nodePointer != rhs.nodePointer; }
    linked_hashmap::value_type *operator->() const noexcept { return nodePointer->value; }
  };

  /**
   * TODO two constructors
   */
  linked_hashmap() {
    storageList = new Node *[Capacity];
    head = new Node();
    tail = new Node();
    head->nxt = tail;
    tail->pre = head;
    memset(storageList, 0, Capacity * sizeof(Node*));
  }
  linked_hashmap(const linked_hashmap &other) {
    Capacity = other.Capacity;
    storageList = new Node *[Capacity];
    head = new Node();
    tail = new Node();
    head->nxt = tail;
    tail->pre = head;
    memset(storageList, 0, Capacity * sizeof(Node*));
    for (Node *i = other.head->nxt; i != other.tail; i = i->nxt)insert(*(i->value));
  }

  /**
   * TODO assignment operator
   */
  linked_hashmap &operator=(const linked_hashmap &other) {
    if (&other == this)return *this;
    clear();
    for (Node *i = other.head->nxt; i != other.tail; i = i->nxt)insert(*(i->value));
    return *this;
  }

  /**
   * TODO Destructors
   */
  ~linked_hashmap() {
    clear();
    delete head, delete tail;
    delete[] storageList;
  }

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    iterator it = find(key);
    if (it != end())return it.nodePointer->value->second;
    throw index_out_of_bound();
  }
  const T &at(const Key &key) const {
    const_iterator it = find(key);
    if (it != cend())return it.nodePointer->value->second;
    throw index_out_of_bound();
  }

  T &operator[](const Key &key) {
    iterator it = find(key);
    if (it.nodePointer != tail)return it.nodePointer->value->second;
    std::pair<iterator, bool> carrier = insert(value_type(key, T()));
    return carrier.first.nodePointer->value->second;
  }

  const T &operator[](const Key &key) const {
    const_iterator it = find(key);
    if (it.nodePointer != tail)return it.nodePointer->value->second;
    throw index_out_of_bound();
  }

  iterator begin() { return iterator(head->nxt, this); }
  const_iterator cbegin() const { return const_iterator(head->nxt, this); }

  iterator end() { return iterator(tail, this); }
  const_iterator cend() const { return const_iterator(tail, this); }

  bool empty() const { return !NodeNum; }

  size_t size() const { return NodeNum; }

  void clear() {
    while (!empty())erase(begin());
  }

  std::pair<iterator, bool> insert(const value_type &value) {
    iterator it = find(value.first);
    if (it != end())return std::make_pair(it, false);

    NodeNum++;
    if (NodeNum > Capacity * LoadFactor)reSize();

    int hash = Hash()(value.first) % Capacity;
    Node *node = new Node(value);
    node->pre = tail->pre;
    node->nxt = tail;
    tail->pre->nxt = node;
    tail->pre = node;
    if (!storageList[hash]) {
      storageList[hash] = new Node;
      storageList[hash]->son = new Node;
      storageList[hash]->son->fa = storageList[hash];
    }
    Node *start = storageList[hash];
    node->fa = start;
    node->son = start->son;
    node->son->fa = node;
    start->son = node;

    return std::make_pair(iterator(node, this), true);
  }

  void erase(iterator pos) {
    if (pos.mapPointer != this || pos.nodePointer == nullptr || pos == this->end())throw index_out_of_bound();

    NodeNum--;
    int hash = Hash()(pos.nodePointer->value->first) % Capacity;
    Node *node = pos.nodePointer;
    node->pre->nxt = node->nxt;
    node->nxt->pre = node->pre;
    node->fa->son = node->son;
    node->son->fa = node->fa;
    if (!node->fa->fa && !node->son->son) {
      delete node->son;
      delete node->fa;
      storageList[hash] = nullptr;
    }
    delete node;
  }

  size_t count(const Key &key) const {
    if (find(key) != cend())return 1;
    return 0;
  }

  iterator find(const Key &key) {
    int hash = Hash()(key) % Capacity;
    if (!storageList[hash])return end();
    for (Node *start = storageList[hash]->son; start->son; start = start->son)
      if (Equal()(start->value->first, key))
        return iterator(start, this);

    return end();
  }
  const_iterator find(const Key &key) const {
    int hash = Hash()(key) % Capacity;
    if (!storageList[hash])return cend();
    for (Node *start = storageList[hash]->son; start->son; start = start->son)
      if (Equal()(start->value->first, key))
        return const_iterator(start, this);

    return cend();
  }
};

}

#endif
