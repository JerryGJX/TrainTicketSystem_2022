#include<bits/stdc++.h>
#include "bpTree.hpp"
using namespace std;

int main() {
    Bptree<int, int, std::less<int>, 5, 5> tr("Bptree_test");
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
    return 0;
}