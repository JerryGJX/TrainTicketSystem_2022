#include "train_manager.hpp"

//-------------------class Train----------------------
Train::Train(const std::string &trainID_,
             int stationNum_,
             int totalSeatNum_,
             sjtu::vector<std::string> &stations_,
             sjtu::vector<int> &sumPrice_,
             int startTime_,
             sjtu::vector<int> &arrivingTimes_,//arrivingTime[0]无意义
             sjtu::vector<int> &leavingTimes_,
             JerryGJX::CalendarTime &startSellDate_,
             JerryGJX::CalendarTime &endSellDate_,
             char type_) {
  //isReleased = false;
  trainID = trainID_;
  stationNum = stationNum_;
  for (int i = 0; i < stations_.size(); ++i) stations[i] = stations_[i];
  totalSeatNum = totalSeatNum_;
  for (int i = 0; i < sumPrice_.size(); ++i) sumPrice[i] = sumPrice_[i];
  startTime = startTime_;
  for (int i = 0; i < arrivingTimes_.size(); ++i)arrivingTime[i] = arrivingTimes_[i];
  for (int i = 0; i < leavingTimes_.size(); ++i)leavingTime[i] = leavingTimes_[i];
  startSellDate = startSellDate_;
  endSellDate = endSellDate_;
  type = type_;
}

Train::Train(const Train &rhs) {
  //isReleased = rhs.isReleased;
  trainID = rhs.trainID;
  stationNum = rhs.stationNum;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i) stations[i] = rhs.stations[i];
  totalSeatNum = rhs.totalSeatNum;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i) sumPrice[i] = rhs.sumPrice[i];
  startTime = rhs.startTime;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)arrivingTime[i] = rhs.arrivingTime[i];
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)leavingTime[i] = rhs.leavingTime[i];
  startSellDate = rhs.startSellDate;
  endSellDate = rhs.endSellDate;
  type = rhs.type;
}
bool Train::operator<(const Train &rhs) const {
  return trainID < rhs.trainID;
}
bool Train::operator>(const Train &rhs) const {
  return rhs < *this;
}
bool Train::operator<=(const Train &rhs) const {
  return !(rhs < *this);
}
bool Train::operator>=(const Train &rhs) const {
  return !(*this < rhs);
}

//----------------dayTrain----------------
TrainManager::DayTrain::DayTrain(const TrainManager::DayTrain &rhs) {
  if (this == &rhs)return;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)seatRemain[i] = rhs.seatRemain[i];
}
int TrainManager::DayTrain::findMin(int lp, int rp) {
  int ans = INT32_MAX;
  for (int i = lp; i <= rp; ++i)ans = std::min(seatRemain[i], ans);
  return ans;
}
void TrainManager::DayTrain::rangeAdd(int lp, int rp, int d) {
  for (int i = lp; i <= rp; ++i) seatRemain[i] += d;
}
//-----------TrainStation--------------
TrainManager::TrainStation::TrainStation(const std::string &trainID_,
                                         JerryGJX::CalendarTime &startSellDate_,
                                         JerryGJX::CalendarTime &endSellDate_) {
  trainID = trainID_;
  startSaleDate = startSellDate_;
  endSaleDate = endSellDate_;
}
//------------------class TrainManager-----------------
TrainManager::TrainManager(const std::string &filename,
                           const std::string &filename_dtts,
                           const std::string &filename_sdb, const std::string &filename_rbu)
    : trainDataBase(filename),
      DayTrainToSeat(filename_dtts),
      stationDataBase(filename_sdb),
      releasedBackUp(filename_rbu) {
  if (releasedBackUp.size()) {
    sjtu::vector<std::pair<ull, bool>> ca;
    releasedBackUp.range_search(0, UINT64_MAX, ca);
    for (auto &T: ca)releasedDatabase.insert(T);
  }
}

ull TrainManager::CalHash(const std::string &str_) {
  return hash_str(str_);
}

