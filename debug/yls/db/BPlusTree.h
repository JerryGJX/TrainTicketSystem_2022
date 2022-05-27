//
// Created by lu'ren'jia'd's on 2022/5/2.
//

#ifndef UNTITLED2_BPLUSTREE_H
#define UNTITLED2_BPLUSTREE_H

#include "MemoryRiver.h"
#include "FileException.h"
#include <functional>
#include <cstring>

namespace ds {
    template<typename Key, typename Info, const int max_key_num = 5, const int max_rcd_num = 5, class KeyCompare = std::less<Key>>

    class BPlusTree {
        //以下两个值暂定，可能以后根据实际情况更改
    public:
        void GetSizeInfo() {
            std::cerr << "The size of Key is " << sizeof(Key) << std::endl;
            std::cerr << "The size of Info is " << sizeof(Info) << std::endl;
            std::cerr << "================================" << std::endl;
        }

        struct Node {
            int nxt = -1, before = -1;// B+树叶子节点构成的的链表
            bool isleaf = true;//标记是不是叶节点
            int children_num = 0;
            int children[max_key_num];
            Key keys[max_key_num];
            int location = -1;

            void operator()(Node lhs, const Node &rhs) {
                lhs.location = rhs.location;
                lhs.isleaf = rhs.isleaf;
                lhs.children_num = rhs.children_num;
                lhs.nxt = rhs.nxt;
                lhs.before = rhs.before;
                for (int i = 0; i < rhs.children_num; ++i)lhs.children[i] = rhs.children[i];
                for (int i = 0; i < rhs.children_num; ++i)lhs.keys[i] = rhs.keys[i];
            }
        };


        struct Block {
            int size = 0;
            Key keys[max_rcd_num];
            Info record[max_rcd_num];
        };

    private:
        KeyCompare cmp;
        MemoryRiver<Node> *index_memory;
        MemoryRiver<Block> *record_memory;
        MemoryRiver<int> *int_memory;
        char *index_file;
        char *record_file;
        Node root;
        int root_index = -1;
        int scale;

        //this is a tool function to do binary search
        //find the first key that is smaller than or equal to the wanted
        //if every key is bigger than this one return -1;
//        void PeekNode(const Node &node) {
//            std::cout << "node location ";
//            std::cout << node.location << std::endl;
//            for (int i = 0; i < node.children_num; ++i) std::cout << node.keys << ' ';
//            std::cout << std::endl;
//        }


        int BinarySearchLess(Key array[], int size, const Key &key) {
            if (size <= 0)return -1;
            if (cmp.operator()(key, array[0]))return -1;
            //find the answer from 0 to size-1
            int l = 0, r = size - 1;
            int mid;
            while (l < r) {
                mid = (l + r + 1) >> 1;
                if (cmp.operator()(array[mid], key)) {
                    //this index may become the answer
                    //but the boundary can be more left
                    l = mid;
                } else if (cmp.operator()(key, array[mid])) r = mid - 1;
                else return mid;
            }
            return l;
        }

        //find the index of the key in the block
        // if don't find return -1;
        int BinarySearch(const Block &block, const Key &key) {
            int l = 0, r = block.size - 1, mid;
            while (l < r) {
                mid = (l + r) >> 1;
                if (cmp.operator()(block.keys[mid], key))l = mid + 1;
                else if (cmp.operator()(key, block.keys[mid]))r = mid - 1;
                else return mid;
            }
            if (l == r && (!cmp.operator()(block.keys[l], key) && !cmp.operator()(key, block.keys[l])))return l;
            return -1;
        }

        //return the index in the record file and the info struct
        // the first int is the index of the block and the second int is the index of the record in the block;
        //the index can be used to modify the struct infomation
        std::pair<std::pair<int, int>, Info> RecursionFind(const Key &key) {
            if (scale == 0) {
                Info tmp;
                return {{-1, -1}, tmp};
            }
            Node cur = root;
            while (!cur.isleaf) {
                int num = BinarySearchLess(cur.keys, cur.children_num, key);
                if (num == -1)num = 0;
                //num is the first element that is less than or equal to the key
                //so we need go to the children the key stand for (num)
//                !index_memory->Read(cur.children[index + 1], cur);
                if (!index_memory->Read(cur.children[num], cur)) {
                    ds::ReadException e;
                    throw e;
                }
            }
            //when the cur is leaf node come out of the circle
            //then do binary search on that leaf node to fina the key
            int num = BinarySearchLess(cur.keys, cur.children_num, key);
            if (num == -1)num = 0;
            Block block;
            if (!record_memory->Read(cur.children[num], block)) {
                ds::ReadException e;
                throw e;
            }
            int key_index = BinarySearch(block, key);
            Info tmp;
            if (key_index == -1)return {{-1, -1}, tmp};
            else return {{cur.children[num], key_index}, block.record[key_index]};
        }

