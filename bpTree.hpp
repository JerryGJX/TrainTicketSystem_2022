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
         memset(children, 0, sizeof children);
         memset(key_list, 0, sizeof key_list);
      }
      //拷贝构造
      Bptree_normal_node(const Bptree_normal_node &obj)  {
         size = obj.size;
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
      //用节点的儿子节点更新该点的
      void pushup() {
         Bptree_normal_node son;
         normal_node_manager.
      }
      //超过数量限制时拆分节点
      std::pair<Bptree_normal_node, Bptree_normal_node> split() {
         Bptree_normal_node x(*this), y;
         x.size = (size + 1) / 2;
         y.size = size / 2;
         x.lowest = y.lowest = this->is_lowest;
         for (int i = 0; i < x.size; ++i) {
            x.children[i] = this->children[i];
         }
         for (int i = 0; i < y.size; ++i) {
            y.children[i] = this->children[x.size + i];
         }
         for (int i = 0; i + 1 < x.size; ++i) {
            x.key[i] = this->key[i];
         }
         for (int i = 0; i + 1 < y.size; ++i) {
            y.key[i] = this->key[x.size + i];
         }
         return std::make_pair(x, y);
      }
      //将两个低于数量限制的节点合并
      Bptree_normal_node merge(Bptree_normal_node &node1, Bptree_normal_node &node2) {
         if (node1.size == 0) {
            return node2;
         } else if (node2.size == 0) {
            return node1;
         }
         Bptree_normal_node merge_node(node1);
         for (int i = 0; i < node2.size; ++i) {
            merge_node.children[i + node1.size] = node2.children[i];
         }
//         merge_node.key[node1.size - 1] = 
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
      }
      Bptree_leaf_node(Bptree_leaf_node &obj) {}
      //析构函数
      ~Bptree_leaf_node() {}
      //超过数量限制时拆分节点
      std::pair<Bptree_leaf_node, Bptree_leaf_node> split();
      //将两个低于数量限制的节点合并
      Bptree_leaf_node merge(Bptree_leaf_node &node1, Bptree_leaf_node &node2);
      //查找前驱 
      Bptree_leaf_node* find_predecessor() const {};
      //查找后继
      Bptree_leaf_node* find_successor() const {};
   };

   //BpTree正文
   //构造函数
   Bptree(const std::string &_prefix_name) : prefix_name(_prefix_name) {
      leaf_node_manager.initialise(prefix_name + "leaf_node");
      normal_node_manager.initialise(prefix_name + "normal_node");
      
      // todo
   }
   //析构函数
   ~Bptree() {
      
   }
   //查看是否有插入指定key值的元素
   bool find(Key data, Value &result) {
      return 1;
   }
   //插入节点，失败返回0
   bool insert(const std::pair<Key, Value> &data) {
      int now = root;
      return 1;
   }
   //删除节点，失败返回0
   bool erase(const Key &data) {
      return 1;
   }
   //修改指定key值的元素，如果不存在返回0
   bool modify(const std::pair<Key, Value> &data) {
      return 1;
   }
   //查找大于等于key_search的第一个值，不存在返回NULL
   Value lower_bound(const Key &key_search) const {
      
   }
   //查找大于key_search的第一个值，不存在返回NULL
   Value upper_bound(const Key &key_search) const {
      
   }
   //查找Key值在[key_l, key_r)中间的值
   void range_search(const Key &key_l, const Key &key_r,  const std::vector<Value> &result) const {
      
   }
};
#endif
