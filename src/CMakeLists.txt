cmake_minimum_required(VERSION 3.16)
project(trainTicket)

set(CMAKE_CXX_STANDARD 17)

add_executable(trainTicket main.cpp user_manager.cpp user_manager.hpp  command_parser.cpp command_parser.hpp tools/Char.hpp tools/Algorithm.hpp tools/Time.hpp mydefs.hpp train_manager.cpp train_manager.hpp order_manager.cpp order_manager.hpp)