        //split the cur into new node and the info about new node are recorded by reference
        void Split(Node &cur, Node &new_node, int &new_node_loc, int &splited_child_index, int num) {
            for (int i = max_key_num / 2, j = 0; i < cur.children_num; ++i, ++j) {
                new_node.keys[j] = cur.keys[i];
                new_node.children[j] = cur.children[i];
            }
            new_node.children_num = max_key_num - max_key_num / 2;
            cur.children_num = max_key_num / 2;
            new_node_loc = new_node.location = index_memory->FindEnd();
            new_node.before = cur.location;
            new_node.nxt = cur.nxt;
            new_node.isleaf = cur.isleaf;//after split they are in the same level;
            if (cur.nxt != -1) {
                Node next_node;
                if (!index_memory->Read(cur.nxt, next_node)) {
                    ds::ReadException e;
                    throw e;
                }
                next_node.before = new_node_loc;
                if (!index_memory->Write(cur.nxt, next_node)) {
                    ds::WriteException e;
                    throw e;
                }
            }
            cur.nxt = new_node.location;
            index_memory->Append(new_node);
            splited_child_index = num;//the info about new node are recorded by reference
            if (!index_memory->Write(cur.location, cur)) {
                ds::WriteException e;
                throw e;
            }
            if (cur.location == root.location)root = cur;
        }

        void AddChild(Node &cur, const Key &first_key, const Key &added_first_key, int added_loc,
                      int num) {//num is the index for the added node
            for (int i = cur.children_num - 1; i >= num + 1; --i) {
                cur.children[i + 1] = cur.children[i];
                cur.keys[i + 1] = cur.keys[i];
            }
            cur.children[num + 1] = added_loc;
            cur.keys[num + 1] = added_first_key;
            cur.keys[num] = first_key;
            cur.children_num++;
            if (!index_memory->Write(cur.location, cur)) {
                ds::WriteException e;
                throw e;
            }
            if (cur.location == root.location)root = cur;
        }

