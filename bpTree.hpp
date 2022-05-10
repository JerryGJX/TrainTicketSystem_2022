#ifndef BPTREE_HPP
#define BPTREE_HPP
#include "memory_manager.hpp"
#include <bits/stdc++.h> // should be deleted later
#include <iostream>
using namespace std; // should be deleted later

template <class Key, class Value, class Compare = std::less<Key>, int M = 10, int L = 10>
class Bptree{
private:
   int root, normal_node_number, leaf_node_number;
   std::string prefix_name;
   class Bptree_leaf_node;
   class Bptree_normal_node;
   MemoryRiver<Bptree_leaf_node, 1> leaf_node_manager;     // info：leaf_node个数
   MemoryRiver<Bptree_normal_node, 2> normal_node_manager; // info：root的编号，normal_node个数
   
   //一些辅助函数
   //从文件中读取根节点
   void get_root() {
      normal_node_manager.get_info(root, 1);
   }
   //将根节点写入文件
   void write_root(int root) {
      normal_node_manager.write_info(root, 1); 
   }
   //从文件中读取normal_node个数
   void get_normal_node_number() {
      normal_node_manager.get_info(normal_node_number, 2);
   }
   //将normal_node个数写入文件
   void write_normal_node_number(int number) {
      normal_node_manager.write_info(normal_node_number, 2);
   }
   //从文件中读取leaf_node个数
   void get_leaf_node_number() {
      leaf_node_manager.get_info(leaf_node_number, 1);
   }
   //将leaf_node个数写入文件
   void write_leaf_node_number(int number) {
      leaf_node_manager.write_info(leaf_node_number, 1);
   }

   // 一般节点类
   class Bptree_normal_node {
   public:
      friend Bptree;
   private:
      int size;
      bool is_lowest;
      int children[M + 1]; //0-base，多出一个用做拆分时的临时节点
      Key key_min, key_list[M];
    
   public:
      //构造函数
      Bptree_normal_node() {
         size = 0;
         is_lowest = 1;
         for (int i = 0; i <= M; ++i) children[i] = -1;
      }
      //拷贝构造
      Bptree_normal_node(const Bptree_normal_node &obj)  {
         size = obj.size;
         key_min = obj.key_min;
         is_lowest = obj.is_lowest;
         for (int i = 0; i <= M; ++i) {
            children[i] = obj.children[i];
         }
         for (int i = 0; i < M; ++i) {
            key_list[i] = obj.key_list[i];
         }
      }
      //析构函数
      ~Bptree_normal_node() {}
      //用节点的儿子节点更新该点的key_min
      void pushup() {
         if (!is_lowest) {
            Bptree_normal_node son;
            normal_node_manager.read(son, children[0]);
            key_min = son.key_min;
         } else {
            Bptree_leaf_node son;
            leaf_node_manager.read(son, children[0]);
            key_min = son.key_list[0];
         }
      }
      //超过数量限制时拆分节点
      std::pair<Bptree_normal_node, Bptree_normal_node> split(const Bptree_normal_node &obj) const {
         Bptree_normal_node x, y;
         x.size = (obj.size + 1) / 2;
         y.size = obj.size / 2;
         x.lowest = y.lowest = obj.is_lowest;
         for (int i = 0; i < x.size; ++i) {
            x.children[i] = obj.children[i];
         }
         for (int i = 0; i < y.size; ++i) {
            y.children[i] = obj.children[x.size + i];
         }
         for (int i = 0; i + 1 < x.size; ++i) {
            x.key[i] = obj.key[i];
         }
         for (int i = 0; i + 1 < y.size; ++i) {
            y.key[i] = obj.key[x.size + i];
         }
         x.key_min = obj.key_min;
         y.pushup();
         return std::make_pair(x, y);
      }
      //将两个低于数量限制的节点合并
      //node1应该是node2相邻的左兄弟
      Bptree_normal_node merge(const Bptree_normal_node &node1, const Bptree_normal_node &node2) const {
         if (node1.size == 0) {
            return node2;
         } else if (node2.size == 0) {
            return node1;
         }
         Bptree_normal_node merge_node(node1);
         merge_node.size = node1.size + node2.size;
         for (int i = 0; i < node2.size; ++i) {
            merge_node.children[i + node1.size] = node2.children[i];
         }
         merge_node.key_list[node1.size - 1] = node2.key_min;
         for (int i = 0; i + 1 < node2.size; ++i) {
            merge_node.children[i + node1.size] = node2.key_list[i];
         }
         return merge_node;
      }
   };
         
