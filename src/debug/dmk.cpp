#include<bits/stdc++.h>
using namespace std;
const int M = 10;

int main() {
    clock_t s1, s2;
    s1 = time(NULL);
    s2 = clock();
//    cout << s1 - 1653100000 << ' ' << s2 << endl;
    srand((s1 - 1653100000) * 10000 + s2);
    int T = 20;
    cout << T << endl;
    for (int i = 1; i <= T; ++i) {
        int op, x, y;
        if (i == T) op = 4;
        if (op == 1 || op > 4) {
            op = 1;
            x = rand() % M, y = rand() % M;
            cout << op << ' ' << x << ' ' << y << endl;
        } else if (op == 2) {
            int x = rand() % M;
            cout << op << ' ' << x << endl;
        } else if (op == 3) {
            int x = rand() % M, y = rand() % M;
            cout << op << ' ' << x << ' ' << y << endl;
        } else if (op == 4) {
            int x = rand() % M, y = rand() % M;
            if (x > y) swap(x, y);
            cout << op << ' ' << x << ' ' << y << endl;
        }
    }
    return 0;
}
