#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

//#include <climits>
//#include <cstddef>
#include <iostream>

namespace sjtu
{
  /**
   * a data container like std::vector
   * store data in a successive memory and support random access.
   */
  template <typename T>
  class vector
  {
  private:
    T **storage;
    int real_size = 0;
    int token_size = 129;

  public:
    class const_iterator;

    class iterator
    {
      // The following code is written for the C++ type_traits library.
      // Type traits is a C++ feature for describing certain properties of a type.
      // For instance, for an iterator, iterator::value_type is the type that the
      // iterator points to.
      // STL algorithms and containers may use these type_traits (e.g. the following
      // typedef) to work properly. In particular, without the following code,
      // @code{std::sort(iter, iter1);} would not compile.
      // See these websites for more information:
      // https://en.cppreference.com/w/cpp/header/type_traits
      // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
      // About iterator_category: https://en.cppreference.com/w/cpp/iterator
    public:
      //    using difference_type = std::ptrdiff_t;
      //    using value_type = T;
      //    using pointer = T *;
      //    using reference = T &;
      //    using iterator_category = std::output_iterator_tag;

      friend vector<T>;

    private:
      T **pointed_location = nullptr;
      vector<T> *vector_head = nullptr;

    public:
      iterator(T **pointed_, vector<T> *vector_pointer) : pointed_location(pointed_),
                                                          vector_head(vector_pointer) {}

      iterator(const iterator &rhs)
      {
        pointed_location = rhs.pointed_location;
        vector_head = rhs.vector_head;
      }

      iterator() = default;

      iterator operator+(const int &n) const
      {
        // TODO
        iterator result(pointed_location + n, vector_head);
        return result;
      }
      iterator operator-(const int &n) const
      {
        // TODO
        iterator result(pointed_location - n, vector_head);
        return result;
      }
      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const iterator &rhs) const
      {

        if (vector_head != rhs.vector_head)
          throw invalid_iterator(); // NOLINT
        return (pointed_location - rhs.pointed_location);
      }
      iterator &operator+=(const int &n)
      {

        iterator result((*this) + n);
        return result;
      }
      iterator &operator-=(const int &n)
      {

        iterator result((*this) - n);
        return result;
      }

      iterator operator++(int)
      { // NOLINT
        iterator answer = *this;
        pointed_location++;
        return answer;
      }

      iterator &operator++()
      {
        pointed_location++;
        return *this;
      }

      iterator operator--(int)
      { // NOLINT
        iterator answer = *this;
        pointed_location--;
        return answer;
      }

      iterator &operator--()
      {
        pointed_location--;
        return *this;
      }

      T &operator*() const
      {
        return **(this->pointed_location);
      }
      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const
      {
        return ((this->pointed_location)) == ((rhs.pointed_location));
      }

      bool operator==(const const_iterator &rhs) const
      {
        return ((this->pointed_location)) == ((rhs.pointed_location));
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const
      {
        return ((this->pointed_location)) != ((rhs.pointed_location));
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return ((this->pointed_location)) != ((rhs.pointed_location));
      }
    };

    class const_iterator
    {
    public:
      //    using difference_type = std::ptrdiff_t;
      //    using value_type = T;
      //    using pointer = T *;
      //    using reference = T &;
      //    using iterator_category = std::output_iterator_tag;

    private:
      /*TODO*/
      T const *const *pointed_location = nullptr;
      const vector<T> *vector_head;

    public:
      const_iterator(T const *const *pointed_, const vector<T> *vector_pointer) : pointed_location(pointed_),
                                                                                  vector_head(vector_pointer) {}

      const_iterator(const const_iterator &rhs)
      {
        pointed_location = rhs.pointed_location;
        vector_head = rhs.vector_head;
      }

      const_iterator() = default;

      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      const_iterator operator+(const int &n) const
      {

        const_iterator result(pointed_location + n, vector_head);
        return result;
      }
      const_iterator operator-(const int &n) const
      {

        const_iterator result(pointed_location - n, vector_head);
        return result;
      }
      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const const_iterator &rhs) const
      {

        if (vector_head != rhs.vector_head)
          throw invalid_iterator(); // NOLINT
        return (pointed_location - rhs.pointed_location);
      }
      const_iterator &operator+=(const int &n)
      {

        const_iterator result((*this) + n);
        return result;
      }
      const_iterator &operator-=(const int &n)
      {

        const_iterator result((*this) - n);
        return result;
      }

      const_iterator operator++(int)
      { // NOLINT
        const_iterator answer = *this;
        pointed_location++;
        return answer;
      }

      const_iterator &operator++()
      {
        pointed_location++;
        return *this;
      }

      const_iterator operator--(int)
      { // NOLINT
        const_iterator answer = *this;
        pointed_location--;
        return answer;
      }

      const_iterator &operator--()
      {
        pointed_location--;
        return *this;
      }

      const T &operator*() const
      {
        return **(this->pointed_location);
      }
      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator &rhs) const
      {
        return pointed_location == rhs.pointed_location;
      }