   //叶子节点类
   class Bptree_leaf_node {
   public:
      friend Bptree;
   private:
      int size;
      int predecessor, succssor;
      Key key_list[L + 1];
      Value value_list[L + 1];
  
   public:  
      //构造函数
      Bptree_leaf_node() {
         size = 0;
         predecessor = succssor = -1;
      }
      Bptree_leaf_node(const Bptree_leaf_node &obj) {
         size = obj.size;
         predecessor = obj.predecessor;
         succssor = obj.succssor;
         for (int i = 0; i <= L; ++i) {
            key_list[i] = obj.key_list[i];
            value_list[i] = obj.value_list[i];
         }
      }
      //析构函数
      ~Bptree_leaf_node() {}
      //超过数量限制时拆分节点
      //index为obj在文件中的位置，拆分后的节点应被写入index和new_index
      std::pair<Bptree_leaf_node, Bptree_leaf_node> split(const Bptree_leaf_node &obj, const int &index, int &new_index) const {
         Bptree_leaf_node x, y;
         new_index = leaf_node_manager.write(y);
         x.size = (obj.size + 1) / 2;
         y.size = obj.size / 2;
         x.predecessor = obj.predecessor;
         x.succssor = new_index;
         y.predecessor = index;
         y.succssor = obj.succssor;
         for (int i = 0; i < x.size; ++i) {
            x.key_list[i] = obj.key_list[i];
            x.value_list[i] = obj.value_list[i];
         }
         for (int i = 0; i < y.size; ++i) {
            y.key_list[i] = obj.key_list[i + x.size];
            y.value_list[i] = obj.value_list[i + x.size];
         }
         return std::make_pair(x, y);
      }
      //将两个低于数量限制的节点合并
      //node1应该是node2相邻的左兄弟
      Bptree_leaf_node merge(const Bptree_leaf_node &node1, const Bptree_leaf_node &node2) const {
         if (node1.size == 0) {
            return node2;
         } else if (node2.size == 0) {
            return node1;
         }
         Bptree_leaf_node merge_node(node1);
         merge_node.size = node1.size + node2.size;
         merge_node.predecessor = node1.predecessor;
         merge_node.succssor = node2.succssor;
         for (int i = 0; i < node2.size; ++i) {
            merge_node.key_list[i + node1.size] = node2.key_list[i];
            merge_node.value_list[i + node1.size] = node2.value_list[i];
         }
         return merge_node;
      }
      //查找前驱 
      Bptree_leaf_node find_predecessor(const Bptree_leaf_node &obj) const {
         assert(obj.predecessor != -1);
         Bptree_leaf_node pre;
         leaf_node_manager.read(pre, obj.successor);
         return pre;
      };
      //查找后继
      Bptree_leaf_node find_successor(const Bptree_leaf_node &obj) const {
         assert(obj.succssor != -1);
         Bptree_leaf_node suc;
         leaf_node_manager.read(suc, obj.succssor);
         return suc;
      };
   };

