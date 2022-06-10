//
// Created by JerryGuo on 2022/5/8.
//

#include "command_parser.hpp"
CommandParser::CommandParser(UserManager &user_manager_, TrainManager &train_manager_, OrderManager &order_manager_)
    : user_manager(user_manager_), train_manager(train_manager_), order_manager(order_manager_) {
  mapFunction.insert(std::make_pair("add_user", &CommandParser::ParseAddUser));
  mapFunction.insert(std::make_pair("login", &CommandParser::ParseLogin));
  mapFunction.insert(std::make_pair("logout", &CommandParser::ParseLogout));
  mapFunction.insert(std::make_pair("query_profile", &CommandParser::ParseQueryProfile));
  mapFunction.insert(std::make_pair("modify_profile", &CommandParser::ParseModifyProfile));
  mapFunction.insert(std::make_pair("add_train", &CommandParser::ParseAddTrain));
  mapFunction.insert(std::make_pair("delete_train", &CommandParser::ParseDeleteTrain));
  mapFunction.insert(std::make_pair("release_train", &CommandParser::ParseReleaseTrain));
  mapFunction.insert(std::make_pair("query_train", &CommandParser::ParseQueryTrain));
  mapFunction.insert(std::make_pair("query_ticket", &CommandParser::ParseQueryTicket));
  mapFunction.insert(std::make_pair("query_transfer", &CommandParser::ParseQueryTransfer));
  mapFunction.insert(std::make_pair("buy_ticket", &CommandParser::ParseBuyTicket));
  mapFunction.insert(std::make_pair("query_order", &CommandParser::ParseQueryOrder));
  mapFunction.insert(std::make_pair("refund_ticket", &CommandParser::ParseRefundTicket));
  mapFunction.insert(std::make_pair("rollback", &CommandParser::ParseRollback));
  mapFunction.insert(std::make_pair("clean", &CommandParser::ParseClean));
  mapFunction.insert(std::make_pair("exit", &CommandParser::ParseExit));
}

void CommandParser::Run() {
  std::string parser_carrier;
  if (std::getline(std::cin, parser_carrier)) {

    //if (std::cin.eof())exit(0);

    bool all_blank_flag = true;
    for (char i: parser_carrier) {
      if (i != ' ') {
        all_blank_flag = false;
        break;
      }
    }
    if (!all_blank_flag) {
      sjtu::vector<std::string> parser_list;
      std::string parser_list_to_use[15];

      SplitString(parser_carrier, parser_list, ' ');
      std::cout << parser_list[0] << " ";
      std::string timestamp = parser_list[0].substr(1, parser_list[0].length() - 2);
      //std::cout << "!!DEBUG!! " << timestamp << std::endl;
      int time_tag = std::stoi(timestamp);
      std::string cmd_type = parser_list[1];
      for (int i = 2; i < parser_list.size(); i += 2) {
        parser_list_to_use[GetInfoRank(parser_list[i][1])]=parser_list[i + 1];
        //parser_list_to_use.insert(std::make_pair(parser_list[i], ));
      }

//      if (cmd_type == "exit") {
//
//        user_manager.exit();
//
//        std::cout << "bye\n";
      if (mapFunction.find(cmd_type) != mapFunction.end()) (this->*mapFunction[cmd_type])(parser_list_to_use);
    }
  }
}

//------------------user manager-----------------
void CommandParser::ParseAddUser(std::string *cmd) {
  if (user_manager.AddUser(cmd))Success();
  else Failure();
}
void CommandParser::ParseLogin(std::string *cmd) {
  if (user_manager.Login(cmd))Success();
  else Failure();
}
void CommandParser::ParseLogout(std::string *cmd) {
  if (user_manager.Logout(cmd))Success();
  else Failure();
}
void CommandParser::ParseQueryProfile(std::string *cmd) {
  std::string result;
  if (!user_manager.queryProfile(cmd, result))Failure();
  else std::cout << result << "\n";

}
void CommandParser::ParseModifyProfile(std::string *cmd) {
  std::string result;
  if (!user_manager.modifyProfile(cmd, result))Failure();
  else std::cout << result << "\n";
}

//------------------train manager-----------------
void CommandParser::ParseAddTrain(std::string *cmd) {
  if (ifTAdd(cmd[JerryGJX::_i])) {
    Failure();
    return;
  }

  sjtu::vector<std::string> stations, _prices, _travelTimes, _stopoverTimes, saleDate;
  sjtu::vector<int> prices, travelTimes, stopoverTimes;
  SplitString(cmd[JerryGJX::_s], stations, '|');
  SplitString(cmd[JerryGJX::_p], _prices, '|');
  SplitString(cmd[JerryGJX::_t], _travelTimes, '|');
  SplitString(cmd[JerryGJX::_o], _stopoverTimes, '|');
  SplitString(cmd[JerryGJX::_d], saleDate, '|');
  for (auto &_price: _prices)prices.push_back(std::stoi(_price));
  for (auto &_travelTime: _travelTimes)travelTimes.push_back(std::stoi(_travelTime));
  if (std::stoi(cmd[JerryGJX::_n]) > 2)
    for (auto &_stopoverTime: _stopoverTimes)
      stopoverTimes.push_back(std::stoi(_stopoverTime));
  char type = cmd[JerryGJX::_y][0];

  train_manager.addTrain(cmd[JerryGJX::_i],
                         std::stoi(cmd[JerryGJX::_n]),
                         std::stoi(cmd[JerryGJX::_m]),
                         stations,
                         prices,
                         cmd[JerryGJX::_x],
                         travelTimes,
                         stopoverTimes,
                         saleDate, type);
  Success();
}
void CommandParser::ParseDeleteTrain(std::string *cmd) {
  if (!ifTAdd(cmd[JerryGJX::_i]) || ifTRel(cmd[JerryGJX::_i])) {
    Failure();
    return;
  }
  train_manager.deleteTrain(cmd[JerryGJX::_i]);
  Success();
}
void CommandParser::ParseReleaseTrain(std::string *cmd) {
  if (!ifTAdd(cmd[JerryGJX::_i]) || ifTRel(cmd[JerryGJX::_i])) {
    Failure();
    return;
  }
  train_manager.releaseTrain(cmd[JerryGJX::_i]);
  Success();
}
void CommandParser::ParseQueryTrain(std::string *cmd) {
  if (!ifTAdd(cmd[JerryGJX::_i])) {
    Failure();
    return;
  }
  sjtu::vector<std::string> result;
  if (!train_manager.queryTrain(cmd[JerryGJX::_i], cmd[JerryGJX::_d], result)) {
    Failure();
    return;
  }
  std::cout << result[0] << " ";
  for (int i = 1; i < result.size(); ++i)std::cout << result[i] << "\n";
  //Success();
}

