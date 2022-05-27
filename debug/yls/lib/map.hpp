/**
 * reference: Introduction to Algorithms
 * using the strange sentinel node --- nil
 * Thank your all family, sentinel
 * (I am sad)
 */
/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    struct my_true_type {
    };
    struct my_false_type {
    };

    template<typename my_iterator>
    struct my_type_traits {
        typedef typename my_iterator::iterator_assignable iterator_assignable;
    };

    template<class Key, class T, class Compare = std::less<Key> >
    class map {
    public:
        typedef pair<const Key, T> value_type;
    private:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        using MAP = map<Key, T, Compare>;

        Compare cmp;

        class node {
        public:
            bool col;// 0-red 1-black
            value_type *data;
            node *ch[2], *fa;

            // key may not have default constructor !!!
            node() {
                data = nullptr;
            }

            node(const value_type &element, bool _col) : col(_col) {
                data = new value_type(element);
                ch[0] = ch[1] = fa = nullptr;
            }
        } *nil, *root, *__begin;

        size_t _size;

        void dfs_copy(node *&o1, node *o2, node *fa, node *o2_nil) {
            if (o2 == o2_nil)return o1 = nil, void();
            o1 = new node(*(o2->data), o2->col);
            o1->fa = fa;
            dfs_copy(o1->ch[0], o2->ch[0], o1, o2_nil);
            dfs_copy(o1->ch[1], o2->ch[1], o1, o2_nil);
        }

        void dfs_destroy(node *&o1) {
            if (o1 == nil)return;
            dfs_destroy(o1->ch[0]);
            dfs_destroy(o1->ch[1]);
            delete o1->data;
            delete o1;
            o1 = nil;
        }

        node *my_find(const Key &key) const {
            node *o = root;
            while (o != nil && (cmp(o->data->first, key) || cmp(key, o->data->first))) {
                if (cmp(key, o->data->first))o = o->ch[0];
                else o = o->ch[1];
            }
            return o;
        }

        inline bool rel(const node *o) const {
            return o->fa->ch[1] == o;
        }

        inline node *U(const node *o) const {
            if (o == root || o->fa == root)return nil;
            return o->fa->fa->ch[rel(o->fa) ^ 1];
        }

        inline node *B(const node *o) const {
            if (o == root)return nil;
            return o->fa->ch[rel(o) ^ 1];
        }

        void rotate(node *o) {// promise that o is not root
            bool r = rel(o);
            node *f = o->fa;

            o->fa = f->fa;
            /*ATTENTION*/
            if (f->fa != nil)f->fa->ch[rel(f)] = o;
            else root = o;

            /*ATTENTION*/
            if (o->ch[r ^ 1] != nil)o->ch[r ^ 1]->fa = f;
            f->ch[r] = o->ch[r ^ 1];

            f->fa = o, o->ch[r ^ 1] = f;

        }

        node *Min(node *o) const {
            while (o->ch[0] != nil)o = o->ch[0];
            return o;
        }

        node *Max(node *o) const {
            while (o->ch[1] != nil)o = o->ch[1];
            return o;
        }

        node *Pre(const node *o) const {
            if (o == nil)return nil;
            if (o->ch[0] != nil)return Max(o->ch[0]);
            while (o != root) {
                if (rel(o) == 1)return o->fa;
                o = o->fa;
            }
            return nil;
        }

        node *Sub(const node *o) const {
            if (o == nil)return nil;
            if (o->ch[1] != nil)return Min(o->ch[1]);
            while (o != root) {
                if (rel(o) == 0)return o->fa;
                o = o->fa;
            }
            return nil;
        }

    public:

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */

            friend MAP;
            MAP *id;
            node *ptr;
            // when ptr == nil , the iterator must be pointed to the end()

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
            using value_type = map::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;
            // If you are interested in type_traits, toy_traits_test provides a place to
            // practice. But the method used in that test is old and rarely used, so you may explore on your own.
            // Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
            // this part is only for bonus.

            typedef my_true_type iterator_assignable;

            iterator(MAP *_id = nullptr, node *_ptr = nullptr) : id(_id), ptr(_ptr) {
                // TODO
            }

            iterator(const iterator &other) : id(other.id), ptr(other.ptr) {
                // TODO
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator ret = *this;
                if (ptr == id->nil)throw invalid_iterator();
                ptr = id->Sub(ptr);
                return ret;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (ptr == id->nil)throw invalid_iterator();
                ptr = id->Sub(ptr);
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ret = *this;
                if (ptr == id->nil)ptr = id->Max(id->root);
                else ptr = id->Pre(ptr);
                if (ptr == id->nil)throw invalid_iterator();
                return ret;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (ptr == id->nil)ptr = id->Max(id->root);
                else ptr = id->Pre(ptr);
                if (ptr == id->nil)throw invalid_iterator();
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (ptr == id->nil)throw invalid_iterator();
                return *(ptr->data);
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                if (ptr == id->nil)throw invalid_iterator();
                return ptr->data;
            }
        };

        class const_iterator {
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            friend MAP;
            const MAP *id;
            const node *ptr;
        public:

            typedef my_false_type iterator_assignable;

            const_iterator(const MAP *_id = nullptr, const node *_ptr = nullptr) : id(_id), ptr(_ptr) {
            }

            const_iterator(
                    const const_iterator &other) : id(other.id), ptr(other.ptr) {
                // TODO
            }

            const_iterator(
                    const iterator &other) : id(other.id), ptr(other.ptr) {
                // TODO
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.

            const_iterator operator++(int) {
                const_iterator ret = *this;
                if (ptr == id->nil)throw invalid_iterator();
                ptr = id->Sub(ptr);
                return ret;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (ptr == id->nil)throw invalid_iterator();
                ptr = id->Sub(ptr);
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator ret = *this;
                if (ptr == id->nil)ptr = id->Max(id->root);
                else ptr = id->Pre(ptr);
                if (ptr == id->nil)throw invalid_iterator();
                return ret;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (ptr == id->nil)ptr = id->Max(id->root);
                else ptr = id->Pre(ptr);
                if (ptr == id->nil)throw invalid_iterator();
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            const value_type &operator*() const {
                if (ptr == id->nil)throw invalid_iterator();
                return *(ptr->data);
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type *operator->() const noexcept {
                if (ptr == id->nil)throw invalid_iterator();
                return ptr->data;
            }
        };

        /**
         * TODO two constructors
         */
        map() : _size(0) {
            // node may not have a default constructor
            nil = new node;
            nil->col = 1;
            nil->data = nullptr;
            nil->ch[0] = nil->ch[1] = nil->fa = nil;
            __begin = root = nil;
        }

        map(const map &other) : _size(other._size) {
            // build the external node "nil" first
            nil = new node;
            nil->col = 1;
            nil->data = nullptr;
            nil->ch[0] = nil->ch[1] = nil->fa = nil;
            root = nil;
            dfs_copy(root, other.root, nil, other.nil);
            __begin = Min(root);
        }

        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {
            if (this == &other)return *this;
            dfs_destroy(root);
            // nil already exist !!!
            _size = other._size;
            dfs_copy(root, other.root, nil, other.nil);
            __begin = Min(root);
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~map() {
            _size = 0;
            dfs_destroy(root);
            // remember delete the external node "nil"
            delete nil;
            nil = nullptr;
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, fan exception of type `index_out_o_bound'
         */
        T &at(const Key &key) {
            node *res = my_find(key);
            if (res == nil)throw index_out_of_bound();
            return res->data->second;
        }

        const T &at(const Key &key) const {
            node *res = my_find(key);
            if (res == nil)throw index_out_of_bound();
            return res->data->second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            node *res = my_find(key);
            if (res == nil) {
                insert(pair<Key, T>(key, T()));
                res = my_find(key);
            }
            return res->data->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            node *res = my_find(key);
            if (res == nil)throw index_out_of_bound();
            return res->data->second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, __begin);
        }

        const_iterator cbegin() const {
            return const_iterator(this, __begin);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, nil);
        }

        const_iterator cend() const {
            return const_iterator(this, nil);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return _size == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        void clear() {
            _size = 0;
            dfs_destroy(root);
            __begin = root = nil;
        }

    private:

        void insert_fixup(node *o) {
            while (o->fa->col == 0) {// o or o's father could be the root
                node *un = U(o), *f = o->fa, *pa = o->fa->fa;
                if (un->col == 0) {
                    f->col = un->col = 1;
                    pa->col = 0;
                    o = pa;
                    continue;
                }
                if (rel(o) == rel(f)) {
                    rotate(f);
                    f->col = 1;
                    o->col = pa->col = 0;
                } else {
                    rotate(o), rotate(o);
                    o->col = 1;
                    f->col = pa->col = 0;
                }
                break;
            }
            root->col = 1;
        }

    public:
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node *o = root, *f = nil;
            while (o != nil && (cmp(o->data->first, value.first) || cmp(value.first, o->data->first))) {
                f = o;
                if (cmp(value.first, o->data->first))o = o->ch[0];
                else o = o->ch[1];
            };
            if (o != nil)return pair<iterator, bool>(iterator(this, o), false);
            ++_size;

            o = new node(value, 0);
            o->fa = f, o->ch[0] = o->ch[1] = nil;
            if (o->fa == nil) root = o;
            else o->fa->ch[cmp(o->fa->data->first, value.first)] = o;
            insert_fixup(o);

            __begin = Min(root);
            return pair<iterator, bool>(iterator(this, o), true);
        }

    private:

        void transplant(node *x, node *y) {
            if (x->fa == nil)root = y;
            else x->fa->ch[rel(x)] = y;
            y->fa = x->fa;
        }

        void erase_fixup(node *o) {
            while (o != root && o->col == 1) {
                bool r = rel(o);
                if (B(o)->col == 0) {
                    rotate(B(o));
                    std::swap(o->fa->col, o->fa->fa->col);
                } else if (B(o)->ch[0]->col == 1 && B(o)->ch[1]->col == 1) {
                    B(o)->col = 0;
                    o = o->fa;
                } else if (B(o)->ch[r]->col == 0 && B(o)->ch[r ^ 1]->col == 1) {
                    node *son = B(o)->ch[r];
                    rotate(son);
                    std::swap(son->col, son->ch[r ^ 1]->col);
                } else {
                    rotate(B(o));
                    std::swap(o->fa->col, o->fa->fa->col);
                    o->fa->fa->ch[r ^ 1]->col = 1;
                    break;
                }
            }
            o->col = 1;
        }

    private:
        void node_swap(node *x, node *y) {
            int rx = rel(x), ry = rel(y);
            if (y->fa != nil)y->fa->ch[ry] = x;
            else root = x;
            if (x->fa != nil)x->fa->ch[rx] = y;
            else root = y;
            std::swap(x->fa, y->fa);

            std::swap(x->ch[0], y->ch[0]);
            if (x->ch[0] != nil)x->ch[0]->fa = x;
            if (y->ch[0] != nil)y->ch[0]->fa = y;

            std::swap(x->ch[1], y->ch[1]);
            if (x->ch[1] != nil)x->ch[1]->fa = x;
            if (y->ch[1] != nil)y->ch[1]->fa = y;

            std::swap(x->col, y->col);
        }

    public:
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.id != this || pos.ptr == nil)throw invalid_iterator();
            bool init_col;
            node *o = pos.ptr, *o1;
            if (o->ch[0] == nil)o1 = o->ch[1];
            else if (o->ch[1] == nil)o1 = o->ch[0];
            else {
                node *suf = Min(o->ch[1]);

                /*std::swap(o->data, suf->data);

                o = suf;
                o1 = o->ch[1];*/

                node_swap(o, suf);
                o1 = o->ch[1];
            }
            init_col = o->col;
            transplant(o, o1);
            delete o->data;
            delete o;
            --_size;

            if (init_col == 1)erase_fixup(o1);
            __begin = Min(root);
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        bool count(const Key &key) const {
            return my_find(key) != nil;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            return iterator(this, my_find(key));
        }

        const_iterator find(const Key &key) const {
            return const_iterator(this, my_find(key));
        }

    private:
        int max_dep;

        void dfs(node *o, int sum, int dep) {
            if (o->col == 0 && (o->ch[0]->col == 0 || o->ch[1]->col == 0))std::cerr << "WARNING" << std::endl;
            if (o == nil)return;
            max_dep = std::max(max_dep, dep);
            dfs(o->ch[0], sum + o->col, dep + 1), dfs(o->ch[1], sum + o->col, dep + 1);
        }

    public:
        void test() {
            max_dep = 0;
            dfs(root, 0, 0);
            std::cerr << max_dep << std::endl;
        }
    };

}

#endif