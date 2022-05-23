//
// Created by JerryGuo on 2022/5/14.
//

#include "order_manager.hpp"

//--------------order--------------------
Order::Order(JerryGJX::orderStatusType order_status,
             const JerryGJX::trainIDType &train_id,
             const JerryGJX::usernameType &username,
             int start_rank,
             int end_rank,
             const JerryGJX::stationType &start_station,
             const JerryGJX::stationType &end_station,
             const JerryGJX::CalendarTime &start_day,
             int start_time,
             int leaving_time,
             int arriving_time,
             int order_id,
             int price,
             int num)
    : orderStatus(order_status),
      trainID(train_id),
      username(username),
      startRank(start_rank),
      endRank(end_rank),
      startStation(start_station),
      endStation(end_station),
      startDay(start_day),
      startTime(start_time),
      leavingTime(leaving_time),
      arrivingTime(arriving_time),
      orderID(order_id),
      price(price),
      num(num) {}

//------------pending order--------------




//----------------tools---------------
//void OrderManager::OrderDataBase_RangeFind(const std::pair<ull, int> &lp,
//                                           const std::pair<ull, int> &rp,
//                                           std::vector<Order> &result) {
//  auto iter = orderDataBase.upper_bound(lp);
//  while (iter->first < rp) {
//    if (iter == orderDataBase.end())break;
//    result.push_back(iter->second);
//    iter++;
//
//  }
//}
//void OrderManager::PendingQueue_RangeFind(const std::pair<std::pair<int, ull>, int> &lp,
//                                          const std::pair<std::pair<int, ull>, int> &rp,
//                                          std::vector<PendingOrder> &result) {
//  auto iter = pendingQueue.upper_bound(lp);
////  auto iter2 = pendingQueue.end();
////  int a = pendingQueue.size();
//  while (iter->first < rp) {
//    if (iter == pendingQueue.end())break;
//    result.push_back(iter->second);
//    iter++;
//  }
//}

//-------------orderManager------------------
OrderManager::OrderManager(const std::string &filenameO,const std::string &filenameP): orderDataBase(filenameO),
                                                                                       pendingQueue(filenameP) {}

int OrderManager::QueryOid() {
  return (int) orderDataBase.size();
}

ull OrderManager::CalHash(const std::string &str_) {
  return hash_str(str_);
}

std::string OrderManager::OrderStr(Order &order_) {
  std::string ans;
  switch (order_.orderStatus) {
    case JerryGJX::SUCCESS:ans += "[success] ";
      break;
    case JerryGJX::PENDING:ans += "[pending] ";
      break;
    case JerryGJX::REFUNDED:ans += "[refunded] ";
  }
  JerryGJX::Time transfer;
  transfer += order_.startDay.ToHour() * 60 ;
  ans +=order_.trainID.str() + " " + order_.startStation.str() + " " + (transfer + order_.leavingTime).ToStr() + " -> ";

  ans += order_.endStation.str() + " " + (transfer + order_.arrivingTime).ToStr() + " " + std::to_string(order_.price)
      + " " + std::to_string(order_.num);
  return ans;
}

void OrderManager::QueryOrderPrivate(const std::string &username_, sjtu::vector<Order> &result) {
  ull Hash = CalHash(username_);
  orderDataBase.range_search(std::make_pair(Hash, 0), std::make_pair(Hash + 1, 0), result);
}

void OrderManager::QueryOrder(const std::string &username_, sjtu::vector<std::string> &result) {//从旧到新
  sjtu::vector<Order> orderList;
  QueryOrderPrivate(username_, orderList);
  result.push_back(std::to_string(orderList.size()));
  for (auto &i: orderList) result.push_back(OrderStr(i));
}

void OrderManager::QueryPendingOrderPrivate(int date_, ull tidHash_, sjtu::vector<PendingOrder> &result) {
  pendingQueue.range_search(std::make_pair(std::make_pair(date_, tidHash_), 0),
                         std::make_pair(std::make_pair(date_, tidHash_ + 1), 0), result);
}

void OrderManager::AddOrder(const std::string &username_, Order &order_) {
  orderDataBase.insert(std::make_pair(std::make_pair(CalHash(username_), order_.orderID), order_));
}

void OrderManager::RemoveOrder(ull uidHash_, int Oid_) {
  orderDataBase.erase(std::make_pair(uidHash_, Oid_));
}

void OrderManager::AddPendingOrder(int date_, ull tidHash_, int Oid_, PendingOrder &pending_order_) {
  pendingQueue.insert(std::make_pair(std::make_pair(std::make_pair(date_, tidHash_), Oid_), pending_order_));
}
void OrderManager::RemovePendingOrder(int date_, ull tidHash_, int Oid_) {
  pendingQueue.erase(std::make_pair(std::make_pair(date_, tidHash_), Oid_));
}
void OrderManager::PendingToSuccess(ull uidHash_, int orderID_) {
  Order or_ca;
  orderDataBase.find(std::make_pair(uidHash_, orderID_),or_ca);
  orderDataBase.erase(std::make_pair(uidHash_, orderID_));
  or_ca.orderStatus = JerryGJX::SUCCESS;
  orderDataBase.insert(std::make_pair(std::make_pair(uidHash_, orderID_), or_ca));
}






