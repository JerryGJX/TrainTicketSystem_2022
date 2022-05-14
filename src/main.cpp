#include <iostream>
#include "tools/Char.hpp"
#include "command_parser.hpp"

int main() {
  freopen("test1.ans", "w", stdout);

  UserManager user_manager("userData");
  TrainManager train_manager("trainData", "dtts", "sdb");
  OrderManager order_manager("orderData");
  CommandParser command_parser(user_manager,train_manager,order_manager);
  while (true) {
      command_parser.Run();
  }

}
