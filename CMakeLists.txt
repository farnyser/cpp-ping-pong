cmake_minimum_required(VERSION 3.12)
project(ping_pong)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads)
find_package(Boost 1.40.0 REQUIRED system)

add_executable(ping_pong main.cpp)
target_link_libraries(ping_pong ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})