#include "user_manager.hpp"


//---------------------------------class User----------------------------------

User::User(const std::string &username_,
           const std::string &password_,
           const std::string &name_,
           const std::string &mailAddr_,
           int privilege_) {
  username = username_;
  password = password_;
  name = name_;
  mailAddr = mailAddr_;
  privilege = privilege_;
}
User::User(const User &lhs) {
  username = lhs.username;
  password = lhs.password;
  name = lhs.name;
  mailAddr = lhs.mailAddr;
  privilege = lhs.privilege;
}
bool User::operator<(const User &rhs) const {
  return username < rhs.username;
}
bool User::operator>(const User &rhs) const {
  return rhs < *this;
}
bool User::operator<=(const User &rhs) const {
  return !(rhs < *this);
}
bool User::operator>=(const User &rhs) const {
  return !(*this < rhs);

}

//-------------------------------class UserManager--------------------------------------
UserManager::UserManager(const std::string &filenameUD) : userDatabase(filenameUD) {}

//bool UserManager::Empty() const {
//  return userDatabase.size() == 0;
//}
//
ull UserManager::CalHash(const std::string &username_) {
  return hash_str(username_);
}

bool UserManager::isReg(const std::string &username_) {
  User u_ca;
  return userDatabase.find(CalHash(username_), u_ca);
}

bool UserManager::isLogin(const string &username_) {
  return onlineUser.find(username_) != onlineUser.end();
}

bool UserManager::AddUser(sjtu::linked_hashmap<std::string, std::string> &info) {
  if (userDatabase.size()) {
    if (!isLogin(info["-c"]))return false;
    if (onlineUser[info["-c"]] <= std::stoi(info["-g"]))return false;
    User u_ca;
    if (userDatabase.find(CalHash(info["-u"]), u_ca))return false;
  } else info["-g"] = std::to_string(10);

  User freshman(info["-u"], info["-p"], info["-n"], info["-m"], std::stoi(info["-g"]));
  userDatabase.insert(std::make_pair(CalHash(info["-u"]), freshman));
  return true;
}

bool UserManager::checkPassword(const std::string &username_, const std::string &password_) {
  JerryGJX::passwordType rhs = password_;
  User ca;
  userDatabase.find(CalHash(username_), ca);
  JerryGJX::passwordType lhs = ca.password;
  return rhs == lhs;
}

bool UserManager::Login(sjtu::linked_hashmap<std::string, std::string> &info) {
  User u_ca;
  if (!userDatabase.find(CalHash(info["-u"]), u_ca))return false;
  if (isLogin(info["-u"]))return false;
  if (!checkPassword(info["-u"], info["-p"]))return false;
  onlineUser.insert(std::make_pair(info["-u"], u_ca.privilege));
  return true;
}
bool UserManager::Logout(sjtu::linked_hashmap<std::string, std::string> &info) {
  if (!isLogin(info["-u"]))return false;
  onlineUser.erase(onlineUser.find(info["-u"]));
  return true;
}

bool UserManager::queryProfile(const sjtu::linked_hashmap<std::string, std::string> &info,
                               std::string &result) {
  User ta_ca;
  if (!userDatabase.find(CalHash(info["-u"]), ta_ca) || !isLogin(info["-c"]))return false;
  if (onlineUser[info["-c"]] <= ta_ca.privilege && info["-c"] != ta_ca.username.str())return false;
  result = ta_ca.to_string();
  return true;
}
bool UserManager::modifyProfile(sjtu::linked_hashmap<std::string, std::string> &info,
                                std::string &result) {
  User ta_ca;
  if (!userDatabase.find(CalHash(info["-u"]), ta_ca) || !isLogin(info["-c"]))return false;
  if (onlineUser[info["-c"]] <= ta_ca.privilege && info["-c"] != ta_ca.username.str())return false;

  if (info.find("-p") != info.end())ta_ca.password = info["-p"];
  if (info.find("-n") != info.end())ta_ca.name = info["-n"];
  if (info.find("-m") != info.end())ta_ca.mailAddr = info["-m"];
  if (info.find("-g") != info.end()) {
    ta_ca.privilege = std::stoi(info["-g"]);
    if (ta_ca.privilege >= onlineUser[info["-c"]])return false;
    if (isLogin(info["-u"]))onlineUser[info["-u"]] = ta_ca.privilege;
  }
  userDatabase.erase(CalHash(info["-u"]));
  userDatabase.insert(std::make_pair(CalHash(info["-u"]), ta_ca));

  result = ta_ca.to_string();
  return true;
}

void UserManager::Clean() {
  userDatabase.clear();
  onlineUser.clear();
}

void UserManager::Exit() {

}










