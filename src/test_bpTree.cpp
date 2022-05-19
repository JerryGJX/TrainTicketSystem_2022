#include<bits/stdc++.h>
#include "bpTree.hpp"
using namespace std;

struct point {
    int x, y;
};

int main() {
    Bptree<int, int, std::less<int>, std::equal_to<int>, 5, 5> tr("Bptree_test");
    tr.insert(1, 1);
    tr.insert(2, 2);
    tr.insert(3, 3);
    tr.insert(4, 4);
    tr.insert(5, 5);
    tr.insert(6, 6);
    int ans = 0;
    cout << tr.find(6, ans) << endl;
    cout << ans << endl;
    cout << tr.find(2, ans) << endl;
    cout << ans << endl;
    tr.modify(5, 100);
    cout << tr.find(5, ans) << endl;
    cout << ans << endl;
    vector<pair<int, int>> vec;
    tr.range_search(2, 6, vec);
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i].first << ' ' << vec[i].second << endl;
    }
    return 0;
}