cmake_minimum_required(VERSION 3.1)
project(lazier1)

find_package(xtensor REQUIRED)

message(STATUS "xtensor include path : ${xtensor_INCLUDE_DIRS}")

set(LAZIER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(lazier INTERFACE)
target_include_directories(lazier INTERFACE $<BUILD_INTERFACE:${LAZIER_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(lazier INTERFACE ${xtensor_INCLUDE_DIRS})
target_link_libraries(lazier INTERFACE xtensor)

OPTION(BUILD_TEST "lazier test suite" ON)

if(BUILD_TEST)
    add_subdirectory(test)
endif()