void CommandParser::ParseQueryTicket(std::string *cmd) {
  sjtu::vector<std::string> result;
  train_manager.QueryTicket(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}

void CommandParser::ParseQueryTransfer(std::string *cmd) {
  sjtu::vector<std::string> result;
  train_manager.QueryTransfer(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}

void CommandParser::ParseBuyTicket(std::string *cmd) {
  if (!ifUReg(cmd[JerryGJX::_u]) || !ifULog(cmd[JerryGJX::_u]) || !ifTAdd(cmd[JerryGJX::_i]) || !ifTRel(cmd[JerryGJX::_i])) {
    Failure();
    return;
  }
  std::string ans = train_manager.BuyTicket(cmd, order_manager);
  std::cout << ans << "\n";
}

void CommandParser::ParseRefundTicket(std::string *cmd) {
  if (!ifULog(cmd[JerryGJX::_u])) {
    Failure();
    return;
  }
  int rank = 1;
  if (!cmd[JerryGJX::_n].empty())rank = std::stoi(cmd[JerryGJX::_n]);
  if (train_manager.RefundTicket(cmd[JerryGJX::_u], rank, order_manager))Success();
  else Failure();

}

//------------------order manager-----------------
void CommandParser::ParseQueryOrder(std::string *cmd) {
  if (!ifUReg(cmd[JerryGJX::_u]) || !ifULog(cmd[JerryGJX::_u])) {
    Failure();
    return;
  }
  sjtu::vector<std::string> ans;
  order_manager.QueryOrder(cmd[JerryGJX::_u], ans);
  std::cout << ans[0] << "\n";
  for (int i = ans.size() - 1; i >= 1; --i)std::cout << ans[i] << "\n";
}

void CommandParser::ParseRollback(std::string *cmd) {
  int TargetTime=stoi(cmd[JerryGJX::_t]);
  if(TimeTag<TargetTime) {
    Failure();
    return;
  }
  user_manager.RollBack(TargetTime);
  train_manager.RollBack(TargetTime);
  order_manager.RollBack(TargetTime);
  Success();
}

void CommandParser::ParseClean(std::string *cmd) {
  user_manager.Clean();
  train_manager.Clean();
  order_manager.Clean();
  Success();
}
void CommandParser::ParseExit(std::string *cmd) {
  user_manager.Exit();
  train_manager.Exit();
  order_manager.Exit();
  std::cout << "bye\n";
  exit(0);
}



//---------------------tool----------------------

void CommandParser::SplitString(const std::string &cmd, sjtu::vector<std::string> &x, const char &flag) {
  int l = cmd.find_first_not_of(flag), r = cmd.find_last_not_of(flag);
  int l_pointer = l, r_pointer = l;
  std::string carrier;
  for (int i = l; i <= r; ++i) {
    if (cmd[i] == flag) {
      r_pointer = i;
      carrier = cmd.substr(l_pointer, r_pointer - l_pointer);
      x.push_back(carrier);
      while (cmd[i] == flag && (i + 1) <= r)++i;
      l_pointer = i;
      i--;
    } else if (i == r) {
      r_pointer = i;
      carrier = cmd.substr(l_pointer, r_pointer - l_pointer + 1);
      x.push_back(carrier);
    }
  }
}

void CommandParser::Success() {
  std::cout << "0\n";
}
void CommandParser::Failure() {
  std::cout << "-1\n";
}

//-------userManager easy form-----------------
int CommandParser::ifULog(const std::string &username_) {
  return user_manager.isLogin(username_);
}
bool CommandParser::ifUReg(const string &username_) {
  return user_manager.isReg(username_);
}
//--------trainManager easy form---------
bool CommandParser::ifTAdd(const std::string &trainID_) {
  return train_manager.isAdded(trainID_);
}
bool CommandParser::ifTRel(const std::string &trainID_) {
  return train_manager.isReleased(trainID_);
}
JerryGJX::infoType CommandParser::GetInfoRank(char tag) {
  switch(tag){
    case 'c':return JerryGJX::_c;
    case 'd':return JerryGJX::_d;
    case 'f':return JerryGJX::_f;
    case 'g':return JerryGJX::_g;
    case 'i':return JerryGJX::_i;
    case 'm':return JerryGJX::_m;
    case 'n':return JerryGJX::_n;
    case 'o':return JerryGJX::_o;
    case 'p':return JerryGJX::_p;
    case 'q':return JerryGJX::_q;
    case 's':return JerryGJX::_s;
    case 't':return JerryGJX::_t;
    case 'u':return JerryGJX::_u;
    case 'x':return JerryGJX::_x;
    default:return JerryGJX::_y;
  }
}











