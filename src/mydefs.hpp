#include "tools/Char.hpp"

namespace JerryGJX {
/**
 * @brief user_manager中使用
 */
constexpr int max_username_length_ = 20;
constexpr int max_password_length_ = 30;
constexpr int max_name_length_ = 20;
constexpr int max_mailAddr_length_ = 30;

using usernameType = Char<max_username_length_>;
using passwordType = Char<max_password_length_>;
using nameType = Char<max_name_length_>;
using mailAddrType = Char<max_mailAddr_length_>;

/**
 * @brief train_manager中使用
 */
constexpr int max_trainID_length_ = 20;
constexpr int max_stationName_length_ = 35;
constexpr int max_time_length_ = 8;
constexpr int max_stationNum = 105;

using trainIDType = Char<max_trainID_length_>;
using stationType = Char<max_stationName_length_>;
using timeType = Char<max_time_length_>;//hhmmdd

/**
 * @brief order_manager中使用
 */
 enum orderStatusType{SUCCESS,PENDING,REFUNDED};

}