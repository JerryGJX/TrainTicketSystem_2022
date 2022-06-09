cmake_minimum_required(VERSION 3.16)
project(trainTicket)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
add_executable(code
    src_rollback/main.cpp
    src_rollback/user_manager.cpp
    src_rollback/command_parser.cpp
    src_rollback/train_manager.cpp
    src_rollback/order_manager.cpp)
