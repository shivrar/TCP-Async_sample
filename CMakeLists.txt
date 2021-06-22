cmake_minimum_required(VERSION 2.8.3)
project(nocs_wifi_ethernet)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11 -Wall -Wextra -Wpedantic)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED)

## System dependencies are found with CMake's conventions
find_package(Threads REQUIRED)


#TEMPORARY for testing the udp and TCP stuff

add_executable(client.o non_blocking_client.cpp)
add_executable(server.o non_block_server.cpp)

target_link_libraries(client.o ${catkin_LIBRARIES} Threads::Threads)
target_link_libraries(server.o ${catkin_LIBRARIES} Threads::Threads)