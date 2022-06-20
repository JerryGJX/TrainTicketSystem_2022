//
// Created by JerryGuo on 2022/5/12.
//

#ifndef TRAIN_MANAGER_CPP__MYDEFS_HPP_
#define TRAIN_MANAGER_CPP__MYDEFS_HPP_

#include "tools/Char.hpp"

namespace JerryGJX
{
    using ull = unsigned long long;
    constexpr unsigned long long ULL_Max = UINT64_MAX;

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
    constexpr int max_stationName_length_ = 40;
    constexpr int max_time_length_ = 11;
    constexpr int max_stationNum = 100;

    using trainIDType = Char<max_trainID_length_>;
    using stationType = Char<max_stationName_length_>;
    // using timeType = Char<max_time_length_>;//完整时间
    using Day = int;
    using Minute = int;
    /**
     * @brief order_manager中使用
     */
    enum orderStatusType
    {
        SUCCESS,
        PENDING,
        REFUNDED
    };

    enum infoType
    {
        _c,
        _d,
        _f,
        _g,
        _i,
        _m,
        _n,
        _o,
        _p,
        _q,
        _s,
        _t,
        _u,
        _x,
        _y
    }; //_y==14
}

#endif // TRAIN_MANAGER_CPP__MYDEFS_HPP_
