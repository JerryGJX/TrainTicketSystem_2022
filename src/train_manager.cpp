#include "train_manager.hpp"

//-------------------class Train----------------------
Train::Train(const std::string &trainID_,
             int stationNum_,
             int totalSeatNum_,
             std::vector<std::string> &stations_,
             std::vector<int> &sumPrice_,
             int startTime_,
             std::vector<int> &arrivingTimes_,//arrivingTime[0]无意义
             std::vector<int> &leavingTimes_,
             JerryGJX::CalendarTime &startSellDate_,
             JerryGJX::CalendarTime &endSellDate_,
             char type_) {
  isReleased = false;
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
  isReleased = rhs.isReleased;
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
                           const std::string &filename_sdb) {}
//    :
//  trainDataBase(filename),
//    DayTrainToSeat(filename_dtts),
//    stationDataBase(filename_sdb) {}

unsigned long TrainManager::CalHash(const std::string &str_) {
  return hash_str(str_);
}

void TrainManager::addTrain(const std::string &trainID_,
                            int stationNum_,
                            int totalSeatNum_,
                            std::vector<std::string> &stations_,
                            std::vector<int> &Price_,
                            const std::string &startTime_,
                            std::vector<int> &travelTimes_,
                            std::vector<int> &stopoverTimes_,
                            std::vector<std::string> &saleDate_,
                            char type_) {
  //-------------cal arv_t lev_t--------------
  int startTime = JerryGJX::ClockTime(startTime_).ToMin();
  std::vector<int> arrivingTimes;
  std::vector<int> leavingTimes;
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

  //-------------station----------------
  std::vector<std::string> stations;
  for (int i = 0; i < stationNum_; ++i)stations.push_back(stations_[i]);

//--------------price &seat remain--------------
  DayTrain dt_ca{};
  std::vector<int> sumPrice;
  sumPrice.push_back(0);

  dt_ca.seatRemain[0] = totalSeatNum_;
  for (int i = 0; i < stationNum_ - 1; ++i) {
    sumPrice.push_back(sumPrice[i] + Price_[i]);
    dt_ca.seatRemain[i + 1] = totalSeatNum_;
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

  trainDataBase.insert(std::make_pair(CalHash(trainID_), ca));
  releasedDatabase.insert(std::make_pair(CalHash(trainID_), false));

  int s_date_rank = startSellDate.ToDay(), e_date_rank = endSellDate.ToDay();
  for (int i = s_date_rank; i <= e_date_rank; ++i) {
    DayTrainToSeat.insert(std::make_pair(std::make_pair(i, CalHash(trainID_)), dt_ca));
  }

  TrainStation train_station(trainID_, startSellDate, endSellDate);

  for (int i = 0; i < stationNum_; ++i) {
    train_station.station = stations[i];
    train_station.rank = i;
    train_station.priceSum = sumPrice[i];
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
  Train tr = trainDataBase[CalHash(trainID_)];
  int s_date_rank = tr.startSellDate.ToDay(), e_date_rank = tr.endSellDate.ToDay();
  for (int i = s_date_rank; i <= e_date_rank; ++i) {
    DayTrainToSeat.erase(std::make_pair(i, CalHash(trainID_)));
  }
  releasedDatabase.erase(CalHash(trainID_));
  trainDataBase.erase(CalHash(trainID_));
  for (int i = 0; i < tr.stationNum; ++i) {
    stationDataBase.erase(std::make_pair(CalHash(tr.stations[i]), CalHash(trainID_)));
  }
}

void TrainManager::releaseTrain(const std::string &trainID_) {
  releasedDatabase[CalHash(trainID_)] = true;
}

void TrainManager::queryTrain(const std::string &trainID_, const std::string &date_, std::vector<std::string> &result) {
  JerryGJX::CalendarTime ca_t = date_;
  unsigned long TrHash = CalHash(trainID_), DaHash = CalHash(ca_t.ToStr());
  Train tr_ca = trainDataBase[TrHash];
  DayTrain dt_ca = DayTrainToSeat[std::make_pair(TrHash, DaHash)];
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
    st_ca += std::string(tr_ca.stations[i]) + (transfer + tr_ca.arrivingTime[i] + add_min).ToStr() + " -> "
        + (transfer + tr_ca.leavingTime[i] + add_min).ToStr() + " ";
    st_ca += std::to_string(tr_ca.sumPrice[i]) + " " + std::to_string(dt_ca.seatRemain[i]);
    result.push_back(st_ca);
  }
  st_ca.clear();
  st_ca +=
      std::string(tr_ca.stations[tr_ca.stationNum - 1])
          + (transfer + tr_ca.arrivingTime[tr_ca.stationNum - 1] + add_min).ToStr()
          + " -> " + "xx-xx xx:xx" + " ";
  st_ca += std::to_string(tr_ca.sumPrice[tr_ca.stationNum - 1]) + " " + "x";
  result.push_back(st_ca);

}

void TrainManager::QueryTicket(std::unordered_map<std::string, std::string> &info, std::vector<std::string> &result) {
  JerryGJX::CalendarTime wanted_time(info["-d"]);
  JerryGJX::Time ans_time(info["-d"], 1);
  int wanted_time_num = wanted_time.ToDay();
  std::string start_station = info["-s"], terminal_station = info["-t"];
  unsigned long start_hash = CalHash(start_station), terminal_hash = CalHash(terminal_station);
  std::vector<TrainStation> result_start, result_terminal;
  StationDataBase_RangeFind(std::make_pair(start_hash, 0), std::make_pair(start_hash + 1, 0), result_start);
  StationDataBase_RangeFind(std::make_pair(terminal_hash, 0), std::make_pair(terminal_hash + 1, 0), result_terminal);
  std::unordered_map<unsigned long, int> find_same;
  for (int i = 0; i < result_start.size(); ++i) {
    if (result_start[i].startSaleDate + result_start[i].leavingTime / (60 * 24) <= wanted_time
        && result_start[i].endSaleDate + result_start[i].leavingTime / (60 * 24) >= wanted_time) {
      find_same.insert(std::make_pair(CalHash(result_start[i].trainID), i));
    }
  }
  std::map<std::pair<int, JerryGJX::trainIDType>, std::pair<int, int>> possible_ans;

  if (info["-p"] == "time") {
    for (int i = 0; i < result_terminal.size(); ++i) {
      if (find_same.find(CalHash(result_terminal[i].trainID)) != find_same.end()) {
        int cnt = find_same.find(CalHash(result_terminal[i].trainID))->second;
        if (result_start[cnt].rank < result_terminal[i].rank) {
          int _time = result_terminal[i].arrivingTime - result_start[cnt].leavingTime;
          possible_ans.insert(std::make_pair(std::make_pair(_time, result_start[cnt].trainID), std::make_pair(cnt, i)));
        }
      }
    }
  } else {
    for (int i = 0; i < result_terminal.size(); ++i) {
      if (find_same.find(CalHash(result_terminal[i].trainID)) != find_same.end()) {
        int cnt = find_same.find(CalHash(result_terminal[i].trainID))->second;
        if (result_start[cnt].rank < result_terminal[i].rank) {
          int _cost = result_terminal[i].priceSum - result_start[cnt].priceSum;
          possible_ans.insert(std::make_pair(std::make_pair(_cost, result_start[cnt].trainID), std::make_pair(cnt, i)));
        }
      }
    }
  }

  result.push_back(std::to_string(possible_ans.size()));

  for (auto i = possible_ans.begin(); i != possible_ans.end(); i++) {
    int _rank = result_start[i->second.first].rank;
    std::string _trainID = result_start[i->second.first].trainID;
    int _seat = DayTrainToSeat[std::make_pair(wanted_time_num, CalHash(_trainID))].seatRemain[_rank];
    JerryGJX::Time _leaving_time = ans_time + result_start[i->second.first].leavingTime % (24 * 60);
    JerryGJX::Time _arrival_time =
        _leaving_time + (result_terminal[i->second.second].arrivingTime - result_start[i->second.first].leavingTime);
    int _price = result_terminal[i->second.second].priceSum - result_start[i->second.first].priceSum;
    std::string _ans = _trainID;
    _ans += " " + start_station + " " + _leaving_time.ToStr() + " -> ";
    _ans += terminal_station + " " + _arrival_time.ToStr() + " " + std::to_string(_price) + " " + std::to_string(_seat);
    result.push_back(_ans);
  }

}

void TrainManager::QueryTransfer(std::unordered_map<std::string, std::string> &info, std::vector<std::string> &result) {

}

//----------tools------------------
void TrainManager::StationDataBase_RangeFind(const std::pair<unsigned long, unsigned long> &lp,
                                             const std::pair<unsigned long, unsigned long> &rp,
                                             std::vector<TrainStation> &result) {
  auto iter = stationDataBase.upper_bound(std::make_pair(lp.first, 0));
  while (iter->first < rp) {
    result.push_back(iter->second);
    iter++;
  }
}


