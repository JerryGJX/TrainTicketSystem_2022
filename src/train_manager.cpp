#include "train_manager.hpp"

//-------------------class Train----------------------
Train::Train(const std::string &trainID_,
             int stationNum_,
             int totalSeatNum_,
             vector<std::string> &stations_,
             vector<int> &sumPrice_,
             const std::string &startTime_,//经过tostr处理
             vector<std::string> &arrivingTimes_,
             vector<std::string> &leavingTimes_,
             string &startSellDate_,
             string &endSellDate_,
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

//------------------class TrainManager-----------------
TrainManager::TrainManager(const string &filename, const string &filename_dtts, const string &filename_sdb){}
//    :
//  trainDataBase(filename),
//    DayTrainToSeat(filename_dtts),
//    stationDataBase(filename_sdb) {}
void TrainManager::addTrain(const std::string &trainID_,
                            int stationNum_,
                            int totalSeatNum_,
                            std::vector<std::string> &stations,
                            std::vector<int> &Price_,
                            const std::string &startTime_,
                            std::vector<int> &travelTimes_,
                            std::vector<int> &stopoverTimes_,
                            std::vector<std::string> &saleDate_,
                            char type_) {
  std::vector<std::string> arrivingTimes;
  std::vector<std::string> leavingTimes;
  JerryGJX::Time tmp;
  arrivingTimes.push_back(tmp.ToStr());
  tmp = JerryGJX::Time(startTime_, 2);
  leavingTimes.push_back(tmp.ToStr());
  for (int i = 0; i < stationNum_ - 2; ++i) {
    tmp += travelTimes_[i];
    arrivingTimes.push_back(tmp.ToStr());
    tmp += stopoverTimes_[i];
    leavingTimes.push_back(tmp.ToStr());
  }
  tmp += travelTimes_[stationNum_ - 2];
  arrivingTimes.push_back(tmp.ToStr());

  std::vector<int> sumPrice;
  sumPrice.push_back(Price_[0]);
  for (int i = 1; i < stationNum_ - 1; ++i) sumPrice.push_back(sumPrice[i - 1] + Price_[i]);

  std::string startTime = JerryGJX::Time(startTime_, 2).ToStr();
  std::string startSellDate = JerryGJX::Time(saleDate_[0] + " 00:00", 3).ToStr();
  std::string endSellDate = JerryGJX::Time(saleDate_[1] + " 23:59", 3).ToStr();
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
  trainDataBase.insert(make_pair(ca.trainID, ca));
}


