#include <iostream>
#include <string>
#include "order_manager.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"

/**
 * @brief 解析输入指令。
 * @brief 三个manager在main函数中构造后引用传入，
 * 之后的Parse函数负责调用三个manager中的函数，
 * 且三个管理成员之间的相互调用在Parse中以指针形式传入
 */


class CommandParser {
 private:
  UserManager &user_manager_;
  TrainManager &train_manager_;
  OrderManager &order_manager_;
  /**
   * @brief 根据指令的第一个单词查找对应的函数，供 run 函数使用
   */
  UnorderedMap<std::string, void (CommandParser::*)(Vector<std::string> &)> mapFunction;
 public:
  /**
   * @brief 循环读入指令并解析，直到遇到 exit。
   *
   */
  void Run();

  void ParseAdd();
  void ParseQuery();

  /**
   * @brief 解析 add_user -c -u -p -n -m -g -c（<cur_username>），创建一个账户名为 -u（<username>），密码为 -p（<password>），用户真实姓名为 -n（<name>），用户邮箱为 -m（<mailAddr>），权限为 -g（<privilege>）的用户
   * @param cmd 传入参数
   */
  void ParseAddUser(Vector<std::string> &cmd);
  void ParseLogin(Vector<std::string> &cmd);
  void ParseLogout(Vector<std::string> &cmd);
  void ParseQueryProfile(Vector<std::string> &cmd);
  void ParseModifyProfile(Vector<std::string> &cmd);
  void ParseAddTrain(Vector<std::string> &cmd);
  void ParseReleaseTrain(Vector<std::string> &cmd);
  void ParseQueryTrain(Vector<std::string> &cmd);
  void ParseQueryTicket(Vector<std::string> &cmd);
  void ParseQueryTransfer(Vector<std::string> &cmd);
  void ParseBuyTicket(Vector<std::string> &cmd);
  void ParseQueryOrder(Vector<std::string> &cmd);
  void ParseRefundTicket(Vector<std::string> &cmd);
  void ParseRollback(Vector<std::string> &cmd);
  void ParseClean(Vector<std::string> &cmd);
  void ParseExit(Vector<std::string> &cmd);
};

