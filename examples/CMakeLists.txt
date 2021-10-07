cmake_minimum_required(VERSION 3.15)
project(mqcpp-examples)

set(CMAKE_CXX_STANDARD 17)

include_directories(../libmqcpp)

add_executable(peer_one peer_one.cpp)
target_link_libraries(peer_one mqcpp)

add_executable(peer_two peer_two.cpp)
target_link_libraries(peer_two mqcpp)
