
cmake_minimum_required(VERSION 3.15)
project(libmqcpp)

set(CMAKE_CXX_STANDARD 14)

file(GLOB HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(mqcpp ${HEADERS} ${SOURCES})

target_include_directories(mqcpp PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(mqcpp PRIVATE rt pthread)