void TrainManager::addTrain(const std::string &trainID_,
                            int stationNum_,
                            int totalSeatNum_,
                            sjtu::vector<std::string> &stations_,
                            sjtu::vector<int> &Price_,
                            const std::string &startTime_,
                            sjtu::vector<int> &travelTimes_,
                            sjtu::vector<int> &stopoverTimes_,
                            sjtu::vector<std::string> &saleDate_,
                            char type_) {
  //-------------cal arv_t lev_t--------------
  int startTime = JerryGJX::ClockTime(startTime_).ToMin();
  sjtu::vector<int> arrivingTimes;
  sjtu::vector<int> leavingTimes;
  int tmp = 0;
  arrivingTimes.push_back(tmp);
  tmp = startTime;
  leavingTimes.push_back(tmp);
  for (int i = 0; i < stationNum_ - 2; ++i) {
    tmp += travelTimes_[i];
    arrivingTimes.push_back(tmp);
    tmp += stopoverTimes_[i];
    leavingTimes.push_back(tmp);
  }
  tmp += travelTimes_[stationNum_ - 2];
  arrivingTimes.push_back(tmp);
  leavingTimes.push_back(INT32_MAX);

  //-------------station----------------
  sjtu::vector<std::string> stations;
  //stations.reserve(stationNum_);
  for (int i = 0; i < stationNum_; ++i)stations.push_back(stations_[i]);

//--------------price &seat remain--------------
  DayTrain dt_ca;
  sjtu::vector<int> sumPrice;
  sumPrice.push_back(0);

  dt_ca.rangeAdd(0, stationNum_ - 1, totalSeatNum_);

  for (int i = 0; i < stationNum_ - 1; ++i) {
    sumPrice.push_back(sumPrice[i] + Price_[i]);
  }

  //------------------st et-------------


  JerryGJX::CalendarTime startSellDate(saleDate_[0]);
  JerryGJX::CalendarTime endSellDate(saleDate_[1]);

  Train ca(trainID_,
           stationNum_,
           totalSeatNum_,
           stations,
           sumPrice,
           startTime,
           arrivingTimes,
           leavingTimes,
           startSellDate,
           endSellDate, type_);

  ull tidHash = CalHash(trainID_);
  trainDataBase.insert(std::make_pair(tidHash, ca));
  releasedDatabase.insert(std::make_pair(tidHash, false));
  releasedBackUp.insert(std::make_pair(tidHash, false));

  int s_date_rank = startSellDate.ToDay(), e_date_rank = endSellDate.ToDay();
  for (int i = s_date_rank; i <= e_date_rank; ++i) {
    DayTrainToSeat.insert(std::make_pair(std::make_pair(i, tidHash), dt_ca));
  }

  TrainStation train_station(trainID_, startSellDate, endSellDate);

  for (int i = 0; i < stationNum_; ++i) {
    train_station.station = stations[i];
    train_station.rank = i;
    train_station.priceSum = sumPrice[i];
    train_station.startTime = startTime;
    train_station.arrivingTime = arrivingTimes[i];
    train_station.leavingTime = leavingTimes[i];
    stationDataBase.insert(std::make_pair(std::make_pair(CalHash(stations[i]), CalHash(trainID_)), train_station));
  }
}

bool TrainManager::isAdded(const std::string &trainID_) {
  return releasedDatabase.find(CalHash(trainID_)) != releasedDatabase.end();
}

bool TrainManager::isReleased(const std::string &trainID_) {//要求先add
  return releasedDatabase[CalHash(trainID_)];
}

void TrainManager::deleteTrain(const std::string &trainID_) {
  Train tr_ca;
  trainDataBase.find(CalHash(trainID_), tr_ca);
  int s_date_rank = tr_ca.startSellDate.ToDay(), e_date_rank = tr_ca.endSellDate.ToDay();
  for (int i = s_date_rank; i <= e_date_rank; ++i) {
    DayTrainToSeat.erase(std::make_pair(i, CalHash(trainID_)));
  }
  releasedDatabase.erase(releasedDatabase.find(CalHash(trainID_)));
  trainDataBase.erase(CalHash(trainID_));
  for (int i = 0; i < tr_ca.stationNum; ++i) {
    stationDataBase.erase(std::make_pair(CalHash(tr_ca.stations[i]), CalHash(trainID_)));
  }
}

