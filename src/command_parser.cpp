//
// Created by JerryGuo on 2022/5/8.
//

#include "command_parser.hpp"
CommandParser::CommandParser(UserManager &user_manager_, TrainManager &train_manager_, OrderManager &order_manager_)
    : user_manager(user_manager_), train_manager(train_manager_), order_manager(order_manager_) {
  mapFunction.emplace("add_user", &CommandParser::ParseAddUser);
  mapFunction.emplace("login", &CommandParser::ParseLogin);
  mapFunction.emplace("logout", &CommandParser::ParseLogout);
  mapFunction.emplace("query_profile", &CommandParser::ParseQueryProfile);
  mapFunction.emplace("modify_profile", &CommandParser::ParseModifyProfile);
  mapFunction.emplace("add_train", &CommandParser::ParseAddTrain);
  mapFunction.emplace("delete_train", &CommandParser::ParseDeleteTrain);
  mapFunction.emplace("release_train", &CommandParser::ParseReleaseTrain);
  mapFunction.emplace("query_train", &CommandParser::ParseQueryTrain);
  mapFunction.emplace("query_ticket", &CommandParser::ParseQueryTicket);
  mapFunction.emplace("query_transfer", &CommandParser::ParseQueryTransfer);
  mapFunction.emplace("buy_ticket", &CommandParser::ParseBuyTicket);
  mapFunction.emplace("query_order", &CommandParser::ParseQueryOrder);
  mapFunction.emplace("refund_ticket", &CommandParser::ParseRefundTicket);
//  mapFunction.emplace("rollback", &CommandParser::ParseRollback);
//  mapFunction.emplace("clean", &CommandParser::ParseClean);
//  mapFunction.emplace("exit", &CommandParser::ParseExit);
}

void CommandParser::Run() {
  std::string parser_carrier;
  if (std::getline(std::cin, parser_carrier)) {

    if (std::cin.eof())exit(0);

    bool all_blank_flag = true;
    for (char i: parser_carrier) {
      if (i != ' ') {
        all_blank_flag = false;
        break;
      }
    }
    if (!all_blank_flag) {
      std::vector<std::string> parser_list;
      std::unordered_map<std::string, std::string> parser_list_to_use;

      SplitString(parser_carrier, parser_list, ' ');
      std::string timestamp = parser_list[0].substr(1, parser_list[0].length() - 2);
      //std::cout << "!!DEBUG!! " << timestamp << std::endl;
      int time_tag = std::stoi(timestamp);
      std::string cmd_type = parser_list[1];
      for (int i = 2; i < parser_list.size(); i += 2) {
        parser_list_to_use.emplace(parser_list[i], parser_list[i + 1]);
      }
      if (cmd_type == "exit") {

        user_manager.exit();

        std::cout << "bye\n";


        //exit(0);
      } else {
        if (mapFunction.find(cmd_type) != mapFunction.end()) {
          // PrintAll();
//          vector<string> split_parser;
//          while (!iss.eof()) {
//            string carrier;
//            iss >> carrier;
//            if (!carrier.empty()) split_parser.push_back(carrier);
//          }
          (this->*mapFunction[cmd_type])(parser_list_to_use);
        }
      }
    }
  }
}

//------------------user manager-----------------
void CommandParser::ParseAddUser(std::unordered_map<std::string, std::string> &cmd) {
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
void CommandParser::ParseLogin(std::unordered_map<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-u"]) || ifULog(cmd["-u"]) != -1 || !ifUPass(cmd["-u"], cmd["-p"])) {
    Failure();
  } else {
    user_manager.Login(cmd["-u"]);
    Success();
  }
}
void CommandParser::ParseLogout(std::unordered_map<std::string, std::string> &cmd) {
  if (ifULog(cmd["-u"]) == -1)Failure();
  else {
    user_manager.Logout(cmd["-u"]);
    Success();
  }
}
void CommandParser::ParseQueryProfile(std::unordered_map<std::string, std::string> &cmd) {
  int prv_c = ifULog(cmd["-c"]);
  std::vector<std::string> result;
  if (prv_c == -1 || !user_manager.queryProfile(cmd["-u"], result, prv_c, cmd["-c"]))Failure();
  else {
    for (auto &i: result)std::cout << i << " ";
    std::cout << "\n";
  }

}
void CommandParser::ParseModifyProfile(std::unordered_map<std::string, std::string> &cmd) {
  int prv_c = ifULog(cmd["-c"]);
  std::vector<std::string> result;
  if (prv_c == -1 || !user_manager.modifyProfile(cmd["-u"], cmd, result, prv_c, cmd["-c"]))Failure();
  else {
    for (auto &i: result)std::cout << i << " ";
    std::cout << "\n";
  }
}

