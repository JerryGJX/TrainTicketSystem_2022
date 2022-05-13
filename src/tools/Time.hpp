//
// Created by JerryGuo on 2022/5/11.
//

#ifndef COMMAND_PARSER_HPP_TOOLS_TIME_HPP_
#define COMMAND_PARSER_HPP_TOOLS_TIME_HPP_



#include <iostream>
namespace JerryGJX {
constexpr int mon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr int monSum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

struct CalendarTime {
  int mm, dd;
  explicit CalendarTime(int mm_ = 0, int dd_ = 0) : mm(mm_), dd(dd_) {}
  CalendarTime(const std::string &str) {
    int len_m = 0, len = int(str.length());
    for (int i = 0; i < len && str[len_m] != '-'; ++i, ++len_m);
    mm = std::stoi(str.substr(0, len_m));
    dd = std::stoi(str.substr(len_m + 1, len));
  }
  CalendarTime(const CalendarTime &rhs) : mm(rhs.mm), dd(rhs.dd) {}

  CalendarTime &operator=(const CalendarTime &rhs) {
    if (&rhs == this)return *this;
    mm = rhs.mm, dd = rhs.dd;
    return *this;
  }

  bool operator<(const CalendarTime &rhs) const {
    if (mm < rhs.mm) return true;
    if (rhs.mm < mm) return false;
    return dd < rhs.dd;
  }
  bool operator>(const CalendarTime &rhs) const {
    return rhs < *this;
  }
  bool operator<=(const CalendarTime &rhs) const {
    return !(rhs < *this);
  }
  bool operator>=(const CalendarTime &rhs) const {
    return !(*this < rhs);
  }

  CalendarTime operator+(int x) {
    CalendarTime lhs = *this;
    lhs.dd += x;
    if (lhs.dd > mon[mm])lhs.dd -= mon[mm], lhs.mm++;
    return lhs;
  }

  CalendarTime &operator+=(int x) {
    *this = *this + x;
    return *this;
  }
  bool operator==(const CalendarTime &rhs) const {
    return mm == rhs.mm && dd == rhs.dd;
  }
  bool operator!=(const CalendarTime &rhs) const {
    return !(rhs == *this);
  }

  std::string ToStr() const {
    std::string ans;
    ans += (mm < 10 ? '0' + std::to_string(mm) : std::to_string(mm)) + '-';
    ans += (dd < 10 ? '0' + std::to_string(dd) : std::to_string(dd));
    return ans;
  }

  int ToDay() const {
    return monSum[mm - 1] + dd;
  }

  int ToHour() const {
    return (monSum[mm - 1] + dd) * 24 * 60;
  }

};

struct ClockTime {
  int hor, min, day;
  explicit ClockTime(int hor_ = 0, int min_ = 0) : hor(hor_), min(min_), day(0) {}
  ClockTime(const std::string &str) {
    int len_h = 0, len = int(str.length());
    for (int i = 0; i < len && str[len_h] != ':'; ++i, ++len_h);
    hor = std::stoi(str.substr(0, len_h));
    min = std::stoi(str.substr(len_h + 1, len));
    day = 0;
  }
  ClockTime(const ClockTime &rhs) : hor(rhs.hor), min(rhs.min), day(rhs.day) {}

  ClockTime &operator=(const ClockTime &rhs) {
    hor = rhs.hor, min = rhs.min;
    return *this;
  }

  bool operator==(const ClockTime &rhs) const {
    return hor == rhs.hor &&
        min == rhs.min;
  }
  bool operator!=(const ClockTime &rhs) const {
    return !(rhs == *this);
  }
  bool operator<(const ClockTime &rhs) const {
    if (hor < rhs.hor)return true;
    if (rhs.hor < hor)return false;
    return min < rhs.min;
  }
  bool operator>(const ClockTime &rhs) const {
    return rhs < *this;
  }
  bool operator<=(const ClockTime &rhs) const {
    return !(rhs < *this);
  }
  bool operator>=(const ClockTime &rhs) const {
    return !(*this < rhs);
  }

  ClockTime operator+(int x) {
    ClockTime lhs = *this;
    lhs.min += x;
    lhs.hor += lhs.min / 60;
    lhs.min %= 60;
    lhs.day = lhs.hor / 24;
    lhs.hor %= 24;
    return lhs;
  }

  ClockTime &operator+=(int x) {
    *this = *this + x;
    return *this;
  }

  std::string ToStr() const {
    std::string ans;
    ans += (hor < 10 ? '0' + std::to_string(hor) : std::to_string(hor)) + ':';
    ans += (min < 10 ? '0' + std::to_string(min) : std::to_string(min));
    return ans;
  }

  int ToMin() const {
    return day * 24 * 60 + hor * 60 + min;
  }
};

struct Time {
  CalendarTime calendar_time;
  ClockTime clock_time;
  explicit Time(int mm_ = 0, int dd_ = 0, int hor_ = 0, int min_ = 0) : calendar_time(mm_, dd_),
                                                                        clock_time(hor_, min_) {}

  Time(const std::string &str, int num) {
    if (num == 1)calendar_time = str;
    else if (num == 2)clock_time = str;
    else {
      int len_b = 0, len = int(str.length());
      for (int i = 0; i < len && str[len_b] != ' '; ++i, ++len_b);
      calendar_time = str.substr(0, len_b);
      clock_time = str.substr(len_b + 1, len);
    }
  }

  Time operator+(int x) {
    Time lhs;
    int sum = 24 * 60 * (calendar_time.dd + monSum[calendar_time.mm - 1]) + 60 * clock_time.hor + clock_time.min;
    sum += x;
    int day = sum / (24 * 60);
    lhs.calendar_time.mm = int(std::lower_bound(monSum, monSum + 12, day) - monSum);
    lhs.calendar_time.dd = day - monSum[lhs.calendar_time.mm - 1];
    sum -= day * 24 * 60;
    lhs.clock_time.hor = sum / 60;
    lhs.clock_time.min = sum - lhs.clock_time.hor * 60;
    return lhs;
  }

  Time &operator+=(int x) {
    *this = *this + x;
    return *this;
  }

  std::string ToStr() const {
    std::string ans;
    ans += calendar_time.ToStr() + ' ';
    ans += clock_time.ToStr();
    return ans;
  }

};
}

#endif //COMMAND_PARSER_HPP_TOOLS_TIME_HPP_
