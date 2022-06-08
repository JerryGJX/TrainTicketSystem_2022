//
// Created by JerryGuo on 2022/5/11.
//

#ifndef COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_
#define COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_

namespace JerryGJX {
template<class T, class Compare=std::less<T>>
void Sort(sjtu::vector<T> &A, int l, int r) {
  if (l >= r)return;
  int low = l, high = r;
  T pivot = A[low];
  while (low < high) {
    while (low < high && A[high] >= pivot) {
      --high;
    }
    A[low] = A[high];
    while (low < high && A[low] <= pivot) {
      ++low;
    }
    A[high] = A[low];
  }
  A[low] = pivot;

  Sort(A, l, low - 1);
  Sort(A, low + 1, r);
}

//int ToHour(std::string &str) {
//  int ans = 0;
//  int len = int(str.length());
//  for (int i = 0; i < len; ++i) ans = ans * 10 + str[i] - '0';
//  return ans;
//}

}

#endif //COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_
