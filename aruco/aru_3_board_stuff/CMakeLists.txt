cmake_minimum_required(VERSION 2.8)
project(aruco_testproject)
SET(CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake/ )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") # added by jaime

find_package(aruco REQUIRED )
add_executable(based_on_aruco_test based_on_aruco_test.cpp)
target_link_libraries(based_on_aruco_test  ${aruco_LIBS})