        //to find the key in the current node;
        //the bool value it returns presents whether it needs to have changes as a father
        //the reference of the new_node is the node that should be added;
        bool
        RecursionInsert(Node &cur, const Key &key, const Info &info, bool &success, int &splited_child_index,
                        Node &new_node,
                        int &new_node_loc) {
            int num;
            if (cur.children_num <= 1)num = 0;
            else num = BinarySearchLess(cur.keys, cur.children_num, key);
            if (num == -1)num = 0;
            //num is the first element that is less than or equal to the key
            //so we need go to the children the key stand for (num+1)
            if (cur.isleaf) {
                if (cur.location == root.location && cur.children_num == 0) {
                    Block block;
                    block.record[0] = info;
                    block.keys[0] = key;
                    block.size = 1;
                    cur.children[0] = record_memory->Append(block);
                    cur.keys[0] = block.keys[0];
                    cur.children_num++;
                    cur.location = index_memory->FindEnd();
                    root_index = index_memory->Append(cur);
                    root = cur;
                    //success = true;
                    return false;
                }
                Block block;
                if (!record_memory->Read(cur.children[num], block)) {
                    ds::ReadException e;
                    throw e;
                }
                int key_index = BinarySearchLess(block.keys, block.size, key);
                if (key_index != -1 &&
                    (!cmp.operator()(block.keys[key_index], key) && !cmp.operator()(key, block.keys[key_index]))) {
                    success = false;//the element is already in the block
                    return false;
                    //the recursion ended;
                }
                if (block.size + 1 < max_rcd_num) {
                    //directly insert
                    //the new element should be inserted into the key_index+1;
                    for (int i = block.size - 1; i >= key_index + 1; --i) {
                        block.keys[i + 1] = block.keys[i];
                        block.record[i + 1] = block.record[i];
                    }
                    block.keys[key_index + 1] = key;
                    block.record[key_index + 1] = info;
//                    record_memory->Write(cur.children[num], block);
                    block.size++;
                    if (!record_memory->Write(cur.children[num], block)) {
                        ds::WriteException e;
                        throw e;
                    }
                    //success = true;//the insert is succeeded and it doesn't need to change the father;
                    if (key_index + 1 == 0)cur.keys[num] = block.keys[0];
                    if (!index_memory->Write(cur.location, cur)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (cur.location == root.location)root = cur;
                    //success = true;
                    return false;//the recursion ended;
                } else {
                    for (int i = block.size - 1; i >= key_index + 1; --i) {
                        block.keys[i + 1] = block.keys[i];
                        block.record[i + 1] = block.record[i];
                    }
                    block.keys[key_index + 1] = key;
                    block.record[key_index + 1] = info;
//                    record_memory->Write(cur.children[num], block);
                    block.size++;
                    Block new_block;
                    //every block should have more than size/2 elements,
                    //the first block is 0~size/2-1, the second block is size/2~size-1;
                    for (int i = block.size / 2, j = 0; i < block.size; ++i, ++j) {
                        new_block.keys[j] = block.keys[i];
                        new_block.record[j] = block.record[i];
                    }
                    block.size = max_rcd_num / 2;
                    new_block.size = max_rcd_num - block.size;
                    int new_block_loc = record_memory->Append(new_block);
                    if (!record_memory->Write(cur.children[num], block)) {
                        ds::WriteException e;
                        throw e;
                    }
                    //now the current node has to deal with an added child
                    if (cur.children_num + 1 < max_key_num) {
                        //the children can be directly added
                        //the former child is num;
                        // so the new children should be placed in num+1;
                        AddChild(cur, block.keys[0], new_block.keys[0], new_block_loc, num);
                        //success = true;
                        return false;
                    } else {
                        //cur need to split and tell his father that he splited
                        //use the new_node and new_node_loc
                        AddChild(cur, block.keys[0], new_block.keys[0], new_block_loc, num);
                        Split(cur, new_node, new_node_loc, splited_child_index, num);
                        if (cur.location == root.location) {
                            root.children_num = 2;
                            root.children[0] = cur.location;
                            root.children[1] = new_node.location;
                            root.keys[0] = cur.keys[0];
                            root.keys[1] = new_node.keys[0];
                            root.isleaf = false;
                            root.nxt = root.before = -1;
                            root.location = index_memory->FindEnd();
                            root_index = index_memory->Append(root);
                            //success = true;
                            return false;
                        }
                        return true;
                    }
                }
            } else {
                //we need to find the real leaf node to insert;
                Node parent = cur;
                if (!index_memory->Read(cur.children[num], cur)) {
                    ds::ReadException e;
                    throw e;
                }
                //now cur is the next node to insert
                Node the_added_children;
                int former_children_index;
                int the_added_loc;
                bool need_change = RecursionInsert(cur, key, info, success, former_children_index,
                                                   the_added_children,
                                                   the_added_loc);
                if (!need_change) {
                    if (cmp.operator()(cur.keys[0], parent.keys[num])) {
                        parent.keys[num] = cur.keys[0];
                        if (!index_memory->Write(parent.location, parent)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (parent.location == root.location)root = parent;
                    }
                    //success = true;
                    return false;//if the father don't have to change then tell the grandfather don't change;
                } else {
                    //his parent's children is broken into two nodes
                    if (parent.children_num + 1 < max_key_num) {
                        //the children can directly added
                        AddChild(parent, cur.keys[0], the_added_children.keys[0], the_added_loc, num);
                        //success = true;
                        return false;
                    } else {
                        //the parent node has to split and tell his father that he splited;
                        //parent need to split and tell his father that he splited
                        //use the new_node and new_node_loc
                        AddChild(parent, cur.keys[0], the_added_children.keys[0], the_added_loc, num);
                        Split(parent, new_node, new_node_loc, splited_child_index, num);
                        if (parent.location == root.location) {
                            root.children_num = 2;
                            root.children[0] = parent.location;
                            root.children[1] = new_node.location;
                            root.keys[0] = parent.keys[0];
                            root.keys[1] = new_node.keys[0];
                            root.isleaf = false;
                            root.nxt = root.before = -1;
                            root.location = index_memory->FindEnd();
                            root_index = index_memory->Append(root);
                            //success = true;
                            return false;
                        }
                        cur = parent;//下面的cur还要用，所以换回要用的值
                        return true;
                    }
                }
            }
        }

        //check if the problem can be dealt with in the cur and his brother
        //if the child can be borrowed return true and the node doesn't merge
        //else return false then merge the children
        bool BorrowChild(Node &cur, int num, Node &parent, int &brother_to_merge_index, Node &brother_to_merge) {
            //cur is the num child of parent
            //so check the brother in num-1 and num+1
            bool flag = false;
            if (cur.location == root.location)return flag;
            if (num - 1 >= 0) {
                Node left_brother;
                if (!index_memory->Read(parent.children[num - 1], left_brother)) {
                    ds::ReadException e;
                    throw e;
                }
                if (left_brother.children_num > max_key_num / 2) {
                    flag = true;
                    parent.keys[num] = left_brother.keys[left_brother.children_num - 1];
                    for (int i = cur.children_num; i >= 1; --i) {
                        cur.children[i] = cur.children[i - 1];
                        cur.keys[i] = cur.keys[i - 1];
                    }
                    cur.children[0] = left_brother.children[left_brother.children_num - 1];
                    cur.keys[0] = left_brother.keys[left_brother.children_num - 1];
                    left_brother.children_num--;
                    cur.children_num++;
                    if (!index_memory->Write(cur.location, cur)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (!index_memory->Write(left_brother.location, left_brother)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (!index_memory->Write(parent.location, parent)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (parent.location == root.location)root = parent;
                } else {
                    brother_to_merge_index = num - 1;
                    brother_to_merge = left_brother;
                }
            }
            if (num + 1 < parent.children_num) {
                Node right_brother;
                if (!index_memory->Read(parent.children[num + 1], right_brother)) {
                    ds::ReadException e;
                    throw e;
                }
                if (right_brother.children_num > max_key_num / 2) {
                    flag = true;
                    parent.keys[num + 1] = right_brother.keys[1];
                    cur.children[cur.children_num] = right_brother.children[0];
                    cur.keys[cur.children_num] = right_brother.keys[0];
                    for (int i = 1; i <= right_brother.children_num - 1; ++i) {
                        right_brother.children[i - 1] = right_brother.children[i];
                        right_brother.keys[i - 1] = right_brother.keys[i];
                    }
                    right_brother.children_num--;
                    cur.children_num++;
                    if (!index_memory->Write(cur.location, cur)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (!index_memory->Write(right_brother.location, right_brother)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (!index_memory->Write(parent.location, parent)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (parent.location == root.location)root = parent;
                } else {
                    brother_to_merge_index = num + 1;
                    brother_to_merge = right_brother;
                }
            }
            return flag;
        }

        //merge rear into the front and then the rear will be deleted;
        void MergeBrothers(Node &front, Node rear) {
            for (int i = front.children_num, j = 0; j < rear.children_num; ++i, ++j) {
                front.children[i] = rear.children[j];
                front.keys[i] = rear.keys[j];
            }
            if (rear.isleaf) {
                front.nxt = rear.nxt;
                if (rear.nxt != -1) {
                    Node after;
                    if (!index_memory->Read(rear.nxt, after)) {
                        ds::ReadException e;
                        throw e;
                    }
                    after.before = front.location;
                    if (!index_memory->Write(rear.nxt, after)) {
                        ds::WriteException e;
                        throw e;
                    }
                }
            }
            front.children_num += rear.children_num;
            if (!index_memory->Write(front.location, front)) {
                ds::WriteException e;
                throw e;
            }
        }

        void MergeBlocks(Block &front, Block &rear, int front_loc) {
            for (int i = front.size, j = 0; j < rear.size; ++i, ++j) {
                front.record[i] = rear.record[j];
                front.keys[i] = rear.keys[j];
            }
            front.size += rear.size;
            if (!record_memory->Write(front_loc, front)) {
                ds::WriteException e;
                throw e;
            }
        }

        //merged index is the child that remains
        //we suppose that always remain the child that close to the front, merge the rare one
        bool RecursionRemove(Node &cur, const Key &key, bool &success, Node parent) {
            int num = BinarySearchLess(cur.keys, cur.children_num, key);
            if (num == -1)num = 0;
            if (cur.isleaf) {
                Block block;
                if (!record_memory->Read(cur.children[num], block)) {
                    ds::ReadException e;
                    throw e;
                }
                int index = BinarySearch(block, key);
                if (index == -1) {
                    success = false;
                    return false;
                }
                for (int i = index; i < block.size; ++i) {
                    block.keys[i] = block.keys[i + 1];
                    block.record[i] = block.record[i + 1];
                }
                block.size--;
                if (index == 0) {
                    cur.keys[num] = block.keys[0];
                    if (root.location == cur.location)root = cur;
                }
                if (block.size >= max_rcd_num / 2) {
                    //directly remove
                    if (!record_memory->Write(cur.children[num], block)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (!index_memory->Write(cur.location, cur)) {
                        ds::WriteException e;
                        throw e;
                    }
                    if (cur.location == root.location)root = cur;
                    //success = true;
                    return false;
                } else {
                    if (cur.location == root.location && cur.children_num == 1) {
                        //directly remove
                        if (!record_memory->Write(cur.children[num], block)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (block.size == 0)cur.children_num = 0;
                        if (!index_memory->Write(cur.location, cur)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (cur.location == root.location)root = cur;
                        //success = true;
                        return false;
                    }
                    //borrow a record from the brother block
                    //or merged
                    Block to_merge_brother;
                    int to_merge_brother_index;
                    if (num - 1 >= 0) {
                        Block left_brother;
                        if (!record_memory->Read(cur.children[num - 1], left_brother)) {
                            ds::ReadException e;
                            throw e;
                        }
                        if (left_brother.size > max_rcd_num / 2) {
                            cur.keys[num] = left_brother.keys[left_brother.size - 1];
                            for (int i = block.size; i >= 1; --i) {
                                block.keys[i] = block.keys[i - 1];
                                block.record[i] = block.record[i - 1];
                            }
                            block.keys[0] = left_brother.keys[left_brother.size - 1];
                            block.record[0] = left_brother.record[left_brother.size - 1];
                            block.size++;
                            left_brother.size--;
                            if (!record_memory->Write(cur.children[num - 1], left_brother)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (!record_memory->Write(cur.children[num], block)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (!index_memory->Write(cur.location, cur)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (cur.location == root.location)root = cur;
                            //success = true;
                            return false;
                        } else {
                            to_merge_brother = left_brother;
                            to_merge_brother_index = num - 1;
                        }
                    }
                    if (num + 1 <= cur.children_num - 1) {
                        Block right_brother;
                        if (!record_memory->Read(cur.children[num + 1], right_brother)) {
                            ds::ReadException e;
                            throw e;
                        }
                        if (right_brother.size > max_rcd_num / 2) {
                            cur.keys[num + 1] = right_brother.keys[1];
                            block.keys[block.size] = right_brother.keys[0];
                            block.record[block.size] = right_brother.record[0];
                            for (int i = 1; i <= right_brother.size - 1; ++i) {
                                right_brother.keys[i - 1] = right_brother.keys[i];
                                right_brother.record[i - 1] = right_brother.record[i];
                            }
                            block.size++;
                            right_brother.size--;
                            if (!record_memory->Write(cur.children[num + 1], right_brother)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (!record_memory->Write(cur.children[num], block)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (!index_memory->Write(cur.location, cur)) {
                                ds::WriteException e;
                                throw e;
                            }
                            if (cur.location == root.location)root = cur;
                            //success = true;
                            return false;
                        } else {
                            to_merge_brother = right_brother;
                            to_merge_brother_index = num + 1;
                        }
                    }
                    int the_merged_index;
                    Block the_merged_child;
                    if (to_merge_brother_index == num + 1) {
                        MergeBlocks(block, to_merge_brother, cur.children[num]);
                        the_merged_child = block;
                        the_merged_index = num;
                    } else {
                        MergeBlocks(to_merge_brother, block, cur.children[num - 1]);
                        the_merged_index = num - 1;
                        the_merged_child = to_merge_brother;
                    }
                    //there are children merged
                    //its index is in the reference;
                    for (int i = the_merged_index + 1; i <= cur.children_num - 2; ++i) {
                        cur.children[i] = cur.children[i + 1];
                        cur.keys[i] = cur.keys[i + 1];
                    }
                    cur.children_num--;
                    cur.keys[the_merged_index] = the_merged_child.keys[0];
                    if (cur.children_num >= max_key_num / 2 || cur.location == root.location) {
                        //the merge can directly happen
                        //now the child in the index is completely well dealt;
                        if (!index_memory->Write(cur.location, cur)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (cur.location == root.location)root = cur;
                        //success = true;
                        return false;
                    } else {
                        if (!index_memory->Write(cur.location, cur)) {
                            ds::WriteException e;
                            throw e;
                        }
                        return true;//这就直接告诉自己的爹，我现在孩子个数不够了，找个兄弟帮我
                    }
                }

            } else {
                //should go to the next remove
                parent = cur;//parent记下原始的父亲节点，下面对parent的修改不会涉及到外面parent的变化
                if (!index_memory->Read(cur.children[num], cur)) {
                    ds::ReadException e;
                    throw e;
                }
                bool need_brother = RecursionRemove(cur, key, success, parent);
                if (!need_brother) {
                    if (cmp.operator()(parent.keys[num], cur.keys[0])) {//todo
                        parent.keys[num] = cur.keys[0];
                        if (!index_memory->Write(parent.location, parent)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (parent.location == root.location)root = parent;
                    }
                    //success = true;
                    cur = parent;
                    return false;//the removal suceeded and the recursion ended;
                } else {
                    //否则的话就是需要brother，现在parent的孩子cur，需要一个brother来帮助他
                    //borrow a child from the brother before ar after;
                    int brother_to_merge_index;
                    Node brother_to_merge;
                    bool borrow_flag = BorrowChild(cur, num, parent, brother_to_merge_index,
                                                   brother_to_merge);
                    Node merged_child;
                    int merged_index;
                    if (borrow_flag) {
                        //success = true;
                        cur = parent;
                        return false;//the recursion is ended;
                    } else {
                        if (brother_to_merge_index > num) {
                            MergeBrothers(cur, brother_to_merge);
                            merged_child = cur;
                            merged_index = num;
                        } else {
                            MergeBrothers(brother_to_merge, cur);
                            merged_child = brother_to_merge;
                            merged_index = num - 1;
                        }
                    }
                    //there are children merged
                    //its index is in the reference;
                    for (int i = merged_index + 1; i <= parent.children_num - 2; ++i) {
                        parent.children[i] = parent.children[i + 1];
                        parent.keys[i] = parent.keys[i + 1];
                    }
                    parent.children_num--;
                    parent.keys[merged_index] = merged_child.keys[0];

                    if (parent.children_num >= max_key_num / 2) {
                        //the merge can directly happen
                        //now the child in the index is completely well dealt;
                        if (!index_memory->Write(parent.location, parent)) {
                            ds::WriteException e;
                            throw e;
                        }
                        if (parent.location == root.location) {
                            root = parent;
                        }
                        //success = true;
                        cur = parent;
                        return false;
                    } else {
                        if (parent.location == root.location) {
                            if (parent.children_num == 1) {
                                root = merged_child;
                                root_index = merged_child.location;
                                //success = true;
                                cur = parent;
                                return false;
                                //相当于根节点不要了
                            } else {
                                if (!index_memory->Write(parent.location, parent)) {
                                    ds::WriteException e;
                                    throw e;
                                }
                                root = parent;
                                //success = true;
                                cur = parent;
                                return false;
                            }
                        }
                        if (!index_memory->Write(cur.location, cur)) {
                            ds::WriteException e;
                            throw e;
                        }
                        cur = parent;
                        return true; //否则他就得申请向自己的爹要一个兄弟来帮忙
                    }
                }
            }
        }

    public:

        //B+树的构造函数,由一个文件构造
        BPlusTree(
                const char *index_file_name,
                const char *record_file_name) {
            index_file = new char[40];
            strcpy(index_file, index_file_name);
            record_file = new char[40];
            strcpy(record_file, record_file_name);
            index_memory = new MemoryRiver<Node>(index_file);
            int_memory = new MemoryRiver<int>(index_file);
            record_memory = new MemoryRiver<Block>(record_file);
            //read the number of the records
            //the index file: scale + root_index + the Nodes
            if (!int_memory->Read(0, scale)) {
                //If the reading fails it means the file is empty
                //and the scale and the root index should be initialized
                scale = 0;
                if (!int_memory->Write(0, scale)) {
                    ds::WriteException e;
                    throw e;
                }
                if (!int_memory->Write(sizeof(int), root_index)) {
                    ds::WriteException e;
                    throw e;
                }
            } else {
                if (!int_memory->Read(sizeof(int), root_index)) {
                    ds::ReadException e;
                    throw e;
                }
                if (root_index != -1) {
                    if (!index_memory->Read(root_index, root)) {
                        ds::ReadException e;
                        throw e;
                    }
                }//read the information for the root;
//                int_memory->Read(sizeof(int), root_index);
//                int_memory->Read(root_index, root);//read the information for the root;
            }
        }

        //B+树的析构函数
        ~BPlusTree() {
            if (!int_memory->Write(0, scale)) {
                ds::WriteException e;
                throw e;
            }
            if (!int_memory->Write(sizeof(int), root_index)) {
                ds::WriteException e;
                throw e;
            }
//            int_memory->Write(0, scale);
//            int_memory->Write(sizeof(int), root_index);
            delete[]index_file;
            delete[]record_file;
            delete index_memory;
            delete record_memory;
            delete int_memory;
        }

        //插入一个元素，参数是插入元素的键值和记录的详细信息，返回插入是否成功
        //如果说这个元素本来存在，插入失败返回false
        bool Insert(const Key &key, const Info &info) {
            bool flag = true;
            Node tmp;
            int children_index;
            int loc;
            Node cur = root;
            RecursionInsert(cur, key, info, flag, children_index, tmp, loc);
            scale += (int) flag;
            return flag;
        }

        //删除一个元素，参数是要删除元素的键值，返回是否删除成功，
        //如果这个元素在B+树中不存在就删除失败
        bool Remove(const Key &key) {
            bool flag = true;
            Node tmp_parent;
            Node cur = root;
            if (root.children_num == 0)return false;
            RecursionRemove(cur, key, flag, tmp_parent);
            scale -= (int) flag;
            return flag;
        }

        //查询一个元素，参数是要查询元素的键值
        //返回值是一个pair，bool代表有没有找到
        //如果找到这个元素存在，返回true，同时返回记录信息的具体值
        //如果没有找到，返回false，这时的返回struct是随机值，不应该被访问
        std::pair<bool, std::pair<Key, Info>> Find(const Key &key) {
            std::pair<std::pair<int, int>, Info> tmp = RecursionFind(key);
            if (tmp.first.first == -1)return {false, {key, tmp.second}};
            else return {true, {key, tmp.second}};
        }

        Info operator[](const Key &key) {
            auto tmp = Find(key);
            if (!tmp.first)throw ds::FindException();
            return tmp.second.second;
        }

        //修改一个元素，参数是要修改元素的键值和修改之后的信息
        //返回一个bool，修改成功返回true，否则返回false
        //如果这个要修改的元素在B+树中不存在就会返回false
        bool Modify(const Key &key, const Info &new_info) {
            std::pair<std::pair<int, int>, Info> tmp = RecursionFind(key);
            if (tmp.first.first == -1)return false;
            else {
//                record_memory->Write(tmp.first, new_info);
                //the first int is the index of the block, the second is the index of the record
                Block block;
                if (!record_memory->Read(tmp.first.first, block)) {
                    ds::ReadException e;
                    throw e;
                }
                block.record[tmp.first.second] = new_info;
                if (!record_memory->Write(tmp.first.first, block)) {
                    ds::WriteException e;
                    throw e;
                }
                return true;
            }
        }

        //清空B+树的有关文件
        void Clear() {
            scale = 0;
            root.isleaf = true;
            root.children_num = 0;
            if (root_index == -1)return;
            if (!index_memory->Write(root_index, root)) {
                ds::WriteException e;
                throw e;
            }
        }

        //返回现在总共有多少条记录
        int GetSize() {
            return scale;
        }

    private:
        int BinarySearchBigger(Key array[], int size, const Key &key) {
            if (size <= 0)return -1;
            if (cmp.operator()(array[size - 1], key))return size;//如果说这个元素比数组中最大的元素还要大，就返回size
            //find the answer from 0 to size-1
            int l = 0, r = size - 1;
            int mid;
            while (l < r) {
                mid = (l + r) >> 1;
                if (cmp.operator()(array[mid], key)) {
                    //this index may become the answer
                    //but the boundary can be more left
                    l = mid + 1;
                } else if (cmp.operator()(key, array[mid])) r = mid;
                else return mid;
            }
            return l;
        }

        //要找到的key就是wanted，找到之后把这几个引用传递值修改了，
        //分别是：这个元素所在的block，这个元素所在的leaf_node，这个元素的key，info，
        //还有在这个叶节点之中是第几个block，在这个block中是第几条记录
        bool
        RecursionFindBigger(Node cur, const Key &wanted, Block &block, Node &leaf, Key &key,
                            Info &info, int &node_index, int &block_index, bool &need_brother) {
            if (cur.children_num == 0) {
                return false;
            }
            if (!cur.isleaf) {//如果说cur不是叶节点，还要往下找
                int num = BinarySearchLess(cur.keys, cur.children_num, wanted);
                if (num == -1)num = 0;
                //num is the first element that is less than or equal to the key
                //so we need go to the children the key stand for (num)
//                !index_memory->Read(cur.children[index + 1], cur);
                Node parent = cur;
                if (!index_memory->Read(cur.children[num], cur)) {
                    ds::ReadException e;
                    throw e;
                }
                bool flag = RecursionFindBigger(cur, wanted, block, leaf, key, info, node_index, block_index,
                                                need_brother);
                if (need_brother) {
                    //如果说parent有一个比自己cur更大的孩子,就去找这个孩子
                    if (num + 1 < parent.children_num) {
                        if (!index_memory->Read(parent.children[num + 1], cur)) {
                            ds::ReadException e;
                            throw e;
                        }
                        need_brother = false;
                        return RecursionFindBigger(cur, wanted, block, leaf, key, info, node_index, block_index,
                                                   need_brother);
                    } else {
                        need_brother = true;
                        return false;
                    }
                } else return flag;
            }
            //when the cur is leaf node come out of the circle
            //then do binary search on that leaf node to fina the key
            leaf = cur;//找到了那个叶节点
            int num = BinarySearchLess(cur.keys, cur.children_num, wanted);
            if (num == -1)num = 0;
            if (!record_memory->Read(cur.children[num], block)) {
                ds::ReadException e;
                throw e;
            }
            int key_index = BinarySearchBigger(block.keys, block.size, wanted);
            if (key_index == block.size) {
                if (num + 1 >= cur.children_num) {
                    need_brother = true;
                    return false;
                }
                if (!record_memory->Read(cur.children[num + 1], block)) {
                    ds::ReadException e;
                    throw e;
                }
                if (block.size <= 0) {
                    ds::FindException e;
                    throw e;
                }
                key = block.keys[0];
                info = block.record[0];
                block_index = 0;
                node_index = num + 1;
                need_brother = false;
                return true;
            } else {
                key = block.keys[key_index];
                info = block.record[key_index];
                block_index = key_index;
                node_index = num;
                need_brother = false;
                return true;
            }
        }

    public:
        class iterator {
        private:
            Key key;
            Info info;
            BPlusTree<Key, Info, max_key_num, max_rcd_num, KeyCompare> *this_bpt = nullptr;
            Node leaf_node;//所在的leaf node
            Block block;//所在的block
            int block_index;//在block中的标号
            int node_index;
            bool at_end = false;
        public:
            iterator(BPlusTree<Key, Info, max_key_num, max_rcd_num, KeyCompare> *bpt) {
                this_bpt = bpt;
            }

            iterator operator++(int x) {
                iterator tmp = *this;
                if (this_bpt == nullptr) {
                    ds::InvalidIterator e;
                    throw e;
                }
                if (block_index == block.size - 1) {
                    //我们需要读下一块
                    if (node_index == leaf_node.children_num - 1) {
                        //我们需要找到下一个leaf node
                        if (at_end) {
                            ds::IteratorOutBound e;
                            throw e;
                        }
                        if (leaf_node.nxt == -1) {
                            at_end = true;
                            return *this;
                        }
                        if (!this_bpt->index_memory.Read(leaf_node.nxt, leaf_node)) {
                            ds::ReadException e;
                            throw e;
                        }
                        if (!this_bpt->record_memory.Read(leaf_node.children[0], block)) {
                            ds::ReadException e;
                            throw e;
                        }
                        node_index = 0;//block在这个node中是第几块
                        block_index = 0;//这个key值是第几个
                        info = block.record[0];
                        key = block.keys[0];
                        return tmp;
                    } else {
                        if (!this_bpt->record_memory.Read(leaf_node.children[node_index + 1], block)) {
                            ds::ReadException e;
                            throw e;
                        }
                        node_index += 1;//block在这个node中是第几块
                        block_index = 0;//这个key值是第几个
                        info = block.record[0];
                        key = block.keys[0];
                        return tmp;
                    }
                } else {
                    block_index += 1;//这个key值是第几个
                    info = block.record[block_index];
                    key = block.keys[block_index];
                    return tmp;
                }
            }//后缀的++

            iterator operator++() {
                if (this_bpt == nullptr) {
                    ds::InvalidIterator e;
                    throw e;
                }
                if (block_index == block.size - 1) {
                    //我们需要读下一块
                    if (node_index == leaf_node.children_num - 1) {
                        //我们需要找到下一个leaf node
                        if (at_end) {
                            ds::IteratorOutBound e;
                            throw e;
                        }
                        if (leaf_node.nxt == -1) {
                            at_end = true;
                            return *this;
                        }
                        if (!this_bpt->index_memory->Read(leaf_node.nxt, leaf_node)) {
                            ds::ReadException e;
                            throw e;
                        }
                        if (!this_bpt->record_memory->Read(leaf_node.children[0], block)) {
                            ds::ReadException e;
                            throw e;
                        }
                        node_index = 0;//block在这个node中是第几块
                        block_index = 0;//这个key值是第几个
                        info = block.record[0];
                        key = block.keys[0];
                        return *this;
                    } else {
                        if (!this_bpt->record_memory->Read(leaf_node.children[node_index + 1], block)) {
                            ds::ReadException e;
                            throw e;
                        }
                        node_index += 1;//block在这个node中是第几块
                        block_index = 0;//这个key值是第几个
                        info = block.record[0];
                        key = block.keys[0];
                        return *this;
                    }
                } else {
                    block_index += 1;//这个key值是第几个
                    info = block.record[block_index];
                    key = block.keys[block_index];
                    return *this;
                }
            }

            friend BPlusTree<Key, Info, max_key_num, max_rcd_num, KeyCompare>::iterator
            BPlusTree<Key, Info, max_key_num, max_rcd_num, KeyCompare>::FindBigger(const Key &wanted);

            std::pair<Key, Info> operator*() {
                if (at_end) {
                    ds::InvalidIterator e;
                    throw e;
                }
                return {key, info};
            }

            bool AtEnd() {
                return at_end;
            }
        };

        //返回指向比key更大的
        iterator FindBigger(const Key &wanted) {
            iterator iter(this);
            bool tmp;
            bool find = RecursionFindBigger(root, wanted, iter.block, iter.leaf_node, iter.key, iter.info,
                                            iter.node_index,
                                            iter.block_index, tmp);
            if (!find) {
                iter.at_end = true;
                return iter;
            }
            iter.at_end = false;
            return iter;
        }

        void Traverse() {
            int *queue = new int[30];
            int head = -1, rear = -1;
            queue[++rear] = root_index;
            while (head < rear) {
                int index = queue[++head];
                Node cur;
                if (!index_memory->Read(index, cur)) {
                    ds::ReadException e;
                    throw e;
                }
                std::cout << (cur.isleaf ? "is leaf  " : "");
                if (!cur.isleaf) {
                    for (int i = 0; i < cur.children_num; ++i) {
                        std::cout << cur.keys[i] << " ";
                        queue[++rear] = cur.children[i];
                    }
                    std::cout << std::endl;
                } else {
                    for (int i = 0; i < cur.children_num; ++i) {
                        std::cout << cur.keys[i] << " ";
                    }
                    std::cout << std::endl;
                    for (int i = 0; i < cur.children_num; ++i) {
                        Block block;
                        if (!record_memory->Read(cur.children[i], block)) {
                            ds::ReadException e;
                            throw e;
                        }
                        std::cout << "-------block ";
                        for (int j = 0; j < block.size; ++j) {
                            std::cout << block.keys[j] << " ";
                        }
                        std::cout << std::endl;
                    }
                }
            }
            delete[]queue;
        }
    };

}
#endif //UNTITLED2_BPLUSTREE_H
