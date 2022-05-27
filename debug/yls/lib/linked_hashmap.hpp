/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKED_HASHMAP_HPP
#define SJTU_LINKED_HASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    /**
     * In linked_hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted into the map.
     */

    template<
            class Key,
            class T,
            class Hash = std::hash<Key>,
            class Equal = std::equal_to<Key>
    >
    class linked_hashmap {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::linked_hashmap as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:

        using HM = linked_hashmap<Key, T, Hash, Equal>;

        Hash hash_fun;
        Equal is_equal;

        const size_t init_index_size = 8;
        const double double_factor = 0.75;
        const double half_factor = 0.25;
        size_t index_size, element_size, mod;
        // mod must be a prime

        struct node {
            node *up, *down, *left, *right;
            value_type *data;

            node() {
                up = down = left = right = nullptr;
                data = nullptr;
            }

            ~node() {
                if (data != nullptr)delete data;
                data = nullptr;
            }
        } *index, *nil;

        bool is_prime(const size_t &x) const {
            for (long long i = 2; i * i <= x; ++i)
                if (x % i == 0)return false;
            return true;
        }

        void update_prime() {
            for (mod = index_size - 1; !is_prime(mod); --mod);
        }

        /// just insert the element(already in the chain) into the hash_table
        void my_insert(node *now) {
            size_t ind = hash_fun(now->data->first) % mod;
            node *ptr = &index[ind], *nex = ptr->down;
            ptr->down = now, now->up = ptr;
            now->down = nex;
            if (nex != nullptr)nex->up = now;
        }

        /// insert a new element into the hash_table
        void insert_into_table(value_type ele) {
            node *nptr = new node;
            nptr->data = new value_type(ele);

            node *pre = nil->left;
            pre->right = nptr, nptr->left = pre;
            nptr->right = nil, nil->left = nptr;
            my_insert(nptr);

            ++element_size;
            re_allocate();
        }

        void re_allocate() {
            if (element_size < index_size * half_factor && index_size > init_index_size) {
                // do half space
                delete[]index;
                index = new node[index_size >>= 1];
                update_prime();

                for (node *ptr = nil->right; ptr != nil; ptr = ptr->right)
                    my_insert(ptr);

            } else if (element_size > index_size * double_factor) {
                // do double space
                delete[]index;
                index = new node[index_size <<= 1];
                update_prime();

                for (node *ptr = nil->right; ptr != nil; ptr = ptr->right)
                    my_insert(ptr);
            }
        }

        void my_deconstructor() {
            for (node *ptr = nil->right; ptr != nil;) {
                node *nex = ptr->right;
                delete ptr;
                ptr = nex;
            }
            delete nil;
            delete[]index;
            nil = index = nullptr;
            index_size = element_size = mod = 0;
        }

        void my_copy(const linked_hashmap &other) {
            index_size = other.index_size;
            element_size = other.element_size;
            mod = other.mod;
            index = new node[index_size];
            nil = new node;
            nil->left = nil->right = nil;

            node *ptr1 = nil, *ptr2 = other.nil;
            while (ptr2->right != other.nil) {
                ptr1->right = new node;
                ptr1->right->left = ptr1;
                ptr1->right->data = new value_type(*(ptr2->right->data));
                ptr1 = ptr1->right;
                ptr2 = ptr2->right;
            }
            ptr1->right = nil;
            nil->left = ptr1;

            for (node *ptr = nil->right; ptr != nil; ptr = ptr->right)
                my_insert(ptr);
        }

        node *my_find(const Key &key) const {
            size_t ind = hash_fun(key) % mod;
            node *ptr = index[ind].down;
            while (ptr != nullptr && !is_equal(ptr->data->first, key))
                ptr = ptr->down;
            return ptr == nullptr ? nil : ptr;
        }

    public:

        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = linked_hashmap.begin(); --it;
         *       or it = linked_hashmap.end(); ++end();
         */
        class const_iterator;

        class iterator {
        private:
            friend HM;
            HM *bel;
            node *ptr;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
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
            using value_type = linked_hashmap::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;


            iterator(HM *_bel = nullptr, node *_ptr = nullptr) : bel(_bel), ptr(_ptr) {
                // TODO
            }

            iterator(const iterator &other) : bel(other.bel), ptr(other.ptr) {
                // TODO
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                if (ptr == bel->nil)throw invalid_iterator();
                iterator ret = *this;
                ptr = ptr->right;
                return ret;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (ptr == bel->nil)throw invalid_iterator();
                ptr = ptr->right;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (ptr->left == bel->nil)throw invalid_iterator();
                iterator ret = *this;
                ptr = ptr->left;
                return ret;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (ptr->left == bel->nil)throw invalid_iterator();
                ptr = ptr->left;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (ptr == bel->nil)throw invalid_iterator();
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
                return ptr->data;
            }
        };

        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            friend HM;
            const HM *bel;
            const node *ptr;
            // data members.
        public:

            using difference_type = std::ptrdiff_t;
            using value_type = linked_hashmap::value_type;
            using pointer = value_type *;
            using reference = value_type &;
            using iterator_category = std::output_iterator_tag;

            const_iterator(const HM *_bel = nullptr, const node *_ptr = nullptr) : bel(_bel), ptr(_ptr) {
                // TODO
            }

            const_iterator(const const_iterator &other) : bel(other.bel), ptr(other.ptr) {
                // TODO
            }

            const_iterator(const iterator &other) : bel(other.bel), ptr(other.ptr) {
                // TODO
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                if (ptr == bel->nil)throw invalid_iterator();
                const_iterator ret = *this;
                ptr = ptr->right;
                return ret;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (ptr == bel->nil)throw invalid_iterator();
                ptr = ptr->right;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                if (ptr->left == bel->nil)throw invalid_iterator();
                const_iterator ret = *this;
                ptr = ptr->left;
                return ret;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (ptr->left == bel->nil)throw invalid_iterator();
                ptr = ptr->left;
                return *this;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                if (ptr == bel->nil)throw invalid_iterator();
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
                return ptr->data;
            }
        };

        /**
         * TODO two constructors
         */
        linked_hashmap() {
            index_size = init_index_size;
            element_size = 0;
            update_prime();
            index = new node[init_index_size];
            nil = new node;
            nil->left = nil->right = nil;
        }

        linked_hashmap(const linked_hashmap &other) {
            my_copy(other);
        }

        /**
         * TODO assignment operator
         */
        linked_hashmap &operator=(const linked_hashmap &other) {
            if (this == &other)return *this;
            my_deconstructor();
            my_copy(other);
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~linked_hashmap() {
            my_deconstructor();
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            node *ptr = my_find(key);
            if (ptr == nil)throw index_out_of_bound();
            return ptr->data->second;
        }

        const T &at(const Key &key) const {
            node *ptr = my_find(key);
            if (ptr == nil)throw index_out_of_bound();
            return ptr->data->second;
        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            node *ptr = my_find(key);
            if (ptr == nil)insert_into_table(value_type(key, T()));
            return my_find(key)->data->second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            node *ptr = my_find(key);
            if (ptr == nil)throw index_out_of_bound();
            return ptr->data->second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            return iterator(this, nil->right);
        }

        const_iterator cbegin() const {
            return const_iterator(this, nil->right);
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
            return element_size == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return element_size;
        }

        /**
         * clears the contents
         */
        void clear() {
            my_deconstructor();
            index_size = init_index_size;
            element_size = 0;
            update_prime();
            index = new node[init_index_size];
            nil = new node;
            nil->left = nil->right = nil;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node *res = my_find(value.first);

            if (res != nil)return {iterator(this, res), false};

            insert_into_table(value);

            res = my_find(value.first);
            return {iterator(this, res), true};
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if (pos.ptr == nullptr)throw invalid_iterator();
            if (pos.bel != this)throw invalid_iterator();
            if (pos.ptr == nil)throw invalid_iterator();

            node *pre = pos.ptr->left, *nex = pos.ptr->right;
            pre->right = nex, nex->left = pre;

            pre = pos.ptr->up, nex = pos.ptr->down;
            pre->down = nex;
            if (nex != nullptr)nex->up = pre;

            delete pos.ptr;
            --element_size;
            re_allocate();
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         */
        size_t count(const Key &key) const {
            const node *ptr = my_find(key);
            return ptr != nil;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            node *ptr = my_find(key);
            return iterator(this, ptr);
        }

        const_iterator find(const Key &key) const {
            node *ptr = my_find(key);
            return const_iterator(this, ptr);
        }
    };

}

#endif
