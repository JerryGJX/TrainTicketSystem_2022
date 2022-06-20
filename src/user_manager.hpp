

#ifndef COMMAND_PARSER_HPP__USER_MANAGER_HPP_
#define COMMAND_PARSER_HPP__USER_MANAGER_HPP_

#include "tools/Char.hpp"
#include "ACMstl/bpTree.hpp"
#include "ACMstl/UnorderedMap.hpp"
#include "ACMstl/Vector.hpp"
#include "mydefs.hpp"
#include "tools/Algorithm.hpp"
#include "tools/Error.hpp"

//#include <map>
//#include <unordered_map>
//#include <vector>

/**
 * @brief 用户相关
 */

using JerryGJX::ull;

class UserManager;
class User
{
  friend class Usermanager;

public:
  JerryGJX::passwordType password;
  JerryGJX::usernameType username;
  JerryGJX::nameType name;
  JerryGJX::mailAddrType mailAddr;
  int privilege{};

  User() = default;
  User(const std::string &username_,
       const std::string &password_,
       const std::string &name_,
       const std::string &mailAddr_,
       int privilege_);
  User(const User &lhs);

  bool operator<(const User &rhs) const;
  bool operator>(const User &rhs) const;
  bool operator<=(const User &rhs) const;
  bool operator>=(const User &rhs) const;

  std::string to_string()
  {
    std::string ans;
    ans += username.str() + " " + name.str() + " ";
    ans += mailAddr.str() + " " + std::to_string(privilege);
    return ans;
  }
};

class UserManager
{
private:
  Bptree<ull, User, 339, 67> userDatabase;   // username -> User(class)
  sjtu::linked_hashmap<ull, int> onlineUser; //维护在线用户 username->privilege
  // Bptree<ull, std::pair<int, bool>> onlineUserBackUp;
  std::hash<std::string> hash_str;

public:
  explicit UserManager(const std::string &filenameUD);

  ull CalHash(const std::string &username_);

  // bool Empty() const;

  bool AddUser(std::string *info);

  bool isReg(const std::string &username_);

  bool isLogin(const std::string &username_);

  bool checkPassword(const std::string &username_, const std::string &password_);

  bool Login(std::string *info);

  bool Logout(std::string *info);

  bool queryProfile(std::string *info, std::string &result);

  bool modifyProfile(std::string *info, std::string &result);

  void Clean();

  void Exit();
};

#endif // COMMAND_PARSER_HPP__USER_MANAGER_HPP_