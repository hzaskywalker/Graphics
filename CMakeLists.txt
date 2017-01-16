cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Release)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

project( main )
add_executable( main main.cpp )
target_link_libraries( main ${OpenCV_LIBS} )

project( simplify )
add_executable( simplify simplify.cpp )
target_link_libraries( simplify ${OpenCV_LIBS} )
