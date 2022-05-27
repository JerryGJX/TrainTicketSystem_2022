#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    private:
        T *p;
        size_t cur_num, lim, sizT;

        // lim >= cur_num+1
        // cur_num 1 based
        void extra_memory() {
            T *p1 = (T *) malloc((lim << 1) * sizT);
            for (int i = 0; i < cur_num; ++i) {
                new(p1 + i) T(p[i]);
                (p + i)->~T();
            }
            free(p);
            p = p1, lim <<= 1;
        }

        void my_destructor() {
            for (int i = 0; i < cur_num; ++i)
                (p + i)->~T();
            cur_num = 0;
        }

    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;

        class iterator {
            /* The following code is written for the C++ type_traits library.
             Type traits is a C++ feature for describing certain properties of a type.
             For instance, for an iterator, iterator::value_type is the type that the
             iterator points to.
             STL algorithms and containers may use these type_traits (e.g. the following
             typedef) to work properly. In particular, without the following code,
             @code{std::sort(iter, iter1);} would not compile.
             See these websites for more information:
             https://en.cppreference.com/w/cpp/header/type_traits
             About value_type: https://blog.csdn.net/u014299153/article/details/72419713
             About iterator_category: https://en.cppreference.com/w/cpp/iterator
             */
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            T *bel, *pp;
            size_t siz;
        public:

            const int get_rank() { return pp - bel; }

            const int get_tot() { return siz; }

            iterator(T *_bel, T *_pp, size_t _siz) : bel(_bel), pp(_pp), siz(_siz) {}

            friend int operator-(const iterator &it1, const iterator &it2) {
                return (int) (it1.pp - it2.pp);
            }

            iterator operator+(const int &n) const {
                /**
                 * return a new iterator which pointer n-next elements
                 * as well as operator-
                 */
                //TODO
                return iterator(bel, pp + n, siz);
            }

            iterator operator-(const int &n) const {
                //TODO
                return iterator(bel, pp - n, siz);
            }

            int operator-(const iterator &rhs) const {
                // return the distance between two iterators,
                // if these two iterators point to different vectors, throw invaild_iterator.
                //TODO
                if (bel != rhs.bel)throw invalid_iterator();
                return abs(pp - rhs.pp);
            }

            iterator &operator+=(const int &n) {
                //TODO
                pp += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                //TODO
                pp -= n;
                return *this;
            }

            iterator operator++(int) {
                /**
                 * TODO iter++
                 */
                iterator ret = *this;
                ++pp;
                return ret;
            }

            iterator &operator++() {
                /**
                 * TODO ++iter
                 */
                ++pp;
                return *this;
            }

            iterator operator--(int) {
                /**
                 * TODO iter--
                 */
                iterator ret = *this;
                --pp;
                return ret;
            }

            iterator &operator--() {
                /**
                 * TODO --iter
                 */
                --pp;
                return *this;
            }

            T &operator*() const {
                /**
                 * TODO *it
                 */
                return *pp;
            }

            bool operator==(const iterator &rhs) const {
                /**
                 * a operator to check whether two iterators are same (pointing to the same memory address).
                 */
                return pp == rhs.pp;
            }

            bool operator==(const const_iterator &rhs) const {
                return pp == rhs.pp;
            }

            bool operator!=(const iterator &rhs) const {
                /**
                 * some other operator for iterator.
                 */
                return pp != rhs.pp;
            }

            bool operator!=(const const_iterator &rhs) const {
                return pp != rhs.pp;
            }
        };

        class const_iterator {
            /**
             * TODO
             * has same function as iterator, just for a const object.
             */
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            T *bel, *pp;
            size_t siz;
        public:

            const int get_rank() { return pp - bel; }

            const int get_tot() { return siz; }

            const_iterator(T *_bel, T *_pp, size_t _siz) : bel(_bel), pp(_pp), siz(_siz) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                //TODO
                return const_iterator(bel, pp + n, siz);
            }

            const_iterator operator-(const int &n) const {
                //TODO
                return const_iterator(bel, pp - n, siz);
            }

            int operator-(const const_iterator &rhs) const {
                // return the distance between two iterators,
                // if these two iterators point to different vectors, throw invaild_iterator.
                //TODO
                if (bel != rhs.bel)throw invalid_iterator();
                return abs(pp - rhs.pp);
            }

            const_iterator &operator+=(const int &n) {
                //TODO
                pp += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                //TODO
                pp -= n;
                return *this;
            }

            const_iterator operator++(int) {
                /**
                 * TODO iter++
                 */
                const_iterator ret = *this;
                ++pp;
                return ret;
            }

            const_iterator &operator++() {
                /**
                 * TODO ++iter
                 */
                ++pp;
                return *this;
            }

            const_iterator operator--(int) {
                /**
                 * TODO iter--
                 */
                const_iterator ret = *this;
                --pp;
                return ret;
            }

            const_iterator &operator--() {
                /**
                 * TODO --iter
                 */
                --pp;
                return *this;
            }

            const T &operator*() const {
                /**
                 * TODO *it
                 */
                return *pp;
            }

            bool operator==(const const_iterator &rhs) const {
                /**
                 * a operator to check whether two iterators are same (pointing to the same memory address).
                 */
                return pp == rhs.pp;
            }

            bool operator==(const iterator &rhs) const {
                return pp == rhs.pp;
            }

            bool operator!=(const const_iterator &rhs) const {
                /**
                 * some other operator for iterator.
                 */
                return pp != rhs.pp;
            }

            bool operator!=(const iterator &rhs) const {
                return pp != rhs.pp;
            }

        };

        vector() : cur_num(0), lim(1), sizT(sizeof(T)) {// lim always bigger than max_num by one !!!
            /**
             * TODO Constructs
             * Atleast two: default constructor, copy constructor
             */
            p = (T *) malloc(sizT);
        }

        // placement new
        vector(const vector &other) : cur_num(other.cur_num), lim(other.lim), sizT(other.sizT) {
            p = (T *) malloc(lim * sizT);
            for (int i = 0; i < cur_num; ++i)
                new(p + i) T(*(other.p + i));
        }

        vector(const T &element) : cur_num(0), lim(1), sizT(sizeof(T)) {
            p = (T *) malloc(sizT);
            this->push_back(element);
        }

        ~vector() {
            /**
             * TODO Destructor
             */
            my_destructor(), free(p);
            p = nullptr;
        }

        //deeply copy, destroy the old objects and use placement new to initialize
        vector &operator=(const vector &other) {
            /**
             * TODO Assignment operator
             */
            if (this == &other)return *this;
            my_destructor(), free(p);
            cur_num = other.cur_num;
            lim = other.lim;
            sizT = other.sizT;
            p = (T *) malloc(lim * sizT);
            for (int i = 0; i < cur_num; ++i)
                new(p + i) T(*(other.p + i));
            return *this;
        }

        T &at(const size_t &pos) {
            /**
             * assigns specified element with bounds checking
             * throw index_out_of_bound if pos is not in [0, size)
             */
            if (pos < 0 || pos >= cur_num)throw index_out_of_bound();
            return *(p + pos);
        }

        const T &at(const size_t &pos) const {// for the const vectors
            if (pos < 0 || pos >= cur_num)throw index_out_of_bound();
            return *(p + pos);
        }

        T &operator[](const size_t &pos) {
            /**
             * assigns specified element with bounds checking
             * throw index_out_of_bound if pos is not in [0, size)
             * !!! Pay attentions
             *   In STL this operator does not check the boundary but I want you to do.
             */
            if (pos < 0 || pos >= cur_num)throw index_out_of_bound();
            return *(p + pos);
        }

        //how do program know which function to use?
        //non-const object of this class can use both const function and non-const function
        //meanwhile const object of this class can only use const function
        //the function below is used for that
        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= cur_num)throw index_out_of_bound();
            return *(p + pos);
        }

        // functions like "front","back",can only access the value, can not modify it.
        const T &front() const {
            /**
             * access the first element.
             * throw container_is_empty if size == 0
             */
            if (cur_num == 0)throw container_is_empty();
            return *p;
        }

        const T &back() const {
            /**
             * access the last element.
             * throw container_is_empty if size == 0
             */
            if (cur_num == 0)throw container_is_empty();
            return *(p + cur_num - 1);
        }

        iterator begin() {
            /**
             * returns an iterator to the beginning.
             */
            return iterator(p, p, cur_num);
        }

        //maybe when you don't want to modify the value, you can apply for a const_iterator
        //so the non-const object can also use cbegin,that's why "cbegin" and "begin" use the different name?
        const_iterator cbegin() const {
            return const_iterator(p, p, cur_num);
        }

        iterator end() {
            /**
             * returns an iterator to the end.
             */
            return iterator(p, p + cur_num, cur_num);
        }

        const_iterator cend() const {
            return const_iterator(p, p + cur_num, cur_num);
        }

        // just to know some status of the vector , no need to modify
        bool empty() const {
            /**
             * checks whether the container is empty
             */
            return cur_num == 0;
        }

        size_t size() const {
            /**
             * returns the number of elements
             */
            return cur_num;
        }

        //!!! release the memory && use my_destructor
        void clear() {
            /**
             * clears the contents
             */
            my_destructor();
            free(p);
            cur_num = 0, lim = 1;
            p = (T *) malloc(lim * sizT);
        }

        // placement new here
        iterator insert(iterator pos, const T &value) {
            /**
             * inserts value before pos
             * returns an iterator pointing to the inserted value.
             */
            int rank = pos.get_rank();
            if (cur_num + 1 >= lim)extra_memory();
            new(p + cur_num) T(value);
            for (int i = cur_num; i > rank; --i)
                std::swap(*(p + i), *(p + i - 1));
            ++cur_num;
            return iterator(p, p + rank, cur_num);
        }

        // placement new here
        iterator insert(const size_t &ind, const T &value) {
            /**
             * inserts value at index ind.
             * after inserting, this->at(ind) == value
             * returns an iterator pointing to the inserted value.
             * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
             */
            if (ind > cur_num)throw index_out_of_bound();
            if (cur_num + 1 >= lim)extra_memory();
            new(p + cur_num) T(value);
            for (int i = cur_num; i > ind; --i)
                std::swap(*(p + i), *(p + i - 1));
            ++cur_num;
            return iterator(p, p + ind, cur_num);
        }

        // destroy when erase
        iterator erase(iterator pos) {
            /**
             * removes the element at pos.
             * return an iterator pointing to the following element.
             * If the iterator pos refers the last element, the end() iterator is returned.
             */
            int rank = pos.get_rank();
            for (int i = rank; i < pos.get_tot(); ++i)
                std::swap(*(p + i), *(p + i + 1));
            --cur_num;
            (p + cur_num)->~T();
            return iterator(p, p + rank, cur_num);
        }

        // destroy when erase
        iterator erase(const size_t &ind) {
            /**
             * removes the element with index ind.
             * return an iterator pointing to the following element.
             * throw index_out_of_bound if ind >= size
             */
            if (ind >= cur_num)throw index_out_of_bound();
            for (size_t i = ind; i < cur_num; ++i)
                std::swap(*(p + i), *(p + i + 1));
            --cur_num;
            (p + cur_num)->~T();
            return iterator(p, p + ind, cur_num);
        }

        // the memory already exists, use the placement new !
        void push_back(const T &value) {
            /**
             * adds an element to the end.
             */
            if (cur_num + 1 >= lim)extra_memory();
            new(p + cur_num) T(value);
            ++cur_num;
        }

        // when cur_num-- , destroy the unused object which is just poped
        void pop_back() {
            /**
             * remove the last element from the end.
             * throw container_is_empty if size() == 0
             */
            if (cur_num == 0)throw container_is_empty();
            --cur_num;
            (p + cur_num)->~T();
        }
    };


}

#endif