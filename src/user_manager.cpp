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
  return onlineUser.find(CalHash(username_)) != onlineUser.end();
}

bool UserManager::AddUser(std::string *info) {
  if (userDatabase.size()) {
    User u_ca;
    if (userDatabase.find(CalHash(info[JerryGJX::_u]), u_ca))
      throw Error("Target User Is Added");


    if (!isLogin(info[JerryGJX::_c]))throw Error("Current User Isn't Online");
      //return false;

    if (onlineUser[CalHash(info[JerryGJX::_c])] <= std::stoi(info[JerryGJX::_g]))throw Error("Current Privilege Is Too Low");
      //return false;



      //return false;
  } else info[JerryGJX::_g] = std::to_string(10);

  User freshman(info[JerryGJX::_u], info[JerryGJX::_p], info[JerryGJX::_n], info[JerryGJX::_m], std::stoi(info[JerryGJX::_g]));
  userDatabase.insert(std::make_pair(CalHash(info[JerryGJX::_u]), freshman));
  return true;
}

bool UserManager::checkPassword(const std::string &username_, const std::string &password_) {
  JerryGJX::passwordType rhs = password_;
  User ca;
  userDatabase.find(CalHash(username_), ca);
  JerryGJX::passwordType lhs = ca.password;
  return rhs == lhs;
}

bool UserManager::Login(std::string *info) {
  User u_ca;
  if (!userDatabase.find(CalHash(info[JerryGJX::_u]), u_ca))
    throw Error("Target User Isn't Added");
    //return false;
  if (isLogin(info[JerryGJX::_u]))
    throw Error("Target User Already Online");
    //return false;
  if (!checkPassword(info[JerryGJX::_u], info[JerryGJX::_p]))
    throw Error("Wrong Password");
    //return false;
  onlineUser.insert(std::make_pair(CalHash(info[JerryGJX::_u]), u_ca.privilege));
  return true;
}
bool UserManager::Logout(std::string *info) {
  if (!isLogin(info[JerryGJX::_u]))
    throw Error("Target User Isn't Online");
    //return false;
  onlineUser.erase(onlineUser.find(CalHash(info[JerryGJX::_u])));
  return true;
}

bool UserManager::queryProfile(std::string *info,
                               std::string &result) {
  User ta_ca;
  if (!isLogin(info[JerryGJX::_c]))throw Error("Current User Isn't Online");
  if (!userDatabase.find(CalHash(info[JerryGJX::_u]), ta_ca))throw Error("Target User Isn't Added");


    //return false;
  if (onlineUser[CalHash(info[JerryGJX::_c])] <= ta_ca.privilege && info[JerryGJX::_c] != ta_ca.username.str())
    throw Error("Current Privilege Is Too Low");
    //return false;
  result = ta_ca.to_string();
  return true;
}
bool UserManager::modifyProfile(std::string *info,
                                std::string &result) {
  User ta_ca;
  if (!userDatabase.find(CalHash(info[JerryGJX::_u]), ta_ca))
    throw Error("Target User Isn't Added");

  if( !isLogin(info[JerryGJX::_c]))
    throw Error("Current User isn't online");
    //return false;
  if (onlineUser[CalHash(info[JerryGJX::_c])] <= ta_ca.privilege && info[JerryGJX::_c] != ta_ca.username.str())
    throw Error("Current Privilege Is Too Low");

    //return false;

  if (!info[JerryGJX::_p].empty())ta_ca.password = info[JerryGJX::_p];
  if (!info[JerryGJX::_n].empty())ta_ca.name = info[JerryGJX::_n];
  if (!info[JerryGJX::_m].empty())ta_ca.mailAddr = info[JerryGJX::_m];
  if (!info[JerryGJX::_g].empty()) {
    ta_ca.privilege = std::stoi(info[JerryGJX::_g]);
    if (ta_ca.privilege >= onlineUser[CalHash(info[JerryGJX::_c])])
      throw Error("Current Privilege Is Too Low");
//      return false;
    if (isLogin(info[JerryGJX::_u]))onlineUser[CalHash(info[JerryGJX::_u])] = ta_ca.privilege;
  }
  userDatabase.erase(CalHash(info[JerryGJX::_u]));
  userDatabase.insert(std::make_pair(CalHash(info[JerryGJX::_u]), ta_ca));

  result = ta_ca.to_string();
  return true;
}

void UserManager::Clean() {
  userDatabase.clear();
  onlineUser.clear();
}

void UserManager::Exit() {

}










