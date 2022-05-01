#include <string>
template <const int size = 64>
class Char {
  /**
   * @brief 多开一位用来放 '\0'
   */
  char content[size + 1];

 public:
  Char();
  Char(const std::string &s);
  Char(const char *cstr);
  operator std::string() const;
  std::string str() const;

  Char &operator=(const Char &that);
  friend bool operator<(const Char<size> &a, const Char<size> &b);
  friend bool operator==(const Char<size> &a, const Char<size> &b);
  friend bool operator>(const Char<size> &a, const Char<size> &b);
  friend bool operator<=(const Char<size> &a, const Char<size> &b);
  friend bool operator>=(const Char<size> &a, const Char<size> &b);
  friend bool operator!=(const Char<size> &a, const Char<size> &b);
  friend std::istream &operator>>(std::istream &is, Char<size> &s);
  friend std::ostream &operator<<(std::ostream &os, const Char<size> &s);
};