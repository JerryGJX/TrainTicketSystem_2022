//
// Created by m1375 on 2022/5/8.
//

#ifndef TRAINTICKETSYSTEM_TOOLS_HPP
#define TRAINTICKETSYSTEM_TOOLS_HPP

#include "vector.hpp"
#include <string>
#include <iostream>

namespace hnyls2002 {

    /*template<typename T>
    void sort(T *begin, T *end) {
        if (begin >= end - 1)return;
        int len = end - begin;
        T mid_val = begin[rand() % len], *l = begin, *r = end - 1;
        while (l <= r) {
            while (*l < mid_val)++l;
            while (mid_val < *r)--r;
            if (l <= r)std::swap(*l, *r), ++l, --r;
        }
        if (l < end)sort(l, end);
        if (r > begin)sort(begin, r + 1);
    }

    template<typename T>
    void sort(T *begin, T *end, bool(*cmp)(const T &, const T &)) {
        if (begin >= end - 1)return;
        int len = end - begin;
        T mid_val = begin[rand() % len], *l = begin, *r = end - 1;
        while (l <= r) {
            while (cmp(*l, mid_val))++l;
            while (cmp(mid_val, *r))--r;
            if (l <= r)std::swap(*l, *r), ++l, --r;
        }
        if (l < end)sort(l, end);
        if (r > begin)sort(begin, r + 1);
    }*/

    template<typename T>
    void sort(typename sjtu::vector<T>::iterator it1, typename sjtu::vector<T>::iterator it2,
              bool(*cmp)(const T &, const T &)) {
        if (it1 - it2 >= -1)return;
        int len = it2 - it1;
        T mid_val = *(it1 + rand() % len);
        auto l = it1, r = it2 - 1;
        while (l - r <= 0) {
            while (cmp(*l, mid_val))++l;
            while (cmp(mid_val, *r))--r;
            if (l - r <= 0)std::swap(*l, *r), ++l, --r;
        }
        if (l - it2 < 0)sort(l, it2, cmp);
        if (r - it1 > 0)sort(it1, r + 1, cmp);
    }

    template<int LEN>
    class fstr {
    public:
        char s[LEN]{};
        int siz;

        fstr() : siz(0) { s[0] = '\0'; }

        fstr(const std::string &str) {
            siz = (int) str.size();
            for (int i = 0; i < siz; ++i)
                s[i] = str[i];
            s[siz] = '\0';
        }

        fstr<LEN> &operator=(const fstr<LEN> &oth) {
            if (&oth == this)return *this;
            siz = oth.siz;
            memcpy(s, oth.s, sizeof(s));
            return *this;
        }

        bool operator==(const fstr<LEN> &oth) const {
            if (siz != oth.siz)return false;
            for (int i = 0; i < siz; ++i)
                if (s[i] != oth.s[i])return false;
            return true;
        }

        bool operator!=(const fstr<LEN> &oth) const {
            return !(operator==(oth));
        }

        bool operator<(const fstr<LEN> &oth) const {
            return strcmp(s, oth.s) < 0;
        }

        std::string to_string() {
            return std::string(s, siz);
        }

        int to_int() { return std::stoi(to_string()); }

        friend std::ostream &operator<<(std::ostream &os, fstr<LEN> value) {
            os << value.s;
            return os;
        }

    };

    sjtu::vector<std::string> split_cmd(const std::string &str, const char &ch) {
        sjtu::vector<std::string> ret;
        int len = (int) str.size();
        std::string tmp;
        for (int i = 0; i < len; ++i) {
            if (str[i] != ch)tmp += str[i];
            else ret.push_back(tmp), tmp.clear();
        }
        if (!tmp.empty())ret.push_back(tmp);
        return ret;
    }

