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

//-------------------------------class UserManager--------------------------------------
UserManager::UserManager(const string &filename) : userDatabase(filename) {
  isEmpty = true;
}

bool UserManager::Empty() const {
  return isEmpty;
}

unsigned long UserManager::CalHash(const string &username_) {
  return hash_str(username_);
}

void UserManager::AddUser(const std::string &username_,
                          const std::string &password_,
                          const std::string &name_,
                          const std::string &mailAddr_,
                          int privilege_) {
  if (isEmpty)isEmpty = false;
  User freshman(username_, password_, name_, mailAddr_, privilege_);
  userDatabase.insert(CalHash(username_), freshman);
}
bool UserManager::isRegistered(const std::string &username_) {
  User ca;
  return userDatabase.find(CalHash(username_), ca);
}

int UserManager::isLogin(const string &username_) {
  return onlineUser.find(CalHash(username_)) == onlineUser.end();
}

void UserManager::Login(const string &username_, const string &password_) {
  unsigned long Hash = UserManager::hash_str(username_);
  onlineUser.insert(make_pair(Hash, true));
}
void UserManager::Logout(const string &&username_) {
  onlineUser.erase(CalHash(username_));
}
void UserManager::queryProfile(const string &username_, vector<std::string> &result) {
  User ca;
  unsigned long Hash = UserManager::hash_str(username_);
  userDatabase.find(Hash, ca);
  result.push_back(ca.username);
  result.push_back(ca.name);
  result.push_back(ca.mailAddr);
  result.push_back(to_string(ca.privilege));
}
void UserManager::modifyProfile(const std::string &username_,
                                unordered_map<int, std::string> &info,
                                vector<std::string> &result) {
  if (info.empty()) {
    queryProfile(username_, result);
    return;
  }
  User ca;
  userDatabase.find(CalHash(username_), ca);
  if(info.find(1) != info.end())ca.password = info[1];
  if(info.find(2) != info.end())ca.name = info[1];
  if(info.find(3) != info.end())ca.mailAddr = info[1];
  if(info.find(4) != info.end())ca.privilege = JerryGJX::ToInt(info[4]);
  result.push_back(ca.username);
  result.push_back(ca.name);
  result.push_back(ca.mailAddr);
  result.push_back(to_string(ca.privilege));
}






