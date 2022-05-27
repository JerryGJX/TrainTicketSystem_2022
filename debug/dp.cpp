#include<bits/stdc++.h>
using namespace std;

int main() {
    //system("rm mine");
    //system("rm std");
    system("g++ ./gjx/main.cpp ./gjx/command_parser.cpp ./gjx/order_manager.cpp ./gjx/train_manager.cpp ./gjx/user_manager.cpp -o mine -fsanitize=address -ftrapv -g");
    system("g++ ./yls/src/main.cpp -o std -w -fsanitize=address -ftrapv -g");
    system("g++ ./dmk.cpp -o dmk");
    int T;
    while (1) {
        system("rm index*");
        system("rm *node*");
        system("rm record*");
        system("./dmk > data.in");
        system("./mine < data.in > mine.out");
        system("./std < data.in > std.out");
        if (system("diff ./mine.out ./std.out -b")) {
            puts("ERROR!!!");
            return 0;
        } else cout << ++T << endl;
    }
    return 0;
}