void TrainManager::releaseTrain(const std::string &trainID_) {
  releasedDatabase[CalHash(trainID_)] = true;
}

void TrainManager::queryTrain(const std::string &trainID_,
                              const std::string &date_,
                              sjtu::vector<std::string> &result) {
  JerryGJX::CalendarTime ca_t = date_;

  Train tr_ca;
  ull TrHash = CalHash(trainID_);
  trainDataBase.find(TrHash, tr_ca);
  DayTrain dt_ca;
  if (!DayTrainToSeat.find(std::make_pair(ca_t.ToDay(), TrHash), dt_ca)) {
    result.push_back("-1");
    return;
  }

  result.push_back(trainID_);
  std::string st_ca;
  result.push_back(st_ca + tr_ca.type);
  st_ca.clear();
  int add_min = 60 * ca_t.ToHour();
  JerryGJX::Time transfer;
  st_ca +=
      std::string(tr_ca.stations[0]) + " xx-xx xx:xx -> " + (transfer + tr_ca.leavingTime[0] + add_min).ToStr()
          + " ";
  st_ca += std::to_string(tr_ca.sumPrice[0]) + " " + std::to_string(dt_ca.seatRemain[0]);
  result.push_back(st_ca);

  for (int i = 1; i < tr_ca.stationNum - 1; ++i) {
    st_ca.clear();
    st_ca += std::string(tr_ca.stations[i]) + " " + (transfer + tr_ca.arrivingTime[i] + add_min).ToStr() + " -> "
        + (transfer + tr_ca.leavingTime[i] + add_min).ToStr() + " ";
    st_ca += std::to_string(tr_ca.sumPrice[i]) + " " + std::to_string(dt_ca.seatRemain[i]);
    result.push_back(st_ca);
  }
  st_ca.clear();
  st_ca += std::string(tr_ca.stations[tr_ca.stationNum - 1]) + " "
      + (transfer + tr_ca.arrivingTime[tr_ca.stationNum - 1] + add_min).ToStr() + " -> " + "xx-xx xx:xx" + " ";
  st_ca += std::to_string(tr_ca.sumPrice[tr_ca.stationNum - 1]) + " " + "x";
  result.push_back(st_ca);

}

