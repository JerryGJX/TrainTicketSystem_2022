#include "System.hpp"
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

void test_logic() {
/*
    system("rm index*");
    system("rm record*");
    freopen("./testdata/normal/basic_4/my.in", "r", stdin);
    freopen("./testdata/normal/basic_4/my.out", "w", stdout);
*/
    clock_t sts = clock();
    hnyls2002::System sys;
    sys.GetSize();
    std::string str;
    while (getline(std::cin, str)) {
        //std::cerr << "Command : " << str << std::endl;
        auto res = sys.Opt(str);
        //std::cout << str.substr(0, 6) << std::endl;
        for (auto it: res) {
            std::cout << it << std::endl;
            //std::cerr << it << std::endl;
        }
    }
    clock_t end = clock();
    //std::cout << "Running Time : Using  " << (double) (end - sts) / CLOCKS_PER_SEC << " seconds " << std::endl;
}

/*void show() {
    auto it = mp.FindBigger(0);
    if (!it.AtEnd())std::cout << "Not at end" << std::endl;
    for (; !it.AtEnd(); ++it)
        std::cout << (*it).first << " , " << (*it).second << std::endl;
    std::cout << "size = " << mp.GetSize() << std::endl;
}

void lower_bound(int x) {
    auto it = mp.FindBigger(x);
    if (!it.AtEnd())std::cout << "Not at end" << std::endl;
    for (; !it.AtEnd(); ++it)
        std::cout << (*it).first << " , " << (*it).second << std::endl;
}

void add(int l, int r, int len) {
    srand((unsigned) time(NULL));
    int las = -1, status = 0;
    for (int i = l; i <= r; ++i) {
        std::string tmp;
        for (int j = 1; j <= len; ++j)
            tmp += 'a' + rand() % 26;
        auto res = mp.Insert(i, tmp);
        if (las == -1)las = i, status = res;
        if (res != status) {
            std::cout << "Inserting...\n" << (status ? "success" : "failed") << " : from " << las << " to " << i - 1
                      << "\n";
            las = i, status = res;
        }
        if (i == r) {
            std::cout << "Inserting...\n" << (status ? "success" : "failed") << " : from " << las << " to " << i
                      << "\n";
        }
    }
}

void erase(int l, int r) {
    int las = -1, status = 0;
    for (int i = l; i <= r; ++i) {
        auto res = mp.Remove(i);
        if (las == -1)las = i, status = res;
        if (res != status) {
            std::cout << "Erasing...\n" << (status ? "success" : "failed") << " : from " << las << " to " << i - 1
                      << "\n";
            las = i, status = res;
        }
        if (i == r) {
            std::cout << "Erasing...\n" << (status ? "success" : "failed") << " : from " << las << " to " << i
                      << "\n";
        }
    }
}

void find(int l, int r) {
    for (int i = l; i <= r; ++i) {
        auto it = mp.Find(i);
        if (!it.first)std::cout << "Finding...False\n";
        else std::cout << "Finding...Success\n" << it.second.first << " , " << it.second.second << std::endl;
    }
}

void test_bptree() {
    clock_t sts = clock();

    using namespace hnyls2002;

    srand((unsigned) time(NULL));

    std::string cmd;

    while (std::getline(std::cin, cmd)) {
        std::cout << "Commander is " << cmd << "\n";
        auto arg = split_cmd(cmd, ' ');
        if (arg[0] == "add")add(std::stoi(arg[1]), std::stoi(arg[2]), std::stoi(arg[3]));
        else if (arg[0] == "erase")erase(std::stoi(arg[1]), std::stoi(arg[2]));
        else if (arg[0] == "show")show();
        else if (arg[0] == "exit")break;
        else if (arg[0] == "clear")mp.Clear();
        else if (arg[0] == "lower_bound")lower_bound(std::stoi(arg[1]));
        else if (arg[0] == "find")find(std::stoi(arg[1]), std::stoi(arg[2]));
        clock_t end = clock();
        std::cout << "Running Time : Using  " << (double) (end - sts) / CLOCKS_PER_SEC << " seconds " << std::endl;
    }

    clock_t end = clock();
    std::cout << "Running Time : Using  " << (double) (end - sts) / CLOCKS_PER_SEC << " seconds " << std::endl;

}*/

int main() {
//	freopen("data.in", "r", stdin); 
    test_logic();

/*
    system("rm ./index");
    system("rm ./record");
    static ds::BPlusTree<hnyls2002::fstr<66>, hnyls2002::fstr<9000>, 3, 4> mp("./index", "./record");
    mp.Clear();
    static int a[110000];
    for (int i = 1; i <= 1000; ++i) {
        a[i] = rand();
        mp.Insert(std::to_string(a[i]), hnyls2002::fstr<9000>());
    }
    for (int i = 1; i <= 1000; ++i)
        if (mp.FindBigger(std::to_string(a[i])).AtEnd())
            std::cerr << i << " " << a[i] << std::endl;
    mp.Clear();
*/

    return 0;
}
