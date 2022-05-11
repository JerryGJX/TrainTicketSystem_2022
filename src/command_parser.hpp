//
// Created by JerryGuo on 2022/5/8.
//

#ifndef COMMAND_PARSER_HPP__COMMAND_PARSER_HPP_
#define COMMAND_PARSER_HPP__COMMAND_PARSER_HPP_

#include <iostream>
#include <cstring>
#include "order_manager.hpp"
#include "train_manager.hpp"
#include "user_manager.hpp"

#include <vector>
#include <unordered_map>
using std::vector;


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
  std::unordered_map<std::string, void (CommandParser::*)(vector<std::string> &)> mapFunction;
 public:
  CommandParser();




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
  void ParseAddUser(vector<std::string> &cmd);
  void ParseLogin(vector<std::string> &cmd);
  void ParseLogout(vector<std::string> &cmd);
  void ParseQueryProfile(vector<std::string> &cmd);
  void ParseModifyProfile(vector<std::string> &cmd);
  void ParseAddTrain(vector<std::string> &cmd);
  void ParseReleaseTrain(vector<std::string> &cmd);
  void ParseQueryTrain(vector<std::string> &cmd);
  void ParseQueryTicket(vector<std::string> &cmd);
  void ParseQueryTransfer(vector<std::string> &cmd);
  void ParseBuyTicket(vector<std::string> &cmd);
  void ParseQueryOrder(vector<std::string> &cmd);
  void ParseRefundTicket(vector<std::string> &cmd);
  void ParseRollback(vector<std::string> &cmd);
  void ParseClean(vector<std::string> &cmd);
  void ParseExit(vector<std::string> &cmd);
};




#endif //COMMAND_PARSER_HPP__COMMAND_PARSER_HPP_