void TrainManager::QueryTicket(sjtu::linked_hashmap<std::string, std::string> &info,
                               sjtu::vector<std::string> &result) {
  JerryGJX::CalendarTime wanted_date(info["-d"]);
  JerryGJX::Time ans_time(info["-d"], 1);
  std::string start_station = info["-s"], terminal_station = info["-t"];
  ull start_hash = CalHash(start_station), terminal_hash = CalHash(terminal_station);
  sjtu::vector<TrainStation> result_start, result_terminal;
  stationDataBase.range_search(std::make_pair(start_hash, 0), std::make_pair(start_hash + 1, 0), result_start);
  stationDataBase.range_search(std::make_pair(terminal_hash, 0), std::make_pair(terminal_hash + 1, 0), result_terminal);
  sjtu::linked_hashmap<ull, int> find_same;
  for (int i = 0; i < result_start.size(); ++i) {
    if (result_start[i].startSaleDate + result_start[i].leavingTime / (60 * 24) <= wanted_date
        && result_start[i].endSaleDate + result_start[i].leavingTime / (60 * 24) >= wanted_date) {
      if (isReleased(result_start[i].trainID))
        find_same.insert(std::make_pair(CalHash(result_start[i].trainID), i));
    }
  }
  sjtu::map<std::pair<int, JerryGJX::trainIDType>, std::pair<int, int>> possible_ans;

  bool if_time = false;
  if (info.find("-p") != info.end() && info["-p"] == "time") if_time = true;

  for (int i = 0; i < result_terminal.size(); ++i) {
    if (find_same.find(CalHash(result_terminal[i].trainID)) != find_same.end()) {
      int cnt = find_same.find(CalHash(result_terminal[i].trainID))->second;
      if (result_start[cnt].rank < result_terminal[i].rank) {
        int ranker;
        if (if_time)ranker = result_terminal[i].arrivingTime - result_start[cnt].leavingTime;
        else ranker = result_terminal[i].priceSum - result_start[cnt].priceSum;
        possible_ans.insert(std::make_pair(std::make_pair(ranker, result_start[cnt].trainID), std::make_pair(cnt, i)));
      }
    }
  }

  result.push_back(std::to_string(possible_ans.size()));

  int tot_num = 0;
  for (auto i = possible_ans.begin(); i != possible_ans.end(); i++) {

    int levT_f = result_start[i->second.first].leavingTime, arvT_t = result_terminal[i->second.second].arrivingTime,
        f_rank = result_start[i->second.first].rank, t_rank = result_terminal[i->second.second].rank;

    std::string _trainID = result_start[i->second.first].trainID;

    ull tidHash = CalHash(_trainID);

    Train wanted_train;
    trainDataBase.find(tidHash, wanted_train);
    //int day_one_tag,day_two_tag,day_three_tag
    int start_date = wanted_date.ToDay() - levT_f / (24 * 60);
    DayTrain dt_ca;
    DayTrainToSeat.find(std::make_pair(start_date, CalHash(_trainID)), dt_ca);
    int _seat = dt_ca.findMin(f_rank, t_rank - 1);
//    if (!_seat)continue;
//    tot_num++;
    JerryGJX::Time _leaving_time = ans_time + result_start[i->second.first].leavingTime % (24 * 60);
    JerryGJX::Time _arrival_time =
        _leaving_time + (result_terminal[i->second.second].arrivingTime - result_start[i->second.first].leavingTime);
    int _price = result_terminal[i->second.second].priceSum - result_start[i->second.first].priceSum;
    std::string _ans = _trainID;
    _ans += " " + start_station + " " + _leaving_time.ToStr() + " -> ";
    _ans += terminal_station + " " + _arrival_time.ToStr() + " " + std::to_string(_price) + " " + std::to_string(_seat);
    result.push_back(_ans);
  }
  //result[0] = std::to_string(tot_num);

}

