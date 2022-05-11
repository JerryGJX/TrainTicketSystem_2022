//
// Created by JerryGuo on 2022/5/11.
//

#ifndef COMMAND_PARSER_HPP_TOOLS_TIME_HPP_
#define COMMAND_PARSER_HPP_TOOLS_TIME_HPP_

#include <iostream>

int mon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int monSum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

struct Date {
  int mm, dd;
  explicit Date(int mm_ = 0, int dd_ = 0) : mm(mm_), dd(dd_) {}
  explicit Date(const std::string &str) {
    int len_m = 0, len = int(str.length());
    for (int i = 0; i < len && str[len_m] != '-'; ++i, ++len_m);
    mm = std::stoi(str.substr(0, len_m));
    dd = std::stoi(str.substr(len_m + 1, len));
  }
  Date(const Date &rhs) : mm(rhs.mm), dd(rhs.dd) {}

  Date &operator=(const Date &rhs) {
    if (&rhs == this)return *this;
    mm = rhs.mm, dd = rhs.dd;
    return *this;
  }

  bool operator<(const Date &rhs) const {
    if (mm < rhs.mm) return true;
    if (rhs.mm < mm) return false;
    return dd < rhs.dd;
  }
  bool operator>(const Date &rhs) const {
    return rhs < *this;
  }
  bool operator<=(const Date &rhs) const {
    return !(rhs < *this);
  }
  bool operator>=(const Date &rhs) const {
    return !(*this < rhs);
  }

  Date operator+(int x) {
    Date lhs = *this;
    lhs.dd += x;
    if (lhs.dd > mon[mm])lhs.dd -= mon[mm], lhs.mm++;
    return lhs;
  }
  bool operator==(const Date &rhs) const {
    return mm == rhs.mm && dd == rhs.dd;
  }
  bool operator!=(const Date &rhs) const {
    return !(rhs == *this);
  }

  std::string ToStr() const {
    std::string ans;
    ans += (mm < 10 ? '0' + std::to_string(mm) : std::to_string(mm)) + '-';
    ans += (dd < 10 ? '0' + std::to_string(dd) : std::to_string(dd));
    return ans;
  }

};

struct Time {
  Date date;
  int hor, min;
  explicit Time(int mm_ = 0, int dd_ = 0, int hor_ = 0, int min_ = 0) : date(mm_, dd_) {
    hor = hor_, min = min_;
  }
  explicit Time(const std::string &str) : date(0, 0) {
    int len_h = 0, len = int(str.length());
    if (len >= 6) {
      for (int i = 0; i < len && str[len_h] != ' '; ++i, ++len_h);
      date = Date(str.substr(0, len_h));
      std::string str_ = str.substr(len_h + 1, len);
      len_h = 0, len = int(str_.length());
      for (int i = 0; i < len && str_[len_h] != ':'; ++i, ++len_h);
      hor = std::stoi(str_.substr(0, len_h));
      min = std::stoi(str_.substr(len_h + 1, len));
    } else {
      for (int i = 0; i < len && str[len_h] != ':'; ++i, ++len_h);
      hor = std::stoi(str.substr(0, len_h));
      min = std::stoi(str.substr(len_h + 1, len));
    }
  }

  std::string ToStr() const {
    std::string ans;
    ans += date.ToStr();
    ans += ' ';
    ans += (hor < 10 ? '0' + std::to_string(hor) : std::to_string(hor)) + ':';
    ans += (min < 10 ? '0' + std::to_string(min) : std::to_string(min));
    return ans;
  }

  Time operator+(int x) {
    int sum = 24 * 60 * (date.dd + monSum[date.mm]) + 60 * hor + min;
    sum += x;
    int day = sum % (24 * 60);
    date.mm = std::lower_bound(monSum, monSum + 12, day);
    date.dd = day - monSum[date.mm];
    sum -= day * 24 * 60;
    hor = sum % 60;
    min = sum - hor * 60;
    return *this;
  }

};

#endif //COMMAND_PARSER_HPP_TOOLS_TIME_HPP_
