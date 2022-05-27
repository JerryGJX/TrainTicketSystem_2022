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

//#include <vector>
//#include <unordered_map>

/**
 * @brief 解析输入指令。
 * @brief 三个manager在main函数中构造后引用传入，
 * 之后的Parse函数负责调用三个manager中的函数，
 * 且三个管理成员之间的相互调用在Parse中以指针形式传入
 */


class CommandParser {
 private:
  UserManager &user_manager;
  TrainManager &train_manager;
  OrderManager &order_manager;
  /**
   * @brief 根据指令的第一个单词查找对应的函数，供 run 函数使用
   */
  sjtu::linked_hashmap<std::string, void (CommandParser::*)(sjtu::linked_hashmap<std::string, std::string> &)> mapFunction;
 public:
  CommandParser(UserManager &user_manager_, TrainManager &train_manager_, OrderManager &order_manager_);

  /**
   * @brief 循环读入指令并解析，直到遇到 exit。
   *
   */
  void Run();

  /**
   * @brief 解析 add_user -c -u -p -n -m -g -c（<cur_username>），创建一个账户名为 -u（<username>），密码为 -p（<password>），用户真实姓名为 -n（<name>），用户邮箱为 -m（<mailAddr>），权限为 -g（<privilege>）的用户
   * @param cmd 传入参数
   */
  void ParseAddUser(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseLogin(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseLogout(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseQueryProfile(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseModifyProfile(sjtu::linked_hashmap<std::string, std::string> &cmd);

  void ParseAddTrain(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseDeleteTrain(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseReleaseTrain(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseQueryTrain(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseQueryTicket(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseQueryTransfer(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseBuyTicket(sjtu::linked_hashmap<std::string, std::string> &cmd);


  void ParseQueryOrder(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseRefundTicket(sjtu::linked_hashmap<std::string, std::string> &cmd);
//  void ParseRollback(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseClean(sjtu::linked_hashmap<std::string, std::string> &cmd);
  void ParseExit(sjtu::linked_hashmap<std::string, std::string> &cmd);


  //---------------------tool----------------------

  void SplitString(const std::string &cmd, sjtu::vector<std::string> &x, const char &flag);

  static void Success();
  static void Failure();

  //-------userManager easy form-----------------
  bool ifUReg(const std::string &username_);
  int ifULog(const std::string &username_);
  bool ifUPass(const std::string &username_, const std::string &password_);

  //--------trainManager easy form---------
  bool ifTAdd(const std::string &trainID_);
  bool ifTRel(const std::string &trainID_);

};

#endif //COMMAND_PARSER_HPP__COMMAND_PARSER_HPP_