void TrainManager::QueryTransfer(sjtu::linked_hashmap<std::string, std::string> &info,
                                 sjtu::vector<std::string> &result) {
  JerryGJX::CalendarTime wanted_date(info["-d"]);
  int wanted_date_toDay = wanted_date.ToDay();
  std::string start_station = info["-s"], terminal_station = info["-t"];
  ull start_hash = CalHash(start_station), terminal_hash = CalHash(terminal_station);
  sjtu::vector<TrainStation> result_start, result_terminal;
  stationDataBase.range_search(std::make_pair(start_hash, 0), std::make_pair(start_hash + 1, 0), result_start);
  stationDataBase.range_search(std::make_pair(terminal_hash, 0), std::make_pair(terminal_hash + 1, 0), result_terminal);

  sjtu::linked_hashmap<ull, std::pair<int, int>> startTime_permit;//trainIDHash,pair(start_date,rank in result_start)
  for (int i = 0; i < result_start.size(); ++i) {
    if (result_start[i].startSaleDate + result_start[i].leavingTime / (60 * 24) <= wanted_date
        && result_start[i].endSaleDate + result_start[i].leavingTime / (60 * 24) >= wanted_date) {
      if (isReleased(result_start[i].trainID)) {
        int levT_f = result_start[i].leavingTime;
        int start_time = wanted_date_toDay - levT_f / (24 * 60);
        startTime_permit.insert(std::make_pair(CalHash(result_start[i].trainID), std::make_pair(start_time, i)));
      }
    }
  }

  Transfer best_choice;
  bool has_choice = false;
  bool if_time = false;
  if (info.find("-p") != info.end() && info["-p"] == "time") if_time = true;

  for (auto it_s = startTime_permit.begin(); it_s != startTime_permit.end(); it_s++) {
    int arr_rank = it_s->second.second;//rank in result_start
    int f_rank = result_start[arr_rank].rank;//出发站在该车的站中编号
    ull f_tr_hash = it_s->first;
    int start_date_1 = it_s->second.first;
    Train f_tr;
    trainDataBase.find(f_tr_hash, f_tr);
    DayTrain tr_1;
    DayTrainToSeat.find(std::make_pair(start_date_1, f_tr_hash), tr_1);
    sjtu::linked_hashmap<ull, std::pair<int, int>>
        StaAndArvMin;//stationHash to which min in the year/station rank of train
    for (int i = f_rank + 1; i < f_tr.stationNum; ++i) {
      int which_min = 24 * 60 * start_date_1 + f_tr.arrivingTime[i];
      StaAndArvMin.insert(std::make_pair(CalHash(f_tr.stations[i].str()), std::make_pair(which_min, i)));
    }

    for (auto &T: result_terminal) {
      ull t_tr_hash = CalHash(T.trainID.str());
      if (t_tr_hash == f_tr_hash)continue;
      Train t_tr;
      trainDataBase.find(t_tr_hash, t_tr);
      if (!isReleased(t_tr.trainID.str()))continue;
      for (int i = 0; i < T.rank; ++i) {
        std::string trans_station = t_tr.stations[i].str();
        auto iter = StaAndArvMin.find(CalHash(trans_station));
        if (iter == StaAndArvMin.end())continue;
        int last_leaving_minute = 24 * 60 * t_tr.endSellDate.ToDay() + t_tr.leavingTime[i];
        int minute_diff = last_leaving_minute - iter->second.first;
        if (minute_diff < 0)continue;
        int best_trans_day = std::max(t_tr.startSellDate.ToDay(), t_tr.endSellDate.ToDay() - (minute_diff) / (24 * 60));
        int start_time_1 = 24 * 60 * start_date_1 + f_tr.leavingTime[f_rank];
        int end_time_1 = 24 * 60 * start_date_1 + f_tr.arrivingTime[iter->second.second];
        int start_time_2 = 24 * 60 * best_trans_day + t_tr.leavingTime[i];
        int end_time_2 = 24 * 60 * best_trans_day + t_tr.arrivingTime[T.rank];
        int cost_1 = f_tr.sumPrice[iter->second.second] - f_tr.sumPrice[f_rank];
        int cost_2 = t_tr.sumPrice[T.rank] - t_tr.sumPrice[i];
        int trans_time = start_time_2 - end_time_1;
        DayTrain tr_2;
        DayTrainToSeat.find(std::make_pair(best_trans_day, t_tr_hash), tr_2);
        int seat_1 = tr_1.findMin(f_rank, iter->second.second - 1);
        int seat_2 = tr_2.findMin(i, T.rank - 1);
        Ticket tk_1(f_tr.trainID.str(), start_station, trans_station, start_time_1, end_time_1, cost_1, seat_1);
        Ticket tk_2(t_tr.trainID.str(), trans_station, terminal_station, start_time_2, end_time_2, cost_2, seat_2);

        if (!has_choice) {
          best_choice = {tk_1, tk_2};
          has_choice = true;
        } else {
//          if (if_time) {
//            int tot_time_now = tk_1.lastTime() + tk_2.lastTime() + trans_time;
//            if (tot_time_now > best_choice.totTime())continue;
//            else if (tot_time_now < best_choice.totTime() ||
//                tk_1.lastTime() < best_choice.tk1.lastTime())
//              best_choice = {tk_1, tk_2};
//          } else {
//            int tot_cost_now = cost_1 + cost_2;
//            if (tot_cost_now > best_choice.totCost())continue;
//            else if (tot_cost_now < best_choice.totCost())best_choice = {tk_1, tk_2};
//            else {
//              if (tk_1.lastTime() < best_choice.tk1.lastTime())best_choice = {tk_1, tk_2};
//            }
//          }

          int tot_time_now = tk_1.lastTime() + tk_2.lastTime() + trans_time;
          int tot_cost_now = cost_1 + cost_2;
          if (if_time) {
            if (tot_time_now > best_choice.totTime())continue;
            else if (tot_time_now < best_choice.totTime())best_choice = {tk_1, tk_2};
            else {
              if (tot_cost_now < best_choice.totCost())best_choice = {tk_1, tk_2};
              else {
                if (tk_1.trainID < best_choice.tk1.trainID ||
                    tk_1.trainID == best_choice.tk1.trainID && tk_2.trainID < best_choice.tk2.trainID)
                  best_choice = {tk_1, tk_2};
              }
            }
          } else {
            if (tot_cost_now > best_choice.totCost())continue;
            else if (tot_cost_now < best_choice.totCost())best_choice = {tk_1, tk_2};
            else {
              if (tot_cost_now < best_choice.totCost())best_choice = {tk_1, tk_2};
              else {
                if (tk_1.trainID < best_choice.tk1.trainID ||
                    tk_1.trainID == best_choice.tk1.trainID && tk_2.trainID < best_choice.tk2.trainID)
                  best_choice = {tk_1, tk_2};
              }
            }
          }

        }
      }
    }
  }

  if (!has_choice)result.push_back("0");
  else {
    result.push_back(best_choice.tk1.Print());
    result.push_back(best_choice.tk2.Print());
  }
}

