#include<bits/stdc++.h>
#include "bpTree.hpp"
using namespace std;

struct point {
    int x, y;
};
Bptree<int, int, 100, 100, std::hash<int>, std::equal_to<int> > tr("Bptree_test");
int T;


int main() {
//    system("del Bptree_test_leaf_node");
//    system("del Bptree_test_leaf_node_recycle");
//    system("del Bptree_test_normal_node");
//    system("del Bptree_test_normal_node_recycle");
//    freopen("data.in", "r", stdin);
    cin >> T;
    while (T--) {
       int op;
       scanf("%d", &op);
       if (op == 1) {
          int key, value;
          scanf("%d%d", &key, &value);
          int k = tr.insert(key, value);
       } else if (op == 2) {
          int x, y, f;
          scanf("%d", &x);
          cout << (f = tr.find(x, y)) << endl;
          if (f == 1) {
             tr.erase(x);
          }
       } else if (op == 3) {
          int x, y, f;
          scanf("%d%d", &x, &y);
          cout << (f = tr.modify(x, y)) << endl;
       } else if (op == 4) {
          sjtu::vector<pair<int, int>> vec;
          int x, y;
          scanf("%d%d", &x, &y);
          tr.range_search(x, y, vec);
          for (int i = 0; i < vec.size(); ++i) {
             cout << vec[i].first << ' ' << vec[i].second << endl;
          }
       } else if (op == 5) {
          cout << tr.size() << endl;
       }
    }
    tr.clear();
    /*
    system("rm Bptree_test_leaf_node");
    system("rm Bptree_test_leaf_node_recycle");
    system("rm Bptree_test_normal_node");
    system("rm Bptree_test_normal_node_recycle");
    */
    return 0;
}
