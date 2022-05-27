#include<bits/stdc++.h>
using namespace std;
const int station_number = 300;
const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int p[station_number + 1];
vector<int> _time;
vector<string> user, psd, station, train_id;
set<string> Set_user, Set_train_id;
vector<vector<string> > stop_station;
vector<int> begin_time, end_time;

char rand_letter() {
    if (rand() & 1) {
        return rand() % 26 + 'a';
    } else {
        return rand() % 26 + 'A';
    }
}

char rand_char() {
    int f = rand() % 10;
    if (f <= 7) return rand_letter();
    else if (f == 8) return rand() % 10 + '0';
    else return '_';
}

string rand_string(int len, int type) {
    if (len < 4) len = 1;
    string res = "";
    for (int i = 1; i <= len; ++i) {
        if (i == 1) res += rand_letter();
        else res += rand_char();
    }
    if (type == 2) {
        res += '@';
        res += "ssjtu.com";
    }
    if (type == 3) {
        res += "名字";
    }
    return res;
}

int date_to_number(int m, int d) {
    int sum = 0;
    for (int i = 1; i < m; ++i) sum += days[i];
    sum += d;
    return sum;
}

pair<int, int> number_to_date(int sum) {
    int m = 1, d = 0;
    while (sum > days[m]) {
        sum -= days[m];
        ++m;
    }
    d = sum;
    return make_pair(m, d);
}

int get_time() {
    int new_time = rand() % 10 + 1 + _time.back();
    _time.push_back(new_time);
    return new_time;
}

void add_user() {
    static int tmp = 0;
    printf("[%d] add_user ", get_time());
    string c = user[rand() % user.size()];
    if (tmp != 0 && tmp <= 20) c = user[1];
    ++tmp;
    string u = rand_string(rand() % 20 + 1, 1);
    string p = rand_string(rand() % 30 + 1, 1);
    string n = rand_string(rand() % 5 + 1, 3);
    string m = rand_string(rand() % 20 + 1, 2);
    if (Set_user.find(u) == Set_user.end()) {
        Set_user.insert(u);
        user.push_back(u);
        psd.push_back(p);
    }
    int g = rand() % 11;
    if (tmp == 1) g = 10;
    cout << "-c " << c << " -u " << u << " -p " << p << " -n " << n << " -m " << m << " -g " << g << endl;
}

string get_psd(int who) {
    string p = psd[who];
    if (rand() % 10 == 0) {
        p[0] = '_';
    }
    return p;
}

void login() {
    printf("[%d] login ", get_time());
    int who = rand()% user.size();
    cout << "-u " << user[who] << " -p " << get_psd(who) << endl;
}

void logout() {
    printf("[%d] logout ", get_time());
    int who = rand()% user.size();
    cout << "-u " << user[who] << endl;
}

void query_profile() {
    printf("[%d] query_profile ", get_time());
    int c = rand()% user.size(), u = rand()% user.size();
    cout << "-c " << user[c] << " -u " << user[u] << endl;
}

void modify_profile() {
    printf("[%d] modify_profile ", get_time());
    int c = rand()% user.size(), u = rand()% user.size();
    cout << "-c " << user[c] << " -u " << user[u];
    string p = rand_string(rand() % 30 + 1, 1);
    string n = rand_string(rand() % 5 + 1, 3);
    string m = rand_string(rand() % 20 + 1, 2);
    int g = rand() % 11;
    if (rand() % 2) {
        cout << " -p " << p;
    }
    if (rand() % 2) {
        cout << " -n " << n;
    }
    if (rand() % 2) {
        cout << " -m " << m;
    }
    if (rand() % 2) {
        cout << " -g " << g;
    }
    cout << endl;
}

int get_day(int m) {
    if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12) {
        return rand() % 31 + 1;
    } else if (m == 4 || m == 6 || m == 9 || m == 11) {
        return rand() % 30 + 1;
    } else {
        return rand() % 28 + 1;
    }
}

string int_to_string(int x) {
    string s;
    if (x == 0) {
        s = "0";
        return s;
    }
    while (x) {
        char c = x % 10 + '0';
        s = c + s;
        x /= 10;
    }
    return s;
}

string two_space(int x) {
    string s = int_to_string(x);
    if (s.length() == 1) s = '0' + s;
    return s;
}

void random_date1() {
    int m = rand() % 12 + 1, d = get_day(m);
    cout << two_space(m) << '-' << two_space(d);
}

void outtime(int x) {
    pair<int, int> tmp = number_to_date(x);
    cout << two_space(tmp.first) << '-' << two_space(tmp.second);
}

void add_train() {
    printf("[%d] add_train ", get_time());
    string i = rand_string(rand() % 20 + 1, 1);
    int n = rand() % 5 + 2;
    if (n > station_number) n = station_number;
    int m = rand() % 100 + 1;
    cout << "-i " << i << " -n " << n << " -m " << m << " -s ";
    vector<string> vec;
    random_shuffle(p + 1, p + station_number + 1);
    for (int i = 1; i <= n; ++i) {
        cout << station[p[i]];
        if (i != n) cout << '|';
        vec.push_back(station[p[i]]);
    }
    cout << " -p ";
    for (int i = 1; i <= n - 1; ++i) {
        cout << rand() % 100 + 1;
        if (i != n - 1) cout << '|';
    }
    cout << " -x " << two_space(rand() % 24) << ':' << two_space(rand() % 60);
    cout << " -t ";
    for (int i = 1; i <= n - 1; ++i) {
        cout << rand() % 50 + 1;
        if (i != n - 1) cout << '|';
    }
    cout << " -o ";
    if (n == 2) {
        cout << "_";
    } else {
        for (int i = 1; i <= n - 2; ++i) {
            cout << rand() % 5 + 1;
            if (i != n - 2) cout << '|';
        }
    }
    cout << " -d ";
    int st = rand() % 365 + 1, ed = rand() % 365 + 1;
    if (st > ed) swap(st, ed);
    outtime(st); putchar('|'); outtime(ed);
    cout << " -y ";
    putchar(rand() % 26 + 'A');
    puts("");
    if (Set_train_id.find(i) == Set_train_id.end()) {
        Set_train_id.insert(i);
        train_id.push_back(i);
        stop_station.push_back(vec);
        begin_time.push_back(st);
        end_time.push_back(ed);
    }
}

