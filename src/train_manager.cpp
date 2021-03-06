#include "train_manager.hpp"

//-------------------class Train----------------------
Train::Train(const std::string &trainID_, sjtu::vector<std::string> &stations_,
             sjtu::vector<int> &sumPrice_,
             sjtu::vector<int> &arrivingTimes_, // arrivingTime[0]无意义
             sjtu::vector<int> &leavingTimes_)
{
  trainID = trainID_;
  for (int i = 0; i < stations_.size(); ++i)
    stations[i] = stations_[i];
  for (int i = 0; i < sumPrice_.size(); ++i)
    sumPrice[i] = sumPrice_[i];
  for (int i = 0; i < arrivingTimes_.size(); ++i)
    arrivingTime[i] = arrivingTimes_[i];
  for (int i = 0; i < leavingTimes_.size(); ++i)
    leavingTime[i] = leavingTimes_[i];
}

Train::Train(const Train &rhs)
{
  trainID = rhs.trainID;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)
    stations[i] = rhs.stations[i];
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)
    sumPrice[i] = rhs.sumPrice[i];
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)
    arrivingTime[i] = rhs.arrivingTime[i];
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)
    leavingTime[i] = rhs.leavingTime[i];
}

//----------------dayTrain----------------
TrainManager::DayTrain::DayTrain(const TrainManager::DayTrain &rhs)
{
  if (this == &rhs)
    return;
  for (int i = 0; i < JerryGJX::max_stationNum; ++i)
    seatRemain[i] = rhs.seatRemain[i];
}
int TrainManager::DayTrain::findMin(int lp, int rp)
{
  int ans = INT32_MAX;
  for (int i = lp; i <= rp; ++i)
    ans = std::min(seatRemain[i], ans);
  return ans;
}
void TrainManager::DayTrain::rangeAdd(int lp, int rp, int d)
{
  for (int i = lp; i <= rp; ++i)
    seatRemain[i] += d;
}
//-----------TrainStation--------------
TrainManager::TrainStation::TrainStation(const std::string &trainID_, int startSellDate_, int endSellDate_)
{
  trainID = trainID_;
  startSaleDate = startSellDate_;
  endSaleDate = endSellDate_;
}
//------------------class TrainManager-----------------
TrainManager::TrainManager(const std::string &filename,
                           const std::string &filename_dtts,
                           const std::string &filename_sdb, const std::string &filename_btb)
    : trainDataBase(filename),
      DayTrainToSeat(filename_dtts),
      stationDataBase(filename_sdb),
      basicTrainBackUp(filename_btb)
{
  if (basicTrainBackUp.size())
  {
    sjtu::vector<std::pair<ull, BasicTrain>> ca;
    basicTrainBackUp.range_search(0, UINT64_MAX, ca);
    for (auto &T : ca)
      basicTrainDatabase.insert(T);
    // basicTrainBackUp.clear();
  }
}

