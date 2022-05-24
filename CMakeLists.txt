cmake_minimum_required(VERSION 3.16)
project(trainTicket)

set(CMAKE_CXX_STANDARD 17)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")
add_executable(code src/main.cpp src/user_manager.cpp src/user_manager.hpp  src/command_parser.cpp src/command_parser.hpp src/tools/Char.hpp src/tools/Algorithm.hpp src/tools/Time.hpp src/mydefs.hpp src/train_manager.cpp src/train_manager.hpp src/order_manager.cpp src/order_manager.hpp)
