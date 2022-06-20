/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
//#include <functional>
//#include <cstddef>
//#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu
{

  template <
      class Key,
      class T,
      class Compare = std::less<Key>>
  class map
  {
  public:
    typedef pair<const Key, T> value_type;

  private:
    struct AvlNode
    {
      value_type *data = nullptr;
      AvlNode *left = nullptr;
      AvlNode *right = nullptr;
      AvlNode *father = nullptr;
      int height = 1;

      AvlNode()
      {
        data = nullptr;
        left = nullptr;
        right = nullptr;
        father = nullptr;
        height = 0;
      }

      AvlNode(const value_type &data_, AvlNode *left_, AvlNode *right_, AvlNode *father_ = nullptr, int height_ = 1)
          : data(new value_type{data_}), left(left_), right(right_), father(father_), height(height_) {}
      AvlNode(AvlNode *lhs, AvlNode *father_, AvlNode *left_ = nullptr, AvlNode *right_ = nullptr)
          : data(new value_type{*(lhs->data)}), father(father_), left(left_), right(right_) {}

      AvlNode(AvlNode &lhs)
          : father(lhs.father),
            left(lhs.left),
            right(lhs.right),
            height(lhs.height)
      {
        data = (lhs.data) ? new value_type{*lhs.data} : nullptr;
      }
      ~AvlNode()
      {
        if (data)
          delete data;
        data = nullptr;
        left = nullptr;
        right = nullptr;
        father = nullptr;
        height = 0;
      }
    };
    AvlNode *root;
    AvlNode *tail;
    int num = 0;

    void build(AvlNode *&now, AvlNode *target, AvlNode *father)
    {
      if (target == nullptr)
        return;
      now = new AvlNode(target, father);
      build(now->left, target->left, now);
      build(now->right, target->right, now);
    }

    AvlNode *privatefind(const Key &x, AvlNode *root_) const
    {
      if (root_ == nullptr || (!Compare()(root_->data->first, x) && !Compare()(x, root_->data->first)))
        return root_;

      if (Compare()(x, root_->data->first))
        return privatefind(x, root_->left);
      else
        return privatefind(x, root_->right);
    }

    AvlNode *findMin(AvlNode *node) const
    {
      if (!node)
        return node;
      while (node->left)
      {
        node = node->left;
      }
      if (node)
        return node;
      else
        return tail;
    }

    AvlNode *findMax(AvlNode *node) const
    {
      if (!node)
        return node;
      while (node->right)
      {
        node = node->right;
      }
      return node;
    }

    AvlNode *findNext(AvlNode *node_) const
    {
      // if (node == tail)throw invalid_iterator();
      AvlNode *node = node_;
      if (node->right)
      {
        node = node->right;
        while (node->left)
          node = node->left;
        return node;
      }
      else
      {
        AvlNode *oldNode = node;
        while (node->father)
        {
          if (Compare()(oldNode->data->first, node->father->data->first))
            return node->father;
          else
          {
            node = node->father;
          }
        }
        return tail;
      }
    }

    AvlNode *findPre(AvlNode *node) const
    {
      if (node == tail)
        return findMax(root);
      if (node->left)
      {
        node = node->left;
        while (node->right)
          node = node->right;
        return node;
      }
      else
      {
        AvlNode *oldNode = node;
        while (node->father)
        {
          if (Compare()(node->father->data->first, oldNode->data->first))
            return node->father;
          else
          {
            node = node->father;
          }
        }
        return nullptr;
      }
    }

    void CheckRtAndConnect(AvlNode *t, AvlNode *tmp)
    {
      if (t->father == nullptr)
        root = tmp;
      else if (t->father->left == t)
        t->father->left = tmp;
      else if (t->father->right == t)
        t->father->right = tmp;
    }

    void insert(const value_type &x, AvlNode *&t, AvlNode *father_)
    {
      if (t == nullptr)
      {
        t = new AvlNode(x, nullptr, nullptr, father_);
        if (!father_)
          root = t;
        else
        {
          if (Compare()(x.first, father_->data->first))
            father_->left = t;
          else
            father_->right = t;
        }
      }
      else if (Compare()(x.first, t->data->first))
      {
        insert(x, t->left, t);
        if (height(t->left) - height(t->right) >= 2)
        {
          if (Compare()(x.first, t->left->data->first))
            LL(t);
          else
            LR(t);
        }
      }
      else if (!Compare()(x.first, t->data->first))
      {
        insert(x, t->right, t);
        if (height(t->right) - height(t->left) >= 2)
        {
          if (Compare()(t->right->data->first, x.first))
            RR(t);
          else
            RL(t);
        }
      }
      t->height = std::max(height(t->left), height(t->right)) + 1;
    }

    void remove(AvlNode *t)
    {
      if (t == nullptr)
        return;
      if (t->left && t->right)
      {
        AvlNode *tmp = findNext(t);
        if (tmp == tail)
        {
          root = nullptr;
          delete t;
          return;
        }
        AvlNode *oldNode = new AvlNode(*tmp);
        CheckRtAndConnect(t, tmp);
        if (tmp == t->right)
        {
          tmp->left = t->left;
          if (t->left)
            t->left->father = tmp;
          tmp->right = oldNode;
          oldNode->father = tmp;
          if (oldNode->right)
            oldNode->right->father = oldNode;
          tmp->father = t->father;
          tmp->height = t->height;
        }
        else
        {
          tmp->left = t->left;
          if (t->left)
            t->left->father = tmp;
          tmp->right = t->right;
          t->right->father = tmp;
          oldNode->father->left = oldNode;
          if (oldNode->right)
            oldNode->right->father = oldNode;
          tmp->father = t->father;
          tmp->height = t->height;
        }
        remove(oldNode);
        delete t;
      }
      else
      {
        CheckRtAndConnect(t, t->left == nullptr ? t->right : t->left);
        if (t->left)
          t->left->father = t->father;
        if (t->right)
          t->right->father = t->father;
        adjust(t->father);
        delete t;
      }
    }
    void makeEmpty(AvlNode *t)
    {
      if (!t)
        return;
      makeEmpty(t->left);
      makeEmpty(t->right);
      delete t;
    }

    int height(AvlNode *t) const { return t == nullptr ? 0 : t->height; }

    void LL(AvlNode *t)
    {
      AvlNode *newroot = t->left;
      if (newroot->right)
        newroot->right->father = t;
      newroot->father = t->father;
      t->left = newroot->right;
      CheckRtAndConnect(t, newroot);
      t->father = newroot;
      newroot->right = t;
      t->height = std::max(height(t->left), height(t->right)) + 1;
      newroot->height = std::max(height(newroot->left), height(t)) + 1;
    }
    void RR(AvlNode *t)
    {
      AvlNode *newroot = t->right;
      if (newroot->left)
        newroot->left->father = t;
      newroot->father = t->father;
      t->right = newroot->left;
      CheckRtAndConnect(t, newroot);
      t->father = newroot;
      newroot->left = t;
      t->height = std::max(height(t->left), height(t->right)) + 1;
      newroot->height = std::max(height(newroot->right), height(t)) + 1;
    }
    void LR(AvlNode *t)
    {
      RR(t->left);
      LL(t);
    }
    void RL(AvlNode *t)
    {
      LL(t->right);
      RR(t);
    }

    void adjust(AvlNode *t)
    {
      if (!t)
        return;
      t->height = std::max(height(t->left), height(t->right)) + 1;
      if (height(t->left) - height(t->right) >= 2)
      {
        if (height(t->left->left) < height(t->left->right))
          LR(t);
        else
          LL(t);
      }
      else if (height(t->right) - height(t->left) >= 2)
      {
        if (height(t->right->left) > height(t->right->right))
          RL(t);
        else
          RR(t);
      }
      adjust(t->father);
    }

  public:
    class const_iterator;
    class iterator
    {
    public:
      AvlNode *node;
      map *target;

      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T *;
      using reference = T &;
      using iterator_category = std::output_iterator_tag;

      iterator() = default;

      iterator(AvlNode *node_, map *target_)
      {
        node = node_;
        target = target_;
      }

      iterator(const iterator &other)
      {
        node = other.node;
        target = other.target;
      }

      iterator operator++(int)
      {
        if (node == target->tail)
          throw invalid_iterator();
        iterator it = *this;
        this->node = target->findNext(node);
        return it;
      }

      iterator &operator++()
      {
        if (node == target->tail)
          throw invalid_iterator();
        this->node = target->findNext(node);
        return *this;
      }

      iterator operator--(int)
      {
        iterator it = *this;
        if (*this == target->begin())
          throw invalid_iterator();
        if (!node)
          throw invalid_iterator();
        this->node = target->findPre(node);
        return it;
      }

      iterator &operator--()
      {
        if (*this == target->begin())
          throw invalid_iterator();
        if (!node)
          throw invalid_iterator();
        this->node = target->findPre(node);
        return *this;
      }

      map::value_type &operator*() const
      {
        return *(node->data);
      }

      bool operator==(const iterator &rhs) const
      {
        return node == rhs.node;
      }
      bool operator==(const const_iterator &rhs) const
      {
        return node == rhs.node;
      }

      bool operator!=(const iterator &rhs) const
      {
        return node != rhs.node;
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return node != rhs.node;
      }

      map::value_type *operator->() const noexcept
      {
        return node->data;
      }
    };

    class const_iterator
    {
    public:
      using value_type = T;

    public:
      AvlNode *node;
      const map *target;

      const_iterator() = default;
      const_iterator(AvlNode *node_, const map *target_)
      {
        node = node_;
        target = target_;
      }
      const_iterator(const const_iterator &other)
      {
        node = other.node;
        target = other.target;
      }
      const_iterator(const iterator &other)
      {
        node = other.node;
        target = other.target;
      }

      const_iterator operator++(int)
      {
        if (node == target->tail)
          throw invalid_iterator();
        const_iterator it = *this;
        this->node = target->findNext(node);
        return it;
      }

      const_iterator &operator++()
      {
        if (node == target->tail)
          throw invalid_iterator();
        this->node = target->findNext(node);
        return *this;
      }

      const_iterator operator--(int)
      {
        const_iterator it = *this;
        if (it == target->cbegin())
          throw invalid_iterator();
        if (!node)
          throw invalid_iterator();
        this->node = target->findPre(node);
        return it;
      }

      const_iterator &operator--()
      {
        const_iterator it = *this;
        if (it == target->cbegin())
          throw invalid_iterator();
        if (!node)
          throw invalid_iterator();
        this->node = target->findPre(node);
        return *this;
      }

      map::value_type &operator*() const
      {
        return *(node->data);
      }

      bool operator==(const iterator &rhs) const
      {
        return node == rhs.node;
      }
      bool operator==(const const_iterator &rhs) const
      {
        return node == rhs.node;
      }

      bool operator!=(const iterator &rhs) const
      {
        return node != rhs.node;
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return node != rhs.node;
      }

      map::value_type *operator->() const noexcept
      {
        return node->data;
      }
    };

    map()
    {
      root = nullptr;
      tail = new AvlNode();
    }

    map(const map &other)
    {
      tail = new AvlNode();
      root = nullptr;
      build(root, other.root, nullptr);
      num = other.num;
    }

    map &operator=(const map &other)
    {
      if (this == &other)
        return *this;
      makeEmpty(root);
      root = nullptr;
      build(root, other.root, nullptr);
      num = other.num;
      return *this;
    }

    ~map()
    {
      makeEmpty(root);
      root = nullptr;
      num = 0;
      delete tail;
    }

    T &at(const Key &key)
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return carrier->data->second;
      throw index_out_of_bound();
    }
    const T &at(const Key &key) const
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return carrier->data->second;
      throw index_out_of_bound();
    }

    T &operator[](const Key &key)
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return carrier->data->second;
      num++;
      insert(value_type(key, T()), root, nullptr);
      AvlNode *node = privatefind(key, root);
      return node->data->second;
    }

    const T &operator[](const Key &key) const
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return carrier->data->second;
      throw index_out_of_bound();
    }

    iterator begin()
    {
      if (root)
        return iterator(findMin(root), this);
      return iterator(tail, this);
    }
    const_iterator cbegin() const
    {
      AvlNode *carrier = findMin(root);
      if (root)
        return const_iterator(carrier, this);
      return const_iterator(tail, this);
    }

    iterator end()
    {
      return iterator(tail, this);
    }
    const_iterator cend() const
    {
      return const_iterator(tail, this);
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const
    {
      return root == nullptr;
    }
    /**
     * returns the number of elements.
     */
    size_t size() const
    {
      return num;
    }
    /**
     * clears the contents
     */
    void clear()
    {
      makeEmpty(root);
      root = nullptr;
      num = 0;
    }

    pair<iterator, bool> insert(const value_type &value)
    {
      AvlNode *carrier = privatefind(value.first, root);
      if (carrier)
        return make_pair(iterator(carrier, this), false);
      num++;
      insert(value, root, nullptr);
      carrier = privatefind(value.first, root);
      return make_pair(iterator(carrier, this), true);
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos)
    {
      if (pos.node == nullptr || pos.target != this || pos.node == tail)
        throw index_out_of_bound();
      num--;
      remove(pos.node);
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return 1;
      return 0;
    }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key)
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return iterator(carrier, this);
      return iterator(tail, this);
    }
    const_iterator find(const Key &key) const
    {
      AvlNode *carrier = privatefind(key, root);
      if (carrier)
        return const_iterator(carrier, this);
      return const_iterator(tail, this);
    }
  };

}

#endif