    int mon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int mon_s[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    struct Time;

    struct Date {
        int mm, dd;

        explicit Date(int _mm = 0, int _dd = 0) : mm(_mm), dd(_dd) {}

        explicit Date(const Time &time);// 这里特地设计成只可以显式转化

        Date(const std::string &str) {
            auto tmp = split_cmd(str, '-');
            mm = std::stoi(tmp[0]);
            dd = std::stoi(tmp[1]);
        }

        bool operator==(const Date &d) const {
            return mm == d.mm && dd == d.dd;
        }

        bool operator<(const Date &d) const {
            if (mm != d.mm)return mm < d.mm;
            return dd < d.dd;
        }

        int to_int() const { return mon_s[mm - 1] + dd; }

        static Date to_date(int sum) {
            Date ret;
            for (int i = 1; i <= 12; ++i)
                if (mon_s[i] >= sum) {
                    sum -= mon_s[i - 1];
                    ret.mm = i, ret.dd = sum;
                    break;
                }
            return ret;
        }

        Date operator+(int x) const { return to_date(to_int() + x); }

        Date &operator+=(int x) { return *this = *this + x; }

        Date operator-(int x) const { return to_date(to_int() - x); }

        Date &operator-=(int x) { return *this = *this - x; }

        friend int operator-(const Date &d1, const Date &d2) {
            return d1.to_int() - d2.to_int();
        }
    };

    struct Time {
        int mm, dd, hr, mi;

        Time(int _mm = 0, int _dd = 0, int _hr = 0, int _mi = 0) : mm(_mm), dd(_dd), hr(_hr), mi(_mi) {}

        Time(const std::string &str) : mm(6), dd(1) {// 只记录时间的话，日期全部设置成儿童节算了
            auto tmp = split_cmd(str, ':');
            hr = std::stoi(tmp[0]);
            mi = std::stoi(tmp[1]);
        }

        // 取date的日期部分和time的时间部分
        Time(const Date &date, const Time &time) : mm(date.mm), dd(date.dd), hr(time.hr), mi(time.mi) {}

        std::string to_string() const {
            std::string ret;
            ret += (mm < 10 ? '0' + std::to_string(mm) : std::to_string(mm)) + '-';
            ret += (dd < 10 ? '0' + std::to_string(dd) : std::to_string(dd)) + ' ';
            ret += (hr < 10 ? '0' + std::to_string(hr) : std::to_string(hr)) + ':';
            ret += mi < 10 ? '0' + std::to_string(mi) : std::to_string(mi);
            return ret;
        }

        int to_int() const {
            return mi + 1 + hr * 60 + (dd - 1) * 24 * 60 + mon_s[mm - 1] * 24 * 60;
        }

        static Time to_Time(int sum) {
            Time ret;
            for (int i = 1; i <= 12; ++i)
                if (mon_s[i] * 24 * 60 >= sum) {
                    sum -= mon_s[i - 1] * 24 * 60;
                    ret.mm = i;
                    break;
                }
            for (int i = 1; i <= mon[ret.mm]; ++i)
                if (i * 24 * 60 >= sum) {
                    sum -= (i - 1) * 24 * 60;
                    ret.dd = i;
                    break;
                }
            for (int i = 0; i <= 23; ++i)
                if ((i + 1) * 60 >= sum) {
                    sum -= i * 60;
                    ret.hr = i, ret.mi = sum - 1;
                    break;
                }
            return ret;
        }

        Time operator+(int x) const {
            return to_Time(to_int() + x);
        }

        Time &operator+=(int x) {
            *this = *this + x;
            return *this;
        }

        friend int operator-(const Time &t1, const Time &t2) {
            return t1.to_int() - t2.to_int();
        }

        bool operator<(const Time &t) const {
            return to_int() < t.to_int();
        }

        Time DayStep(int x) const {// 这个时间往后跳x天
            Date tmp = Date(*this);
            tmp += x;
            return Time{tmp, *this};
        }
    };

    Date::Date(const Time &time) { this->mm = time.mm, this->dd = time.dd; }// 只取time的日期部分

}

#endif //TRAINTICKETSYSTEM_TOOLS_HPP