   //BpTree正文
   //构造函数
   Bptree(const std::string &_prefix_name) : prefix_name(_prefix_name) {
      leaf_node_manager.initialise(prefix_name + "leaf_node");
      normal_node_manager.initialise(prefix_name + "normal_node");
      get_root();
      get_normal_node_number();
      if (normal_node_number == 0) { //如果是空树创建根节点
         Bptree_normal_node tmp;
         root = normal_node_manager.write(tmp);
         ++normal_node_number;
      }
   }
   //析构函数
   ~Bptree() {
      write_root();
      write_normal_node_number();
   }
   //查看是否有插入指定key值的元素
   bool find(const Key &data, Value &result) const {
      return dfs_find(root, 0, data, result) != -1;
   }
   //find的辅助函数，如果找到返回叶节点的index，否则返回-1
   int dfs_find(int pos, bool is_leaf, const Key &data, Value &result) const {
      if (is_leaf) {
         Bptree_leaf_node node;
         leaf_node_manager.read(node, pos);
         for (int i = 0; i < L && node.key_list[i] <= data; ++i) {
            if (node.key_list[i] == data) {
               result = node.value_list[i];
               return pos;
            }
         }
         return -1;
      } else {
         Bptree_normal_node node;
         normal_node_manager.read(node, pos);
         if (node.size == 0) return -1; // BpTree为空树
         for (int i = 0; i < node.size; ++i) {
            if (data < node.key_list[i]) {
               return dfs_find(node.children[i], node.is_lowest, data, result);
            }
         }
         return dfs_find(node.children[node.size - 1], node.is_lowest, data, result);
      }
   }
   //插入节点，失败返回0（如果给定key值已有节点，也会返回0）
   bool insert(const Key &key, const Value &value) {
      Bptree_normal_node virtual_root;
      return dfs_insert(root, 0, 0, virtual_root, key, key, value);
   }
   //另一个版本的insert
   bool insert(const std::pair<Key, Value> &data) {
      return insert(data.first, data.second);
   }
   //insert的辅助函数
   bool dfs_insert(int pos, bool is_leaf, int rank, Bptree_normal_node &parent, Key &key, Value &value) {
      if (is_leaf) {
         Bptree_leaf_node self;
         
      }
   }
   //删除节点，失败返回0
   bool erase(const Key &key) {
      Bptree_normal_node virtual_root;
      return dfs_erase(root, 0, 0, virtual_root, key);
   }
   //erase的辅助函数
   bool dfs_erase(int pos, bool is_leaf, int rank, Bptree_normal_node &parent, Key &key) {
      if (is_leaf) {
         Bptree_leaf_node self;
         leaf_node_manager(self, pos);
         bool flag = 0;
         for (int i = 0; i < self.size; ++i) {
            if (self.key_list[i] == key) {
               flag = 1;
               --self.size;
               for (int j = i; j + 1 < self.size; ++j) {
                  self.key_list[j] = self.key_list[j + 1];
                  self.value_list[j] = self.value_list[j + 1];
               }
               break;
            }
         }
         if (flag && self.size < (L + 1) / 2) {
            if (self.predecessor == -1 && self.succssor == -1) { //根节点直接挂下来的叶子节点
               if (self.size == 0) { //整棵树删空
                  parent.size = 0;
                  parent.children[0] = -1;
               }
            } else {
               Bptree_leaf_node brother;
               if (rank + 1 != parent.size) { //向后一个兄弟合并
                  leaf_node_manager.read(brother, parent.children[rank + 1]);
                  maintain_size_decrease_leaf(rank, self, brother, parent);
               } else {
                  leaf_node_manager.read(brother, parent.children[rank - 1]);
                  maintain_size_decrease_leaf(rank - 1, brother, self, parent);
               }
            }
         }
         return flag;
      } else {
         Bptree_normal_node self;
         normal_node_manager.read(pos, self);
         if (self.size == 0) return 0;
         bool found = 0;
         for (int i = 0; i < self.size; ++i) {
            if (i + 1 == self.size || key < self.key_list[i]) {
               found = dfs_erase(self.children[i], self.is_lowest, i, self, key);
               break;
            }
         }
         if (found) {
            if (pos == root && !self.is_lowest && self.size == 1) { //删根
               root = self.children[0];
               normal_node_manager.Delete(pos);
            } else if (self.size >= (M + 1) / 2 || pos == root) {
               normal_node_manager.update(pos, self);
            } else {
               Bptree_normal_node brother;
               if (rank + 1 != parent.size) {
                  normal_node_manager.read(brother, parent.children[rank + 1]);
                  maintain_size_decrease_normal(rank, self, brother, parent);
               } else {
                  normal_node_manager.read(brother, parent.children[rank - 1]);
                  maintain_size_decrease_normal(rank - 1, brother, self, parent);
               }
            }
         }
         return found;
      }
   }
   //在叶节点删除某个值后，在node1和node2之间调整使他们满足Bptree对size的要求，返回是否进行了merge操作
   bool maintain_size_decrease_leaf(int rank, Bptree_leaf_node &node1, Bptree_leaf_node &node2, Bptree_normal_node &parent) {
      int id1 = parent.children[rank], id2 = parent.children[rank + 1];
      if (node1.size == (L + 1) / 2 || node2.size == (L + 1) / 2) {
         node1 = Bptree_leaf_node::merge(node1, node2);
         leaf_node_manager.update(id1, node1);
         leaf_node_manager.Delete(id2);
         --parent.size;
         for (int i = rank + 1; i < parent.size; ++i) {
            parent.children[i] = parent.children[i + 1];
         }
         return 1;
      } else if (node1.size < (L + 1) / 2) {
         node1.key_list[node1.size] = node2.key_list[0];
         node2.value_list[node1.size] = node2.value_list[0];
         ++node1.size; --node2.size;
         for (int i = 0; i < node2.size; ++i) {
            node2.key_list[i] = node2.key_list[i + 1];
            node2.value_list[i] = node2.value_list[i + 1];
         }
         parent.key_list[rank] = node2.key_list[0];
         leaf_node_manager.update(id1, node1);
         leaf_node_manager.update(id2, node2);
         return 0;
      } else {
         for (int i = node2.size; i > 0; --i) {
            node2.key_list[i] = node2.key_list[i - 1];
            node2.value_list[i] = node2.value_list[i - 1];
         }
         --node1.size; ++node2.size;
         node2.key_list[0] = node1.key_list[node1.size];
         node2.value_list[0] = node1.value_list[node1.size];
         parent.key_list[rank] = node2.key_list[0];
         leaf_node_manager.update(id1, node1);
         leaf_node_manager.update(id2, node2);
         return 0;
      }
   }
   //在普通节点删除某个值后，在node1和node2之间调整使他们满足Bptree对size的要求，返回是否进行了merge操作
   bool maintain_size_decrease_normal(int rank, Bptree_normal_node &node1, Bptree_normal_node &node2, Bptree_normal_node &parent) {
      int id1 = parent.children[rank], id2 = parent.children[rank + 1];
      if (node1.size == (M + 1) / 2 || node2.size == (M + 1) / 2) {
         node1 = Bptree_normal_node::merge(node1, node2);
         normal_node_manager.update(id1, node1);
         normal_node_manager.Delete(id2);
         --parent.size;
         for (int i = rank + 1; i < parent.size; ++i) {
            parent.children[i] = parent.children[i + 1];
         }
         return 1;
      } else if (node1.size < (M + 1) / 2) {
         node1.children[node1.size] = node2.children[0];
         node1.key_list[node1.size - 1] = node2.key_min;
         node2.key_min = node2.key_list[0]; 
         ++node1.size; --node2.size;
         for (int i = 0; i < node2.size; ++i) {
            node2.children[i] = node2.children[i + 1];
            if (i + 1 != node2.size) {
               node2.key_list[i] = node2.key_list[i + 1];
            }
         }
         parent.key_list[rank] = node2.key_list[0];
         normal_node_manager.update(id1, node1);
         normal_node_manager.update(id2, node2);
         return 0;
      } else {
         for (int i = node2.size; i > 0; --i) {
            node2.children[i] = node2.children[i - 1];
            if (i != node2.size) {
               node2.key_list[i] = node2.key_list[i - 1];
            }
         }
         --node1.size; ++node2.size; 
         node2.children[0] = node1.children[node1.size];
         node2.key_list[0] = node2.key_min;
         node2.key_min = node1.key_list[node1.size - 1];
         parent.key_list[rank] = node2.key_list[0];
         normal_node_manager.update(id1, node1);
         normal_node_manager.update(id2, node2);
         return 0;
      }
   }
   //修改指定key值的元素，如果不存在返回0
   bool modify(const Key &key, const Value &value) {
      return 1;
   }
   //另一个版本的modify
   bool modify(const std::pair<Key, Value> &data) {
      return modify(data.first, data.second);
   }
   //查找大于等于key_search的第一个值，不存在返回NULL
   Value lower_bound(const Key &key_search) const {
      
   }
   //查找大于key_search的第一个值，不存在返回NULL
   Value upper_bound(const Key &key_search) const {
      
   }
   //查找Key值在[key_l, key_r)中间的值,按key升序放入rusult
   void range_search(const Key &key_l, const Key &key_r, const std::vector<std::pair<Key, Value>> &result) const {
      
   }
};
#endif
