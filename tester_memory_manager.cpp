#include "memory_manager.hpp"
#include<bits/stdc++.h>
using namespace std;

int main() {
   MemoryRiver<pair<int, int>> tmp("data");
   tmp.write_info(3, 1);
   pair<int, int> p = make_pair(1, 2);
   int idx = tmp.write(p);
   tmp.read(p, idx);
   cout << p.first << ' ' << p.second << endl;
   pair<int, int> q = make_pair(2, 3);
   tmp.update(q, idx);
   tmp.read(p, idx);
   cout << p.first << ' ' << p.second << endl;
   return 0;
}
