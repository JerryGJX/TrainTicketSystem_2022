# include "Char.hpp"
# include "bpTree.hpp"
# include "UnorderedMap.hpp"
#include "Vector.hpp"
#include "mydefs.hpp"
#include "order_manager.hpp"
#include "Queue.hpp"


struct Train {
  bool isReleased;
  JerryGJX::trainIDType trainID;
  int stationNum;
  JerryGJX::stationType stations[JerryGJX::max_stationNum];
  int totalSeatNum;
  int sumPrice[JerryGJX::max_stationNum];//存价格前缀和，且sumPrice[0]=0
  JerryGJX::timeType startTime;
  JerryGJX::timeType arrivingTime[JerryGJX::max_stationNum];//存到达时间，且arrivingTime[0]=0
  JerryGJX::timeType leavingTime[JerryGJX::max_stationNum];//存离站时间，且leavingTime[0]=startTime
  JerryGJX::timeType startSellDate;
  JerryGJX::timeType endSellDate;
  char type;
};

class TrainManager {
 public:
  Bptree<JerryGJX::trainIDType, Train> trainDataBase;

  void addTrain(const std::string trainID_,
                const int stationNum_,
                const int totalSeatNum_,
                Vector<std::string> stations,
                Vector<int> Price_,
                const std::string startTime_,
                Vector<std::string> travelTimes_,
                Vector<std::string> stopoverTimes_,
                Vector<std::string> saleDate_,
                const char type_);

  bool isAdded(const std::string trainID_);
  /**
   * @brief 修改对应列车的isreleased变量
   */
  void releaseTrain(const std::string trainID_);

  /**
   * @brief 考虑到query_train,buy_ticket都需要知道release情况，可能此处可以用unordered map优化
   */
  UnorderedMap<JerryGJX::trainIDType, bool> releasedDatabase;

  bool isReleased(const std::string trainID_);//如果可以返回一个代表存储位置的量，此处可优化
  /**
  * @brief 用于query_train函数
  */
  struct DayTrain {
    int seatRemain[JerryGJX::max_stationNum];
  };
  Bptree<std::pair < JerryGJX::timeType, JerryGJX::trainIDType>,DayTrain>
  DayTrainToSeat;
  void queryTrain(JerryGJX::trainIDType trainId_, JerryGJX::timeType targetDate_);

  /**
   * @brief 将不同火车到达同一站视为不同站
   */
  struct TrainStation {
    JerryGJX::trainIDType trainID;
    JerryGJX::stationType station;
    int rank, priceSum;//rank表示从始发站向下的站次，priceSum表示始发站到该站的总价格
    JerryGJX::timeType startSaleDate, endSaleDate, arrivingTime, leavingTime;
  };

  /**
   * @brief 此处用pair为了防止bpt中发生key碰撞，pair中内容换为字符串哈希可能更优
   * @brief 此处要求bpt具有将一定区间内所有符合情况返回的能力
   */
  Bptree<std::pair < JerryGJX::stationType, JerryGJX::trainIDType>,TrainStation>
  stationDataBase;

  void QueryTicket(std::string startStation_,
                   std::string terminalStation_,
                   UnorderedMap<std::string, std::string> info);
  /**
   * @param info 传入是price还是time
   * @brief 直接输出结果
   */
  void QueryTransfer(std::string startStation_,
                     std::string terminalStation_,
                     UnorderedMap<std::string, std::string> info);

  struct Ticket {
    TrainStation startStation, endStation;
  };

  /**
   * @param order_manager_ 用于修改下单信息
   * @brief 判断用户是否登录在commandParser中完成
   */
  void BuyTicket(const std::string username_,
                 const std::string trainID_,
                 const std::string date_,
                 const int number,
                 const std::string startStation_,
                 const std::string terminalStation_,
                 const bool if_wait,
                 OrderManager* order_manager_);

  void Clear();
};

