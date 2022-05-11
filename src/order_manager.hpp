#include "mydefs.hpp"
#include "ACMstl/bpTree.hpp"
#include <iostream>

struct Order {
  JerryGJX::orderStatusType orderStatus;
  JerryGJX::trainIDType trainID;
  JerryGJX::usernameType username;
  JerryGJX::stationType startStation;
  JerryGJX::stationType endStation;
  //todo
};

struct PendingOrder{};

class OrderManager {
  int orderNum;
 public:
  /**
   * @brief 为了由uid查订单，pair.second为订单号，用于防碰撞，bpt需要支持范围查询
   */
  Bptree<std::pair<JerryGJX::usernameType,int>,Order>orderDataBase;
  Bptree<std::pair<JerryGJX::trainIDType ,int>,PendingOrder>PendingOrderDataBase;
  void QueryOrder(std::string username);
  void AddOrder(std::string username,Order);
};

