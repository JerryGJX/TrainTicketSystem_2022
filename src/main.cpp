#include <iostream>
#include "tools/Char.hpp"
#include "command_parser.hpp"

int main() {
  //freopen("test1.ans", "w", stdout);
//  freopen("../test/data/basic_2/1.in", "r", stdin);

  UserManager user_manager("userData");
  TrainManager train_manager("trainData", "dtts", "sdb");
  OrderManager order_manager("orderData","pendingData");
  CommandParser command_parser(user_manager,train_manager,order_manager);
  while (true) {
      command_parser.Run();
  }

}
