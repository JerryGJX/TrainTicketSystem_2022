# include "Char.hpp"
# include "bpTree.hpp"
# include "UnorderedMap.hpp"
#include "Vector.hpp"
#include "mydefs.hpp"
#include "order_manager.hpp"

/**
 * @brief 用户相关
 */




class UserManager;
class User {
  friend class Usermanager;
 private:
  JerryGJX::passwordType password;

 public:
  JerryGJX::usernameType username;
  JerryGJX::nameType name;
  JerryGJX::mailAddrType mailAddr;
  int privilege;
};

class UserManager {
 private:
 public:
  /**
   *
   * @brief username -> User(class)
   */
  Bptree<JerryGJX::usernameType , User> userDatabase;
  /**
   * 维护在线用户
   */
  UnorderedMap<JerryGJX::usernameType , User> onlineUser;
  /**
   *
   * @brief 该函数只负责添加用户，能否插入由ParserCommander判断
   */
  void AddUser(const std::string username_,
               const std::string password_,
               const std::string name_,
               const std::string mailAddr_,
               const int privilege_);
  /**
   *
   * @return userDatabase中该用户名是否存在
   */
  bool isRegistered(const std::string username_);
  /**
   *
   * @return onlineUser中该用户是否存在，存在则返回对应privilege，否则返回-1
   */
  int isLogin(const std::string username_);
  /**
   *
   * @brief 该函数只负责用户登录，能否登录由ParserCommander判断
   */
  void Login(const std::string &username_, const std::string &password_);
  /**
   *
   * @brief 该函数只负责用户退出，能否退出由ParserCommander判断
   */
  void Logout(const std::string &username_);
  /**
   *
   * @brief 该函数只负责返回信息，能否查询由ParserCommander判断
   * @return 被查询用户的<username>，<name>，<mailAddr>和<privilege>
   */
  Vector<std::string> queryProfile(const std::string &username_);

  /**
   *
   * @brief 以读出，删除再插入的方式实现
   * @param username_ 待修改用户的用户名
   * @param cmd 存有经过确认的待修改信息 e.g. password->target password
   * @param order_manager_ 用于应对修改uid的情况
   * @return 被修改用户的<username>，<name>，<mailAddr>和<privilege>
   */
  Vector<std::string> modifyProfile(const std::string &username_, const UnorderedMap<std::string, std::string> &info,OrderManager* order_manager_);
};
