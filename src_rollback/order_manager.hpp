//
// Created by JerryGuo on 2022/5/14.
//

#ifndef TRAIN_MANAGER_HPP__ORDER_MANAGER_HPP_
#define TRAIN_MANAGER_HPP__ORDER_MANAGER_HPP_

#include "mydefs.hpp"
#include "tools/Time.hpp"
#include "ACMstl/bptree_rollback.hpp"
#include "ACMstl/Vector.hpp"

#include <iostream>
//#include <map>
//#include <vector>

using JerryGJX::ull;

struct Order {
  JerryGJX::orderStatusType orderStatus{};
  JerryGJX::trainIDType trainID{};
  JerryGJX::usernameType username{};
  int startRank{}, endRank{};//要求是trainManager产生
  JerryGJX::stationType startStation{}, endStation{};
  JerryGJX::CalendarTime startDay{};
  int startTime{}, leavingTime{}, arrivingTime{};//全部用分钟为单位，后两者为绝对分钟数
  int orderID{}, price{}, num{};
  //todo
  Order() = default;
  Order(JerryGJX::orderStatusType order_status,
        const JerryGJX::trainIDType &train_id,
        const JerryGJX::usernameType &username,
        int start_rank,
        int end_rank,
        const JerryGJX::stationType &start_station,
        const JerryGJX::stationType &end_station,
        int start_day,
        int start_time,
        int leaving_time,
        int arriving_time,
        int order_id,
        int price,
        int num);
};

struct PendingOrder {
  ull tidHash{}, uidHash{};
  int startRank{}, endRank{};
  int orderID{}, num{};
  JerryGJX::CalendarTime startDay{};

  PendingOrder() = default;
  PendingOrder(ull tid_hash,
               ull uid_hash,
               int start_rank,
               int end_rank,
               int order_id,
               int num,
               int start_day)
      : tidHash(tid_hash),
        uidHash(uid_hash),
        startRank(start_rank),
        endRank(end_rank),
        orderID(order_id),
        num(num) { startDay += start_day; }
};

class OrderManager {
  /**
    * @brief 为了由uid查订单，pair.second为订单号，用于防碰撞，bpt需要支持范围查询
    */
  template<class T1, class T2>
  class PairHash {
   public:
    ull operator()(const std::pair<T1, T2> &ValueType) const {
      return ValueType.first + ValueType.second;
    }
  };

  class PairPairHash {
   public:
    ull operator()(const std::pair<std::pair<int, ull>, int> &ValueType) const {
      return ValueType.first.first+ValueType.first.second + ValueType.second;
    }
  };


  int TimeTag=0;
  Bptree<std::pair<ull, int>, Order, 339, 38,PairHash<ull,int>> orderDataBase;//hashUid,oid
  Bptree<std::pair<std::pair<int, ull>, int>, PendingOrder, 254, 144,PairPairHash> pendingQueue;//(第几天(指始发天数)，hash(trainID)),oid

  //  void OrderDataBase_RangeFind(const std::pair<ull, int> &lp,
//                               const std::pair<ull, int> &rp, std::vector<Order> &result);
//
//  void PendingQueue_RangeFind(const std::pair<std::pair<int, ull>, int> &lp,
//                              const std::pair<std::pair<int, ull>, int> &rp,
//                              std::vector<PendingOrder> &result);
  std::hash<std::string> hash_str;
 public:
  OrderManager(const std::string &filenameO, const std::string &filenameP);

  int QueryOid();
  ull CalHash(const std::string &str_);
  std::string OrderStr(Order &order_);

  void QueryOrderPrivate(const std::string &username_, sjtu::vector<Order> &result);

  void QueryOrder(const std::string &username_, sjtu::vector<std::string> &result);

  void QueryPendingOrderPrivate(int date_, ull tidHash_, sjtu::vector<PendingOrder> &result);

  void AddOrder(const std::string &username_, Order &order_);

  void RemoveOrder(ull uidHash_, int Oid_);

  void AddPendingOrder(int date_, ull tidHash_, int Oid_, PendingOrder &pending_order_);

  void RemovePendingOrder(int date_, ull tidHash_, int Oid_);

  void PendingToSuccess(ull uidHash_, int orderID_);//修改order情况

  void Clean();

  void Exit();

  void RollBack(int target_time);

  void GetTime(int time_tag);
};

#endif //TRAIN_MANAGER_HPP__ORDER_MANAGER_HPP_
