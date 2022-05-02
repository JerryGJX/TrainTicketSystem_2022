# Ticket System 2021

A train ticket management system based on B+ Tree

## group member

| 成员                                           | 分工             |
| ---------------------------------------------- | ---------------- |
| 郭俊贤 [JerryGJX](https://github.com/JerryGJX) | 后端主体逻辑部分 |
| 冯跃洋 [Vexoben](https://github.com/Vexoben)   | B+树主体         |

 

## 模块划分及功能

### 划分

#### 数据结构相关

- 模板B+树（BpTree.cpp）

- 外存顺序读写类（Queue.cpp）

- 变长数组类（Vector.cpp）//可能不需要

- 内存查找类（UnorderedMap.cpp）

#### 工具部件

- 支持比较的字符串类（Char.cpp）
- hash,sort等工具函数（Algorithm.cpp）

#### 后端逻辑相关

- 指令解析类（command_parser.cpp）
- 订单管理类（order_manager.cpp）
- 火车管理类（train_manager.cpp）
- 用户管理类（user_manager.cpp）



## 主体逻辑设计

### 指令解析类（command_parser.cpp）

#### 重要成员

```cpp
  UserManager &user_manager_;
  TrainManager &train_manager_;
  OrderManager &order_manager_;
UnorderedMap<std::string, void (CommandParser::*)(Vector<std::string> &)> mapFunction;
```



#### 功能与实现

- ```main```函数中完成三个manager的构造后引用传入 ``` CommanParser ```构造函数中，用于处理三者相互调用

- ```run```函数负责不断读入，之后利用```mapFunction```调用其他成员函数执行操作
  
- ```cpp
  void ParseAddUser(Vector<std::string> &cmd);
  void ParseLogin(Vector<std::string> &cmd);
  void ParseLogout(Vector<std::string> &cmd);
  void ParseQueryProfile(Vector<std::string> &cmd);
  void ParseModifyProfile(Vector<std::string> &cmd);
  void ParseAddTrain(Vector<std::string> &cmd);
  void ParseReleaseTrain(Vector<std::string> &cmd);
  void ParseQueryTrain(Vector<std::string> &cmd);
  void ParseQueryTicket(Vector<std::string> &cmd);
  void ParseQueryTransfer(Vector<std::string> &cmd);
  void ParseBuyTicket(Vector<std::string> &cmd);
  void ParseQueryOrder(Vector<std::string> &cmd);
  void ParseRefundTicket(Vector<std::string> &cmd);
  void ParseRollback(Vector<std::string> &cmd);
  void ParseClean(Vector<std::string> &cmd);
  void ParseExit(Vector<std::string> &cmd);
  ```
  等成员函数负责调用三个manager的对应成员函数以执行指令
  

### Manager类

### ``` UserManager ```类

#### 重要数据成员

```cpp
Bptree<JerryGJX::usernameType , User> userDatabase;

UnorderedMap<JerryGJX::usernameType , User> onlineUser;//是否要换为bpt？
```

#### 成员函数

- ```cpp
  void AddUser(const std::string username_,
             const std::string password_,
             const std::string name_,
             const std::string mailAddr_,
             const int privilege_);
  ```
  该函数只负责添加用户，能否插入由```ParserCommander```判断

- ```cpp
  bool isRegistered(const std::string username_);
  ```
  ```UserDataBase```中该用户是否存在

- ```cpp
  int isLogin(const std::string username_);
  ```
  ```onlineUser```中该用户是否存在，存在则返回对应privilege，否则返回-1

- ```cpp
  void Login(const std::string &username_, const std::string &password_);
  ```
  用户登录
  
- ```cpp
  void Logout(const std::string &username_);
  ```

  用户登出

- ```cpp
  Vector<std::string> queryProfile(const std::string &username_);
  ```

  查询一名用户的信息

- ```cpp
  Vector<std::string> modifyProfile(const std::string &username_, const UnorderedMap<std::string, std::string> &info,OrderManager* order_manager_);
  ```

  修改一名用户的信息

### ```TrainManager```类

#### 工具结构体

- ```Train```

  存储火车所有信息

  ```cpp
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
  ```

- ```DayTrain```

  ```cpp
  struct DayTrain {
    int seatRemain[JerryGJX::max_stationNum];
  };
  ```

  用于由（date，trainID）查询剩余座位，应对```QueryTrain```函数

  ```cpp
  Bptree<std::pair < JerryGJX::timeType, JerryGJX::trainIDType>,DayTrain>
  DayTrainToSeat;
  ```

- ```TrainStation```

  ```cpp
  struct TrainStation {
    JerryGJX::trainIDType trainID;
    JerryGJX::stationType station;
    int rank, priceSum;//rank表示从始发站向下的站次，priceSum表示始发站到该站的总价格
    JerryGJX::timeType startSaleDate, endSaleDate, arrivingTime, leavingTime;
  };
  ```

  用于

  ```cpp
  Bptree<std::pair < JerryGJX::stationType, JerryGJX::trainIDType>,TrainStation>
  stationDataBase;
  ```

  配合bpt范围搜索之后遍历，实现

  ```cpp
  void QueryTicket(std::string startStation_,
                   std::string terminalStation_,
                   UnorderedMap<std::string, std::string> info);
  void QueryTransfer(std::string startStation_,
                       std::string terminalStation_,
                       UnorderedMap<std::string, std::string> info);
  ```

- ```Ticket```

  ```cpp
  struct Ticket {
    TrainStation startStation, endStation;
  };
  ```

  

#### 成员函数

- ```cpp
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
  ```
  增加一辆火车

- ```cpp
  bool isAdded(const std::string trainID_);
  ```
  询问火车是否存在

- ```cpp
  void releaseTrain(const std::string trainID_);
  ```
  释放一辆火车

- ```cpp
  bool isReleased(const std::string trainID_);
  ```
  询问是否release

- ```cpp
  void queryTrain(JerryGJX::trainIDType trainId_, JerryGJX::timeType targetDate_);
  ```

  询问符合条件的火车

- ```cpp
  void QueryTicket(std::string startStation_,
                   std::string terminalStation_,
                   UnorderedMap<std::string, std::string> info);
  ```
  询问票

- ```cpp
  void QueryTransfer(std::string startStation_,
                     std::string terminalStation_,
                     UnorderedMap<std::string, std::string> info);
  ```
  询问符合条件的换乘路线

- ```cpp
  void BuyTicket(const std::string username_,
                 const std::string trainID_,
                 const std::string date_,
                 const int number,
                 const std::string startStation_,
                 const std::string terminalStation_,
                 const bool if_wait,
                 OrderManager* order_manager_);
  ```
  买票

- ```cpp
  void Clear();
  ```
  清除

### 模板B+树（BpTree.cpp）

- B+树类定义

```cpp
template <class Key, class Value, class Compare = std::less<Key>>
class Bptree{
    class Bptree_normal_node;
    class Bptree_leaf_node;
  private:    
    const children_maximum, record_maximum; // 最多允许的孩子数、记录数，对应于书上的 M 和 L
    Bptree_leaf_node *root; //根节点
  public:
    //插入节点，失败返回0
    bool insert(const &std::pair<Key, Value> data);
    //删除节点，失败返回0
    bool delete(const &std::pair<Key, Value> data);
    //查找大于等于key_search的第一个叶节点，不存在返回NULL
    Bptree_leaf_node *lower_bound(Key &key_search) const {};
    //查找大于key_search的第一个叶节点，不存在返回NULL
    Bptree_leaf_node *upper_bound(Key &key_search) const {};
    //查找Key值在[key_l, key_r)中间的值
    vector<Bptree_leaf_node*> range_search(Key &key_l, Key &key_r) const {};
};
```

- B+树普通节点类定义

```cpp
template <class Key, class Value, class Compare = std::les<Key>>
class Bptree_normal_node {
  public:
    friend Bptree;
  private:
    Bptree_normal_node *parent;
    Bptree_normal_node *children[M + 1]; //0-base，多出一个用做拆分时的临时节点
    Key *key_list[M];
    
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
    Bptree_leaf_node *parent;
    Bptree_leaf_node *predecessor，*successor;
    Key *key_list[L];
    Value *value_list[L];
  
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