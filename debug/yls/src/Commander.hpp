#ifndef TrainTicketSystem_Commander_HPP
#define TrainTicketSystem_Commander_HPP

#include <iostream>
#include <cstring>

namespace hnyls2002 {

    const int CmdMax = 17;
    const int argMax = 26;

    struct CmdType {
        int FuncID, TimeStamp;
        std::string cmd_arg[argMax];

        std::string &operator[](const char &c) {
            return cmd_arg[c - 'a'];
        }

        const std::string &operator[](const char &c) const {
            return cmd_arg[c - 'a'];
        }
    };

    const std::string CmdName[CmdMax] = {"add_user", "login", "logout", "query_profile",
                                         "modify_profile", "add_train", "delete_train",
                                         "release_train", "query_train", "query_ticket",
                                         "query_transfer", "buy_ticket", "query_order",
                                         "refund_ticket", "rollback", "clean", "exit"};

    sjtu::vector<std::string> split(const std::string &str) {
        sjtu::vector<std::string> ret;
        int len = str.size();
        std::string tmp;
        for (int i = 0; i < len; ++i) {
            if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r') {
                if (!tmp.empty())ret.push_back(tmp);
                tmp.clear();
            } else tmp += str[i];
        }
        if (!tmp.empty())ret.push_back(tmp);
        return ret;
    }

    CmdType Parser(const std::string &str) {
        CmdType ret;
        auto res = split(str);
        std::cout << res[0] << " ";
        ret.TimeStamp = std::stoi(res[0].substr(1, res[0].size() - 2));
        for (int i = 0; i < CmdMax; ++i)
            if (res[1] == CmdName[i]) {
                ret.FuncID = i;
                break;
            }
        for (int i = 2, b = res.size(); i < b; i += 2)
            ret[res[i][1]] = res[i + 1];
        return ret;
    }

}

#endif