std::string TrainManager::BuyTicket(sjtu::linked_hashmap<std::string, std::string> &info,
                                    OrderManager &order_manager_) {
  int wanted_date = JerryGJX::CalendarTime(info["-d"]).ToDay();
  ull tidHash = CalHash(info["-i"]);
  //由于已检查过release，则一定存在
  Train wanted_train;
  trainDataBase.find(tidHash, wanted_train);
  int levT_f = 0, arvT_t = 0, f_rank = 0, t_rank = 0;
  for (int i = 0; i < wanted_train.stationNum; ++i) {
    if (wanted_train.stations[i].str() == info["-f"]) {
      levT_f = wanted_train.leavingTime[i];
      f_rank = i;
    }
    if (wanted_train.stations[i].str() == info["-t"]) {
      if (!levT_f)return "-1";
      else {
        arvT_t = wanted_train.arrivingTime[i];
        t_rank = i;
        break;
      }
    }
  }
  if (!levT_f || !arvT_t)return "-1";


  //int day_one_tag,day_two_tag,day_three_tag

  int start_date = wanted_date - (levT_f / (24 * 60));
  JerryGJX::CalendarTime start_day;
  start_day += start_date;

  DayTrain dt_ca;
  int seat = wanted_train.totalSeatNum;
  int wanted_seat = std::stoi(info["-n"]);
  if (!DayTrainToSeat.find(std::make_pair(start_date, tidHash), dt_ca)) return "-1";

  seat = dt_ca.findMin(f_rank, t_rank - 1);

  if (seat < wanted_seat && info["-q"] == "false")return "-1";

  int price = wanted_train.sumPrice[t_rank] - wanted_train.sumPrice[f_rank];

  int Oid = order_manager_.QueryOid() + 1;

  Order or_ca(JerryGJX::SUCCESS, info["-i"], info["-u"], f_rank, t_rank,
              info["-f"], info["-t"], start_day, wanted_train.startTime, levT_f,
              arvT_t, Oid, price, wanted_seat);

  if (seat >= wanted_seat) {
    dt_ca.rangeAdd(f_rank, t_rank - 1, -wanted_seat);
    DayTrainToSeat.erase(std::make_pair(start_date, tidHash));
    DayTrainToSeat.insert(std::make_pair(std::make_pair(start_date, tidHash), dt_ca));
    order_manager_.AddOrder(info["-u"], or_ca);
    return std::to_string(price * wanted_seat);
  } else {
    if (info.find("-q") != info.end() && info["-q"] == "true") {
      or_ca.orderStatus = JerryGJX::PENDING;
      order_manager_.AddOrder(info["-u"], or_ca);
      PendingOrder por_ca(CalHash(info["-i"]),
                          CalHash(info["-u"]),
                          f_rank,
                          t_rank,
                          Oid,
                          wanted_seat,
                          start_day);
      order_manager_.AddPendingOrder(start_date, por_ca.tidHash, Oid, por_ca);
      return "queue";
    } else return "-1";
  }
}

