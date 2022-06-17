cmake_minimum_required(VERSION 3.16)
project(trainTicket)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
add_executable(code
    src/main.cpp
    src/user_manager.cpp
    src/command_parser.cpp
    src/train_manager.cpp
    src/order_manager.cpp)
