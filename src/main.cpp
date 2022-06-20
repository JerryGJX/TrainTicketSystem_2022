//#pragma GCC optimize(2)
#include <iostream>
#include "tools/Char.hpp"
#include "command_parser.hpp"

UserManager user_manager("userData");
TrainManager train_manager("trainData", "dtts", "sdb", "btb");
OrderManager order_manager("orderData", "pendingData");
CommandParser command_parser(user_manager, train_manager, order_manager);
int main()
{
  //  freopen("test1.ans", "w", stdout);
  //  freopen("../src/test/data/basic_2/1.in", "r", stdin);
  std::ios::sync_with_stdio(false);

  while (true)
  {
    // try {

    command_parser.Run();
    std::cout << "__end_of_output__" << std::endl;
  }
}