      bool operator==(const const_iterator &rhs) const
      {
        return pointed_location == rhs.pointed_location;
      }

      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator &rhs) const
      {
        return pointed_location != rhs.pointed_location;
      }
      bool operator!=(const const_iterator &rhs) const
      {
        return pointed_location != rhs.pointed_location;
      }
    };

    vector()
    {
      storage = new T *[token_size];
    }
    vector(const vector &other)
    {
      storage = new T *[other.token_size];
      for (int i = 0; i < other.real_size; i++)
      {
        storage[i] = new T(*other.storage[i]);
      }

      real_size = other.real_size;
      token_size = other.token_size;
    }

    ~vector()
    {
      for (int i = 0; i < real_size; i++)
      {
        delete storage[i];
      }
      delete[] storage;
      storage = nullptr;
      //    real_size = 0;
      //    token_size = 0;
    }

    vector &operator=(const vector &other)
    {
      if (this == &other)
        return *this;
      for (int i = 0; i < real_size; i++)
        delete storage[i];
      delete[] storage;
      storage = new T *[other.token_size];
      for (int i = 0; i < other.real_size; i++)
      {
        storage[i] = new T(*other.storage[i]);
      }
      real_size = other.real_size;
      token_size = other.token_size;
      return *this;
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */

    T &at(const size_t &pos)
    {
      if (pos < 0 || pos >= real_size)
        throw index_out_of_bound(); // NOLINT
      return **(storage + pos);
    }

    const T &at(const size_t &pos) const
    {
      if (pos < 0 || pos >= real_size)
        throw index_out_of_bound(); // NOLINT
      return **(storage + pos);
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T &operator[](const size_t &pos)
    {
      if (pos < 0 || pos >= real_size)
        throw index_out_of_bound(); // NOLINT
      return **(storage + pos);
    }
    const T &operator[](const size_t &pos) const
    {
      if (pos < 0 || pos >= real_size)
        throw index_out_of_bound(); // NOLINT
      return **(storage + pos);
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const
    {
      if (!real_size)
        throw container_is_empty(); // NOLINT
      return **storage;
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const
    {
      if (!real_size)
        throw container_is_empty(); // NOLINT
      return **(storage + real_size - 1);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin()
    {
      iterator result(this->storage, this);
      return result;
    }
    const_iterator cbegin() const
    {
      return const_iterator(storage, this);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end()
    {
      iterator result(this->storage + real_size, this);
      return result;
    }
    const_iterator cend() const
    {
      const_iterator result(this->storage + real_size, this);
      return result;
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const
    {
      return !(this->real_size);
    }
    /**
     * returns the number of elements
     */
    size_t size() const
    {
      return this->real_size;
    }
    /**
     * clears the contents
     */
    void clear()
    {
      for (int i = 0; i < real_size; i++)
      {
        delete storage[i];
      }
      delete[] storage;
      token_size = 10;
      storage = new T *[token_size];
      //    storage = nullptr;
      real_size = 0;
    }

    void doublespace()
    {
      T **carrier = storage;
      // delete[]storage;
      storage = new T *[token_size * 2];
      for (int i = 0; i < real_size; i++)
      {
        //      storage[i] = new T(*carrier[i]);
        storage[i] = carrier[i];
      }
      delete[] carrier;
      token_size *= 2;
    }

    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value)
    {
      int index = (pos.pointed_location - storage);
      if ((real_size + 1) >= token_size)
        doublespace();
      //    if (real_size > 0) {
      //      storage[real_size] = new T(*storage[real_size - 1]);
      for (int i = real_size; i > index; i--)
      {
        storage[i] = storage[i - 1];
      }
      // delete storage[index];
      //    }
      real_size++;
      storage[index] = new T(value);
      iterator result(storage + index, this);
      return result;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value)
    {
      if (ind > real_size)
        throw index_out_of_bound(); // NOLINT
      if ((real_size + 1) >= token_size)
        doublespace();
      //    if (real_size > 0) {
      //      storage[real_size] = new T(*storage[real_size - 1]);
      for (int i = real_size; i > ind; i--)
      {
        storage[i] = storage[i - 1];
      }
      // delete storage[ind];
      //    }
      real_size++;
      storage[ind] = new T(value);
      iterator result(storage + ind, this);
      return result;
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos)
    {
      int index = (pos.pointed_location - storage);
      delete storage[index];
      for (int i = index; i < real_size - 1; i++)
      {
        storage[i] = storage[i + 1];
      }
      real_size--;

      if (index == real_size)
      {
        return end();
      }
      else
      {
        iterator result(storage + index, this);
        return result;
      }

      //    iterator result(storage + index, this);
      //    return result;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind)
    {
      if (ind >= real_size)
        throw index_out_of_bound(); // NOLINT
      delete storage[ind];
      for (size_t i = ind; i < real_size - 1; i++)
      {
        storage[i] = storage[i + 1];
      }
      //    storage[real_size] = nullptr;
      real_size--;
      if (ind == real_size)
      {
        iterator result(storage + (ind - 1), this);
        return result;
      }
      else
      {
        iterator result(storage + ind, this);
        return result;
      }
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value)
    {
      insert(real_size, value);
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back()
    {
      if (real_size == 0)
        throw container_is_empty(); // NOLINT
      delete storage[real_size - 1];
      real_size--;
    }
  };

}

#endif
