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
UserManager::UserManager(const std::string &filename)
//:userDatabase(filename)
{
  isEmpty = true;
}

bool UserManager::Empty() const {
  return isEmpty;
}

ull UserManager::CalHash(const std::string &username_) {
  return hash_str(username_);
}

void UserManager::AddUser(const std::string &username_,
                          const std::string &password_,
                          const std::string &name_,
                          const std::string &mailAddr_,
                          int privilege_) {
  if (isEmpty)isEmpty = false;
  User freshman(username_, password_, name_, mailAddr_, privilege_);
  userDatabase.insert(std::make_pair(CalHash(username_), freshman));
  onlineUser.insert(std::make_pair(CalHash(username_), std::make_pair(privilege_, false)));
}
bool UserManager::isRegistered(const std::string &username_) {
  return onlineUser.find(CalHash(username_)) != onlineUser.end();
}

int UserManager::isLogin(const std::string &username_) {
  if (onlineUser[CalHash(username_)].second)return onlineUser[CalHash(username_)].first;
  return -1;
}

bool UserManager::checkPassword(const std::string &username_, const std::string &password_) {
  JerryGJX::passwordType rhs = password_;
  JerryGJX::passwordType lhs = userDatabase.find(CalHash(username_))->second.password;
  return rhs == lhs;
}

void UserManager::Login(const std::string &username_) {
  onlineUser[CalHash(username_)].second = true;
}
void UserManager::Logout(const std::string &username_) {
  onlineUser[CalHash(username_)].second = false;
}
bool UserManager::queryProfile(const std::string &username_,
                               sjtu::vector<std::string> &result,
                               int prv_c,
                               const std::string &cur_user) {
  User ca;
  ull Hash = UserManager::hash_str(username_);
  if (userDatabase.find(Hash) == userDatabase.end())return false;
  ca = userDatabase.find(Hash)->second;
  if (ca.privilege >= prv_c && username_ != cur_user)return false;
  result.push_back(ca.username);
  result.push_back(ca.name);
  result.push_back(ca.mailAddr);
  result.push_back(std::to_string(ca.privilege));
  return true;
}
bool UserManager::modifyProfile(const std::string &username_,
                                std::unordered_map<std::string, std::string> &info,
                                sjtu::vector<std::string> &result, int prv_c, const std::string &cur_user) {
  if (info.empty()) return queryProfile(username_, result, prv_c, cur_user);

  User ca;
  ull Hash = UserManager::hash_str(username_);
  if (userDatabase.find(Hash) == userDatabase.end())return false;
  ca = userDatabase.find(Hash)->second;
  if (ca.privilege >= prv_c && username_ != cur_user)return false;

  if (info.find("-p") != info.end())ca.password = info["-p"];
  if (info.find("-n") != info.end())ca.name = info["-n"];
  if (info.find("-m") != info.end())ca.mailAddr = info["-m"];
  if (info.find("-g") != info.end()) {
    ca.privilege = std::stoi(info["-g"]);
    onlineUser[Hash].first = ca.privilege;
  }
  userDatabase.erase(Hash);
  userDatabase.insert(std::make_pair(Hash, ca));

  result.push_back(ca.username);
  result.push_back(ca.name);
  result.push_back(ca.mailAddr);
  result.push_back(std::to_string(ca.privilege));
  return true;
}
void UserManager::exit() {
  for(auto &U:onlineUser){
    U.second.second=false;
  }
}







