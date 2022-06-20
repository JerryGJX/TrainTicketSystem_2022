//
// Created by JerryGuo on 2022/6/19.
//

#ifndef MAIN_CPP_SRC_TOOLS_ERROR_HPP_
#define MAIN_CPP_SRC_TOOLS_ERROR_HPP_

#include <ostream>
class Error : public std::exception
{
  const string message;

public:
  Error() = default;
  Error(const string &error_disc) : message(error_disc){};
  Error(const Error &x) : message(x.message){};
  friend std::ostream &operator<<(std::ostream &os, const Error &error)
  {
    os << "message: " << error.message;
    return os;
  }
};

// throw Error("Current User Isn't Online");
// throw Error("Current Privilege Is Too Low");
// throw Error("Target User Isn't Added");
//  throw Error("Target User Already Online");
// throw Error("Wrong Password");
// throw Error("Target User Isn't Online");
// throw Error("Target User Is Added");

// throw Error("Train Is Added");
// throw Error("Train Isn't Added");
// throw Error("Train Is Released");
// throw Error("Train Isn't Released");
// throw Error("Isn't Sell Date");
// throw Error("Order Number Isn't Enough");
// throw Error("Order Is Refunded");

#endif // MAIN_CPP_SRC_TOOLS_ERROR_HPP_
