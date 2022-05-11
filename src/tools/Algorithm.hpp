//
// Created by JerryGuo on 2022/5/11.
//

#ifndef COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_
#define COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_

namespace JerryGJX {
template<class T, class Compare=std::less<T>>
void Sort(T *a, int l, int r) {
  if (l >= r)return;
  T ca = a[l];
  int lp = l, rp = r;
  do {
    while (lp < rp && Compare()(a[rp], ca))--rp;
    if (lp < rp) {
      a[lp] = a[rp];
      ++lp;
    }
    while (lp < rp && Compare()(ca, a[lp]))++lp;
    if (lp < rp) {
      a[rp] = a[lp];
      --rp;
    }
  } while (lp != rp);
  a[lp] = ca;
  int mid = lp;
  sort(a, l, mid);
  sort(a, mid + 1, r);
}

int ToInt(std::string &str) {
  int ans = 0;
  int len = int(str.length());
  for (int i = 0; i < len; ++i) ans = ans * 10 + str[i] - '0';
  return ans;
}

}

#endif //COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_
