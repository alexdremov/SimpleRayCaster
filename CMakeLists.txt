cmake_minimum_required(VERSION 3.19)
project(RayCaster)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -msse4.2 -mavx -march=native -fenable-matrix")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_library(SDL2_LIBRARY NAME SDL2)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)

set(RayCaster_SRC main.cpp src/Matrix.cpp src/Linalg.cpp)

add_executable(RayCaster "${RayCaster_SRC}")
include_directories(RayCaster ${SDL2_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIRS} ./include)
target_link_libraries(RayCaster ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2_GFX_LIBRARY})