#include<bits/stdc++.h>
using namespace std;

int n, m;

int main() {
    int T = 0; 
    system("g++ dmk.cpp -o dmk -fsanitize=address -ftrapv -g");
    system("g++ std.cpp -o std -fsanitize=address -ftrapv -g");
    system("g++ test_bpTree.cpp -o a -fsanitize=address -ftrapv -g");
    while (1) {
        system("./dmk > data.in");
        system("./a < data.in > a.out"); 
        system("./std < data.in > std.out");
        if (system("diff std.out a.out -b")) {
        //    system("./a < data.in > a.out"); 
            if (system("diff std.out a.out -b")) {
                puts("ERROR!!!");
                return 0;
            }
        } else cout << ++T << endl;
    }
    return 0;
}