//------------------train manager-----------------
void CommandParser::ParseAddTrain(std::unordered_map<std::string, std::string> &cmd) {
  if (ifTAdd(cmd["-i"])) {
    Failure();
    return;
  }

  std::vector<std::string> stations, _prices, _travelTimes, _stopoverTimes, saleDate;
  std::vector<int> prices, travelTimes, stopoverTimes;
  SplitString(cmd["-s"], stations, '|');
  SplitString(cmd["-p"], _prices, '|');
  SplitString(cmd["-t"], _travelTimes, '|');
  SplitString(cmd["-o"], _stopoverTimes, '|');
  SplitString(cmd["-d"], saleDate, '|');
  for (auto &_price: _prices)prices.push_back(std::stoi(_price));
  for (auto &_travelTime: _travelTimes)travelTimes.push_back(std::stoi(_travelTime));
  for (auto &_stopoverTime: _stopoverTimes)stopoverTimes.push_back(std::stoi(_stopoverTime));
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
void CommandParser::ParseDeleteTrain(std::unordered_map<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"]) || ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  train_manager.deleteTrain(cmd["-i"]);
  Success();
}
void CommandParser::ParseReleaseTrain(std::unordered_map<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"]) || ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  train_manager.releaseTrain(cmd["-i"]);
  Success();
}
void CommandParser::ParseQueryTrain(std::unordered_map<std::string, std::string> &cmd) {
  if (!ifTAdd(cmd["-i"])) {
    Failure();
    return;
  }
  std::vector<std::string> result;
  train_manager.queryTrain(cmd["-i"], cmd["-d"], result);
  if (result[0] == "-1") {
    std::cout << result[0] << "\n";
    return;
  }
  std::cout << result[0] << " ";
  for (int i = 1; i < result.size(); ++i)std::cout << result[i] << "\n";
  //Success();
}
void CommandParser::ParseQueryTicket(std::unordered_map<std::string, std::string> &cmd) {
  std::vector<std::string> result;
  train_manager.QueryTicket(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}
void CommandParser::ParseQueryTransfer(std::unordered_map<std::string, std::string> &cmd) {
  std::vector<std::string> result;
  train_manager.QueryTransfer(cmd, result);
  for (auto &i: result)std::cout << i << "\n";
}

void CommandParser::ParseBuyTicket(std::unordered_map<std::string, std::string> &cmd) {
  if (!ifUReg(cmd["-u"]) || ifULog(cmd["-u"]) == -1 || !ifTAdd(cmd["-i"]) || !ifTRel(cmd["-i"])) {
    Failure();
    return;
  }
  std::string ans = train_manager.BuyTicket(cmd, order_manager);
  std::cout << ans << "\n";
}

void CommandParser::ParseRefundTicket(std::unordered_map<std::string, std::string> &cmd) {
  if (ifULog(cmd["-u"]) == -1) {
    Failure();
    return;
  }
  if (train_manager.RefundTicket(cmd["-u"], std::stoi(cmd["-n"]), order_manager))Success();
  else Failure();

}

//------------------order manager-----------------
void CommandParser::ParseQueryOrder(std::unordered_map<std::string, std::string> &cmd) {
  if (ifULog(cmd["-u"]) == -1) {
    Failure();
    return;
  }
  std::vector<std::string> ans;
  order_manager.QueryOrder(cmd["-u"], ans);
  std::cout << ans[0] << "\n";
  for (int i = ans.size() - 1; i >= 1; --i)std::cout << ans[i] << "\n";
}


//---------------------tool----------------------

void CommandParser::SplitString(const std::string &cmd, std::vector<std::string> &x, const char &flag) {
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








