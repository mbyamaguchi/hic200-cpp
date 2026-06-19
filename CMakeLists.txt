cmake_minimum_required(VERSION 3.16)
project(hic200_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(ZLIB REQUIRED)

add_library(hic200core
    src/common.cpp
    src/gz_reader.cpp
    src/bin_def.cpp
    src/read_and_dump.cpp
    src/aggregate.cpp
)
target_include_directories(hic200core PUBLIC include)
target_link_libraries(hic200core PUBLIC ZLIB::ZLIB)

add_executable(make_bin_def2 src/main_make_bin_def.cpp)
target_link_libraries(make_bin_def2 PRIVATE hic200core)

add_executable(read_and_dump2 src/main_read_and_dump.cpp)
target_link_libraries(read_and_dump2 PRIVATE hic200core)

add_executable(dump_to_sort src/main_dump_to_sort.cpp)
target_link_libraries(dump_to_sort PRIVATE hic200core)

add_executable(hic200 src/main_hic200.cpp)
target_link_libraries(hic200 PRIVATE hic200core)