void delete_train() {
    printf("[%d] delete_train ", get_time());
    cout << "-i " << train_id[rand() % train_id.size()] << endl;
}

void release_train() {
    printf("[%d] release_train ", get_time());
    cout << "-i " << train_id[rand() % train_id.size()] << endl; 
}

void query_train() {
    printf("[%d] query_train ", get_time());
    cout << "-i " << train_id[rand() % train_id.size()];
    cout << " -d ";
    random_date1();
    cout << endl;
}

void query_ticket() {
    printf("[%d] query_ticket ", get_time());
    cout << "-s " << train_id[rand() % train_id.size()] << " -t " << train_id[rand() % train_id.size()];
    cout << " -d ";
    random_date1();
    if (rand() % 2) {
        if (rand() % 2) cout << " -p cost";
        else cout << " -p time";
    }
    puts("");
}

void query_transfer() {
    printf("[%d] query_transfer ", get_time());
    cout << "-s " << train_id[rand() % train_id.size()] << " -t " << train_id[rand() % train_id.size()];
    cout << " -d ";
    random_date1();
    if (rand() % 2) {
        if (rand() % 2) cout << " -p cost";
        else cout << " -p time";
    }
    puts("");
}

void buy_ticket() {
    printf("[%d] buy_ticket ", get_time());
    int who = rand() % train_id.size();
    cout << "-u " << user[rand() % user.size()];
    cout << " -i " << train_id[who];
    cout << " -d ";
    int dt = begin_time[who] + rand() % (end_time[who] - begin_time[who] + 1);
    outtime(dt);
    cout << " -n ";
    cout << rand() % 50 + 1;
    int f = rand() % stop_station[who].size();
    int t = rand() % stop_station[who].size();
    while (f == t) t = rand() % stop_station[who].size();
    if (f > t) swap(f, t);
    cout << " -f " << stop_station[who][f];
    cout << " -t " << stop_station[who][t];
    if (rand() % 2) {
        cout << " -q ";
        if (rand() % 2) cout << "true";
        else cout << "false";
    }
    puts("");
}

void query_order() {
    printf("[%d] query_order ", get_time());
    cout << "-u " << user[rand() % user.size()] << endl;
}

void refund_ticket() {
    printf("[%d] refund_ticket ", get_time());
    cout << "-u " << user[rand() % user.size()];
    cout << " -n " << rand() % 10 + 1 << endl;
}

void clean() {
    printf("[%d] clean\n", get_time());
}

void exit() {
    printf("[%d] exit\n", get_time());
}

void init() {
    _time.push_back(0);
    user.push_back("balabala");
    psd.push_back("balabala");
    train_id.push_back("balabala");
    station.push_back("station0");
    for (int i = 1; i <= station_number; ++i) {
        string s = "station" + int_to_string(i);
        station.push_back(s);
        p[i] = i;
    }
    vector<string> vec;
    vec.push_back(station[0]);
    vec.push_back(station[1]);
    stop_station.push_back(vec);
    begin_time.push_back(0);
    end_time.push_back(0);
}

int main() {
    srand(time(0));
    init();
    int fre[20];
    void (*p[20]) ();
    p[0] = add_user; fre[0] = 10;
    p[1] = login; fre[1] = 100;
    p[2] = logout; fre[2] = 100;
    p[3] = query_profile; fre[3] = 1000;
    p[4] = modify_profile; fre[4] = 100;
    p[5] = add_train; fre[5] = 10;
    p[6] = delete_train; fre[6] = 10;
    p[7] = release_train; fre[7] = 10;
    p[8] = query_train; fre[8] = 10;
    p[9] = query_ticket; fre[9] = 1000;
    p[10] = query_transfer; fre[10] = 10;
    p[11] = buy_ticket; fre[11] = 1000;
    p[12] = query_order; fre[12] = 100;
    p[13] = refund_ticket; fre[13] = 10;
    p[14] = clean; fre[14] = 1;
    p[15] = exit; fre[15] = 0;
    for (int i = 1; i <= 20; ++i) {
        (*p[0])();
        (*p[1])();
    }
    for (int i = 1; i <= 20; ++i) (*p[5])();
    for (int i = 1; i <= 15; ++i) {
        if (rand() % 3 == 0) (*p[6])();
        else (*p[7])();
    }
    int sum = 0;
    for (int i = 0; i <= 15; ++i) sum += fre[i];
    for (int i = 1; i <= 1000; ++i) {
        int x = rand() % sum + 1;
        for (int j = 0; j <= 15; ++j) {
            x -= fre[j];
            if (x <= 0) {
                (*p[j])();
                break;
            }
        }
    }
    exit();
    return 0;
}