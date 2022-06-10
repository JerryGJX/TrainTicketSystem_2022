

#ifndef COMMAND_PARSER_HPP__USER_MANAGER_HPP_
#define COMMAND_PARSER_HPP__USER_MANAGER_HPP_

# include "tools/Char.hpp"
# include "ACMstl/bptree_rollback.hpp"
# include "ACMstl/UnorderedMap.hpp"
#include "ACMstl/Vector.hpp"
#include "mydefs.hpp"
#include "tools/Algorithm.hpp"


//#include <map>
//#include <unordered_map>
//#include <vector>

/**
 * @brief 用户相关
 */


using JerryGJX::ull;

class UserManager;
class User {
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

  std::string to_string() {
    std::string ans;
    ans += username.str() + " " + name.str() + " ";
    ans += mailAddr.str() + " " + std::to_string(privilege);
    return ans;
  }
};

class UserManager {
 private:
  int TimeTag = 0;
  Bptree<ull, User, 339, 67> userDatabase;//username -> User(class)
  sjtu::linked_hashmap<ull, int> onlineUser;//维护在线用户 username->privilege
  //Bptree<ull, std::pair<int, bool>> onlineUserBackUp;
  std::hash<std::string> hash_str;
 public:
  explicit UserManager(const std::string &filenameUD);

  ull CalHash(const std::string &username_);

  //bool Empty() const;

  bool AddUser(std::string *info);//该函数只负责添加用户，能否插入由ParserCommander判断

  bool isReg(const std::string &username_);

  bool isLogin(const std::string &username_);//onlineUser中该用户是否存在，存在则返回对应privilege，否则返回-1

  bool checkPassword(const std::string &username_, const std::string &password_);

  bool Login(std::string *info);//该函数只负责用户登录，能否登录由ParserCommander判断

  bool Logout(std::string *info);//该函数只负责用户退出，能否退出由ParserCommander判断
  /**
   * @brief 该函数只负责返回信息，能否查询由ParserCommander判断
   * @return 被查询用户的<username>，<name>，<mailAddr>和<privilege>
   */
  bool queryProfile(std::string *info, std::string &result);

  /**
   * @brief 以读出，删除再插入的方式实现
   * @param username_ 待修改用户的用户名
   * @param cmd 存有经过确认的待修改信息 e.g. password->target password
   * @param order_manager_ 用于应对修改uid的情况
   * @return 被修改用户的<username>，<name>，<mailAddr>和<privilege>
   */
  bool modifyProfile(std::string *info, std::string &result);

  void Clean();

  void Exit();

//----------rollback-----------

  void RollBack(int target_time);

  void GetTime(int time_tag);

};

#endif //COMMAND_PARSER_HPP__USER_MANAGER_HPP_