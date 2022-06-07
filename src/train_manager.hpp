

#ifndef COMMAND_PARSER_HPP__TRAIN_MANAGER_HPP_
#define COMMAND_PARSER_HPP__TRAIN_MANAGER_HPP_

# include "tools/Char.hpp"
# include "ACMstl/bpTree.hpp"
# include "ACMstl/UnorderedMap.hpp"
#include "ACMstl/Map.hpp"
#include "ACMstl/Vector.hpp"
#include "mydefs.hpp"
//#include "ACMstl/Queue.hpp"
#include "tools/Time.hpp"
#include "order_manager.hpp"

//#include <utility>
//#include <vector>
//#include <map>
//#include <unordered_map>

using JerryGJX::ull;

struct Train {
  JerryGJX::trainIDType trainID;
  JerryGJX::stationType stations[JerryGJX::max_stationNum];//0 base
  int sumPrice[JerryGJX::max_stationNum]{};//存价格前缀和，且sumPrice[0]=0
  int arrivingTime[JerryGJX::max_stationNum]{};//存到达时间，且arrivingTime[0]=0
  int leavingTime[JerryGJX::max_stationNum]{};//存离站时间，且leavingTime[0]=startTime
  Train() = default;
  Train(const std::string &TrainID_, sjtu::vector<std::string> &stations_, sjtu::vector<int> &sumPrice_,
        sjtu::vector<int> &arrivingTimes_, sjtu::vector<int> &leavingTimes_);
  Train(const Train &rhs);
};

struct BasicTrain {
  JerryGJX::trainIDType trainID{};
  int stationNum{}, totalSeatNum{};
  bool isReleased = false;
  JerryGJX::Minute startTime{};
  JerryGJX::Day startSellDate = 0;
  JerryGJX::Day endSellDate = 0;
  char type{};
  BasicTrain() = default;

  BasicTrain(const std::string &train_id,
             int station_num,
             int total_seat_num,
             bool is_released,
             int start_time,
             int start_sell_date,
             int end_sell_date,
             char type)
      : trainID(train_id),
        stationNum(station_num),
        totalSeatNum(total_seat_num),
        isReleased(is_released),
        startTime(start_time),
        startSellDate(start_sell_date),
        endSellDate(end_sell_date),
        type(type) {}

  BasicTrain(const BasicTrain &rhs) {
    trainID = rhs.trainID;
    stationNum = rhs.stationNum;
    totalSeatNum = rhs.totalSeatNum;
    isReleased = rhs.isReleased;
    startTime = rhs.startTime;
    startSellDate = rhs.startSellDate;
    endSellDate = rhs.endSellDate;
    type = rhs.type;
  }
};

class TrainManager {
 private:
  struct DayTrain {
    int seatRemain[JerryGJX::max_stationNum]{};
    DayTrain() = default;
    DayTrain(const DayTrain &rhs);
    int findMin(int lp, int rp);
    void rangeAdd(int lp, int rp, int d);
  };

  struct TrainStation {
    JerryGJX::trainIDType trainID{};
    JerryGJX::stationType station{};
    int rank = 0, priceSum = 0;//rank表示从始发站向下的站次，priceSum表示始发站到该站的总价格
    JerryGJX::Day startSaleDate{}, endSaleDate{};
    int startTime{}, arrivingTime{}, leavingTime{};
    TrainStation() = default;
    TrainStation(const std::string &trainID_, int startSellDate_, int endSellDate_);
  };

  struct Ticket {
    std::string trainID{};
    std::string startStation{}, endStation{};
    int start_time{}, end_time{};//在一年中的第几分钟
    int cost{}, seat{};

    Ticket() = default;
    Ticket(std::string train_id,
           std::string start_station,
           std::string end_station,
           int start_time,//分钟
           int end_time,
           int cost, int seat)
        : trainID(std::move(train_id)),
          startStation(std::move(start_station)),
          endStation(std::move(end_station)),
          start_time(start_time),
          end_time(end_time),
          cost(cost), seat(seat) {}

