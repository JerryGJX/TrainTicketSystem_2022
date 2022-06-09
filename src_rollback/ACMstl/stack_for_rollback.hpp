#ifndef STACK_FOR_ROLLBACK_HPP
#define STACK_FOR_ROLLBACK_HPP

#include <fstream>
#include <iostream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T>
class StackForRollback {
private:
  const int info_len = 1;
  int _size;
  fstream file;
  string file_name;
  constexpr static int sizeofT = sizeof(T);
public:
  StackForRollback() = default;

  StackForRollback(const string& file_name) : file_name(file_name) {
    initialise();
  }

  ~StackForRollback() {
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&size), sizeof(int));
    file.close();
  }

  void initialise(string FN = "") {
    if (FN != "") {
      file_name = FN + "_rollback";
    }
    file.open(file_name);
    if (!file) {
      file.close();
      file.open(file_name, std::ios::out);
      int tmp = 0;
      for (int i = 0; i < info_len; ++i) {
        file.seekp(i * sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      }
      _size = 0;
    } else {
      file.seekg(0);
      file.read(reinterpret_cast<char *> (&_size), sizeof(int));
    }
    file.close();
  }

  //在文件末尾写入类对象t
  void push(T &t) {
    file.seekp(sizeof(int) + (_size++) * sizeofT);
    file.write(reinterpret_cast<char *> (&t), sizeofT);
  }

  //读出文件末尾的元素写入t
  void top(T &t) {
    if (_size == 0) return;
    file.seekg((_size - 1) * sizeofT);
    file.read(reinterpret_cast<char *> (&t), sizeofT);
  }

  void pop() {
    --_size;
  }

  void clear() {
    _size = 0;
    file.open(file_name, std::ios::out | std::ios::trunc);
    file.close();
  }

  int size() {
    return _size;
  }

  bool empty() {
    return !_size;
  }
};


#endif //STACK_FOR_ROLLBACK_HPP
