#include <iostream>
#include "tools/Char.hpp"
#include "command_parser.hpp"

UserManager user_manager("userData");
TrainManager train_manager("trainData", "dtts", "sdb","btb");
OrderManager order_manager("orderData","pendingData");
CommandParser command_parser(user_manager,train_manager,order_manager);
int main() {
  //freopen("test1.ans", "w", stdout);
  //freopen("../test/data/basic/1.in", "r", stdin);
  std::cout << sizeof(TrainStation) << std::endl;
  while (true) {
      command_parser.Run();
  }
}
