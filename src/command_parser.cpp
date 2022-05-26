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
//  mapFunction.insert(std::make_pair("rollback", &CommandParser::ParseRollback));
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
      sjtu::linked_hashmap<std::string, std::string> parser_list_to_use;

      SplitString(parser_carrier, parser_list, ' ');
      std::cout << parser_list[0] << " ";
      std::string timestamp = parser_list[0].substr(1, parser_list[0].length() - 2);
      //std::cout << "!!DEBUG!! " << timestamp << std::endl;
      int time_tag = std::stoi(timestamp);
      std::string cmd_type = parser_list[1];
      for (int i = 2; i < parser_list.size(); i += 2) {
        parser_list_to_use.insert(std::make_pair(parser_list[i], parser_list[i + 1]));
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
void CommandParser::ParseAddUser(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (user_manager.Empty()) {
    user_manager.AddUser(cmd["-u"], cmd["-p"], cmd["-n"], cmd["-m"], 10);
    Success();
  } else {
    int prv_c = ifULog(cmd["-c"]);
    int prv_u = std::stoi(cmd["-g"]);
    bool exist = ifUReg(cmd["-u"]);
    if (exist || prv_c <= prv_u)Failure();
    else {
      user_manager.AddUser(cmd["-u"], cmd["-p"], cmd["-n"], cmd["-m"], prv_u);
      Success();
    }
  }
}
void CommandParser::ParseLogin(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-u"]) || ifULog(cmd["-u"]) != -1 || !ifUPass(cmd["-u"], cmd["-p"])) {
    Failure();
  } else {
    user_manager.Login(cmd["-u"]);
    Success();
  }
}
void CommandParser::ParseLogout(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-u"]) || ifULog(cmd["-u"]) == -1)Failure();
  else {
    user_manager.Logout(cmd["-u"]);
    Success();
  }
}
void CommandParser::ParseQueryProfile(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-c"]) || ifULog(cmd["-c"]) == -1)Failure();

  else{
    int prv_c = ifULog(cmd["-c"]);
    sjtu::vector<std::string> result;
    if (prv_c == -1 || !user_manager.queryProfile(cmd["-u"], result, prv_c, cmd["-c"]))Failure();
    else {
      for (auto &i: result)std::cout << i << " ";
      std::cout << "\n";
    }

  }
}
void CommandParser::ParseModifyProfile(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-c"]) || ifULog(cmd["-c"]) == -1)Failure();

  else{
    int prv_c = ifULog(cmd["-c"]);
    sjtu::vector<std::string> result;
    if (prv_c == -1 || !user_manager.modifyProfile(cmd["-u"], cmd, result, prv_c, cmd["-c"]))Failure();
    else {
      for (auto &i: result)std::cout << i << " ";
      std::cout << "\n";
    }

  }
}

//------------------train manager-----------------
void CommandParser::ParseAddTrain(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (ifTAdd(cmd["-i"])) {
    Failure();
    return;
  }

  sjtu::vector<std::string> stations, _prices, _travelTimes, _stopoverTimes, saleDate;
  sjtu::vector<int> prices, travelTimes, stopoverTimes;
  SplitString(cmd["-s"], stations, '|');
  SplitString(cmd["-p"], _prices, '|');
  SplitString(cmd["-t"], _travelTimes, '|');
  SplitString(cmd["-o"], _stopoverTimes, '|');
  SplitString(cmd["-d"], saleDate, '|');
  for (auto &_price: _prices)prices.push_back(std::stoi(_price));
  for (auto &_travelTime: _travelTimes)travelTimes.push_back(std::stoi(_travelTime));
  if (std::stoi(cmd["-n"]) > 2)
    for (auto &_stopoverTime: _stopoverTimes)
      stopoverTimes.push_back(std::stoi(_stopoverTime));
  char type = cmd["-y"][0];

  train_manager.addTrain(cmd["-i"],
                         std::stoi(cmd["-n"]),
                         std::stoi(cmd["-m"]),
                         stations,
                         prices,
                         cmd["-x"],
                         travelTimes,
                         stopoverTimes,
                         saleDate, type);
  Success();
}
void CommandParser::ParseDeleteTrain(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"]) || ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  train_manager.deleteTrain(cmd["-i"]);
  Success();
}
void CommandParser::ParseReleaseTrain(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"]) || ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  train_manager.releaseTrain(cmd["-i"]);
  Success();
}
void CommandParser::ParseQueryTrain(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"])) {
    Failure();
    return;
  }
  sjtu::vector<std::string> result;
  train_manager.queryTrain(cmd["-i"], cmd["-d"], result);
  if (result[0] == "-1") {
    std::cout << result[0] << "\n";
    return;
  }
  std::cout << result[0] << " ";
  for (int i = 1; i < result.size(); ++i)std::cout << result[i] << "\n";
  //Success();
}

void CommandParser::ParseQueryTicket(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  sjtu::vector<std::string> result;
  train_manager.QueryTicket(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}

void CommandParser::ParseQueryTransfer(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  sjtu::vector<std::string> result;
  train_manager.QueryTransfer(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}

void CommandParser::ParseBuyTicket(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-u"]) || ifULog(cmd["-u"]) == -1 || !ifTAdd(cmd["-i"]) || !ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  std::string ans = train_manager.BuyTicket(cmd, order_manager);
  std::cout << ans << "\n";
}

void CommandParser::ParseRefundTicket(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (ifULog(cmd["-u"]) == -1) {
    Failure();
    return;
  }
  if (train_manager.RefundTicket(cmd["-u"], std::stoi(cmd["-n"]), order_manager))Success();
  else Failure();

}

//------------------order manager-----------------
void CommandParser::ParseQueryOrder(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  if (ifULog(cmd["-u"]) == -1) {
    Failure();
    return;
  }
  sjtu::vector<std::string> ans;
  order_manager.QueryOrder(cmd["-u"], ans);
  std::cout << ans[0] << "\n";
  for (int i = ans.size() - 1; i >= 1; --i)std::cout << ans[i] << "\n";
}

void CommandParser::ParseClean(sjtu::linked_hashmap<std::string, std::string> &cmd) {
  user_manager.Clean();
  train_manager.Clean();
  order_manager.Clean();
  Success();
}
void CommandParser::ParseExit(sjtu::linked_hashmap<std::string, std::string> &cmd) {
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
bool CommandParser::ifUReg(const std::string &username_) {
  return user_manager.isRegistered(username_);
}
int CommandParser::ifULog(const std::string &username_) {
  return user_manager.isLogin(username_);
}
bool CommandParser::ifUPass(const std::string &username_, const std::string &password_) {
  return user_manager.checkPassword(username_, password_);
}

//--------trainManager easy form---------
bool CommandParser::ifTAdd(const std::string &trainID_) {
  return train_manager.isAdded(trainID_);
}
bool CommandParser::ifTRel(const std::string &trainID_) {
  return train_manager.isReleased(trainID_);
}









