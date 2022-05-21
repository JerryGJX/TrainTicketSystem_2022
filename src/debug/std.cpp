#include<bits/stdc++.h>
using namespace std;

map<int, int> Map;

int main() {
    int T;
    cin >> T;
    while (T--) {
       int op;
       scanf("%d", &op);
       if (op == 1) {
          int key, value;
          scanf("%d%d", &key, &value);
          if (Map.find(key) != Map.end()) continue;
          Map[key] = value;
       } else if (op == 2) {
          int x, y, f;
          scanf("%d", &x);
          cout << (f = (Map.find(x) != Map.end())) << endl;
          if (f == 1) {
             Map.erase(x);
          }
       } else if (op == 3) {
          int x, y, f;
          scanf("%d%d", &x, &y);
          cout << (f = (Map.find(x) != Map.end())) << endl; 
          if (f) {
             Map[x] = y;
          }
       } else if (op == 4) {
          vector<pair<int, int>> vec;
          int x, y;
          scanf("%d%d", &x, &y);
          map<int, int>::iterator it;
          for (it = Map.begin(); it != Map.end(); ++it) {
             if (it -> first >= x && it -> first < y) {
                vec.push_back(*it);
             }
          }
          for (int i = 0; i < vec.size(); ++i) {
             cout << vec[i].first << ' ' << vec[i].second << endl;
          }
       }
    }
    return 0;
}