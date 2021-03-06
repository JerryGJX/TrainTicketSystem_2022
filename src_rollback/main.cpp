//#pragma GCC optimize(2)
#include <iostream>
#include "tools/Char.hpp"
#include "command_parser.hpp"

UserManager user_manager("userData");
TrainManager train_manager("trainData", "dtts", "sdb","btb");
OrderManager order_manager("orderData","pendingData");
CommandParser command_parser(user_manager,train_manager,order_manager);
int main() {
//  freopen("test1.ans", "w", stdout);
//  freopen("../src_rollback/test/data_rollback/basic_1/1.in", "r", stdin);
  std::ios::sync_with_stdio(false);

  while (true) {
      command_parser.Run();
  }
}