ull TrainManager::CalHash(const std::string &str_)
{
  return hash_str(str_); // todo
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
                            char type_)
{
  //-------------cal arv_t lev_t--------------
  int startTime = JerryGJX::ClockTime(startTime_).ToMin();
  sjtu::vector<int> arrivingTimes;
  sjtu::vector<int> leavingTimes;
  int tmp = 0;
  arrivingTimes.push_back(tmp);
  tmp = startTime;
  leavingTimes.push_back(tmp);
  for (int i = 0; i < stationNum_ - 2; ++i)
  {
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
  // stations.reserve(stationNum_);
  for (int i = 0; i < stationNum_; ++i)
    stations.push_back(stations_[i]);

  //--------------price &seat remain--------------
  DayTrain dt_ca;
  sjtu::vector<int> sumPrice;
  sumPrice.push_back(0);

  dt_ca.rangeAdd(0, stationNum_ - 1, totalSeatNum_);

  for (int i = 0; i < stationNum_ - 1; ++i)
  {
    sumPrice.push_back(sumPrice[i] + Price_[i]);
  }

  //------------------st et-------------

  JerryGJX::CalendarTime startSellDate(saleDate_[0]);
  JerryGJX::CalendarTime endSellDate(saleDate_[1]);

  Train tr_ca(trainID_, stations, sumPrice, arrivingTimes, leavingTimes);
  ull tidHash = CalHash(trainID_);
  trainDataBase.insert(std::make_pair(tidHash, tr_ca));

  int s_date_rank = startSellDate.ToDay(), e_date_rank = endSellDate.ToDay();

  BasicTrain b_tr_ca(trainID_, stationNum_, totalSeatNum_, false, startTime, s_date_rank, e_date_rank, type_);
  basicTrainDatabase.insert(std::make_pair(tidHash, b_tr_ca));
  basicTrainBackUp.insert(std::make_pair(tidHash, b_tr_ca));

  //  TrainStation train_station(trainID_, s_date_rank, e_date_rank);
  //
  //  for (int i = 0; i < stationNum_; ++i) {
  //    train_station.station = stations[i];
  //    train_station.rank = i;
  //    train_station.priceSum = sumPrice[i];
  //    train_station.startTime = startTime;
  //    train_station.arrivingTime = arrivingTimes[i];
  //    train_station.leavingTime = leavingTimes[i];
  //    stationDataBase.insert(std::make_pair(std::make_pair(CalHash(stations[i]), CalHash(trainID_)), train_station));
  //  }
}

bool TrainManager::isAdded(const std::string &trainID_)
{
  return basicTrainDatabase.find(CalHash(trainID_)) != basicTrainDatabase.end();
}

bool TrainManager::isReleased(const std::string &trainID_)
{ //要求先add
  if (!isAdded(trainID_))
    return false;
  return basicTrainDatabase.find(CalHash(trainID_))->second.isReleased;
}

void TrainManager::deleteTrain(const std::string &trainID_)
{
  ull tr_hash = CalHash(trainID_);
  Train tr_ca;
  trainDataBase.find(tr_hash, tr_ca);
  BasicTrain b_tr_ca = basicTrainDatabase[tr_hash];
  int s_date_rank = b_tr_ca.startSellDate, e_date_rank = b_tr_ca.endSellDate;
  for (int i = s_date_rank; i <= e_date_rank; ++i)
  {
    DayTrainToSeat.erase(std::make_pair(i, tr_hash));
  }
  basicTrainDatabase.erase(basicTrainDatabase.find(tr_hash));
  basicTrainBackUp.erase(tr_hash);
  trainDataBase.erase(tr_hash);
  for (int i = 0; i < b_tr_ca.stationNum; ++i)
  {
    stationDataBase.erase(std::make_pair(CalHash(tr_ca.stations[i]), std::make_pair(b_tr_ca.startSellDate, tr_hash)));
  }
}

void TrainManager::releaseTrain(const std::string &trainID_)
{
  ull tr_hash = CalHash(trainID_);
  basicTrainDatabase[tr_hash].isReleased = true;
  Train tr_ca;
  BasicTrain b_tr_ca = basicTrainDatabase[tr_hash];
  trainDataBase.find(tr_hash, tr_ca);

  TrainStation train_station(trainID_, b_tr_ca.startSellDate, b_tr_ca.endSellDate);

  for (int i = 0; i < b_tr_ca.stationNum; ++i)
  {
    // train_station.station = tr_ca.stations[i];
    train_station.rank = i;
    train_station.priceSum = tr_ca.sumPrice[i];
    train_station.startTime = b_tr_ca.startTime;
    train_station.arrivingTime = tr_ca.arrivingTime[i];
    train_station.leavingTime = tr_ca.leavingTime[i];
    stationDataBase.insert(std::make_pair(std::make_pair(CalHash(tr_ca.stations[i]),
                                                         std::make_pair(b_tr_ca.startSellDate, tr_hash)),
                                          train_station));
  }
}

bool TrainManager::queryTrain(const std::string &trainID_,
                              const std::string &date_,
                              sjtu::vector<std::string> &result)
{
  JerryGJX::Day wanted_date = JerryGJX::CalendarTime(date_).ToDay();
  if (!isAdded(trainID_))
    throw Error("Train Isn't Added");
  // return false;
  ull tr_hash = CalHash(trainID_);
  BasicTrain b_tr_ca = basicTrainDatabase[tr_hash];
  if (wanted_date < b_tr_ca.startSellDate || wanted_date > b_tr_ca.endSellDate)
    throw Error("Isn't Sell Date");
  // return false;
  Train tr_ca;
  trainDataBase.find(tr_hash, tr_ca);

  bool if_added = true;
  DayTrain dt_ca;
  if (!DayTrainToSeat.find(std::make_pair(wanted_date, tr_hash), dt_ca))
    if_added = false;

  result.push_back(trainID_);
  std::string st_ca;
  result.push_back(st_ca + b_tr_ca.type);
  st_ca.clear();
  int add_min = 24 * 60 * wanted_date;
  JerryGJX::Time transfer;
  st_ca += std::string(tr_ca.stations[0]) + " xx-xx xx:xx -> " +
           (transfer + tr_ca.leavingTime[0] + add_min).ToStr() + " " + std::to_string(tr_ca.sumPrice[0]) + " ";
  st_ca += if_added ? std::to_string(dt_ca.seatRemain[0]) : std::to_string(b_tr_ca.totalSeatNum);
  result.push_back(st_ca);

  for (int i = 1; i < b_tr_ca.stationNum - 1; ++i)
  {
    st_ca.clear();
    st_ca += std::string(tr_ca.stations[i]) + " " +
             (transfer + tr_ca.arrivingTime[i] + add_min).ToStr() + " -> " +
             (transfer + tr_ca.leavingTime[i] + add_min).ToStr() + " " + std::to_string(tr_ca.sumPrice[i]) + " ";
    st_ca += if_added ? std::to_string(dt_ca.seatRemain[i]) : std::to_string(b_tr_ca.totalSeatNum);
    result.push_back(st_ca);
  }
  st_ca.clear();
  st_ca += std::string(tr_ca.stations[b_tr_ca.stationNum - 1]) + " " +
           (transfer + tr_ca.arrivingTime[b_tr_ca.stationNum - 1] + add_min).ToStr() +
           " -> " + "xx-xx xx:xx" + " ";
  st_ca += std::to_string(tr_ca.sumPrice[b_tr_ca.stationNum - 1]) + " " + "x";
  result.push_back(st_ca);

  return true;
}

void TrainManager::QueryTicket(std::string *info,
                               sjtu::vector<std::string> &result)
{
  JerryGJX::Day wanted_date = JerryGJX::CalendarTime(info[JerryGJX::_d]).ToDay();

  std::string startStation = info[JerryGJX::_s], endStation = info[JerryGJX::_t];

  ull start_hash = CalHash(startStation), terminal_hash = CalHash(endStation);
  sjtu::vector<std::pair<std::pair<ull, std::pair<int, ull>>, TrainStation>> result_start, result_terminal;
  stationDataBase.range_search(std::make_pair(start_hash, std::make_pair(0, 0)),
                               std::make_pair(start_hash, std::make_pair(wanted_date + 1, 0)),
                               result_start);
  stationDataBase.range_search(std::make_pair(terminal_hash, std::make_pair(0, 0)),
                               std::make_pair(terminal_hash, std::make_pair(wanted_date + 1, 0)),
                               result_terminal);
  // sjtu::linked_hashmap<ull, int> find_same;

  bool if_time = true;
  if (!info[JerryGJX::_p].empty() && info[JerryGJX::_p] == "cost")
    if_time = false;

  sjtu::vector<std::pair<std::pair<int, JerryGJX::trainIDType>, std::pair<int, int>>> possible_ans;

  // sjtu::vector<int> find_same;
  for (int i = 0, j = 0; i < result_start.size() && j < result_terminal.size(); ++i)
  {
    while (j < result_terminal.size() && result_start[i].first.second > result_terminal[j].first.second)
      j++;
    if (j >= result_terminal.size())
      break;
    if (result_start[i].second.trainID == result_terminal[j].second.trainID)
    {
      int add_time = result_start[i].second.leavingTime / (60 * 24);
      if (result_start[i].second.endSaleDate + add_time >= wanted_date && result_start[i].second.startSaleDate + add_time <= wanted_date)
      {
        if (result_start[i].second.rank < result_terminal[j].second.rank)
        {
          int ranker;
          if (if_time)
            ranker = result_terminal[j].second.arrivingTime - result_start[i].second.leavingTime;
          else
            ranker = result_terminal[j].second.priceSum - result_start[i].second.priceSum;
          possible_ans.push_back(std::make_pair(std::make_pair(ranker, result_start[i].second.trainID),
                                                std::make_pair(i, j)));
        }
      }
    }
  }

  JerryGJX::Sort(possible_ans, 0, int(possible_ans.size() - 1));

  //  sjtu::map<std::pair<int, JerryGJX::trainIDType>, std::pair<int, int>> possible_ans;
  //
  //  bool if_time = true;
  //  if (info.find(JerryGJX::_p) != info.end() && info[JerryGJX::_p] == "cost") if_time = false;
  //
  //  for (int i = 0; i < result_terminal.size(); ++i) {
  //    if (find_same.find(CalHash(result_terminal[i].trainID)) != find_same.end()) {
  //      int cnt = find_same.find(CalHash(result_terminal[i].trainID))->second;
  //      if (result_start[cnt].rank < result_terminal[i].rank) {
  //        int ranker;
  //        if (if_time)ranker = result_terminal[i].arrivingTime - result_start[cnt].leavingTime;
  //        else ranker = result_terminal[i].priceSum - result_start[cnt].priceSum;
  //        possible_ans.insert(std::make_pair(std::make_pair(ranker, result_start[cnt].trainID), std::make_pair(cnt, i)));
  //      }
  //    }
  //  }

  result.push_back(std::to_string(possible_ans.size()));

  // int tot_num = 0;
  for (int i = 0; i < possible_ans.size(); i++)
  {

    int levT_f = result_start[possible_ans[i].second.first].second.leavingTime,
        arvT_t = result_terminal[possible_ans[i].second.second].second.arrivingTime,
        f_rank = result_start[possible_ans[i].second.first].second.rank,
        t_rank = result_terminal[possible_ans[i].second.second].second.rank;

    std::string _trainID = result_start[possible_ans[i].second.first].second.trainID;

    ull tr_hash = CalHash(_trainID);
    int start_date = wanted_date - levT_f / (24 * 60);
    bool if_added = true;
    DayTrain dt_ca;
    if (!DayTrainToSeat.find(std::make_pair(start_date, tr_hash), dt_ca))
      if_added = false;
    int _seat = if_added ? dt_ca.findMin(f_rank, t_rank - 1) : basicTrainDatabase[tr_hash].totalSeatNum;

    JerryGJX::Minute start_time = 24 * 60 * start_date + levT_f;
    JerryGJX::Minute end_time = 24 * 60 * start_date + arvT_t;

    //    JerryGJX::Time _leaving_time = start_day + result_start[i->second.first].leavingTime % (24 * 60);
    //    JerryGJX::Time _arrival_time =
    //        _leaving_time + (result_terminal[i->second.second].arrivingTime - result_start[i->second.first].leavingTime);
    int _price = result_terminal[possible_ans[i].second.second].second.priceSum - result_start[possible_ans[i].second.first].second.priceSum;

    Ticket tk_ca(_trainID, startStation, endStation, start_time, end_time, _price, _seat);

    result.push_back(tk_ca.Print());
  }
}

void TrainManager::QueryTransfer(std::string *info,
                                 sjtu::vector<std::string> &result)
{
  JerryGJX::Day wanted_date = JerryGJX::CalendarTime(info[JerryGJX::_d]).ToDay();
  std::string start_station = info[JerryGJX::_s], terminal_station = info[JerryGJX::_t];
  ull start_hash = CalHash(start_station), terminal_hash = CalHash(terminal_station);
  sjtu::vector<TrainStation> result_start, result_terminal;
  //  stationDataBase.range_search(std::make_pair(start_hash, 0), std::make_pair(start_hash + 1, 0), result_start);
  //  stationDataBase.range_search(std::make_pair(terminal_hash, 0), std::make_pair(terminal_hash + 1, 0), result_terminal);

  stationDataBase.range_search(std::make_pair(start_hash, std::make_pair(0, 0)),
                               std::make_pair(start_hash, std::make_pair(wanted_date + 1, 0)),
                               result_start);
  stationDataBase.range_search(std::make_pair(terminal_hash, std::make_pair(0, 0)),
                               std::make_pair(terminal_hash, std::make_pair(wanted_date + 90, 0)),
                               result_terminal);

  sjtu::linked_hashmap<ull, std::pair<int, int>> startTime_permit; // trainIDHash,pair(start_date,rank in result_start)
  for (int i = 0; i < result_start.size(); ++i)
  {
    int add_time = result_start[i].leavingTime / (60 * 24);
    if (result_start[i].endSaleDate + add_time >= wanted_date && result_start[i].startSaleDate + add_time <= wanted_date)
    {
      int levT_f = result_start[i].leavingTime;
      int start_time = wanted_date - levT_f / (24 * 60);
      startTime_permit.insert(std::make_pair(CalHash(result_start[i].trainID), std::make_pair(start_time, i)));
    }
  }

  Transfer best_choice;
  bool has_choice = false;
  bool if_time = false;
  if (!info[JerryGJX::_p].empty() && info[JerryGJX::_p] == "time")
    if_time = true;

  for (auto it_s = startTime_permit.begin(); it_s != startTime_permit.end(); it_s++)
  {
    int arr_rank = it_s->second.second;       // rank in result_start
    int f_rank = result_start[arr_rank].rank; //出发站在该车的站中编号
    ull f_tr_hash = it_s->first;
    int start_date_1 = it_s->second.first;
    Train f_tr;
    trainDataBase.find(f_tr_hash, f_tr);
    BasicTrain b_f_tr_ca = basicTrainDatabase[f_tr_hash];
    bool if_added_tr1 = true;
    DayTrain tr_1;
    if (!DayTrainToSeat.find(std::make_pair(start_date_1, f_tr_hash), tr_1))
      if_added_tr1 = false;

    sjtu::linked_hashmap<ull, std::pair<int, int>>
        StaAndArvMin; // stationHash to which min in the year/station rank of train
    for (int i = f_rank + 1; i < b_f_tr_ca.stationNum; ++i)
    {
      int which_min = 24 * 60 * start_date_1 + f_tr.arrivingTime[i];
      StaAndArvMin.insert(std::make_pair(CalHash(f_tr.stations[i].str()), std::make_pair(which_min, i)));
    }

    for (auto &T : result_terminal)
    {
      ull t_tr_hash = CalHash(T.trainID.str());
      if (t_tr_hash == f_tr_hash)
        continue;
      Train t_tr;
      trainDataBase.find(t_tr_hash, t_tr);
      BasicTrain b_t_tr_ca = basicTrainDatabase[t_tr_hash];

      if (!isReleased(t_tr.trainID.str()))
        continue;
      for (int i = 0; i < T.rank; ++i)
      {
        std::string trans_station = t_tr.stations[i].str();
        auto iter = StaAndArvMin.find(CalHash(trans_station));
        if (iter == StaAndArvMin.end())
          continue;
        int last_leaving_minute = 24 * 60 * b_t_tr_ca.endSellDate + t_tr.leavingTime[i];
        int minute_diff = last_leaving_minute - iter->second.first;
        if (minute_diff < 0)
          continue;
        int best_trans_day = std::max(b_t_tr_ca.startSellDate, b_t_tr_ca.endSellDate - (minute_diff) / (24 * 60));
        int start_time_1 = 24 * 60 * start_date_1 + f_tr.leavingTime[f_rank];
        int end_time_1 = 24 * 60 * start_date_1 + f_tr.arrivingTime[iter->second.second];
        int start_time_2 = 24 * 60 * best_trans_day + t_tr.leavingTime[i];
        int end_time_2 = 24 * 60 * best_trans_day + t_tr.arrivingTime[T.rank];
        int cost_1 = f_tr.sumPrice[iter->second.second] - f_tr.sumPrice[f_rank];
        int cost_2 = t_tr.sumPrice[T.rank] - t_tr.sumPrice[i];
        int trans_time = start_time_2 - end_time_1;

        bool if_added_tr2 = true;
        DayTrain tr_2;
        if (!DayTrainToSeat.find(std::make_pair(best_trans_day, t_tr_hash), tr_2))
          if_added_tr2 = false;

        int seat_1 = if_added_tr1 ? tr_1.findMin(f_rank, iter->second.second - 1) : b_f_tr_ca.totalSeatNum;
        int seat_2 = if_added_tr2 ? tr_2.findMin(i, T.rank - 1) : b_t_tr_ca.totalSeatNum;
        Ticket tk_1(f_tr.trainID.str(), start_station, trans_station, start_time_1, end_time_1, cost_1, seat_1);
        Ticket tk_2(t_tr.trainID.str(), trans_station, terminal_station, start_time_2, end_time_2, cost_2, seat_2);

        if (!has_choice)
        {
          best_choice = {tk_1, tk_2};
          has_choice = true;
        }
        else
        {
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
          if (if_time)
          {
            if (tot_time_now > best_choice.totTime())
              continue;
            else if (tot_time_now < best_choice.totTime())
              best_choice = {tk_1, tk_2};
            else
            {
              if (tot_cost_now > best_choice.totCost())
                continue;
              else if (tot_cost_now < best_choice.totCost())
                best_choice = {tk_1, tk_2};
              else
              {
                if (tk_1.trainID < best_choice.tk1.trainID ||
                    tk_1.trainID == best_choice.tk1.trainID && tk_2.trainID < best_choice.tk2.trainID)
                  best_choice = {tk_1, tk_2};
              }
            }
          }
          else
          {
            if (tot_cost_now > best_choice.totCost())
              continue;
            else if (tot_cost_now < best_choice.totCost())
              best_choice = {tk_1, tk_2};
            else
            {
              if (tot_time_now > best_choice.totTime())
                continue;
              else if (tot_time_now < best_choice.totTime())
                best_choice = {tk_1, tk_2};
              else
              {
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

  if (!has_choice)
    result.push_back("0");
  else
  {
    result.push_back(best_choice.tk1.Print());
    result.push_back(best_choice.tk2.Print());
  }
}

std::string TrainManager::BuyTicket(std::string *info,
                                    OrderManager &order_manager_)
{

  int wanted_date = JerryGJX::CalendarTime(info[JerryGJX::_d]).ToDay();
  std::string trainID = info[JerryGJX::_i];
  ull tr_hash = CalHash(trainID);
  std::string start_station = info[JerryGJX::_f], terminal_station = info[JerryGJX::_t];
  ull start_hash = CalHash(start_station), terminal_hash = CalHash(terminal_station);
  if (basicTrainDatabase.find(tr_hash) == basicTrainDatabase.end())
    return "-1";
  BasicTrain b_tr_ca = basicTrainDatabase[tr_hash];
  //由于已检查过release，则一定存在
  //  Train wanted_train;
  //  trainDataBase.find(tidHash, wanted_train);
  TrainStation f_ts_ca, t_ts_ca;
  if (!stationDataBase.find(std::make_pair(start_hash, std::make_pair(b_tr_ca.startSellDate, tr_hash)), f_ts_ca) ||
      !stationDataBase.find(std::make_pair(terminal_hash, std::make_pair(b_tr_ca.startSellDate, tr_hash)), t_ts_ca))
    return "-1";

  int levT_f = f_ts_ca.leavingTime, arvT_t = t_ts_ca.arrivingTime, f_rank = f_ts_ca.rank, t_rank = t_ts_ca.rank;
  if (f_rank > t_rank)
    return "-1";

  int start_date = wanted_date - (levT_f / (24 * 60));
  //  JerryGJX::CalendarTime start_day;
  //  start_day += start_date;

  // BasicTrain b_tr_ca = basicTrainDatabase[tr_hash];
  int wanted_seat = std::stoi(info[JerryGJX::_n]);
  if (start_date < b_tr_ca.startSellDate || start_date > b_tr_ca.endSellDate)
    return "-1";
  if (b_tr_ca.totalSeatNum < wanted_seat)
    return "-1";
  DayTrain dt_ca;
  if (!DayTrainToSeat.find(std::make_pair(start_date, tr_hash), dt_ca))
    dt_ca.rangeAdd(0,
                   b_tr_ca.stationNum - 1,
                   b_tr_ca.totalSeatNum);

  int seat = dt_ca.findMin(f_rank, t_rank - 1);

  bool ifQueue = false;
  if (!info[JerryGJX::_q].empty() && info[JerryGJX::_q] == "true")
    ifQueue = true;

  if (seat < wanted_seat && !ifQueue)
    return "-1";

  int price = t_ts_ca.priceSum - f_ts_ca.priceSum;

  int Oid = order_manager_.QueryOid() + 1;

  Order or_ca(JerryGJX::SUCCESS, info[JerryGJX::_i], info[JerryGJX::_u], f_rank, t_rank,
              info[JerryGJX::_f], info[JerryGJX::_t], start_date, b_tr_ca.startTime, levT_f,
              arvT_t, Oid, price, wanted_seat);

  if (seat >= wanted_seat)
  {
    dt_ca.rangeAdd(f_rank, t_rank - 1, -wanted_seat);
    DayTrainToSeat.erase(std::make_pair(start_date, tr_hash));
    DayTrainToSeat.insert(std::make_pair(std::make_pair(start_date, tr_hash), dt_ca));
    order_manager_.AddOrder(info[JerryGJX::_u], or_ca);
    return std::to_string(price * wanted_seat);
  }
  else
  {
    if (ifQueue)
    {
      or_ca.orderStatus = JerryGJX::PENDING;
      order_manager_.AddOrder(info[JerryGJX::_u], or_ca);
      PendingOrder por_ca(CalHash(info[JerryGJX::_i]),
                          CalHash(info[JerryGJX::_u]),
                          f_rank,
                          t_rank,
                          Oid,
                          wanted_seat,
                          start_date);
      order_manager_.AddPendingOrder(start_date, por_ca.tidHash, Oid, por_ca);
      return "queue";
    }
    else
      return "-1";
  }
}

bool TrainManager::RefundTicket(const std::string &username_, int rank_, OrderManager &order_manager_)
{
  sjtu::vector<Order> order_list;
  order_manager_.QueryOrderPrivate(username_, order_list);
  int ra_ca = (int)order_list.size() - rank_;
  if (ra_ca < 0)
    throw Error("Order Number Isn't Enough");
  // return false;
  Order target_order = order_list[ra_ca];
  if (target_order.orderStatus == JerryGJX::REFUNDED)
    throw Error("Order Is Refunded");
  // return false;
  else if (target_order.orderStatus == JerryGJX::PENDING)
  {
    order_manager_.RemoveOrder(CalHash(target_order.username.str()), target_order.orderID);
    target_order.orderStatus = JerryGJX::REFUNDED;
    order_manager_.AddOrder(target_order.username.str(), target_order);
    order_manager_.RemovePendingOrder(target_order.startDay.ToDay(),
                                      CalHash(target_order.trainID.str()),
                                      target_order.orderID);
  }
  else
  {
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

    for (int i = 0; i < pending_order_list.size(); ++i)
    {
      PendingOrder &pod_ca = pending_order_list[i];
      if (pod_ca.startRank >= t_rank || pod_ca.endRank <= f_rank || dt_ca.findMin(pod_ca.startRank, pod_ca.endRank - 1) < pod_ca.num)
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

void TrainManager::Clean()
{
  trainDataBase.clear();
  basicTrainDatabase.clear();
  DayTrainToSeat.clear();
  stationDataBase.clear();
  basicTrainBackUp.clear();
}

void TrainManager::Exit()
{
  // basicTrainBackUp.clear();
  for (auto &T : basicTrainDatabase)
  {
    basicTrainBackUp.modify(T.first, T.second);
    //    BasicTrain ca;
    //    basicTrainBackUp.find(T.first, ca);
    //    std::cout << ca.trainID << "\n";
  }
}

//----------tools------------------
// void TrainManager::StationDataBase_RangeFind(const std::pair<ull, ull> &lp,
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

std::string TrainManager::Ticket::Print()
{
  JerryGJX::Time ca;
  std::string start_t = (ca + start_time).ToStr();
  std::string end_t = (ca + end_time).ToStr();
  std::string ans;
  ans.reserve(120);
  ans += trainID;
  ans += " ";
  ans += startStation;
  ans += " ";
  ans += start_t;
  ans += " -> ";
  ans += endStation;
  ans += " ";
  ans += end_t;
  ans += " ";
  ans += std::to_string(cost);
  ans += " ";
  ans += std::to_string(seat);
  return ans;
}