    Ticket(Ticket &rhs) {
      trainID = rhs.trainID;
      startStation = rhs.startStation;
      endStation = rhs.endStation;
      start_time = rhs.start_time;
      end_time = rhs.end_time;
      cost = rhs.cost;
      seat = rhs.seat;
    }

    [[nodiscard]] int lastTime() const {
      return end_time - start_time;
    }

    std::string Print();

  };

  struct Transfer {
    Ticket tk1{}, tk2{};

    Transfer() = default;
    Transfer(Ticket &tk_1_, Ticket &tk_2_) {
      tk1 = tk_1_, tk2 = tk_2_;
    };

    [[nodiscard]] int totTime() const {
      return tk2.end_time - tk1.start_time;
    }
    [[nodiscard]] int totCost() const {
      return tk1.cost + tk2.cost;
    }
  };

  template<class T1, class T2>
  class PairHash {
   public:
    ull operator()(const std::pair<T1, T2> &ValueType) const {
      return ValueType.first + ValueType.second;
    }
  };

  Bptree<ull, Train, 338, 3> trainDataBase;
  sjtu::linked_hashmap<ull, BasicTrain> basicTrainDatabase;
  Bptree<ull, BasicTrain, 338, 65> basicTrainBackUp;//
  Bptree<std::pair<JerryGJX::Day, ull>, DayTrain, 253, 8, PairHash<JerryGJX::Day, ull>>
      DayTrainToSeat;//(第几天，hash(trainID))
  Bptree<std::pair<ull, ull>, TrainStation, 201, 65, PairHash<ull, ull>> stationDataBase;//(HashStation，HashTrain）



  //------------缓存区-----------
  //对于queryticket和querytrain的vector的缓存



//  void StationDataBase_RangeFind(const std::pair<ull, ull> &lp,
//                                 const std::pair<ull, ull> &rp, sjtu::vector<TrainStation> &result);

  std::hash<std::string> hash_str;

 public:
  TrainManager(const std::string &filename_tdb,
               const std::string &filename_dtts,
               const std::string &filename_sdb,
               const std::string &filename_btb);



  void addTrain(const std::string &trainID_,
                int stationNum_,
                int totalSeatNum_,
                sjtu::vector<std::string> &stations,
                sjtu::vector<int> &Price_,
                const std::string &startTime_,
                sjtu::vector<int> &travelTimes_,
                sjtu::vector<int> &stopoverTimes_,
                sjtu::vector<std::string> &saleDate_,
                char type_);

  bool isAdded(const std::string &trainID_);

  void deleteTrain(const std::string &trainID_);

  /**
   * @brief 修改对应列车的isreleased变量
   */
  void releaseTrain(const std::string &trainID_);

  /**
   * @brief 考虑到query_train,buy_ticket都需要知道release情况，可能此处可以用unordered map优化
   */
  bool queryTrain(const std::string &trainID_, const std::string &date_, sjtu::vector<std::string> &result);

  bool isReleased(const std::string &trainID_);//如果可以返回一个代表存储位置的量，此处可优化
  /**
  * @brief 用于query_train函数
  */
  ull CalHash(const std::string &str_);
  /**
   * @brief 将不同火车到达同一站视为不同站
   */

  void QueryTicket(sjtu::linked_hashmap<std::string, std::string> &info, sjtu::vector<std::string> &result);
  /**
   * @param info 传入是price还是time
   * @brief 直接输出结果
   */
  void QueryTransfer(sjtu::linked_hashmap<std::string, std::string> &info, sjtu::vector<std::string> &result);

  /**
   * @param order_manager_ 用于修改下单信息
   * @brief 判断用户是否登录在commandParser中完成
   */
  std::string BuyTicket(sjtu::linked_hashmap<std::string, std::string> &info, OrderManager &order_manager_);

  bool RefundTicket(const std::string &username_, int rank_, OrderManager &order_manager_);//从新到旧第rank_个(1-base)

  void Clean();

  void Exit();

};

#endif //COMMAND_PARSER_HPP__TRAIN_MANAGER_HPP_

