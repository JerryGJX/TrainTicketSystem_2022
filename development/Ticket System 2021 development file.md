# Ticket System 2021

A train ticket management system based on B+ Tree

## group member

| 成员                                           | 分工             |
| ---------------------------------------------- | ---------------- |
| 郭俊贤 [JerryGJX](https://github.com/JerryGJX) | 后端主体逻辑部分 |
| 冯跃洋 [Vexoben](https://github.com/Vexoben)   | B+树主体         |

 

## 文件结构 & 运行逻辑

```
├── CMakeLists.txt
├── README.md
├── development
│   ├── Ticket System 2021 development file.md 
│   └── frontpageDevelopmentDoc.md
├── src
│   ├── ACMstl
│   │   ├── Map.hpp
│   │   ├── UnorderedMap.hpp
│   │   ├── Vector.hpp
│   │   ├── bpTree.hpp
│   │   ├── bptree_rollback.hpp
│   │   ├── exceptions.hpp
│   │   ├── memory_manager.hpp
│   │   └── stack_for_rollback.hpp
│   ├── command_parser.cpp
│   ├── command_parser.hpp
│   ├── main.cpp
│   ├── mydefs.hpp
│   ├── order_manager.cpp
│   ├── order_manager.hpp
│   ├── tools
│   │   ├── Algorithm.hpp
│   │   ├── Char.hpp
│   │   ├── Error.hpp
│   │   └── Time.hpp
│   ├── train_manager.cpp
│   ├── train_manager.hpp
│   ├── user_manager.cpp
│   └── user_manager.hpp
└── src_rollback
    ├── ACMstl
    │   ├── Map.hpp
    │   ├── UnorderedMap.hpp
    │   ├── Vector.hpp
    │   ├── bpTree.hpp
    │   ├── bptree_rollback.hpp
    │   ├── exceptions.hpp
    │   ├── memory_manager.hpp
    │   └── stack_for_rollback.hpp
    ├── command_parser.cpp
    ├── command_parser.hpp
    ├── main.cpp
    ├── mydefs.hpp
    ├── order_manager.cpp
    ├── order_manager.hpp
    ├── tools
    │   ├── Algorithm.hpp
    │   ├── Char.hpp
    │   └── Time.hpp
    ├── train_manager.cpp
    ├── train_manager.hpp
    ├── user_manager.cpp
    └── user_manager.hpp
```

- `src/` 中为主体逻辑代码：
  - `main.cpp` 为主入口，构造manager类，将输入的指令传给命令解析类。
  - `command_parser.hpp` 负责解析命令，调用对应的执行函数。
  - `user_manager.hpp` 用户管理类，执行和用户相关的操作。
  - `train_manager.hpp` 火车管理类，执行和火车、车票相关的操作。
  - `order_manager.hpp` 订单管理类，执行添加删除pending order以及添加，查找order
- `src_rollback/` 含有回滚功能的主体逻辑代码。
- 通过 `CMakeLists.txt` 控制是否含有回滚功能。 

## 数据存储结构

### 内存：

- 在线用户，value存下登录用户的权限。
- trainID ——> 火车基础信息（数据量较大小但需求较的数据）

## 外存：

- userID  ——> 用户信息
  
- trainID ——> 火车信息（数据量较大但需求较小的数据）
- trainID ——> 火车基础信息（数据量较大小但需求较的数据）
- （站名，（开始售卖日期，trainID））——> 火车通过车站的信息
- （日期，trainID）——> 火车票数信息

- （userID，order编号）——> 订单信息
- （(第几天(指始发天数)，trainID),order编号）——> 候补订单信息

> 为优化 B+ 树的比较操作，上文的“ID”均为哈希值
## 实现框架

### 工具类 & 数据结构

#### 定长字符串类（in `tools/Char.hpp`）

```c++
template<int maxLength = 65>
struct Char {
  // +1 是因为要存结尾 '\0'
  char content[maxLength + 1]{};
  // 默认为空字符串
  Char();

  Char(const std::string &s);

  Char(const char *cstr);

  operator std::string() const { return str(); };

  std::string str() const { return std::string(content); };

  // 赋值操作
  Char &operator=(const Char<maxLength> &that);

  bool operator<(const Char<maxLength> &that) const;

  bool operator>(const Char<maxLength> &that) const;

  bool operator<=(const Char<maxLength> &that) const;

  bool operator>=(const Char<maxLength> &that) const;

  bool operator==(const Char<maxLength> &that) const;

  bool operator!=(const Char<maxLength> &that) const;
};
```


#### 时间类（in `tools/Time.hpp`）

```c++
namespace JerryGJX {
constexpr int mon[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr int monSum[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

struct CalendarTime {
  int mm, dd;
  
  CalendarTime &operator=(const CalendarTime &rhs);

  bool operator<(const CalendarTime &rhs) const;

  bool operator>(const CalendarTime &rhs) const;

  bool operator<=(const CalendarTime &rhs) const;

  bool operator>=(const CalendarTime &rhs) const;

  CalendarTime operator+(int x);

  CalendarTime operator-(int x);

  CalendarTime &operator+=(int x);

  bool operator==(const CalendarTime &rhs) const;

  bool operator!=(const CalendarTime &rhs) const;

  std::string ToStr() const;

  int ToDay() const {
    return monSum[mm - (mm != 0)] + dd;
  }

  int ToHour() const {
    return (monSum[mm - (mm != 0)] + dd) * 24;
  }

};

struct ClockTime {
  int hor, min, day;
  bool operator==(const ClockTime &rhs) const {
    return hor == rhs.hor &&
        min == rhs.min;
  }
  bool operator!=(const ClockTime &rhs) const {
    return !(rhs == *this);
  }
  bool operator<(const ClockTime &rhs) const {
    if (hor < rhs.hor)return true;
    if (rhs.hor < hor)return false;
    return min < rhs.min;
  }
  bool operator>(const ClockTime &rhs) const {
    return rhs < *this;
  }
  bool operator<=(const ClockTime &rhs) const {
    return !(rhs < *this);
  }
  bool operator>=(const ClockTime &rhs) const {
    return !(*this < rhs);
  }

  ClockTime operator+(int x);

  ClockTime &operator+=(int x);

  std::string ToStr() const;

  int ToMin() const;
};

struct Time {
  CalendarTime calendar_time;
  ClockTime clock_time;
  explicit Time(int mm_ = 0, int dd_ = 0, int hor_ = 0, int min_ = 0) : calendar_time(mm_, dd_),
                                                                        clock_time(hor_, min_) {}

  Time(const std::string &str, int num);

  Time operator+(int x);

  Time &operator+=(int x);

  std::string ToStr() const;

};
}
```



#### B+ 树（in `ACMstl/bpTree.hpp`）

- B+树类定义

```cpp
template <class Key, class Value, class Compare = std::less<Key>>
class Bptree{
    class Bptree_normal_node;
    class Bptree_leaf_node;
    friend Bptree_normal_node;
    friend Bptree_leaf_node;
  private:    
    const int children_maximum, record_maximum; // 最多允许的孩子数、记录数，对应于书上的 M 和 L
    int root, self_position; //根节点
  public:
    //构造函数
    Bptree() {
      MemoryRiver<Bptree_normal_node> normal_node_list;
      MemoryRiver<Bptree_leaf_node> leaf_node_list;
      // todo
    }
    //析构函数
    ~BpTree() {}
    //插入节点，失败返回0
    bool insert(const &std::pair<Key, Value> data);
    //删除节点，失败返回0
    bool delete(const &std::pair<Key, Value> data);
    //查找大于等于key_search的第一个叶节点，不存在返回NULL
    Bptree_leaf_node lower_bound(Key &key_search) const {};
    //查找大于key_search的第一个叶节点，不存在返回NULL
    Bptree_leaf_node upper_bound(Key &key_search) const {};
    //查找Key值在[key_l, key_r)中间的值
    vector<Bptree_leaf_node> range_search(Key &key_l, Key &key_r) const {};
};
```

- B+树普通节点类定义

```cpp
template <class Key, class Value, class Compare = std::les<Key>>
class Bptree_normal_node {
  public:
    friend Bptree;
  private:
    int parent;
    int self_position;
    int children[M + 1]; //0-base，多出一个用做拆分时的临时节点
    Key key_list[M];
    
  public:
    //构造函数
    Bptree_normal_node() {} 
    Bptree_normal_node(Bptree_normal_node &obj) {}
    //析构函数
    ~Bptree_normal_node() {}
    //超过数量限制时拆分节点
    std::pair<Bptree_normal_node，Bptree_normal_node> split();
    //将两个低于数量限制的节点合并
    Bptree_normal_node merge(Bptree_normal_node &node1, Bptree_normal_node &node2);
}
```

- B+树叶子节点类定义

```cpp
template <class Key, class Value, class Compare = std::les<Key>>
class Bptree_leaf_node {
  public:
    friend Bptree;
  private:
    int parent;
    int self_position
    int predecessor, successor;
    Key key_list[L];
    Value value_list[L];
  
  public:  
    //构造函数
    Bptree_leaf_node() {} 
    Bptree_leaf_node(Bptree_leaf_node &obj) {}
    //析构函数
    ~Bptree_leaf_node() {}
    //超过数量限制时拆分节点
    std::pair<Bptree_leaf_node，Bptree_leaf_node> split();
    //将两个低于数量限制的节点合并
    Bptree_leaf_node merge(Bptree_leaf_node &node1, Bptree_leaf_node &node2);
    //查找前驱 
    Bptree_leaf_node* find_predecessor() const {};
    //查找后继
    Bptree_leaf_node* find_successor() const {};
}
```

### 用户部分

#### 用户信息类（in `user_manager.hpp`）

```c++
class User {
  friend class Usermanager;
 public:
  JerryGJX::passwordType password;  // 密码
  JerryGJX::nameType name;          // 真实姓名
  JerryGJX::mailAddrType mailAddr;  // 邮件地址
  int privilege{};                  // 权限 [0, 10] 
}；
```

#### 用户管理类（in `user_manager.hpp`）

```c++
class UserManager {
 private:
  int TimeTag = 0;
  Bptree<ull, User, 339, 67> userDatabase;//username -> User(class)
  sjtu::linked_hashmap<ull, int> onlineUser;//维护在线用户 username->privilege
 public:
  explicit UserManager(const std::string &filenameUD);

  ull CalHash(const std::string &username_);

  bool AddUser(std::string *info);

  bool isReg(const std::string &username_);

  bool isLogin(const std::string &username_);

  bool checkPassword(const std::string &username_, const std::string &password_);

  bool Login(std::string *info);

  bool Logout(std::string *info);

  bool queryProfile(std::string *info, std::string &result);

  bool modifyProfile(std::string *info, std::string &result);

  void Clean();

  void Exit();

//----------rollback-----------

  void RollBack(int target_time);

  void GetTime(int time_tag);

};
```

### 火车 & 车票部分

#### 火车信息（in `train_manager.hpp`）

```c++
struct Train {
  JerryGJX::trainIDType trainID;
  JerryGJX::stationType stations[JerryGJX::max_stationNum];//0 base
  int sumPrice[JerryGJX::max_stationNum]{};//存价格前缀和，且sumPrice[0]=0
  int arrivingTime[JerryGJX::max_stationNum]{};//存到达时间，且arrivingTime[0]=0
  int leavingTime[JerryGJX::max_stationNum]{};//存离站时间，且leavingTime[0]=startTime
};
```


#### 火车基础信息（in `train_manager.hpp`）

```c++
struct BasicTrain {
  int stationNum{}, totalSeatNum{};
  bool isReleased = false;
  JerryGJX::Minute startTime{};
  JerryGJX::Day startSellDate = 0;
  JerryGJX::Day endSellDate = 0;
  char type{};
};
```


#### 火车通过车站的信息（in `train_manager.hpp`）

```c++
struct TrainStation {
    JerryGJX::trainIDType trainID{};
    int rank = 0, priceSum = 0;//rank表示从始发站向下的站次，priceSum表示始发站到该站的总价格
    JerryGJX::Day startSaleDate{}, endSaleDate{};
    int startTime{}, arrivingTime{}, leavingTime{};
  };
```

#### 火车票量信息（in `train_manager.hpp`）

```c++
struct DayTrain {
    int seatRemain[JerryGJX::max_stationNum]{};
    int findMin(int lp, int rp);
    void rangeAdd(int lp, int rp, int d);
  };
```



#### 火车管理类（in `train_manager.hpp`）

```c++
class TrainManager {
 private:
  struct Ticket {
    std::string trainID{};
    std::string startStation{}, endStation{};
    int start_time{}, end_time{};//在一年中的第几分钟
    int cost{}, seat{};
    
    [[nodiscard]] int lastTime() const {
      return end_time - start_time;
    }
    std::string Print();
  };

  struct Transfer {
    Ticket tk1{}, tk2{};
    [[nodiscard]] int totTime() const {
      return tk2.end_time - tk1.start_time;
    }
    [[nodiscard]] int totCost() const {
      return tk1.cost + tk2.cost;
    }
  };
  int TimeTag = 0;
  Bptree<ull, Train, 338, 3> trainDataBase;
  //trainID->Train
  sjtu::linked_hashmap<ull, BasicTrain> basicTrainDatabase;
  //trainID->BasicTrain
  Bptree<ull, BasicTrain, 338, 65> basicTrainBackUp;
  //用于在开关程序的时候保存信息
  Bptree<std::pair<JerryGJX::Day, ull>, DayTrain, 339, 18, JerryGJX::pair_hash>
      DayTrainToSeat;
      //(第几天，trainID)->车票信息
  Bptree<std::pair<ull, std::pair<int, ull>>, TrainStation, 339, 101, JerryGJX::pair_pair_hash>
      stationDataBase;
      //（站名，（开始售卖日期，trainID））——> 火车通过车站的信息

  //--------rollback-----------
  enum Op {toAdd, toRemove,toDown};
  struct RollBackInfo{
  int TimeTag=0;
  Op op=toAdd;
  ull tidHash=0;
  BasicTrain data{}；
  };
  StackForRollback<RollBackInfo> rollbackData;
  //为实现内存中的回滚而写

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

  void releaseTrain(const std::string &trainID_);

  bool queryTrain(const std::string &trainID_, const std::string &date_, sjtu::vector<std::string> &result);

  bool isReleased(const std::string &trainID_);

  ull CalHash(const std::string &str_);


  void QueryTicket(std::string *info, sjtu::vector<std::string> &result);
  
  void QueryTransfer(std::string *info, sjtu::vector<std::string> &result);

  std::string BuyTicket(std::string *info, OrderManager &order_manager_);

  bool RefundTicket(const std::string &username_, int rank_, OrderManager &order_manager_);//从新到旧第rank_个(1-base)

  void Clean();

  void Exit();

  void RollBack(int target_time);

  void GetTime(int time_tag);
};
```


#### 订单信息（in `order_manager.hpp`）

```c++
struct Order {
  JerryGJX::orderStatusType orderStatus{};
  JerryGJX::trainIDType trainID{};
  int startRank{}, endRank{};//要求是trainManager产生
  JerryGJX::stationType startStation{}, endStation{};
  JerryGJX::CalendarTime startDay{};
  int startTime{}, leavingTime{}, arrivingTime{};//全部用分钟为单位，后两者为绝对分钟数
  int orderID{}, price{}, num{};
};
```

#### 候补订单信息（in `order_manager.hpp`）

```c++
struct PendingOrder {
  ull tidHash{}, uidHash{};
  int startRank{}, endRank{};
  int orderID{}, num{};
  JerryGJX::CalendarTime startDay{};
};
```

#### 订单管理类（in `order_manager.hpp`）

```c++
class OrderManager {
  template<class T1, class T2>
  class PairHash {
   public:
    ull operator()(const std::pair<T1, T2> &ValueType) const {
      return ValueType.first + ValueType.second;
    }
  };
  int TimeTag=0;
  Bptree<std::pair<ull, int>, Order, 339, 38,JerryGJX::pair_hash> orderDataBase;//hashUid,oid
  Bptree<std::pair<std::pair<int, ull>, int>, PendingOrder, 254, 144,JerryGJX::pair_pair_hash_nd> pendingQueue;
  //(第几天(指始发天数)，hash(trainID)),oid
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
```