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
  T key=A[low+std::rand()%(high-low+1)];
  //T pivot = A[low];
  while (low <= high) {
    while (A[high] > key) high--;
    while (A[low] < key) low++;
    if(low<=high) std::swap(A[low++],A[high--]);
  }

  Sort(A, l, high);
  Sort(A, low, r);
}


template <typename T>
inline void hash_combine(std::size_t &seed, const T &val) {
  seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T> inline void hash_val(std::size_t &seed, const T &val) {
  hash_combine(seed, val);
}
template <typename T, typename... Types>
inline void hash_val(std::size_t &seed, const T &val, const Types &... args) {
  hash_combine(seed, val);
  hash_val(seed, args...);
}

template <typename... Types>
inline std::size_t hash_val(const Types &... args) {
  std::size_t seed = 0;
  hash_val(seed, args...);
  return seed;
}

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    return hash_val(p.first, p.second);
  }
};


struct pair_pair_hash {
  template <class T1, class T2, class T3>
  std::size_t operator()(const std::pair<T1, std::pair<T2, T3>> &p) const {
    return hash_val(p.first, p.second.first,p.second.second);
  }
};

struct pair_pair_hash_nd {
  template <class T1, class T2, class T3>
  std::size_t operator()(const std::pair<std::pair<T1, T2>, T3> &p) const {
    return hash_val(p.first.first, p.first.second,p.second);
  }
};

}

#endif //COMMAND_PARSER_HPP_TOOLS_ALGORITHM_HPP_
