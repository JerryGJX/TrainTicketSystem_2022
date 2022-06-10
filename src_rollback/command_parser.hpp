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
  int TimeTag = 0;
  UserManager &user_manager;
  TrainManager &train_manager;
  OrderManager &order_manager;
  /**
   * @brief 根据指令的第一个单词查找对应的函数，供 run 函数使用
   */
  sjtu::linked_hashmap<std::string, void (CommandParser::*)(std::string *)> mapFunction;

  JerryGJX::infoType GetInfoRank(char tag);

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
  void ParseAddUser(std::string *info);
  void ParseLogin(std::string *info);
  void ParseLogout(std::string *info);
  void ParseQueryProfile(std::string *info);
  void ParseModifyProfile(std::string *info);

  void ParseAddTrain(std::string *info);
  void ParseDeleteTrain(std::string *info);
  void ParseReleaseTrain(std::string *info);
  void ParseQueryTrain(std::string *info);
  void ParseQueryTicket(std::string *info);
  void ParseQueryTransfer(std::string *info);
  void ParseBuyTicket(std::string *info);


  void ParseQueryOrder(std::string *info);
  void ParseRefundTicket(std::string *info);
  void ParseRollback(std::string *info);
  void ParseClean(std::string *info);
  void ParseExit(std::string *info);


  //---------------------tool----------------------

  void SplitString(const std::string &cmd, sjtu::vector<std::string> &x, const char &flag);

  static void Success();
  static void Failure();

  //-----------rollback-----------------
  void PutInTag(int time_tag);
  //-------userManager easy form-----------------
  bool ifUReg(const std::string &username_);
  int ifULog(const std::string &username_);
  //--------trainManager easy form---------
  bool ifTAdd(const std::string &trainID_);
  bool ifTRel(const std::string &trainID_);

};

#endif //COMMAND_PARSER_HPP__COMMAND_PARSER_HPP_