bool TrainManager::RefundTicket(const std::string &username_, int rank_, OrderManager &order_manager_) {
  sjtu::vector<Order> order_list;
  order_manager_.QueryOrderPrivate(username_, order_list);
  int ra_ca = (int) order_list.size() - rank_;
  if (ra_ca < 0)return false;
  Order target_order = order_list[ra_ca];
  if (target_order.orderStatus == JerryGJX::REFUNDED)return false;
  else if (target_order.orderStatus == JerryGJX::PENDING) {
    order_manager_.RemoveOrder(CalHash(target_order.username.str()), target_order.orderID);
    target_order.orderStatus = JerryGJX::REFUNDED;
    order_manager_.AddOrder(target_order.username.str(), target_order);
    order_manager_.RemovePendingOrder(target_order.startDay.ToDay(),
                                      CalHash(target_order.trainID.str()),
                                      target_order.orderID);
  } else {
    int start_date = target_order.startDay.ToDay();
    ull trHash = CalHash(target_order.trainID.str());
    order_manager_.RemoveOrder(CalHash(target_order.username.str()), target_order.orderID);
    target_order.orderStatus = JerryGJX::REFUNDED;
    order_manager_.AddOrder(target_order.username.str(), target_order);
    sjtu::vector<PendingOrder> pending_order_list;
    order_manager_.QueryPendingOrderPrivate(start_date, trHash, pending_order_list);

    int f_rank = target_order.startRank, t_rank = target_order.endRank;
    DayTrain dt_ca;
    DayTrainToSeat.find(std::make_pair(start_date, trHash), dt_ca);
    dt_ca.rangeAdd(f_rank, t_rank - 1, target_order.num);

    for (int i = 0; i < pending_order_list.size(); ++i) {
      PendingOrder pod_ca = pending_order_list[i];
      if (pod_ca.startRank >= t_rank || pod_ca.endRank <= f_rank
          || dt_ca.findMin(pod_ca.startRank, pod_ca.endRank - 1) < pod_ca.num)
        continue;
      dt_ca.rangeAdd(pod_ca.startRank, pod_ca.endRank - 1, -pod_ca.num);
      order_manager_.PendingToSuccess(pod_ca.uidHash, pod_ca.orderID);
      order_manager_.RemovePendingOrder(start_date, trHash, pod_ca.orderID);
    }
    DayTrainToSeat.erase(std::make_pair(start_date, trHash));
    DayTrainToSeat.insert(std::make_pair(std::make_pair(start_date, trHash), dt_ca));
  }
  return true;
}
void TrainManager::Clean() {
  trainDataBase.clear();
  releasedDatabase.clear();
  DayTrainToSeat.clear();
  stationDataBase.clear();
  releasedBackUp.clear();
}
void TrainManager::Exit() {
  for (auto &T: releasedDatabase) {
    releasedBackUp.modify(T.first, T.second);
  }
}

//----------tools------------------
//void TrainManager::StationDataBase_RangeFind(const std::pair<ull, ull> &lp,
//                                             const std::pair<ull, ull> &rp,
//                                             sjtu::vector<TrainStation> &result) {
//  auto iter = stationDataBase.upper_bound(lp);
//  while (iter->first < rp) {
//    if (iter == stationDataBase.end())break;
//    result.push_back(iter->second);
//    iter++;
//
//  }
//}

std::string TrainManager::Ticket::Print() {
  JerryGJX::Time ca;
  std::string start_t = (ca + start_time).ToStr();
  std::string end_t = (ca + end_time).ToStr();
  std::string ans =
      trainID + " " + startStation + " " + start_t + " -> "
          + endStation + " " + end_t + " " + std::to_string(cost)
          + " " + std::to_string(seat);
  return ans;
}
